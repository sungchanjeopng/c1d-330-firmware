package com.wws2.densitymeter.domain

import android.app.Activity
import android.content.Intent
import android.graphics.pdf.PdfDocument
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.Toast
import androidx.core.content.FileProvider
import com.wws2.densitymeter.model.ReportData
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

/**
 * Renders the report HTML to a PDF without going through the system print
 * dialog, then hands the file to the standard share sheet.
 *
 * Rendering pitfalls handled here (clipped waveform → blank-space page →
 * fully blank page, in that historical order):
 *  1. The WebView is attached with a TALL fixed height (20000px) so
 *     chromium tile-renders the whole document up front. Attaching small
 *     and manually re-layouting larger produces a fully blank capture.
 *  2. The page height comes from document.body.offsetHeight (via JS) ×
 *     density — NOT from contentHeight, which is max(document, viewport)
 *     and balloons to the pre-render height (= huge blank space below).
 *  3. draw() runs on a delay after measurement so the renderer has time
 *     to finish painting tiles.
 */
object ReportPdfExporter {

    private const val TAG = "ReportPdfExporter"

    // A4 portrait at 150 DPI — wide enough to keep the desktop-ish layout
    // readable while staying well under typical PdfDocument page limits.
    private const val PAGE_WIDTH_PX = 1240

    // Pre-render viewport height: must exceed any realistic report length so
    // chromium renders ALL tiles before capture. (A small attach + manual
    // re-layout produced a fully blank PDF — chromium never rasterized the
    // grown area.) The blank-space problem is solved at the page-height
    // calculation instead: body.offsetHeight via JS, not contentHeight.
    private const val PRE_RENDER_HEIGHT_PX = 20000

    enum class Format(val ext: String, val mime: String) {
        PDF("pdf", "application/pdf"),
        JPEG("jpg", "image/jpeg"),
    }

    fun generateAndShare(activity: Activity, data: ReportData, format: Format = Format.PDF) {
        Log.d(TAG, "generateAndShare: start label=${data.label} format=$format")
        Toast.makeText(activity, "Generating ${format.ext.uppercase()}...", Toast.LENGTH_SHORT).show()
        val html = ReportHtmlExporter.buildHtml(data)
        val safeLabel = data.title.ifBlank { data.label }.replace(Regex("[\\\\/:*?\"<>|]"), "_")
        val stamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).format(Date())
        val fileName = "Report_${safeLabel}_$stamp.${format.ext}"
        val dir = File(activity.cacheDir, "report_export").apply { mkdirs() }
        val pdfFile = File(dir, fileName)

        // Attach an invisible WebView to the activity content view. Without
        // being attached to a window, the WebView's chromium backend skips
        // rendering and Canvas draws produce a blank page.
        val rootView = activity.findViewById<ViewGroup>(android.R.id.content)
        val webView = WebView(activity).apply {
            settings.defaultTextEncodingName = "UTF-8"
            // 자체 생성 HTML 전용 — body.offsetHeight 측정에만 JS 사용
            settings.javaScriptEnabled = true
            setLayerType(View.LAYER_TYPE_SOFTWARE, null)
            alpha = 0f
        }
        val lp = ViewGroup.LayoutParams(PAGE_WIDTH_PX, PRE_RENDER_HEIGHT_PX)
        rootView.addView(webView, lp)

