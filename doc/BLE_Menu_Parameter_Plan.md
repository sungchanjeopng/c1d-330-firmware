# 메뉴 화면 (전체 파라미터 읽기/쓰기) 구현 계획서

- 문서 버전: v1.0 (2026-06-11)
- 대상 제품: C1D-330 (ENV130, 초음파 슬러지 계면계, CH1/CH2 통합)
- 범위: 펌웨어(STM32F769) + Android(wws2_android) + iOS(wws2-ios)
- 상태: **설계 확정, 구현 보류**

---

## 1. 개요

### 1.1 목표
현재 BLE로 접근 가능한 파라미터는 13개(전체의 약 7%)에 불과하다.
본 계획은 펌웨어 메뉴 시스템의 **전체 파라미터(~180개)를 앱에서 읽기/쓰기** 가능하게 하고,
앱에 본체 LCD 메뉴 트리를 미러링하는 **"Device Settings" 화면**을 신설하는 것을 목표로 한다.

### 1.2 설계 원칙 (확정)
| 원칙 | 내용 |
|------|------|
| 프레임 불변 | 기존 SOF 0x02(요청)/0x03(쓰기) 7바이트 프레임 구조를 그대로 유지. 새 SOF 추가하지 않음 |
| 읽기 일괄화 | **cmd 100 (0x0064) 한 번으로 전체 파라미터 덤프** (CH1/CH2 통합 기기이므로 양 채널 모두 포함) |
| 쓰기 개별화 | **cmd 0x2000 \| idx** 로 개별 쓰기. 읽기 배열의 인덱스와 동일 체계 |
| 단일 테이블 | "파라미터 순서 테이블" 하나를 펌웨어/Android/iOS가 공유 — 인덱스가 곧 파라미터 ID |
| 구버전 호환 | 기존 cmd(1~13, heartbeat류)는 한 줄도 변경하지 않음. 신 cmd는 구버전에서 unknown 처리 |

---

## 2. 현황 분석

### 2.1 현재 BLE 프로토콜
```
프레임(7B 고정): [SOF 1B][CMD 2B BE][DATA 2B BE][CRC16-Modbus 2B LE]

SOF 0x02 = 요청(heartbeat)/응답   SOF 0x03 = 설정 쓰기
```

#### SOF 0x02 cmd 사용 현황 (data_commu.c DaBT_ProcMain)
| cmd | 기능 |
|-----|------|
| 0x00 / 0x10 | Status CH1/CH2 (측정값+설정값 26~32B) |
| 0x01 / 0x11 | Waveform CH1/CH2 (~2400B 청크 전송) |
| 0x05 / 0x15 | Waveform Average CH1/CH2 |
| 0x02 / 0x12 | Trend CH1/CH2 |
| 0x07 / 0x17 | Trend Full CH1/CH2 (CSV 다운로드) |
| 0x04 / 0x14 | Diag CH1/CH2 (22B) |
| 0x50 / 0x51 | OTA 시작/종료 |
| 0xF0 | PIN 페어링 |

#### SOF 0x03 cmd 사용 현황 (DaBT_ApplyAppSetting)
| cmd (CH1) | 항목 | CH2 |
|-----------|------|-----|
| 1~13 | EchoAmp, ThrLight(A/M), ThrHeavy(A/M), Freq, Offset, 4mA, 20mA, TVG, Damping, Empty, DeadZone | +1000 (1001~1013) |

#### 빈 공간 (충돌 검증 완료)
- `0x0064(100)`: 미사용 ✅
- `0x2000~0x2FFF`: 미사용 ✅ (기존 최대 cmd = 0x03F5)

### 2.2 펌웨어 메뉴 시스템 구조 (`_Wess/menu/mn_section/`)
모든 섹션에 Get/Set 함수가 **이미 존재**한다. 값 타입은 전부 S32.

