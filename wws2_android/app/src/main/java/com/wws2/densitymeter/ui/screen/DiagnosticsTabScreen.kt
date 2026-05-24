package com.wws2.densitymeter.ui.screen

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.HealthAndSafety
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.wws2.densitymeter.model.TemperatureUnit
import com.wws2.densitymeter.ui.component.DeviceStripBar
import com.wws2.densitymeter.ui.component.DiagRow
import com.wws2.densitymeter.ui.component.SettingsPanel
import com.wws2.densitymeter.ui.component.StatusInfoPanel
import com.wws2.densitymeter.ui.theme.AppColors
import com.wws2.densitymeter.viewmodel.DeviceType
import com.wws2.densitymeter.viewmodel.MainUiState
import com.wws2.densitymeter.viewmodel.MainViewModel
import kotlin.math.roundToInt

private data class ConfigEdit(
    val title: String,
    val cmd: Int,
    val value: Int,
    val min: Int,
    val max: Int,
    val step: Int,
    val formatter: (Int) -> String,
)

@Composable
fun DiagnosticsTabScreen(vm: MainViewModel) {
    val state by vm.state.collectAsStateWithLifecycle()
    val devices = state.connectedDevices
    var edit by remember { mutableStateOf<ConfigEdit?>(null) }

    if (devices.isEmpty()) {
        EmptyTabState(
            icon = { Icon(Icons.Outlined.HealthAndSafety, contentDescription = null, modifier = Modifier.size(56.dp), tint = AppColors.WeakText) },
            title = "Parameter", desc = "", onOpenPairing = { vm.openPairing() },
        )
    } else {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .verticalScroll(rememberScrollState())
                .padding(12.dp),
        ) {
            DeviceStripBar(devices = devices, selectedDeviceId = state.activeDeviceId,
                onDeviceTap = { vm.requestConnectDevice(it) }, onMoreTap = { vm.openPairing() })
            Spacer(Modifier.height(8.dp))

            if (state.deviceType == DeviceType.INTERFACE) {
                InterfaceParametersPanel(state, onEdit = { edit = it })
                Spacer(Modifier.height(8.dp))
                InterfaceStatusPanel(state)
            } else {
                SettingsPanel(
                    deviceLabel = if (vm.isConnected) state.activeDeviceLabel else "--",
                    damping = state.damping, set4mA = state.set4mA, set20mA = state.set20mA,
                    pipeDia = state.pipeDia, freqMHz = state.freqMHz,
                    densUnit = state.densUnit,
                )
                Spacer(Modifier.height(8.dp))
                StatusInfoPanel(
                    currentMA = state.currentMA,
                    temperature = state.temperatureC,
                    tempUnit = state.tempUnit,
                    relay = state.relay,
                    extIn1En = state.extIn1En,
                    extIn1State = state.extIn1State,
                    extIn2En = state.extIn2En,
                    extIn2State = state.extIn2State,
                )
            }
        }
    }

    edit?.let { cfg ->
        ConfigEditDialog(
            config = cfg,
            onDismiss = { edit = null },
            onApply = { value ->
                vm.sendAppSetting(cfg.cmd, value)
                edit = null
            },
        )
    }
}

