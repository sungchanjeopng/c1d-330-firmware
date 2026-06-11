package com.wws2.densitymeter.ui.screen

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.wws2.densitymeter.model.ReportData
import com.wws2.densitymeter.model.ReportStage
import com.wws2.densitymeter.ui.component.InterfaceEchoChart
import com.wws2.densitymeter.ui.theme.AppColors
import com.wws2.densitymeter.viewmodel.MainViewModel

private val ReportPurple = Color(0xFF7C3AED)
private val LightBlue = Color(0xFF3182F6)
private val HeavyOrange = Color(0xFFFF8C00)

@Composable
fun ReportScreen(vm: MainViewModel, onExportPdf: () -> Unit) {
    val state by vm.state.collectAsStateWithLifecycle()

    Box(modifier = Modifier.fillMaxSize().background(AppColors.Background)) {
        when (state.reportStage) {
            ReportStage.SELECT -> ReportDeviceSelect(vm)
            ReportStage.COLLECTING -> ReportCollecting()
            ReportStage.ERROR -> ReportError(state.reportError, onRetry = { vm.backToReportSelect() })
            ReportStage.DONE -> {
                val data = state.reportData
                if (data == null) ReportError("No report data", onRetry = { vm.backToReportSelect() })
                else ReportResult(data, onExportPdf = onExportPdf, onNew = { vm.backToReportSelect() })
            }
        }
    }
}

// ───────────────────── 기기 선택 ─────────────────────
@Composable
private fun ReportDeviceSelect(vm: MainViewModel) {
    val state by vm.state.collectAsStateWithLifecycle()
    val devices = state.connectedDevices.filter { it.label.startsWith("ENV130") }

    Column(modifier = Modifier.fillMaxSize().padding(20.dp)) {
        Text("Report", fontSize = 26.sp, fontWeight = FontWeight.W800, color = AppColors.DarkText, letterSpacing = (-0.5).sp)
        Spacer(Modifier.height(4.dp))
        Text("Pick a device to capture its current state.", fontSize = 14.sp, color = AppColors.GrayLabel)
        Spacer(Modifier.height(20.dp))

        if (devices.isEmpty()) {
            Box(modifier = Modifier.fillMaxWidth().padding(top = 60.dp), contentAlignment = Alignment.Center) {
                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Text("📋", fontSize = 40.sp)
                    Spacer(Modifier.height(10.dp))
                    Text("No connected ENV130 devices", fontSize = 15.sp, fontWeight = FontWeight.W600, color = AppColors.GrayLabel)
                }
            }
        } else {
            devices.forEach { device ->
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .shadow(3.dp, RoundedCornerShape(18.dp))
                        .clip(RoundedCornerShape(18.dp))
                        .background(AppColors.White)
                        .padding(horizontal = 18.dp, vertical = 16.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    Box(
                        Modifier.size(42.dp).clip(RoundedCornerShape(12.dp)).background(Color(0xFFEDE9FE)),
                        contentAlignment = Alignment.Center,
                    ) { Text("📊", fontSize = 20.sp) }
                    Spacer(Modifier.width(14.dp))
                    Column(modifier = Modifier.weight(1f)) {
                        Text(device.label, fontSize = 17.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
                        Text("Sludge Level Meter", fontSize = 12.sp, color = AppColors.WeakText)
                    }
                    Button(
                        onClick = { vm.selectReportDevice(device.id) },
                        colors = ButtonDefaults.buttonColors(containerColor = ReportPurple),
                        shape = RoundedCornerShape(12.dp),
                        contentPadding = PaddingValues(horizontal = 18.dp, vertical = 10.dp),
                    ) {
                        Text("Create", fontSize = 14.sp, fontWeight = FontWeight.W700, color = AppColors.White)
                    }
                }
                Spacer(Modifier.height(12.dp))
            }
        }
    }
}

@Composable
private fun ReportCollecting() {
    Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            CircularProgressIndicator(color = ReportPurple, strokeWidth = 3.dp)
            Spacer(Modifier.height(18.dp))
            Text("Collecting data…", fontSize = 16.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
            Spacer(Modifier.height(4.dp))
            Text("Measurement · Settings · Waveforms", fontSize = 13.sp, color = AppColors.GrayLabel)
        }
    }
}

@Composable
private fun ReportError(message: String?, onRetry: () -> Unit) {
    Box(modifier = Modifier.fillMaxSize().padding(28.dp), contentAlignment = Alignment.Center) {
        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            Text("⚠️", fontSize = 36.sp)
            Spacer(Modifier.height(10.dp))
            Text(message ?: "Report failed", fontSize = 15.sp, fontWeight = FontWeight.W600, color = AppColors.Error)
            Spacer(Modifier.height(18.dp))
            Button(onClick = onRetry, colors = ButtonDefaults.buttonColors(containerColor = ReportPurple), shape = RoundedCornerShape(12.dp)) {
                Text("Back", color = AppColors.White, fontWeight = FontWeight.W700)
            }
        }
    }
}