| # | 섹션 | 헤더 | SINGLE enum (확인된 항목) | Get/Set API |
|---|------|------|--------------------------|-------------|
| 0 | Measurement Base (MS0) | menu_measure.h | UNIT, OPERATION, EMPTY, DEADZONE, MEASURE_CYCLE, MEASURE_RESET | `MnMSR_BaseGet/Set_Ch_Value(ch, iIt, val)` |
| 1 | Measurement Cal (MS1) | menu_measure.h | FREQ, AUTO_CAL, AUTO_FAMP, ECHO_AMP, THR_HEAVY, THR_LIGHT, ASF_HEAVY, ASF_LIGHT, DAMPING, OFFSET | `MnMSR_CalGet/Set_Ch_Value` + `MnMSR_Get/Set_Threshold_Ch_Value` |
| 2 | Output (OS0) | menu_output.h | ASSIGN, SET_04mA, SET_20mA, TRM_12mA, TRM_20mA, OUT_04mA | `MnOUT_CurPrGet/Set_Ch_Value` |
| 3 | Data (DS0/DS1) | menu_data.h | DS0: INTERVAL, DOWNLOAD, DELETE, DISPLAY_TERM / DS1: SAVEMODE, SAVEINTERVAL, ECHO_LOAD, ECHO_VIEW_MODE, DELETE | `MnDAT_SavPr/ComPr/EchoSavePr Get/Set` |
| 4 | System (SYS) | menu_system.h | PASSCODE, LANG, TIME, SITE_NAME, FTR_RST | `MnSYS_PrGet/Set...` |
| 5 | Engineer (EGN) | menu_engineer.h | THRESHOLD_SYNC, PROFILE, TVG, WINDOW_RESET_TIME, WINDOW_RANGE, MSR | `MnEGN_PrGet/Set_Ch_Value` |
| 6 | Factory (FTR) | menu_factory.h | (13개, Phase 0에서 상세 추출) | `MnFTR_PrGet/Set_CH_Value` |
| 7 | Test (TST) | menu_test.h | (52개, Phase 0에서 상세 추출) | `MnTST_*` |

참고 문서: repo 루트 `MenuTree_c1d-330.xlsx` (메뉴 트리 와이어프레임)

### 2.3 현재 한계
1. **쓰기**: SOF 0x03의 DATA가 16-bit — 펌웨어 파라미터는 S32 (단, 실제 값 범위는 대부분 ±32767 이내로 추정 → Phase 0에서 전수 확인)
2. **읽기**: 전용 cmd 없음. STATUS 응답에 박힌 일부 필드만 수동 파싱
3. cmd 1~13만 매핑 — 단위(UNIT), 운전모드(OPERATION), 측정주기, 로깅 설정, 사이트명 등 대부분 접근 불가

---

## 3. 프로토콜 설계 (확정)

### 3.1 읽기 — cmd 100 전체 덤프

```
요청 (앱 → 펌웨어, 7B):
  ┌──────┬───────────┬───────────┬──────────────┐
  │ 0x02 │ 0x00 0x64 │ 0x00 0x00 │ CRC_LO CRC_HI│
  └──────┴───────────┴───────────┴──────────────┘

응답 (펌웨어 → 앱, 5 + 2 + N×2 + 2 바이트):
  ┌──────┬───────────┬────────────┬─────────┬─────────┬─...─┬──────────────┐
  │ 0x02 │ 0x00 0x64 │ count 2B BE│ p[0] 2B │ p[1] 2B │     │ CRC_LO CRC_HI│
  └──────┴───────────┴────────────┴─────────┴─────────┴─...─┴──────────────┘
```

- `count` = 펌웨어가 보유한 파라미터 개수 (버전 안전장치, §3.4)
- `p[i]` = 순서 테이블 i번째 파라미터의 현재 값 (S16 BE; 음수는 2의 보수)
- 예상 크기: count 180 기준 약 **370B**

