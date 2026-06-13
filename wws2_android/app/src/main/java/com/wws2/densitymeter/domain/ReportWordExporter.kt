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
 * 리포트를 Word가 열 수 있는 MHTML(.doc)로 내보낸다.
 * HTML + 파형 PNG 2장을 multipart/related 한 파일에 포장 — Word에서
 * 더블클릭으로 열리고 표/텍스트 편집 가능. (진짜 OOXML .docx 아님)
 */
object ReportWordExporter {

    private const val TAG = "ReportWordExporter"
    private const val BOUNDARY = "----=_NextPart_WWS2_REPORT"

    fun generateAndShare(activity: Activity, data: ReportData) {
        try {
            Toast.makeText(activity, "Generating Word...", Toast.LENGTH_SHORT).show()
            val safeLabel = data.title.ifBlank { data.label }.replace(Regex("[\\\\/:*?\"<>|]"), "_")
            val stamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).format(Date())
            val fileName = "Report_${safeLabel}_$stamp.doc"
            val dir = File(activity.cacheDir, "report_export").apply { mkdirs() }
            val file = File(dir, fileName)
            file.writeText(buildMhtml(data))

            ReportPdfExporter.saveCopyToDocuments(file)?.let { path ->
                Toast.makeText(activity, "Saved: $path", Toast.LENGTH_SHORT).show()
            }

            val uri = FileProvider.getUriForFile(activity, "${activity.packageName}.fileprovider", file)
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "application/msword"
                putExtra(Intent.EXTRA_STREAM, uri)
                putExtra(Intent.EXTRA_SUBJECT, fileName)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            activity.startActivity(Intent.createChooser(intent, "Share Report"))
        } catch (e: Exception) {
            Log.e(TAG, "Word export failed: ${e.message}", e)
            Toast.makeText(activity, "Word export failed: ${e.message}", Toast.LENGTH_LONG).show()
        }
    }

    private fun buildMhtml(data: ReportData): String {
        val (realB64, avgB64) = ReportHtmlExporter.waveImagesBase64(data)
        val html = ReportHtmlExporter.buildHtml(data, "wave_real.png", "wave_avg.png", forWord = true)
        val htmlB64 = java.util.Base64.getMimeEncoder().encodeToString(html.toByteArray(Charsets.UTF_8))

        fun wrap76(b64: String) = b64.chunked(76).joinToString("\r\n")

        return buildString {
            append("MIME-Version: 1.0\r\n")
            append("Content-Type: multipart/related; boundary=\"$BOUNDARY\"; type=\"text/html\"\r\n")
            append("\r\n")

            append("--$BOUNDARY\r\n")
            append("Content-Type: text/html; charset=\"utf-8\"\r\n")
            append("Content-Transfer-Encoding: base64\r\n")
            append("Content-Location: report.html\r\n")
            append("\r\n")
            append(htmlB64)
            append("\r\n\r\n")

            append("--$BOUNDARY\r\n")
            append("Content-Type: image/png\r\n")
            append("Content-Transfer-Encoding: base64\r\n")
            append("Content-Location: wave_real.png\r\n")
            append("\r\n")
            append(wrap76(realB64))
            append("\r\n\r\n")

            append("--$BOUNDARY\r\n")
            append("Content-Type: image/png\r\n")
            append("Content-Transfer-Encoding: base64\r\n")
            append("Content-Location: wave_avg.png\r\n")
            append("\r\n")
            append(wrap76(avgB64))
            append("\r\n\r\n")

            append("--$BOUNDARY--\r\n")
        }
    }
}