        webView.webViewClient = object : WebViewClient() {
            override fun onPageFinished(view: WebView, url: String) {
                Log.d(TAG, "onPageFinished, contentHeight=${view.contentHeight}")
                // Give the engine a moment to settle (images, fonts).
                view.postDelayed({ measureAndRender(activity, view, rootView, pdfFile, fileName, format) }, 800)
            }
        }
        webView.loadDataWithBaseURL(null, html, "text/html", "UTF-8", null)
    }

    private fun measureAndRender(
        activity: Activity,
        view: WebView,
        rootView: ViewGroup,
        pdfFile: File,
        fileName: String,
        format: Format,
    ) {
        // 페이지 높이는 문서(body) 기준이어야 한다. contentHeight는
        // max(문서, 뷰포트)라 20000px 뷰포트에서 빈 여백 페이지가 된다.
        view.evaluateJavascript("(function(){return document.body.offsetHeight;})()") { raw ->
            try {
                val density = view.resources.displayMetrics.density
                val cssH = raw?.trim()?.toFloatOrNull() ?: 0f
                val fallbackPx = view.contentHeight * density
                val totalHeight = (if (cssH > 0f) cssH * density else fallbackPx)
                    .toInt().coerceAtLeast(1)
                Log.d(TAG, "measureAndRender: cssH=$cssH density=$density -> totalH=$totalHeight")

                view.postDelayed({
                    try {
                        if (format == Format.PDF) drawPdfAndShare(activity, view, totalHeight, pdfFile, fileName)
                        else drawJpegAndShare(activity, view, totalHeight, pdfFile, fileName)
                    } catch (e: Exception) {
                        Log.e(TAG, "PDF draw failed: ${e.message}", e)
                        Toast.makeText(activity, "PDF export failed: ${e.message}", Toast.LENGTH_LONG).show()
                    } finally {
                        rootView.removeView(view)
                    }
                }, 400)
            } catch (e: Exception) {
                Log.e(TAG, "PDF measure failed: ${e.message}", e)
                Toast.makeText(activity, "PDF export failed: ${e.message}", Toast.LENGTH_LONG).show()
                rootView.removeView(view)
            }
        }
    }

    private fun drawPdfAndShare(
        activity: Activity,
        view: WebView,
        totalHeight: Int,
        pdfFile: File,
        fileName: String,
    ) {
        // Render the entire report on a single tall page so waveforms never
        // get clipped at a page boundary.
        val pdf = PdfDocument()
        val pageInfo = PdfDocument.PageInfo.Builder(PAGE_WIDTH_PX, totalHeight, 1).create()
        val page = pdf.startPage(pageInfo)
        view.draw(page.canvas)
        pdf.finishPage(page)

        FileOutputStream(pdfFile).use { fos -> pdf.writeTo(fos) }
        pdf.close()

        launchShareSheet(activity, pdfFile, fileName, Format.PDF.mime)
    }

    private fun drawJpegAndShare(
        activity: Activity,
        view: WebView,
        totalHeight: Int,
        file: File,
        fileName: String,
    ) {
        val bmp = android.graphics.Bitmap.createBitmap(
            PAGE_WIDTH_PX, totalHeight, android.graphics.Bitmap.Config.ARGB_8888)
        val canvas = android.graphics.Canvas(bmp)
        canvas.drawColor(android.graphics.Color.WHITE)
        view.draw(canvas)
        FileOutputStream(file).use { fos ->
            bmp.compress(android.graphics.Bitmap.CompressFormat.JPEG, 90, fos)
        }
        bmp.recycle()

        launchShareSheet(activity, file, fileName, Format.JPEG.mime)
    }

    /** 공유와 별개로 파일 관리자에서 보이도록 Documents/WESSWARE에 복사 저장 (CSV와 동일 위치). */
    fun saveCopyToDocuments(file: File): String? = try {
        val docsDir = android.os.Environment.getExternalStoragePublicDirectory(android.os.Environment.DIRECTORY_DOCUMENTS)
        val appDir = File(docsDir, "WESSWARE").apply { mkdirs() }
        val dest = File(appDir, file.name)
        file.copyTo(dest, overwrite = true)
        dest.absolutePath
    } catch (e: Exception) {
        Log.e(TAG, "saveCopyToDocuments failed: ${e.message}")
        null
    }

    private fun launchShareSheet(activity: Activity, file: File, fileName: String, mime: String) {
        saveCopyToDocuments(file)?.let { path ->
            Toast.makeText(activity, "Saved: $path", Toast.LENGTH_SHORT).show()
        }
        try {
            val uri = FileProvider.getUriForFile(
                activity,
                "${activity.packageName}.fileprovider",
                file,
            )
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = mime
                putExtra(Intent.EXTRA_STREAM, uri)
                putExtra(Intent.EXTRA_SUBJECT, fileName)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            Log.d(TAG, "launchShareSheet: uri=$uri size=${file.length()}")
            activity.startActivity(Intent.createChooser(intent, "Share Report"))
        } catch (e: Exception) {
            Log.e(TAG, "launchShareSheet failed: ${e.message}", e)
            Toast.makeText(activity, "Share failed: ${e.message}", Toast.LENGTH_LONG).show()
        }
    }
}