#### 전송 방식
- 기존 waveform 응답(~2400B)과 동일하게 BT 모듈이 BLE notification 단위로 자동 분할
- 펌웨어는 `DaBT_SendWaveform_Ch` 패턴 재사용: ~98B씩 `URT_TxPkt` + `HAL_Delay(10)`
- 앱은 기존 rxBuf 누적 + 길이 도달 + CRC 검증 패턴 (`tryParseFrame`의 `expectedDataSize`에 추가)

### 3.2 쓰기 — cmd 0x2000 | idx 개별 쓰기

```
요청 (앱 → 펌웨어, 7B):
  ┌──────┬─────────────────┬────────────┬──────────────┐
  │ 0x03 │ (0x2000|idx) BE │ value 2B BE│ CRC          │
  └──────┴─────────────────┴────────────┴──────────────┘

ACK (펌웨어 → 앱, 7B):
  ┌──────┬─────────────────┬─────────────┬──────────────┐
  │ 0x02 │ (0x2000|idx) BE │ result 2B BE│ CRC          │
  └──────┴─────────────────┴─────────────┴──────────────┘
```

- idx = 순서 테이블 인덱스 (0~0x1FFF, 8191까지 여유)
- value = S16 (signed 항목은 2의 보수 그대로, 펌웨어가 (S16) 캐스팅)
- result 코드:

| result | 의미 |
|--------|------|
| 0 | OK (적용 완료) |
| 1 | Range error (min/max 벗어남) |
| 2 | Locked (읽기 전용 또는 잠긴 섹션) |
| 3 | Unknown idx (구펌웨어에 없는 항목) |

### 3.3 기존 cmd와의 관계
- 기존 setting cmd 1~13/1001~1013: **무변경 유지** (구버전 앱 호환)
- 신규 idx 체계와 기존 cmd가 같은 파라미터를 가리켜도 충돌 없음 (둘 다 동일 Get/Set 함수에 도달)
- 장기적으로 앱은 신규 체계로 통일하고 기존 cmd는 유지보수만

### 3.4 버전 호환 전략
| 시나리오 | 동작 |
|---------|------|
| 신앱 + 구펌웨어 | cmd 100에 무응답(타임아웃) → 앱은 "이 펌웨어는 메뉴 기능 미지원" 안내 |
| 구앱 + 신펌웨어 | 구앱은 cmd 100을 안 보냄. 기존 기능 그대로 동작 |
| 펌웨어에 파라미터 추가됨 | count가 늘어남. 구앱은 자신이 아는 idx까지만 표시 |
| 앱이 더 많은 idx를 앎 | count 밖 idx는 "지원 안 함" 표시. 쓰기 시 result 3 |

**순서 테이블 운영 규칙: 기존 인덱스의 의미는 절대 변경 금지. 새 항목은 끝에만 추가.**

---

## 4. 파라미터 순서 테이블 (v1 초안)

> Phase 0에서 전 항목 확정. 아래는 구조 예시.
> 규칙: 섹션 순서 고정(MS0→MS1→OUT→DAT→SYS→EGN→FTR→TST), 각 섹션 내 CH1 전체 → CH2 전체.

| idx | 섹션 | 항목 | ch | 단위/형식 | 비고 |
|-----|------|------|----|----------|------|
| 0 | MS0 | UNIT | 공통 | enum(m/ft...) | |
| 1 | MS0 | OPERATION | CH1 | enum(Dist/Sludge) | |
| 2 | MS0 | EMPTY | CH1 | x0.01 m | 기존 cmd 12와 동일 대상 |
| 3 | MS0 | DEADZONE | CH1 | x0.01 m | 기존 cmd 13과 동일 대상 |
| 4 | MS0 | MEASURE_CYCLE | CH1 | sec | |
| 5 | MS0 | MEASURE_RESET | CH1 | trigger | 쓰기 시 동작 실행형 |
| 6~10 | MS0 | (CH2 동일 5개) | CH2 | | |
| 11 | MS1 | FREQ | CH1 | enum(380/270/160/130k) | 기존 cmd 6 |
| 12 | MS1 | AUTO_CAL | CH1 | | |
| 13 | MS1 | AUTO_FAMP | CH1 | | |
| 14 | MS1 | ECHO_AMP | CH1 | 1~50 | 기존 cmd 1 |
| 15 | MS1 | THR_HEAVY (mode) | CH1 | enum(Auto/Manual) | |
| 16 | MS1 | THR_HEAVY (value) | CH1 | % 또는 0.1V | Threshold_Ch_Value |
| 17 | MS1 | THR_LIGHT (mode) | CH1 | | |
| 18 | MS1 | THR_LIGHT (value) | CH1 | | |
| 19 | MS1 | ASF_HEAVY | CH1 | | |
| 20 | MS1 | ASF_LIGHT | CH1 | | |
| 21 | MS1 | DAMPING | CH1 | 1~100 | 기존 cmd 11 |
| 22 | MS1 | OFFSET | CH1 | signed x0.01m | 기존 cmd 7 |
| 23~34 | MS1 | (CH2 동일) | CH2 | | |
| ... | OUT/DAT/SYS/EGN/FTR/TST | | | | Phase 0 확정 |

