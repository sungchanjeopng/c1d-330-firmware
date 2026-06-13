package com.wws2.densitymeter.ui.screen

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowLeft
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material.icons.automirrored.filled.ShowChart
import androidx.compose.material.icons.outlined.Edit
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import androidx.compose.foundation.border
import com.wws2.densitymeter.model.InterfaceEchoReading
import com.wws2.densitymeter.ui.component.DeviceStripBar
import com.wws2.densitymeter.ui.component.DialogActionButton
import com.wws2.densitymeter.ui.component.EchoChart
import com.wws2.densitymeter.ui.component.InterfaceEchoChart
import com.wws2.densitymeter.ui.component.StatRow
import com.wws2.densitymeter.ui.component.StepperTile
import com.wws2.densitymeter.ui.theme.AppColors
import com.wws2.densitymeter.ui.theme.isTablet
import com.wws2.densitymeter.ui.theme.isWideLayout
import com.wws2.densitymeter.viewmodel.DeviceType
import com.wws2.densitymeter.viewmodel.EchoMode
import com.wws2.densitymeter.model.DensityUnit
import com.wws2.densitymeter.viewmodel.MainViewModel

private val OrangeColor = androidx.compose.ui.graphics.Color(0xFFFFA500)
private val DzEmptyColor = androidx.compose.ui.graphics.Color(0xFF3182F6)

@Composable
fun EchoTabScreen(vm: MainViewModel) {
    val state by vm.state.collectAsStateWithLifecycle()
    val densUnit = DensityUnit.fromInt(state.densUnit)
    val devices = state.connectedDevices
    val isInterface = state.deviceType == DeviceType.INTERFACE

    if (devices.isEmpty()) {
        EmptyTabState(
            icon = { Icon(Icons.AutoMirrored.Filled.ShowChart, contentDescription = null, modifier = Modifier.size(56.dp), tint = AppColors.WeakText) },
            title = "Echo", desc = "", onOpenPairing = { vm.openPairing() }
        )
    } else if (isInterface) {
        // ─── 계면계 Echo 화면 ───
        val ifReading = state.interfaceEchoReading
        val wide = isWideLayout
        val tablet = isTablet
        Column(modifier = Modifier.fillMaxSize().padding(12.dp)) {
            DeviceStripBar(devices = devices, selectedDeviceId = state.activeDeviceId, reconnectingIds = state.reconnectingIds,
                onDeviceTap = { vm.requestConnectDevice(it) }, onMoreTap = { vm.openPairing() })
            Spacer(Modifier.height(8.dp))

            if (tablet) {
                // 태블릿: 차트 위 + 컨트롤/카드 아래, 스크롤 없이 한 화면
                EchoModeToggle(currentMode = state.echoMode, onModeChange = { vm.setEchoMode(it) })
                Spacer(Modifier.height(8.dp))
                InterfaceEchoInfoRow(ifReading, vm)
                Spacer(Modifier.height(4.dp))
                InterfaceEchoChart(reading = ifReading, modifier = Modifier.weight(1f))
                Spacer(Modifier.height(8.dp))
                InterfaceLevelCards(ifReading)
            } else if (wide) {
                // 폰 가로: 차트 크게 → 스크롤하면 정보
                val chartH = (LocalConfiguration.current.screenHeightDp * 0.65f).dp
                Column(modifier = Modifier.weight(1f).verticalScroll(rememberScrollState())) {
                    EchoModeToggle(currentMode = state.echoMode, onModeChange = { vm.setEchoMode(it) })
                    Spacer(Modifier.height(8.dp))
                    InterfaceEchoInfoRow(ifReading, vm)
                    Spacer(Modifier.height(4.dp))
                    InterfaceEchoChart(reading = ifReading, modifier = Modifier.fillMaxWidth().height(chartH))
                    Spacer(Modifier.height(8.dp))
                    InterfaceLevelCards(ifReading)
                }
            } else {
                // 세로: 기존 레이아웃
                EchoModeToggle(currentMode = state.echoMode, onModeChange = { vm.setEchoMode(it) })
                Spacer(Modifier.height(8.dp))
                InterfaceEchoInfoRow(ifReading, vm)
                Spacer(Modifier.height(4.dp))
                InterfaceEchoChart(reading = ifReading, modifier = Modifier.weight(1f))
                Spacer(Modifier.height(8.dp))
                InterfaceLevelCards(ifReading)
            }
        }
    } else {
        // ─── 농도계 Echo 화면 ───
        val reading = state.echoReading
        val wide = isWideLayout
        val tablet = isTablet
        Column(modifier = Modifier.fillMaxSize().padding(12.dp)) {
            DeviceStripBar(devices = devices, selectedDeviceId = state.activeDeviceId, reconnectingIds = state.reconnectingIds,
                onDeviceTap = { vm.requestConnectDevice(it) }, onMoreTap = { vm.openPairing() })
            Spacer(Modifier.height(8.dp))

            if (tablet) {
                EchoChart(echoReading = reading, isInterface = false, modifier = Modifier.weight(1f))
                Spacer(Modifier.height(8.dp))
                StatRow(listOf(
                    Triple("EEA.R", reading?.eeaR?.toString() ?: "--", AppColors.Primary),
                    Triple("EEA.D", reading?.eeaD?.toString() ?: "--", AppColors.DarkText),
                    Triple("Density(${densUnit.unitStr})", reading?.let { densUnit.format(it.level) } ?: "--", AppColors.Primary),
                ))
            } else if (wide) {
                val chartH = (LocalConfiguration.current.screenHeightDp * 0.65f).dp
                Column(modifier = Modifier.weight(1f).verticalScroll(rememberScrollState())) {
                    EchoChart(echoReading = reading, isInterface = false, modifier = Modifier.fillMaxWidth().height(chartH))
                    Spacer(Modifier.height(8.dp))
                    StatRow(listOf(
                        Triple("EEA.R", reading?.eeaR?.toString() ?: "--", AppColors.Primary),
                        Triple("EEA.D", reading?.eeaD?.toString() ?: "--", AppColors.DarkText),
                        Triple("Density(${densUnit.unitStr})", reading?.let { densUnit.format(it.level) } ?: "--", AppColors.Primary),
                    ))
                }
            } else {
                EchoChart(echoReading = reading, isInterface = false, modifier = Modifier.weight(1f))
                Spacer(Modifier.height(8.dp))
                StatRow(listOf(
                    Triple("EEA.R", reading?.eeaR?.toString() ?: "--", AppColors.Primary),
                    Triple("EEA.D", reading?.eeaD?.toString() ?: "--", AppColors.DarkText),
                    Triple("Density(${densUnit.unitStr})", reading?.let { densUnit.format(it.level) } ?: "--", AppColors.Primary),
                ))
            }
        }
    }
}

