package com.wws2.densitymeter.domain

import android.app.Activity
import android.content.Intent
import android.graphics.pdf.PdfDocument
import android.util.Log
import android.view.View
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
 * dialog. WebView is measured/laid out off-screen, then drawn into a
 * multi-page PdfDocument (so long reports don't get clipped). The
 * finished file is handed off to the standard share sheet.
 */
object ReportPdfExporter {

    private const val TAG = "ReportPdfExporter"

    // A4 portrait at 150 DPI — wide enough to keep the desktop-ish layout
    // readable while staying well under typical PdfDocument page limits.
    private const val PAGE_WIDTH_PX = 1240
    private const val PAGE_HEIGHT_PX = 1754

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
        val rootView = activity.findViewById<android.view.ViewGroup>(android.R.id.content)
        val webView = WebView(activity).apply {
            settings.defaultTextEncodingName = "UTF-8"
            settings.javaScriptEnabled = false
            setLayerType(View.LAYER_TYPE_SOFTWARE, null)
            alpha = 0f
        }
        val lp = android.view.ViewGroup.LayoutParams(PAGE_WIDTH_PX, android.view.ViewGroup.LayoutParams.WRAP_CONTENT)
        rootView.addView(webView, lp)

        webView.webViewClient = object : WebViewClient() {
            override fun onPageFinished(view: WebView, url: String) {
                Log.d(TAG, "onPageFinished, contentHeight=${view.contentHeight}")
                // Give the engine a moment to settle (images, fonts).
                view.postDelayed({
                    renderWebViewToPdf(activity, view, pdfFile, fileName)
                    rootView.removeView(view)
                }, 800)
            }
        }
        webView.loadDataWithBaseURL(null, html, "text/html", "UTF-8", null)
    }

    private fun renderWebViewToPdf(activity: Activity, view: WebView, pdfFile: File, fileName: String) {
        try {
            val widthSpec = View.MeasureSpec.makeMeasureSpec(PAGE_WIDTH_PX, View.MeasureSpec.EXACTLY)
            val heightSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED)
            view.measure(widthSpec, heightSpec)
            view.layout(0, 0, view.measuredWidth, view.measuredHeight)

            val totalHeight = view.measuredHeight.coerceAtLeast(1)
            Log.d(TAG, "renderWebViewToPdf: measuredH=$totalHeight (single page)")

            // Render the entire report on a single tall page so waveforms
            // never get clipped at a page boundary.
            val pdf = PdfDocument()
            val pageInfo = PdfDocument.PageInfo.Builder(PAGE_WIDTH_PX, totalHeight, 1).create()
            val page = pdf.startPage(pageInfo)
            view.draw(page.canvas)
            pdf.finishPage(page)

            FileOutputStream(pdfFile).use { fos -> pdf.writeTo(fos) }
            pdf.close()

            launchShareSheet(activity, pdfFile, fileName)
        } catch (e: Exception) {
            Log.e(TAG, "PDF render failed: ${e.message}", e)
            Toast.makeText(activity, "PDF export failed: ${e.message}", Toast.LENGTH_LONG).show()
        }
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