주의 항목 (Phase 0에서 처리 방식 결정):
- **TIME (SYS)**: 날짜+시간은 16-bit 초과 → 여러 idx로 분해 (년/월/일/시/분) 또는 제외
- **SITE_NAME (SYS)**: 문자열 → v1에서는 제외 (BLE 광고명 파싱으로 이미 표시 중)
- **DOWNLOAD/DELETE/RESET (DAT/MS0)**: 값이 아닌 "동작 실행" 항목 → 쓰기 전용 trigger로 분류, UI에서 버튼으로 표현
- **PASSCODE (SYS)**: 보안 항목 → 읽기 응답에서 마스킹(0 반환) 또는 제외

---

## 5. 펌웨어 구현 계획 (`_Wess/function/data/data_commu.c`)

### 5.1 순서 테이블 정의 (신규 파일 권장: `data_param_table.c/h`)
```c
typedef struct {
    U08 section;   // 0=MS0, 1=MS1, 2=OUT, 3=DAT, 4=SYS, 5=EGN, 6=FTR, 7=TST
    U08 item;      // 섹션 내 *_OPT_SINGLE_* enum 값
    U08 ch;        // 0=CH1, 1=CH2, 0xFF=공통
    U08 flags;     // bit0: 읽기전용, bit1: trigger형, bit2: 잠금(EGN/FTR/TST)
    S16 min;
    S16 max;
} ParamDef;

extern const ParamDef PARAM_TABLE[];
extern const U16 PARAM_COUNT;
```

### 5.2 공용 디스패처
```c
static S32 Param_Get(const ParamDef* p) {
    switch (p->section) {
        case 0: return MnMSR_BaseGet_Ch_Value(p->ch, p->item);
        case 1: return MnMSR_CalGet_Ch_Value(p->ch, p->item);   // THR value는 별도 분기
        case 2: return MnOUT_CurPrGet_Ch_Value(p->ch, p->item);
        case 3: return MnDAT_..._Get(...);
        case 4: return MnSYS_PrGet...(p->item);
        case 5: return MnEGN_PrGet_CH_Item(p->ch, p->item);
        case 6: return MnFTR_PrGet_CH_Value(p->ch, p->item);
        case 7: return MnTST_PrGet...(p->item);
    }
    return 0;
}
static U16 Param_Set(const ParamDef* p, S16 value);  // min/max 검증 + 섹션별 Set 호출 + result 반환
```

### 5.3 cmd 100 핸들러 (DaBT_ProcMain의 SOF 0x02 switch에 추가)
```c
case 0x64: DaBT_SendAllParams(); break;

void DaBT_SendAllParams(void) {
    // [02][00 64][count 2B][PARAM_COUNT개 × 2B] ... [CRC]
    // 누적 CRC 방식 (Crc16_UpdateBuf) — waveform 송신 패턴 재사용
    // ~98B 단위 URT_TxPkt + HAL_Delay(10)
    for (i = 0; i < PARAM_COUNT; i++) {
        S32 v = Param_Get(&PARAM_TABLE[i]);
        // S16 클램핑 후 BE 2B 직렬화
    }
}
```

