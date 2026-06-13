package com.wws2.densitymeter.model

/** 리포트 생성 단계. */
enum class ReportStage { SELECT, COLLECTING, DONE, ERROR }

/**
 * 한 ENV130 채널에 대해 리포트 생성 시 BLE로 수집한 스냅샷.
 * 측정값 + 설정값은 STATUS 응답에서, 파형은 ECHO 실시간/평균 응답에서 모은다.
 */
data class ReportData(
    val deviceId: String,
    val label: String,
    val firmwareVersion: String,
    val timestamp: String,
    // 측정값 (STATUS)
    val lightLevel: Double,
    val heavyLevel: Double,
    val temperatureC: Double,
    val currentMA: Double,
    // 설정값 (STATUS)
    val freqMHz: Double,
    val offset: Double,
    val emptyDistance: Double,
    val deadZone: Double,
    val set4mA: Double,
    val set20mA: Double,
    val damping: Int,
    // 임계값/게인/릴레이 (ECHO + STATUS)
    val thrLightSet: Int,
    val thrLightMode: Int,   // 0=Auto(%), 1=Manual(0.1V)
    val thrHeavySet: Int,
    val thrHeavyMode: Int,
    val echoAmp: Int,
    val relay: Int,
    // 파형
    val realEcho: InterfaceEchoReading?,
    val avgEcho: InterfaceEchoReading?,
    // 사용자 입력 제목 (파일명/리스트 표시용, 비우면 label 사용)
    val title: String = "",
    // 점검자 의견 (리포트 화면에서 입력, 스냅샷/PDF에 포함)
    val comment: String = "",
)