@Composable
private fun EchoModeToggle(currentMode: EchoMode, onModeChange: (EchoMode) -> Unit) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .shadow(2.dp, RoundedCornerShape(12.dp))
            .background(AppColors.White, RoundedCornerShape(12.dp))
            .padding(4.dp),
    ) {
        val modes = listOf(EchoMode.REAL to "Real", EchoMode.AVG to "Avg")
        modes.forEach { (mode, label) ->
            val isSelected = currentMode == mode
            Box(
                modifier = Modifier
                    .weight(1f)
                    .clip(RoundedCornerShape(10.dp))
                    .background(if (isSelected) AppColors.Primary else AppColors.White)
                    .clickable { onModeChange(mode) }
                    .padding(vertical = 10.dp),
                contentAlignment = Alignment.Center,
            ) {
                Text(
                    label,
                    fontSize = 16.sp,
                    fontWeight = FontWeight.W700,
                    color = if (isSelected) AppColors.White else AppColors.GrayLabel,
                )
            }
        }
    }
}

@Composable
private fun InterfaceEchoInfoRow(ifReading: InterfaceEchoReading?, vm: MainViewModel) {
    var edit by remember { mutableStateOf<EchoEdit?>(null) }
    // 항목 5개라 좁음 → 좌우 스크롤. 양끝 페이드+화살표로 스크롤 가능함을 표시
    val rowScroll = rememberScrollState()
    Box(modifier = Modifier.fillMaxWidth()) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .horizontalScroll(rowScroll)
            .padding(horizontal = 4.dp),
        horizontalArrangement = Arrangement.spacedBy(20.dp),
        verticalAlignment = Alignment.CenterVertically,
    ) {
        EditableEchoInfo(
            text = "Thr.Light  ${ifReading?.let { if (it.thrLightMode == 1) "%.1fV".format(it.thrLightSet / 10.0) else "${it.thrLightSet}%" } ?: "--"}",
            color = AppColors.GrayLabel,
            textAlign = TextAlign.Start,
            onClick = {
                if (ifReading != null) {
                    edit = if (ifReading.thrLightMode == 1) EchoEdit("Thr.Light Manual", 4, ifReading.thrLightSet, 0, 32, 1) { "%.1fV".format(it / 10.0) }
                    else EchoEdit("Thr.Light Auto", 2, ifReading.thrLightSet, 0, 95, 5) { "$it%" }
                }
            },
        )
        EditableEchoInfo(
            text = "Thr.Heavy  ${ifReading?.let { if (it.thrHeavyMode == 1) "%.1fV".format(it.thrHeavySet / 10.0) else "${it.thrHeavySet}%" } ?: "--"}",
            color = OrangeColor,
            textAlign = TextAlign.Start,
            onClick = {
                if (ifReading != null) {
                    edit = if (ifReading.thrHeavyMode == 1) EchoEdit("Thr.Heavy Manual", 5, ifReading.thrHeavySet, 0, 32, 1) { "%.1fV".format(it / 10.0) }
                    else EchoEdit("Thr.Heavy Auto", 3, ifReading.thrHeavySet, 0, 95, 5) { "$it%" }
                }
            },
        )
        EditableEchoInfo(
            text = "Echo Amp  ${ifReading?.echoAmp?.toString() ?: "--"}",
            color = AppColors.Primary,
            textAlign = TextAlign.Start,
            onClick = {
                edit = EchoEdit("Echo Amp", 1, ifReading?.echoAmp ?: 15, 1, 50, 1) { it.toString() }
            },
        )
        EditableEchoInfo(
            text = "Empty  ${ifReading?.let { "%.2fm".format(it.empty * 0.01) } ?: "--"}",
            color = DzEmptyColor,
            textAlign = TextAlign.Start,
            onClick = {
                if (ifReading != null) {
                    edit = EchoEdit("Empty", 12, ifReading.empty, 1, 1000, 1, decimalScale = 100) { "%.2f m".format(it / 100.0) }
                }
            },
        )
        EditableEchoInfo(
            text = "Dead Zone  ${ifReading?.let { "%.2fm".format(it.deadzone * 0.01) } ?: "--"}",
            color = DzEmptyColor,
            textAlign = TextAlign.Start,
            onClick = {
                if (ifReading != null) {
                    edit = EchoEdit("Dead Zone", 13, ifReading.deadzone, 35, 1000, 1, decimalScale = 100) { "%.2f m".format(it / 100.0) }
                }
            },
        )
    }
    if (rowScroll.canScrollBackward) {
        Row(
            modifier = Modifier.matchParentSize(),
            horizontalArrangement = Arrangement.Start,
            verticalAlignment = Alignment.CenterVertically,
        ) {
            Box(
                Modifier.fillMaxHeight().background(AppColors.Background),
                contentAlignment = Alignment.Center,
            ) {
                Icon(Icons.AutoMirrored.Filled.KeyboardArrowLeft, contentDescription = null,
                    modifier = Modifier.size(18.dp), tint = AppColors.GrayLabel)
            }
            Box(
                Modifier.width(24.dp).fillMaxHeight().background(
                    Brush.horizontalGradient(listOf(AppColors.Background, AppColors.Background.copy(alpha = 0f)))
                )
            )
        }
    }
    if (rowScroll.canScrollForward) {
        Row(
            modifier = Modifier.matchParentSize(),
            horizontalArrangement = Arrangement.End,
            verticalAlignment = Alignment.CenterVertically,
        ) {
            Box(
                Modifier.width(24.dp).fillMaxHeight().background(
                    Brush.horizontalGradient(listOf(AppColors.Background.copy(alpha = 0f), AppColors.Background))
                )
            )
            Box(
                Modifier.fillMaxHeight().background(AppColors.Background),
                contentAlignment = Alignment.Center,
            ) {
                Icon(Icons.AutoMirrored.Filled.KeyboardArrowRight, contentDescription = null,
                    modifier = Modifier.size(18.dp), tint = AppColors.GrayLabel)
            }
        }
    }
    }
    edit?.let { cfg ->
        EchoEditDialog(
            config = cfg,
            onDismiss = { edit = null },
            onApply = { value -> vm.sendAppSetting(cfg.cmd, value) },
        )
    }
}