### 5.4 쓰기 분기 (SOF 0x03 처리부)
```c
if (sof == 0x03) {
    U16 result;
    if (cmd16 & 0x2000) {
        U16 idx = cmd16 & 0x1FFF;
        result = (idx < PARAM_COUNT)
               ? Param_Set(&PARAM_TABLE[idx], (S16)data16)
               : 3;
    } else {
        result = DaBT_ApplyAppSetting(cmd16, data16);   // 기존 그대로
    }
    DaBT_SendSettingAck(cmd16, result);
    ...
}
```

### 5.5 검증/안전
- Param_Set에서 min/max 검증 (result 1)
- flags의 잠금 비트 검사 (result 2) — EGN/FTR/TST는 v1에서 잠금
- trigger형 항목은 value 무시하고 동작 실행
- 설정 적용 후 NVM 저장은 기존 Set 함수 내부 로직(MRM_WrByte/WrWord) 그대로 동작

### 5.6 펌웨어 측 예상 작업량: 0.5~1일

---

## 6. Android 구현 계획 (`wws2_android`)

### 6.1 파라미터 메타 테이블 (신규: `model/DeviceParam.kt`)
```kotlin
data class DeviceParam(
    val idx: Int,
    val section: ParamSection,      // MS0/MS1/OUT/DAT/SYS (v1 노출 범위)
    val ch: Int?,                   // null=공통
    val label: String,              // "Empty Distance"
    val unit: String,               // "m", "%", "kHz", ""
    val scale: Double,              // 표시 변환 (0.01 등)
    val min: Int, val max: Int, val step: Int,
    val enumLabels: List<String>?,  // enum형이면 선택지 ["380 kHz", ...]
    val readOnly: Boolean = false,
    val isTrigger: Boolean = false, // 버튼형 (RESET 등)
)
val PARAM_TABLE: List<DeviceParam> = listOf(/* 펌웨어 테이블과 1:1, idx 순서 동일 */)
```

### 6.2 프로토콜 레이어 (BleProtocolService / MainViewModel)
- `buildHeartbeat(0x64)` 재사용해 cmd 100 요청
- `expectedDataSize(0x0064)` = 가변: 헤더에서 count 읽고 `2 + count*2`로 계산 (기존 CRC fallback 패턴 보강)
- 파싱 결과 `Map<idx, Int>` → StateFlow로 노출
- 쓰기: 기존 `sendAppSetting` 패턴 복제 — idle 윈도우 대기 + `[03][0x2000|idx][value]` + ACK 대기 + (보강) cmd 100 재요청 verify

### 6.3 UI (신규: `ui/screen/DeviceSettingsScreen.kt`)
```
Menu 탭 → "Device Settings" 행 추가 (subPage = "deviceSettings")

화면 구조:
┌─ Device Settings ──────────────┐
│ [Measurement]  ▸               │   ← 섹션 카드 (토스 스타일)
│ [Calibration]  ▸               │
│ [Output]       ▸               │
│ [Data/Logging] ▸               │
│ [System]       ▸               │
└────────────────────────────────┘
  진입 시 cmd 100 1회 로드 (로딩 스피너)
       ↓ 섹션 탭
┌─ Measurement (CH1) ────[CH1|CH2]┐  ← 채널 토글
│ Unit          m          ✎     │
│ Operation     Sludge     ✎     │
│ Empty         4.02 m     ✎     │
│ ...                            │
└────────────────────────────────┘
       ↓ 항목 탭
기존 ConfigEditDialog 재활용:
  - 숫자형: stepper + 텍스트 입력 (decimalScale 지원)
  - enum형: 선택 리스트
  - trigger형: 확인 다이얼로그 → 실행
  - Sending... / Success / Failed 상태 표시 (기존 패턴)
```