@Composable
private fun InterfaceParametersPanel(state: MainUiState, onEdit: (ConfigEdit) -> Unit) {
    val echo = state.interfaceEchoReading
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .shadow(4.dp, RoundedCornerShape(16.dp))
            .background(AppColors.White, RoundedCornerShape(16.dp))
    ) {
        Text("CONFIGURATION", modifier = Modifier.padding(start = 16.dp, top = 14.dp, bottom = 6.dp),
            fontSize = 18.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
        EditableDiagRow("Freq", "%d kHz".format((state.freqMHz * 1000).roundToInt())) {
            val current = when ((state.freqMHz * 1000).roundToInt()) { 380 -> 0; 270 -> 1; 160 -> 2; 130 -> 3; else -> 0 }
            onEdit(ConfigEdit("Frequency", 6, current, 0, 3, 1) { v -> when (v) { 0 -> "380 kHz"; 1 -> "270 kHz"; 2 -> "160 kHz"; 3 -> "130 kHz"; else -> "--" } })
        }
        EditableDiagRow("Offset", "%.2f m".format(state.offset)) {
            onEdit(ConfigEdit("Offset", 7, (state.offset * 100).roundToInt(), -100, 100, 1) { v -> "%.2f m".format(v / 100.0) })
        }
        EditableDiagRow("4mA Set", "%.2f".format(state.set4mA)) {
            onEdit(ConfigEdit("4mA Set", 8, (state.set4mA * 100).roundToInt(), 0, 1000, 1) { v -> "%.2f".format(v / 100.0) })
        }
        EditableDiagRow("20mA Set", "%.2f".format(state.set20mA)) {
            onEdit(ConfigEdit("20mA Set", 9, (state.set20mA * 100).roundToInt(), 0, 1000, 1) { v -> "%.2f".format(v / 100.0) })
        }
        EditableDiagRow("TVG", state.tvg.toString()) {
            onEdit(ConfigEdit("TVG", 10, state.tvg, 0, 7, 1) { it.toString() })
        }
        EditableDiagRow("Damping", state.damping.toString()) {
            onEdit(ConfigEdit("Damping", 11, state.damping, 1, 100, 1) { it.toString() })
        }
        EditableDiagRow("Echo Amp", echo?.echoAmp?.toString() ?: "--") {
            onEdit(ConfigEdit("Echo Amp", 1, echo?.echoAmp ?: 15, 1, 50, 1) { it.toString() })
        }
        val lightAuto = echo?.thrLightMode != 1
        EditableDiagRow("Thr.Light", echo?.let { if (lightAuto) "${it.thrLightSet}% Auto" else "%.1fV Manual".format(it.thrLightSet / 10.0) } ?: "--") {
            if (lightAuto) onEdit(ConfigEdit("Thr.Light Auto", 2, echo?.thrLightSet ?: 50, 0, 95, 5) { "$it%" })
            else onEdit(ConfigEdit("Thr.Light Manual", 4, echo?.thrLightSet ?: 16, 0, 32, 1) { "%.1fV".format(it / 10.0) })
        }
        val heavyAuto = echo?.thrHeavyMode != 1
        EditableDiagRow("Thr.Heavy", echo?.let { if (heavyAuto) "${it.thrHeavySet}% Auto" else "%.1fV Manual".format(it.thrHeavySet / 10.0) } ?: "--") {
            if (heavyAuto) onEdit(ConfigEdit("Thr.Heavy Auto", 3, echo?.thrHeavySet ?: 50, 0, 95, 5) { "$it%" })
            else onEdit(ConfigEdit("Thr.Heavy Manual", 5, echo?.thrHeavySet ?: 16, 0, 32, 1) { "%.1fV".format(it / 10.0) })
        }
    }
}

@Composable
private fun EditableDiagRow(label: String, value: String, onClick: () -> Unit) {
    Row(
        modifier = Modifier.fillMaxWidth().clickable(onClick = onClick).padding(horizontal = 16.dp, vertical = 12.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically,
    ) {
        Text(label, fontSize = 19.sp, color = AppColors.GrayLabel)
        Text(value, fontSize = 20.sp, fontWeight = FontWeight.W600, color = AppColors.Primary)
    }
}

@Composable
private fun ConfigEditDialog(config: ConfigEdit, onDismiss: () -> Unit, onApply: (Int) -> Unit) {
    var value by remember(config) { mutableIntStateOf(config.value.coerceIn(config.min, config.max)) }
    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text(config.title) },
        text = {
            Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween, verticalAlignment = Alignment.CenterVertically) {
                Button(onClick = { value = (value - config.step).coerceAtLeast(config.min) }) { Text("-") }
                Text(config.formatter(value), fontSize = 24.sp, fontWeight = FontWeight.W700)
                Button(onClick = { value = (value + config.step).coerceAtMost(config.max) }) { Text("+") }
            }
        },
        confirmButton = { TextButton(onClick = { onApply(value) }) { Text("Apply") } },
        dismissButton = { TextButton(onClick = onDismiss) { Text("Cancel") } },
    )
}

@Composable
private fun InterfaceStatusPanel(state: MainUiState) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .shadow(4.dp, RoundedCornerShape(16.dp))
            .background(AppColors.White, RoundedCornerShape(16.dp))
    ) {
        Text("Status", modifier = Modifier.padding(start = 16.dp, top = 14.dp, bottom = 6.dp),
            fontSize = 18.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
        val tUnit = TemperatureUnit.fromInt(state.tempUnit)
        DiagRow("Temperature", "${tUnit.format(state.temperatureC)} ${tUnit.unitStr}")
        DiagRow("Current", "%.2f mA".format(state.currentMA))
        DiagRow("Relay", if (state.relay == 1) "Act" else "Stop")
    }
}
