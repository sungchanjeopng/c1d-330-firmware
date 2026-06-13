package com.wws2.densitymeter.ui.screen

import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.graphics.Bitmap
import android.widget.Toast
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Delete
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.drawWithContent
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.asAndroidBitmap
import androidx.compose.ui.graphics.layer.drawLayer
import androidx.compose.ui.graphics.rememberGraphicsLayer
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.wws2.densitymeter.domain.ReportSnapshotStore
import com.wws2.densitymeter.ui.component.DialogActionButton
import com.wws2.densitymeter.ui.component.TitleInputDialog
import com.wws2.densitymeter.model.ReportData
import com.wws2.densitymeter.model.ReportStage
import com.wws2.densitymeter.ui.component.InterfaceEchoChart
import com.wws2.densitymeter.ui.theme.AppColors
import androidx.core.content.FileProvider
import com.wws2.densitymeter.viewmodel.MainViewModel
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

private val ReportPurple = Color(0xFF7C3AED)
private val LightBlue = Color(0xFF3182F6)
private val HeavyOrange = Color(0xFFFF8C00)

@Composable
fun ReportScreen(vm: MainViewModel, onExportPdf: () -> Unit, onExportWord: () -> Unit, onExportImage: () -> Unit, onExportCsv: () -> Unit) {
    val state by vm.state.collectAsStateWithLifecycle()

    Box(modifier = Modifier.fillMaxSize().background(AppColors.Background)) {
        when (state.reportStage) {
            ReportStage.SELECT -> ReportDeviceSelect(vm)
            ReportStage.COLLECTING -> ReportCollecting()
            ReportStage.ERROR -> ReportError(state.reportError, onRetry = { vm.backToReportSelect() })
            ReportStage.DONE -> {
                val data = state.reportData
                if (data == null) ReportError("No report data", onRetry = { vm.backToReportSelect() })
                else ReportResult(data, onExportPdf = onExportPdf, onExportWord = onExportWord,
                    onExportImage = onExportImage, onExportCsv = onExportCsv,
                    onNew = { vm.backToReportSelect() },
                    onSaveComment = { vm.updateReportComment(it) })
            }
        }
    }
}