private enum class EchoSendingState { IDLE, SENDING, DONE, FAILED }

@Composable
private fun EditableEchoInfo(
    text: String,
    color: androidx.compose.ui.graphics.Color,
    textAlign: TextAlign,
    modifier: Modifier = Modifier,
    onClick: () -> Unit,
) {
    Box(
        modifier = modifier.clickable(onClick = onClick),
        contentAlignment = when (textAlign) {
            TextAlign.Center -> Alignment.Center
            TextAlign.End -> Alignment.CenterEnd
            else -> Alignment.CenterStart
        },
    ) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text(
                text,
                fontSize = 15.sp,
                fontWeight = FontWeight.W700,
                color = color,
                textAlign = textAlign,
            )
            Spacer(Modifier.width(3.dp))
            Icon(
                imageVector = Icons.Outlined.Edit,
                contentDescription = "Edit",
                modifier = Modifier.size(12.dp),
                tint = AppColors.WeakText,
            )
        }
    }
}

private data class EchoEdit(
    val title: String,
    val cmd: Int,
    val value: Int,
    val min: Int,
    val max: Int,
    val step: Int,
    val decimalScale: Int = 1,
    val formatter: (Int) -> String,
)

@Composable
private fun EchoEditDialog(config: EchoEdit, onDismiss: () -> Unit, onApply: suspend (Int) -> Boolean) {
    // 소수 입력 지원 (decimalScale=100이면 raw 1000 → "10" + "00") — ConfigEditDialog 미러
    fun integerText(raw: Int): String {
        if (config.decimalScale <= 1) return raw.toString()
        val absRaw = kotlin.math.abs(raw)
        val sign = if (raw < 0) "-" else ""
        return sign + (absRaw / config.decimalScale).toString()
    }
    fun fractionText(raw: Int): String {
        val absRaw = kotlin.math.abs(raw)
        return (absRaw % config.decimalScale).toString().padStart(2, '0')
    }
    fun parseRaw(intInput: String, fracInput: String): Int? {
        if (config.decimalScale <= 1) return intInput.toIntOrNull()
        val intPart = intInput.toIntOrNull() ?: return null
        val fracPart = fracInput.toIntOrNull() ?: return null
        if (fracPart !in 0 until config.decimalScale) return null
        val negative = intInput.trim().startsWith("-")
        val rawAbs = kotlin.math.abs(intPart) * config.decimalScale + fracPart
        return if (negative) -rawAbs else rawAbs
    }

    var value by remember(config) { mutableIntStateOf(config.value.coerceIn(config.min, config.max)) }
    var intText by remember(config) { mutableStateOf(TextFieldValue(integerText(config.value.coerceIn(config.min, config.max)), selection = TextRange(integerText(config.value.coerceIn(config.min, config.max)).length))) }
    var fracText by remember(config) { mutableStateOf(TextFieldValue(fractionText(config.value.coerceIn(config.min, config.max)), selection = TextRange(fractionText(config.value.coerceIn(config.min, config.max)).length))) }
    val parsed = parseRaw(intText.text, fracText.text)
    val validValue = parsed?.takeIf { it in config.min..config.max }

    var sendingState by remember(config) { mutableStateOf(EchoSendingState.IDLE) }
    val scope = rememberCoroutineScope()

    fun setValue(newValue: Int) {
        value = newValue.coerceIn(config.min, config.max)
        intText = TextFieldValue(integerText(value), selection = TextRange(integerText(value).length))
        fracText = TextFieldValue(fractionText(value), selection = TextRange(fractionText(value).length))
    }

    // Design mirrors iOS EchoEditSheet (EchoTabScreen.swift:227-345): rounded
    // primary-tinted +/- tiles, centered value, rounded-border field (red when
    // invalid), full-width Cancel/Apply buttons.
    Dialog(onDismissRequest = { if (sendingState != EchoSendingState.SENDING) onDismiss() }) {
        Column(
            modifier = Modifier
                .clip(RoundedCornerShape(16.dp))
                .background(AppColors.White)
                .padding(20.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
        ) {
            Text(
                config.title,
                fontSize = 20.sp,
                fontWeight = FontWeight.W700,
                color = AppColors.DarkText,
            )

            Spacer(Modifier.height(18.dp))

            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically,
            ) {
                StepperTile("-") { setValue((validValue ?: value) - config.step) }
                Text(
                    config.formatter(validValue ?: value),
                    fontSize = 24.sp,
                    fontWeight = FontWeight.W700,
                    color = AppColors.DarkText,
                )
                StepperTile("+") { setValue((validValue ?: value) + config.step) }
            }

            Spacer(Modifier.height(18.dp))

            if (config.decimalScale > 1) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.Center,
                    verticalAlignment = Alignment.Bottom,
                ) {
                    OutlinedTextField(
                        value = intText,
                        onValueChange = { input ->
                            val filtered = input.text.filterIndexed { index, ch -> ch.isDigit() || (ch == '-' && index == 0 && config.min < 0) }
                            intText = TextFieldValue(filtered, selection = TextRange(filtered.length))
                        },
                        modifier = Modifier.width(100.dp),
                        singleLine = true,
                        shape = RoundedCornerShape(8.dp),
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        textStyle = LocalTextStyle.current.copy(textAlign = TextAlign.End, fontSize = 20.sp),
                        colors = OutlinedTextFieldDefaults.colors(cursorColor = androidx.compose.ui.graphics.Color.Transparent),
                        isError = validValue == null,
                    )
                    Text(
                        ".",
                        modifier = Modifier.padding(start = 6.dp, end = 6.dp, bottom = 8.dp),
                        fontSize = 24.sp,
                        fontWeight = FontWeight.W700,
                        color = AppColors.DarkText,
                    )
                    OutlinedTextField(
                        value = fracText,
                        onValueChange = { input ->
                            val filtered = input.text.filter { it.isDigit() }.take(2)
                            fracText = TextFieldValue(filtered, selection = TextRange(filtered.length))
                        },
                        modifier = Modifier.width(80.dp),
                        singleLine = true,
                        shape = RoundedCornerShape(8.dp),
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        textStyle = LocalTextStyle.current.copy(textAlign = TextAlign.Start, fontSize = 20.sp),
                        colors = OutlinedTextFieldDefaults.colors(cursorColor = androidx.compose.ui.graphics.Color.Transparent),
                        isError = validValue == null,
                    )
                }
                Spacer(Modifier.height(4.dp))
                Text("Range ${config.formatter(config.min)} ~ ${config.formatter(config.max)}", fontSize = 12.sp, color = AppColors.GrayLabel)
            } else {
                OutlinedTextField(
                    value = intText,
                    onValueChange = { input ->
                        val filtered = input.text.filterIndexed { index, ch -> ch.isDigit() || (ch == '-' && index == 0 && config.min < 0) }
                        intText = TextFieldValue(filtered, selection = TextRange(filtered.length))
                    },
                    modifier = Modifier.fillMaxWidth(),
                    singleLine = true,
                    shape = RoundedCornerShape(8.dp),
                    keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                    label = { Text("Value") },
                    supportingText = { Text("Range ${config.min} ~ ${config.max} / ${validValue?.let(config.formatter) ?: "Invalid"}") },
                    colors = OutlinedTextFieldDefaults.colors(cursorColor = androidx.compose.ui.graphics.Color.Transparent),
                    isError = validValue == null,
                )
            }

            Spacer(Modifier.height(18.dp))

            when (sendingState) {
                EchoSendingState.IDLE -> {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(12.dp),
                    ) {
                        DialogActionButton(
                            text = "Cancel",
                            background = AppColors.LightGray,
                            contentColor = AppColors.DarkText,
                            modifier = Modifier.weight(1f),
                            onClick = onDismiss,
                        )
                        DialogActionButton(
                            text = "Apply",
                            background = if (validValue == null) AppColors.WeakText else AppColors.Primary,
                            contentColor = AppColors.White,
                            enabled = validValue != null,
                            modifier = Modifier.weight(1f),
                            onClick = {
                                validValue?.let { v ->
                                    scope.launch {
                                        sendingState = EchoSendingState.SENDING
                                        val ok = onApply(v)
                                        sendingState = if (ok) EchoSendingState.DONE else EchoSendingState.FAILED
                                        if (ok) {
                                            delay(800)
                                            onDismiss()
                                        }
                                    }
                                }
                            },
                        )
                    }
                }
                EchoSendingState.SENDING -> {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.Center,
                    ) {
                        CircularProgressIndicator(
                            modifier = Modifier.size(20.dp),
                            strokeWidth = 2.dp,
                            color = AppColors.Primary,
                        )
                        Spacer(Modifier.width(12.dp))
                        Text("Sending...", fontSize = 16.sp, fontWeight = FontWeight.W600, color = AppColors.DarkText)
                    }
                }
                EchoSendingState.DONE -> {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.Center,
                    ) {
                        Text("✓", fontSize = 22.sp, fontWeight = FontWeight.W700, color = AppColors.Success)
                        Spacer(Modifier.width(8.dp))
                        Text("Success", fontSize = 16.sp, fontWeight = FontWeight.W600, color = AppColors.Success)
                    }
                }
                EchoSendingState.FAILED -> {
                    Column(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalAlignment = Alignment.CenterHorizontally,
                    ) {
                        Text("✗ Failed", fontSize = 16.sp, fontWeight = FontWeight.W700, color = androidx.compose.ui.graphics.Color(0xFFD0342C))
                        Spacer(Modifier.height(12.dp))
                        DialogActionButton(
                            text = "Close",
                            background = AppColors.LightGray,
                            contentColor = AppColors.DarkText,
                            modifier = Modifier.fillMaxWidth(),
                            onClick = onDismiss,
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun InterfaceLevelCards(ifReading: InterfaceEchoReading?) {
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
        Column(
            modifier = Modifier.weight(1f)
                .shadow(2.dp, RoundedCornerShape(12.dp))
                .background(AppColors.White, RoundedCornerShape(12.dp))
                .padding(10.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
        ) {
            Text("Light Level", fontSize = 14.sp, fontWeight = FontWeight.W700, color = AppColors.GrayLabel)
            Spacer(Modifier.height(2.dp))
            Text(ifReading?.let { "%.2f m".format(it.lightLevel) } ?: "--", fontSize = 22.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
        }
        Column(
            modifier = Modifier.weight(1f)
                .shadow(2.dp, RoundedCornerShape(12.dp))
                .background(AppColors.White, RoundedCornerShape(12.dp))
                .padding(10.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
        ) {
            Text("Heavy Level", fontSize = 14.sp, fontWeight = FontWeight.W700, color = OrangeColor)
            Spacer(Modifier.height(2.dp))
            Text(ifReading?.let { "%.2f m".format(it.heavyLevel) } ?: "--", fontSize = 22.sp, fontWeight = FontWeight.W700, color = OrangeColor)
        }
    }
}

@Composable
fun EmptyTabState(icon: @Composable () -> Unit, title: String, desc: String, onOpenPairing: (() -> Unit)? = null) {
    // 페어링 미연결 상태: "No device connected" + Open Pairing 버튼
    Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
        if (onOpenPairing != null) {
            Column(
                modifier = Modifier.fillMaxWidth().padding(horizontal = 28.dp),
                horizontalAlignment = Alignment.CenterHorizontally,
            ) {
                Text(
                    "No device connected",
                    fontSize = 16.sp,
                    fontWeight = FontWeight.W600,
                    color = AppColors.GrayLabel,
                )
                Spacer(Modifier.height(14.dp))
                androidx.compose.material3.Button(
                    onClick = onOpenPairing,
                    modifier = Modifier.fillMaxWidth(),
                    colors = androidx.compose.material3.ButtonDefaults.buttonColors(containerColor = AppColors.Primary),
                    shape = RoundedCornerShape(14.dp),
                    contentPadding = androidx.compose.foundation.layout.PaddingValues(vertical = 16.dp),
                ) {
                    Text("Open Pairing", fontSize = 18.sp, fontWeight = FontWeight.W700)
                }
            }
        }
    }
}
