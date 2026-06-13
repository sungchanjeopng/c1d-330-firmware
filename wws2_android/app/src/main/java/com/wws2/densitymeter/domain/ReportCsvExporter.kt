package com.wws2.densitymeter.domain

import android.app.Activity
import android.content.Intent
import android.util.Log
import android.widget.Toast
import androidx.core.content.FileProvider
import com.wws2.densitymeter.model.ReportData
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

/**
 * 리포트를 CSV로 내보낸다. 이미지는 못 담으므로 파형은 raw 데이터
 * (Index, Real, Avg 컬럼)로 포함 — 엑셀에서 직접 차트를 그릴 수 있다.
 */
object ReportCsvExporter {

    private const val TAG = "ReportCsvExporter"

    fun generateAndShare(activity: Activity, data: ReportData) {
        try {
            Toast.makeText(activity, "Generating CSV...", Toast.LENGTH_SHORT).show()
            val safeLabel = data.title.ifBlank { data.label }.replace(Regex("[\\\\/:*?\"<>|]"), "_")
            val stamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).format(Date())
            val fileName = "Report_${safeLabel}_$stamp.csv"
            val dir = File(activity.cacheDir, "report_export").apply { mkdirs() }
            val file = File(dir, fileName)
            // BOM — 엑셀에서 한글 깨짐 방지
            file.writeText("\uFEFF" + buildCsv(data))

            ReportPdfExporter.saveCopyToDocuments(file)?.let { path ->
                Toast.makeText(activity, "Saved: $path", Toast.LENGTH_SHORT).show()
            }

            val uri = FileProvider.getUriForFile(activity, "${activity.packageName}.fileprovider", file)
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "text/csv"
                putExtra(Intent.EXTRA_STREAM, uri)
                putExtra(Intent.EXTRA_SUBJECT, fileName)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            activity.startActivity(Intent.createChooser(intent, "Share Report"))
        } catch (e: Exception) {
            Log.e(TAG, "CSV export failed: ${e.message}", e)
            Toast.makeText(activity, "CSV export failed: ${e.message}", Toast.LENGTH_LONG).show()
        }
    }

    private fun q(s: String) = "\"" + s.replace("\"", "\"\"") + "\""

    private fun buildCsv(data: ReportData): String = buildString {
        appendLine("Title,${q(data.title.ifBlank { data.label })}")
        appendLine("Device,${q(data.label)}")
        appendLine("Model,ENV130")
        appendLine("Timestamp,${q(data.timestamp)}")
        appendLine()
        appendLine("[Measurement]")
        appendLine("Light Level (m),%.2f".format(data.lightLevel))
        appendLine("Heavy Level (m),%.2f".format(data.heavyLevel))
        appendLine("Temperature (C),%.1f".format(data.temperatureC))
        appendLine("Current (mA),%.2f".format(data.currentMA))
        appendLine()
        appendLine("[Parameter]")
        appendLine("Echo Amp,${data.echoAmp}")
        appendLine("Frequency (kHz),%.0f".format(data.freqMHz * 1000))
        val thrL = if (data.thrLightMode == 1) "%.1f V".format(data.thrLightSet / 10.0) else "${data.thrLightSet} %"
        val thrH = if (data.thrHeavyMode == 1) "%.1f V".format(data.thrHeavySet / 10.0) else "${data.thrHeavySet} %"
        appendLine("Thr.Light,${q(thrL)}")
        appendLine("Thr.Heavy,${q(thrH)}")
        appendLine("Offset (m),%.2f".format(data.offset))
        appendLine("Empty (m),%.2f".format(data.emptyDistance))
        appendLine("Dead Zone (m),%.2f".format(data.deadZone))
        appendLine("Damping,${data.damping}")
        appendLine("Set 4mA (m),%.2f".format(data.set4mA))
        appendLine("Set 20mA (m),%.2f".format(data.set20mA))
        appendLine()
        appendLine("[Comment]")
        appendLine(q(data.comment))
        appendLine()
        appendLine("[Waveform]")
        appendLine("Index,Real,Avg")
        val real = data.realEcho?.wave ?: emptyList()
        val avg = data.avgEcho?.wave ?: emptyList()
        for (i in 0 until maxOf(real.size, avg.size)) {
            appendLine("$i,${real.getOrNull(i)?.toString() ?: ""},${avg.getOrNull(i)?.toString() ?: ""}")
        }
    }
}