// ───────────────────── 기기 선택 ─────────────────────
@Composable
private fun ReportDeviceSelect(vm: MainViewModel) {
    val state by vm.state.collectAsStateWithLifecycle()
    val devices = state.connectedDevices.filter { it.label.startsWith("ENV130") }
    var titlePromptFor by remember { mutableStateOf<String?>(null) }

    titlePromptFor?.let { deviceId ->
        TitleInputDialog(
            heading = "Report Title",
            confirmText = "Create",
            onDismiss = { titlePromptFor = null },
            onConfirm = { title ->
                titlePromptFor = null
                vm.selectReportDevice(deviceId, title)
            },
        )
    }

    Column(modifier = Modifier.fillMaxSize().padding(20.dp)) {
        Text("Report", fontSize = 26.sp, fontWeight = FontWeight.W800, color = AppColors.DarkText, letterSpacing = (-0.5).sp)
        Spacer(Modifier.height(4.dp))
        Text("Pick a device to capture its current state.", fontSize = 14.sp, color = AppColors.GrayLabel)
        Spacer(Modifier.height(20.dp))

        if (devices.isEmpty()) {
            Box(modifier = Modifier.fillMaxWidth().padding(top = 20.dp, bottom = 20.dp), contentAlignment = Alignment.Center) {
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
                        onClick = { titlePromptFor = device.id },
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

        SavedReportsSection(vm)
    }
}

// ───────────────────── 저장된 리포트 목록 (스냅샷 → ReportResult 복원) ─────────────────────
@Composable
private fun ColumnScope.SavedReportsSection(vm: MainViewModel) {
    val context = LocalContext.current
    var savedReports by remember { mutableStateOf(ReportSnapshotStore.list(context)) }

    if (savedReports.isEmpty()) return

    Spacer(Modifier.height(16.dp))
    Text("Saved Reports", fontSize = 17.sp, fontWeight = FontWeight.W800, color = AppColors.DarkText)
    Spacer(Modifier.height(10.dp))
    Column(
        modifier = Modifier.weight(1f, fill = false).verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.spacedBy(10.dp),
    ) {
        savedReports.forEach { file ->
            SavedReportRow(
                file = file,
                onOpen = { vm.openReportSnapshot(file) },
                onDelete = {
                    file.delete()
                    savedReports = ReportSnapshotStore.list(context)
                },
            )
        }
        Spacer(Modifier.height(8.dp))
    }
}

@Composable
private fun SavedReportRow(file: File, onOpen: () -> Unit, onDelete: () -> Unit) {
    val meta = remember(file) {
        SimpleDateFormat("yyyy-MM-dd HH:mm", Locale.getDefault()).format(Date(file.lastModified()))
    }
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .shadow(2.dp, RoundedCornerShape(14.dp))
            .clip(RoundedCornerShape(14.dp))
            .background(AppColors.White)
            .clickable(onClick = onOpen)
            .padding(start = 14.dp, end = 4.dp, top = 10.dp, bottom = 10.dp),
        verticalAlignment = Alignment.CenterVertically,
    ) {
        Text("📄", fontSize = 18.sp)
        Spacer(Modifier.width(10.dp))
        Column(modifier = Modifier.weight(1f)) {
            Text(file.nameWithoutExtension, fontSize = 13.sp, fontWeight = FontWeight.W600, color = AppColors.DarkText,
                maxLines = 1, overflow = TextOverflow.Ellipsis)
            Text(meta, fontSize = 11.sp, color = AppColors.WeakText)
        }
        IconButton(onClick = onDelete) {
            Icon(Icons.Outlined.Delete, contentDescription = "Delete", tint = AppColors.WeakText, modifier = Modifier.size(18.dp))
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
private fun ReportResult(data: ReportData, onExportPdf: () -> Unit, onExportWord: () -> Unit, onExportImage: () -> Unit, onExportCsv: () -> Unit, onNew: () -> Unit, onSaveComment: (String) -> Unit) {
    var showExportChoice by remember { mutableStateOf(false) }
    Column(modifier = Modifier.fillMaxSize()) {
        Column(
            modifier = Modifier.weight(1f).verticalScroll(rememberScrollState()).padding(horizontal = 16.dp, vertical = 16.dp),
        ) {
            HeaderCard(data)
            Spacer(Modifier.height(18.dp))

            SectionLabel("Measurement", LightBlue)
            Spacer(Modifier.height(10.dp))
            CopyableBlock("measurement") {
                GridTable2(listOf(
                    Triple("Light Level", "%.2f m".format(data.lightLevel), LightBlue),
                    Triple("Heavy Level", "%.2f m".format(data.heavyLevel), HeavyOrange),
                    Triple("Temperature", "%.1f °C".format(data.temperatureC), null),
                    Triple("Current", "%.2f mA".format(data.currentMA), null),
                ))
            }
            Spacer(Modifier.height(22.dp))

            val thrLightStr = if (data.thrLightMode == 1) "%.1f V".format(data.thrLightSet / 10.0)
                              else "${data.thrLightSet} %"
            val thrHeavyStr = if (data.thrHeavyMode == 1) "%.1f V".format(data.thrHeavySet / 10.0)
                              else "${data.thrHeavySet} %"
            SectionLabel("Parameter", LightBlue)
            Spacer(Modifier.height(10.dp))
            CopyableBlock("parameter") {
                GridTable4(listOf(
                    listOf("Echo Amp", data.echoAmp.toString(),
                           "Frequency", "%.0f kHz".format(data.freqMHz * 1000)),
                    listOf("Thr.Light", thrLightStr,
                           "Thr.Heavy", thrHeavyStr),
                    listOf("Offset", "%.2f m".format(data.offset),
                           "Empty", "%.2f m".format(data.emptyDistance)),
                    listOf("Dead Zone", "%.2f m".format(data.deadZone),
                           "Damping", data.damping.toString()),
                    listOf("Set 4mA", "%.2f m".format(data.set4mA),
                           "Set 20mA", "%.2f m".format(data.set20mA)),
                ))
            }
            Spacer(Modifier.height(22.dp))

            SectionLabel("Echo", LightBlue)
            Spacer(Modifier.height(14.dp))
            CopyableBlock("real_wave") { WaveBlock("Real", LightBlue, data.realEcho) }
            Spacer(Modifier.height(14.dp))
            CopyableBlock("avg_wave") { WaveBlock("Average", HeavyOrange, data.avgEcho) }
            Spacer(Modifier.height(22.dp))

            SectionLabel("Comment", LightBlue)
            Spacer(Modifier.height(10.dp))
            CommentBox(comment = data.comment, onSave = onSaveComment)
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
                onClick = { showExportChoice = true },
                modifier = Modifier.weight(2f).height(52.dp),
                colors = ButtonDefaults.buttonColors(containerColor = ReportPurple),
                shape = RoundedCornerShape(14.dp),
            ) { Text("Export", color = AppColors.White, fontWeight = FontWeight.W800, fontSize = 16.sp) }
        }
    }

    if (showExportChoice) {
        Dialog(onDismissRequest = { showExportChoice = false }) {
            Column(
                modifier = Modifier
                    .clip(RoundedCornerShape(16.dp))
                    .background(AppColors.White)
                    .padding(20.dp),
                horizontalAlignment = Alignment.CenterHorizontally,
            ) {
                Text("Export Report", fontSize = 20.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
                Spacer(Modifier.height(18.dp))
                DialogActionButton(
                    text = "PDF",
                    background = ReportPurple,
                    contentColor = AppColors.White,
                    modifier = Modifier.fillMaxWidth(),
                    onClick = { showExportChoice = false; onExportPdf() },
                )
                Spacer(Modifier.height(10.dp))
                DialogActionButton(
                    text = "Word (.doc)",
                    background = AppColors.Primary,
                    contentColor = AppColors.White,
                    modifier = Modifier.fillMaxWidth(),
                    onClick = { showExportChoice = false; onExportWord() },
                )
                Spacer(Modifier.height(10.dp))
                DialogActionButton(
                    text = "Image (.jpg)",
                    background = HeavyOrange,
                    contentColor = AppColors.White,
                    modifier = Modifier.fillMaxWidth(),
                    onClick = { showExportChoice = false; onExportImage() },
                )
                Spacer(Modifier.height(10.dp))
                DialogActionButton(
                    text = "CSV (.csv)",
                    background = AppColors.Success,
                    contentColor = AppColors.White,
                    modifier = Modifier.fillMaxWidth(),
                    onClick = { showExportChoice = false; onExportCsv() },
                )
                Spacer(Modifier.height(10.dp))
                DialogActionButton(
                    text = "Cancel",
                    background = AppColors.LightGray,
                    contentColor = AppColors.DarkText,
                    modifier = Modifier.fillMaxWidth(),
                    onClick = { showExportChoice = false },
                )
            }
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
                Text(data.title.ifBlank { data.label }, fontSize = 24.sp, fontWeight = FontWeight.W800, color = Color.White, letterSpacing = (-0.5).sp)
            }
            Spacer(Modifier.height(10.dp))
            Text("ENV130", fontSize = 13.sp, color = Color.White.copy(alpha = 0.85f))
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

// 꾹 누르면 해당 블록을 이미지로 캡처해 클립보드에 복사 (카톡 등에 붙여넣기)
@Composable
private fun CopyableBlock(label: String, content: @Composable () -> Unit) {
    val context = LocalContext.current
    val scope = rememberCoroutineScope()
    val graphicsLayer = rememberGraphicsLayer()
    Box(
        modifier = Modifier
            .drawWithContent {
                graphicsLayer.record { this@drawWithContent.drawContent() }
                drawLayer(graphicsLayer)
            }
            .pointerInput(Unit) {
                detectTapGestures(onLongPress = {
                    scope.launch {
                        try {
                            val bitmap = graphicsLayer.toImageBitmap().asAndroidBitmap()
                            copyBitmapToClipboard(context, bitmap, label)
                            Toast.makeText(context, "Copied to clipboard", Toast.LENGTH_SHORT).show()
                        } catch (e: Exception) {
                            Toast.makeText(context, "Copy failed: ${e.message}", Toast.LENGTH_SHORT).show()
                        }
                    }
                })
            },
    ) { content() }
}

private fun copyBitmapToClipboard(context: Context, bitmap: Bitmap, label: String) {
    val dir = File(context.cacheDir, "report_export").apply { mkdirs() }
    val file = File(dir, "copy_${label}_${System.currentTimeMillis()}.png")
    FileOutputStream(file).use { bitmap.compress(Bitmap.CompressFormat.PNG, 100, it) }
    val uri = FileProvider.getUriForFile(context, "${context.packageName}.fileprovider", file)
    val cm = context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
    cm.setPrimaryClip(ClipData.newUri(context.contentResolver, label, uri))
}

@Composable
private fun CommentBox(comment: String, onSave: (String) -> Unit) {
    var editing by remember { mutableStateOf(false) }
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .background(AppColors.White)
            .border(1.5.dp, TableBorderCol)
            .clickable { editing = true }
            .padding(14.dp)
            .heightIn(min = 80.dp),
    ) {
        if (comment.isBlank()) {
            Text("Tap to write a comment", fontSize = 14.sp, color = AppColors.WeakText)
        } else {
            Text(comment, fontSize = 14.sp, fontWeight = FontWeight.W600, color = AppColors.DarkText, lineHeight = 21.sp)
        }
    }
    if (editing) {
        CommentEditDialog(
            initial = comment,
            onDismiss = { editing = false },
            onSave = { editing = false; onSave(it) },
        )
    }
}

@Composable
private fun CommentEditDialog(initial: String, onDismiss: () -> Unit, onSave: (String) -> Unit) {
    var text by remember { mutableStateOf(initial) }
    Dialog(onDismissRequest = onDismiss) {
        Column(
            modifier = Modifier
                .clip(RoundedCornerShape(16.dp))
                .background(AppColors.White)
                .padding(20.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
        ) {
            Text("Comment", fontSize = 20.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
            Spacer(Modifier.height(16.dp))
            OutlinedTextField(
                value = text,
                onValueChange = { text = it },
                modifier = Modifier.fillMaxWidth().height(160.dp),
                shape = RoundedCornerShape(8.dp),
            )
            Spacer(Modifier.height(16.dp))
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
                    text = "Save",
                    background = AppColors.Primary,
                    contentColor = AppColors.White,
                    modifier = Modifier.weight(1f),
                    onClick = { onSave(text.trim()) },
                )
            }
        }
    }
}

// PDF 표와 동일한 격자 스타일 (ReportHtmlExporter CSS 미러)
private val TableBorderCol = Color(0xFFC9CFD6)
private val TableLineCol = Color(0xFFD8DDE3)
private val ThBgCol = Color(0xFFFAFBFC)
private val ThTextCol = Color(0xFF4E5968)

@Composable
private fun RowScope.TableTh(text: String, weight: Float) {
    Box(
        modifier = Modifier
            .weight(weight)
            .fillMaxHeight()
            .background(ThBgCol)
            .border(0.5.dp, TableLineCol)
            .padding(horizontal = 12.dp, vertical = 11.dp),
        contentAlignment = Alignment.CenterStart,
    ) { Text(text, fontSize = 13.sp, fontWeight = FontWeight.W600, color = ThTextCol) }
}

@Composable
private fun RowScope.TableTd(text: String, weight: Float, color: Color = AppColors.DarkText) {
    Box(
        modifier = Modifier
            .weight(weight)
            .fillMaxHeight()
            .background(AppColors.White)
            .border(0.5.dp, TableLineCol)
            .padding(horizontal = 12.dp, vertical = 11.dp),
        contentAlignment = Alignment.CenterStart,
    ) { Text(text, fontSize = 14.sp, fontWeight = FontWeight.W700, color = color) }
}

@Composable
private fun GridTable2(rows: List<Triple<String, String, Color?>>) {
    Column(Modifier.fillMaxWidth().border(1.5.dp, TableBorderCol)) {
        rows.forEach { (k, v, c) ->
            Row(Modifier.fillMaxWidth().height(IntrinsicSize.Min)) {
                TableTh(k, 0.45f)
                TableTd(v, 0.55f, c ?: AppColors.DarkText)
            }
        }
    }
}

@Composable
private fun GridTable4(rows: List<List<String>>) {
    Column(Modifier.fillMaxWidth().border(1.5.dp, TableBorderCol)) {
        rows.forEach { r ->
            Row(Modifier.fillMaxWidth().height(IntrinsicSize.Min)) {
                TableTh(r[0], 0.22f)
                TableTd(r[1], 0.28f)
                TableTh(r[2], 0.22f)
                TableTd(r[3], 0.28f)
            }
        }
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
