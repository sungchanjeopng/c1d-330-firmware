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
 * Clipping pitfalls handled here (the last waveform used to get cut off):
 *  1. The WebView is attached with a large fixed height instead of
 *     WRAP_CONTENT, so chromium treats the whole document as viewport and
 *     tile-renders everything up front (no lazy rendering of the bottom).
 *  2. The page height takes max(measuredHeight, contentHeight * density)
 *     because measure(UNSPECIFIED) can under-report while images are still
 *     being laid out.
 *  3. draw() runs on a second delay after the final layout pass so the
 *     renderer has time to paint any newly exposed tiles.
 */
object ReportPdfExporter {

    private const val TAG = "ReportPdfExporter"

    // A4 portrait at 150 DPI — wide enough to keep the desktop-ish layout
    // readable while staying well under typical PdfDocument page limits.
    private const val PAGE_WIDTH_PX = 1240

    // Pre-render viewport height: must exceed any realistic report length so
    // chromium renders all tiles before we capture.
    private const val PRE_RENDER_HEIGHT_PX = 20000

    fun generateAndShare(activity: Activity, data: ReportData) {
        Log.d(TAG, "generateAndShare: start label=${data.label}")
        Toast.makeText(activity, "Generating PDF...", Toast.LENGTH_SHORT).show()
        val html = ReportHtmlExporter.buildHtml(data)
        val safeLabel = data.label.replace(Regex("[^A-Za-z0-9_-]"), "_")
        val stamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).format(Date())
        val fileName = "Report_${safeLabel}_$stamp.pdf"
        val dir = File(activity.cacheDir, "report_export").apply { mkdirs() }
        val pdfFile = File(dir, fileName)

        // Attach an invisible WebView to the activity content view. Without
        // being attached to a window, the WebView's chromium backend skips
        // rendering and Canvas draws produce a blank page.
        val rootView = activity.findViewById<ViewGroup>(android.R.id.content)
        val webView = WebView(activity).apply {
            settings.defaultTextEncodingName = "UTF-8"
            settings.javaScriptEnabled = false
            setLayerType(View.LAYER_TYPE_SOFTWARE, null)
            alpha = 0f
        }
        // Fix 1: large fixed height — the system layout pass would otherwise
        // clamp the WebView viewport to the screen height and chromium would
        // lazily skip rendering the bottom of the document (= clipped
        // Average waveform).
        val lp = ViewGroup.LayoutParams(PAGE_WIDTH_PX, PRE_RENDER_HEIGHT_PX)
        rootView.addView(webView, lp)

        webView.webViewClient = object : WebViewClient() {
            override fun onPageFinished(view: WebView, url: String) {
                Log.d(TAG, "onPageFinished, contentHeight=${view.contentHeight}")
                // Give the engine a moment to settle (images, fonts).
                view.postDelayed({ measureAndRender(activity, view, rootView, pdfFile, fileName) }, 800)
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
    ) {
        try {
            val widthSpec = View.MeasureSpec.makeMeasureSpec(PAGE_WIDTH_PX, View.MeasureSpec.EXACTLY)
            val heightSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED)
            view.measure(widthSpec, heightSpec)

            // Fix 2: measure(UNSPECIFIED) can under-report while image layout
            // is still settling. contentHeight (CSS px) * density is the
            // renderer's own idea of the document height — take the max.
            val contentPx = (view.contentHeight * view.resources.displayMetrics.density).toInt()
            val totalHeight = maxOf(view.measuredHeight, contentPx).coerceAtLeast(1)
            Log.d(TAG, "measureAndRender: measuredH=${view.measuredHeight} contentPx=$contentPx -> totalH=$totalHeight")

            view.layout(0, 0, PAGE_WIDTH_PX, totalHeight)

            // Fix 3: give chromium time to paint tiles for any area exposed
            // by the layout pass above before capturing.
            view.postDelayed({
                try {
                    drawPdfAndShare(activity, view, totalHeight, pdfFile, fileName)
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

        launchShareSheet(activity, pdfFile, fileName)
    }

    private fun launchShareSheet(activity: Activity, file: File, fileName: String) {
        try {
            val uri = FileProvider.getUriForFile(
                activity,
                "${activity.packageName}.fileprovider",
                file,
            )
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "application/pdf"
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