// ───────────────────── 리포트 결과 ─────────────────────
@Composable
private fun ReportResult(data: ReportData, onExportPdf: () -> Unit, onNew: () -> Unit) {
    Column(modifier = Modifier.fillMaxSize()) {
        Column(
            modifier = Modifier.weight(1f).verticalScroll(rememberScrollState()).padding(horizontal = 16.dp, vertical = 16.dp),
        ) {
            HeaderCard(data)
            Spacer(Modifier.height(18.dp))

            SectionLabel("Measurement", LightBlue)
            Spacer(Modifier.height(10.dp))
            Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                HeroStat("Light Level", "%.2f".format(data.lightLevel), "m", LightBlue, Modifier.weight(1f))
                HeroStat("Heavy Level", "%.2f".format(data.heavyLevel), "m", HeavyOrange, Modifier.weight(1f))
            }
            Spacer(Modifier.height(12.dp))
            Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                MiniStat("Temperature", "%.1f °C".format(data.temperatureC), Modifier.weight(1f))
                MiniStat("Current", "%.2f mA".format(data.currentMA), Modifier.weight(1f))
            }
            Spacer(Modifier.height(22.dp))

            Card {
                KvRow4("Echo Amp", data.echoAmp.toString(),
                       "Frequency", "%.0f kHz".format(data.freqMHz * 1000))
                Divider()
                KvRow4("Offset", "%.2f m".format(data.offset),
                       "Empty Distance", "%.2f m".format(data.emptyDistance))
                Divider()
                KvRow4("Dead Zone", "%.2f m".format(data.deadZone),
                       "Damping", data.damping.toString())
                Divider()
                KvRow4("Current 4mA", "%.2f m".format(data.set4mA),
                       "Current 20mA", "%.2f m".format(data.set20mA))
                Divider()
                KvRow4("Temperature", "%.1f °C".format(data.temperatureC),
                       "Current", "%.2f mA".format(data.currentMA))
            }
            Spacer(Modifier.height(22.dp))

            SectionLabel("Echo", LightBlue)
            Spacer(Modifier.height(10.dp))
            val thrLightStr = if (data.thrLightMode == 1) "%.1f V".format(data.thrLightSet / 10.0)
                              else "${data.thrLightSet} %"
            val thrHeavyStr = if (data.thrHeavyMode == 1) "%.1f V".format(data.thrHeavySet / 10.0)
                              else "${data.thrHeavySet} %"
            Card {
                KvRow("Thr.Light", thrLightStr)
                Divider()
                KvRow("Thr.Heavy", thrHeavyStr)
            }
            Spacer(Modifier.height(14.dp))
            WaveBlock("Real", LightBlue, data.realEcho)
            Spacer(Modifier.height(14.dp))
            WaveBlock("Average", HeavyOrange, data.avgEcho)
            Spacer(Modifier.height(8.dp))
        }

        // 하단 액션 바
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .shadow(8.dp)
                .background(AppColors.White)
                .padding(16.dp),
            horizontalArrangement = Arrangement.spacedBy(12.dp),
        ) {
            Button(
                onClick = onNew,
                modifier = Modifier.weight(1f).height(52.dp),
                colors = ButtonDefaults.buttonColors(containerColor = AppColors.Background),
                shape = RoundedCornerShape(14.dp),
            ) { Text("New", color = AppColors.SubText, fontWeight = FontWeight.W700, fontSize = 16.sp) }
            Button(
                onClick = onExportPdf,
                modifier = Modifier.weight(2f).height(52.dp),
                colors = ButtonDefaults.buttonColors(containerColor = ReportPurple),
                shape = RoundedCornerShape(14.dp),
            ) { Text("Export PDF", color = AppColors.White, fontWeight = FontWeight.W800, fontSize = 16.sp) }
        }
    }
}

@Composable
private fun HeaderCard(data: ReportData) {
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .shadow(6.dp, RoundedCornerShape(22.dp))
            .clip(RoundedCornerShape(22.dp))
            .background(Brush.linearGradient(listOf(Color(0xFF7C3AED), Color(0xFF3B82F6))))
            .padding(22.dp),
    ) {
        Column {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Box(Modifier.size(10.dp).clip(CircleShape).background(Color(0xFF4ADE80)))
                Spacer(Modifier.width(8.dp))
                Text(data.label, fontSize = 24.sp, fontWeight = FontWeight.W800, color = Color.White, letterSpacing = (-0.5).sp)
            }
            Spacer(Modifier.height(10.dp))
            Text("Sludge Level Meter  ·  FW ${data.firmwareVersion.ifEmpty { "—" }}", fontSize = 13.sp, color = Color.White.copy(alpha = 0.85f))
            Text(data.timestamp, fontSize = 13.sp, color = Color.White.copy(alpha = 0.75f))
        }
    }
}