### 6.4 기존 코드 재활용 포인트
| 재활용 대상 | 용도 |
|------------|------|
| `ConfigEditDialog` (DiagnosticsTabScreen) | 값 편집 UI + Sending/Success/Failed |
| `sendAppSetting`의 idle-윈도우 + verify 패턴 | 쓰기 신뢰성 |
| `tryParseFrame` rxBuf 누적 + CRC | cmd 100 응답 파싱 |
| `TossGroupLabel`/`Card`/`KvRow` (ReportScreen) | 섹션/항목 리스트 디자인 |

### 6.5 Android 예상 작업량: 2~3일

---

## 7. iOS 구현 계획 (`wws2-ios`)
- Android 미러: `DeviceParam.swift` 테이블(공용 포맷 유지), AppViewModel에 cmd 100 파싱 + 쓰기, `DeviceSettingsScreen.swift`
- 값 편집은 기존 `ConfigEditSheet` 재활용
- 빌드 검증: GitHub Actions (macos-15)
- 예상 작업량: 1~2일

---

## 8. 안전장치
1. **섹션 잠금**: v1 노출 = MS0/MS1/OUT/DAT/SYS. EGN/FTR/TST는 펌웨어 flags 잠금(result 2) + 앱 미노출
2. **PASSCODE 마스킹**: 읽기 응답에서 0 반환
3. **trigger형 확인 다이얼로그**: RESET/DELETE류는 "정말 실행?" 2단계 확인
4. **쓰기 후 verify**: ACK(result 0) + cmd 100 재읽기로 값 반영 확인 (이중 확인)
5. **연결 끊김 중 쓰기 방지**: 기존 disconnect 처리와 연동

---

## 9. 테스트 계획
| 단계 | 항목 |
|------|------|
| 펌웨어 단독 | BLE 터미널(nRF Connect)로 cmd 100 raw 응답 확인, 쓰기 ACK/range/locked 케이스 |
| 프로토콜 | count 일치, 전 idx 값이 본체 LCD 메뉴와 일치하는지 전수 대조 |
| 앱 | 섹션별 표시/편집, trigger 동작, CH1/CH2 토글, 연결 끊김 중 동작 |
| 호환성 | 구펌웨어 + 신앱 (타임아웃 안내), 신펌웨어 + 구앱 (기존 기능 무영향) |
| 회귀 | 기존 cmd 1~13 설정, Report, Echo, OTA 정상 동작 확인 |

---

## 10. 일정 요약
| Phase | 내용 | 기간 |
|-------|------|------|
| 0 | 파라미터 전수조사 + 순서 테이블 v1 확정 (16-bit 초과 항목 처리 결정 포함) | 0.5일 |
| 1 | 펌웨어: cmd 100 + 0x2000 쓰기 + 디스패처 + 단독 테스트 | 0.5~1일 |
| 2 | Android: 테이블 + 프로토콜 + Device Settings UI + 실기 테스트 | 2~3일 |
| 3 | iOS 미러 + CI 검증 | 1~2일 |
| 4 | 통합 테스트 + 문서 갱신 | 0.5일 |
| **계** | | **4.5~7일** |

---

## 11. 리스크 및 미결정 사항
| # | 항목 | 상태 |
|---|------|------|
| 1 | 16-bit(±32767) 초과 파라미터 존재 여부 | Phase 0에서 전수 확인. 발견 시: 스케일 조정 또는 상/하위 워드 2-idx 분할 |
| 2 | TIME/SITE_NAME 등 비정형 항목 | TIME=다중 idx 분해 또는 v1 제외 / SITE_NAME=v1 제외 (제안) |
| 3 | EGN/FTR/TST 노출 시점 | v2 이후, PIN 잠금 방식으로 (제안) |
| 4 | trigger형 항목의 부작용 (DELETE 등) | 확인 다이얼로그 + 잠금 플래그로 보호 |
| 5 | 순서 테이블 관리 | 펌웨어 `data_param_table.c`가 원본(single source of truth). 앱 테이블은 이를 따라 수동 동기화. 변경 시 끝에 추가만 |