@Composable
private fun SectionLabel(title: String, accent: Color) {
    Row(verticalAlignment = Alignment.CenterVertically) {
        Box(Modifier.size(width = 4.dp, height = 16.dp).clip(RoundedCornerShape(2.dp)).background(accent))
        Spacer(Modifier.width(8.dp))
        Text(title, fontSize = 17.sp, fontWeight = FontWeight.W800, color = AppColors.DarkText, letterSpacing = (-0.3).sp)
    }
}

@Composable
private fun HeroStat(label: String, value: String, unit: String, color: Color, modifier: Modifier = Modifier) {
    Column(
        modifier = modifier
            .shadow(3.dp, RoundedCornerShape(18.dp))
            .clip(RoundedCornerShape(18.dp))
            .background(AppColors.White)
            .padding(horizontal = 16.dp, vertical = 18.dp),
    ) {
        Text(label, fontSize = 13.sp, fontWeight = FontWeight.W700, color = color)
        Spacer(Modifier.height(8.dp))
        Row(verticalAlignment = Alignment.Bottom) {
            Text(value, fontSize = 30.sp, fontWeight = FontWeight.W800, color = AppColors.DarkText, letterSpacing = (-1).sp)
            Spacer(Modifier.width(3.dp))
            Text(unit, fontSize = 15.sp, fontWeight = FontWeight.W700, color = AppColors.GrayLabel, modifier = Modifier.padding(bottom = 4.dp))
        }
    }
}

@Composable
private fun MiniStat(label: String, value: String, modifier: Modifier = Modifier) {
    Row(
        modifier = modifier
            .shadow(2.dp, RoundedCornerShape(16.dp))
            .clip(RoundedCornerShape(16.dp))
            .background(AppColors.White)
            .padding(horizontal = 16.dp, vertical = 16.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically,
    ) {
        Text(label, fontSize = 13.sp, color = AppColors.SubText)
        Text(value, fontSize = 16.sp, fontWeight = FontWeight.W800, color = AppColors.DarkText)
    }
}

@Composable
private fun Card(content: @Composable ColumnScope.() -> Unit) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .shadow(3.dp, RoundedCornerShape(18.dp))
            .clip(RoundedCornerShape(18.dp))
            .background(AppColors.White)
            .padding(horizontal = 18.dp, vertical = 6.dp),
        content = content,
    )
}

@Composable
private fun Divider() {
    Box(Modifier.fillMaxWidth().height(1.dp).background(AppColors.Background))
}

@Composable
private fun GroupHeader(title: String) {
    Text(
        title,
        modifier = Modifier.padding(top = 14.dp, bottom = 4.dp),
        fontSize = 11.sp,
        fontWeight = FontWeight.W800,
        color = AppColors.GrayLabel,
        letterSpacing = 0.8.sp,
    )
}

@Composable
private fun TossGroupLabel(title: String) {
    Text(
        title,
        modifier = Modifier.padding(start = 4.dp, bottom = 6.dp),
        fontSize = 12.sp,
        fontWeight = FontWeight.W700,
        color = AppColors.GrayLabel,
        letterSpacing = 0.3.sp,
    )
}

@Composable
private fun KvRow4(k1: String, v1: String, k2: String?, v2: String?) {
    Row(
        modifier = Modifier.fillMaxWidth().padding(vertical = 13.dp),
        verticalAlignment = Alignment.CenterVertically,
    ) {
        Text(k1, modifier = Modifier.weight(1f), fontSize = 13.sp, color = AppColors.SubText)
        Text(v1, modifier = Modifier.weight(1f), fontSize = 14.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText,
            textAlign = androidx.compose.ui.text.style.TextAlign.End)
        Spacer(Modifier.width(16.dp))
        if (k2 != null && v2 != null) {
            Text(k2, modifier = Modifier.weight(1f), fontSize = 13.sp, color = AppColors.SubText)
            Text(v2, modifier = Modifier.weight(1f), fontSize = 14.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText,
                textAlign = androidx.compose.ui.text.style.TextAlign.End)
        } else {
            Spacer(Modifier.weight(2f))
        }
    }
}

@Composable
private fun KvRow(key: String, value: String) {
    Row(
        modifier = Modifier.fillMaxWidth().padding(vertical = 13.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically,
    ) {
        Text(key, fontSize = 14.sp, color = AppColors.SubText)
        Text(value, fontSize = 15.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
    }
}

@Composable
private fun WaveBlock(tag: String, accent: Color, reading: com.wws2.densitymeter.model.InterfaceEchoReading?) {
    Column(modifier = Modifier.fillMaxWidth()) {
        Box(
            Modifier.clip(RoundedCornerShape(999.dp)).background(accent.copy(alpha = 0.12f)).padding(horizontal = 10.dp, vertical = 4.dp),
        ) { Text(tag, fontSize = 12.sp, fontWeight = FontWeight.W800, color = accent) }
        Spacer(Modifier.height(8.dp))
        // InterfaceEchoChart 자체가 흰 카드라 별도 래퍼 없이 그대로 사용
        InterfaceEchoChart(reading = reading, modifier = Modifier.fillMaxWidth().height(420.dp))
    }
}
