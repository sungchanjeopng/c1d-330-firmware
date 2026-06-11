package com.wws2.densitymeter.domain

import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.DashPathEffect
import android.graphics.Paint
import android.graphics.Path
import android.util.Base64
import com.wws2.densitymeter.model.InterfaceEchoReading
import com.wws2.densitymeter.model.ReportData
import java.io.ByteArrayOutputStream

/**
 * ENV130 리포트를 수정 가능한 HTML 문서로 생성한다.
 * - 측정값/설정값 = HTML 표 (Word/한글에서 편집 가능)
 * - 파형(실시간/평균) = android.graphics 로 그린 PNG를 base64 로 인라인 삽입 (파일 1개로 완결)
 */
object ReportHtmlExporter {

    private const val ADC_MAX = 4095f

    /** 인터페이스 파형(InterfaceEchoReading)을 Bitmap 으로 렌더. (InterfaceEchoChart 와 동일 형태) */
    fun renderWaveformBitmap(reading: InterfaceEchoReading?, width: Int = 900, height: Int = 400): Bitmap {
        val bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmp)
        c.drawColor(Color.WHITE)
        val w = width.toFloat()
        val totalH = height.toFloat()
        // Reserve bottom margin for x-axis labels (same layout idea as
        // InterfaceEchoChart's Box(padding bottom = 28dp)).
        val axisPad = 34f
        val h = totalH - axisPad
        c.drawRect(1f, 1f, w - 1, h - 1, Paint().apply {
            color = Color.parseColor("#E0E0E0"); style = Paint.Style.STROKE; strokeWidth = 1f
        })

        if (reading == null || reading.wave.size < 2) {
            c.drawText("No Data", w / 2, h / 2, Paint().apply {
                color = Color.parseColor("#8B95A1"); textSize = 26f
                textAlign = Paint.Align.CENTER; isAntiAlias = true
            })
            return bmp
        }

        val wave = reading.wave
        val n = wave.size
        val dz = reading.deadzone
        val empty = reading.empty
        fun xOf(idx: Float) = w * idx / (n - 1).toFloat()
        fun yOf(v: Int) = h - (v.toFloat() / ADC_MAX).coerceIn(0f, 1f) * h

        fun drawSeg(start: Int, end: Int, colorHex: String) {
            val s = start.coerceAtLeast(0)
            val e = end.coerceAtMost(n - 1)
            if (e <= s) return
            val fill = Path()
            val line = Path()
            var started = false
            var lastX = 0f
            for (i in s..e) {
                val x = xOf(i.toFloat())
                val y = yOf(wave[i])
                if (!started) { line.moveTo(x, y); fill.moveTo(x, h); fill.lineTo(x, y); started = true }
                else { line.lineTo(x, y); fill.lineTo(x, y) }
                lastX = x
            }
            fill.lineTo(lastX, h); fill.close()
            val col = Color.parseColor(colorHex)
            c.drawPath(fill, Paint().apply { color = col; style = Paint.Style.FILL; isAntiAlias = true })
            c.drawPath(line, Paint().apply { color = col; style = Paint.Style.STROKE; strokeWidth = 2f; isAntiAlias = true })
        }
        drawSeg(0, dz, "#1B4050")
        drawSeg(dz, empty, "#02F1AB")
        drawSeg(empty, n - 1, "#1B4050")

        fun vline(x: Float, colorHex: String) =
            c.drawLine(x, 0f, x, h, Paint().apply { color = Color.parseColor(colorHex); strokeWidth = 2.5f })
        if (dz > 0) vline(xOf(dz.toFloat()), "#3182F6")
        if (empty in 1 until n) vline(xOf(empty.toFloat()), "#3182F6")
        if (reading.thrLightDist > 0) vline(xOf(reading.thrLightDist.toFloat()), "#666666")
        if (reading.thrHeavyDist > 0) vline(xOf(reading.thrHeavyDist.toFloat()), "#FF8C00")

        fun hdash(y: Float, colorHex: String) = c.drawLine(0f, y, w, y, Paint().apply {
            color = Color.parseColor(colorHex); strokeWidth = 2.5f; pathEffect = DashPathEffect(floatArrayOf(6f, 4f), 0f)
        })
        if (reading.thrLightReal > 0) hdash(yOf(reading.thrLightReal), "#666666")
        if (reading.thrHeavyReal > 0) hdash(yOf(reading.thrHeavyReal), "#FF8C00")

        // X-axis labels (same layout as InterfaceEchoChart):
        // 10 ticks from Empty (left, 0.00m at right). x = w*(emptyM-v)/totalRangeM
        val emptyM = empty * 0.01f
        val totalRangeM = (n - 1) * 0.01f
        if (emptyM > 0f && totalRangeM > 0f) {
            val labelPaint = Paint().apply {
                color = Color.parseColor("#8B95A1")
                textSize = 18f
                textAlign = Paint.Align.CENTER
                isAntiAlias = true
            }
            for (i in 0..10) {
                val v = emptyM - (emptyM / 10f) * i
                val x = w * (emptyM - v) / totalRangeM
                if (x < 0f || x > w) continue
                val label = "%.2f".format(v)
                c.drawText(label, x.coerceIn(20f, w - 20f), h + 22f, labelPaint)
            }
        }
        return bmp
    }

    private fun bitmapToBase64Png(bmp: Bitmap): String {
        val out = ByteArrayOutputStream()
        bmp.compress(Bitmap.CompressFormat.PNG, 100, out)
        return Base64.encodeToString(out.toByteArray(), Base64.NO_WRAP)
    }

    private fun esc(s: String): String =
        s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")

    fun buildHtml(data: ReportData): String {
        val realImg = bitmapToBase64Png(renderWaveformBitmap(data.realEcho))
        val avgImg = bitmapToBase64Png(renderWaveformBitmap(data.avgEcho))

        // 데이터 행은 <table> 로 — Word 로 열어도 편집 가능하게.
        fun row(k: String, v: String, vColor: String? = null): String {
            val vStyle = if (vColor != null) " style=\"color:$vColor\"" else ""
            return "<tr><th>${esc(k)}</th><td$vStyle>${esc(v)}</td></tr>"
        }
        val measurement = buildString {
            append(row("Light Level", "%.2f m".format(data.lightLevel), "#3182F6"))
            append(row("Heavy Level", "%.2f m".format(data.heavyLevel), "#FF8C00"))
            append(row("Temperature", "%.1f °C".format(data.temperatureC)))
            append(row("Current", "%.2f mA".format(data.currentMA)))
        }
        val thrLightStr = if (data.thrLightMode == 1) "%.1f V".format(data.thrLightSet / 10.0)
                          else "${data.thrLightSet} %"
        val thrHeavyStr = if (data.thrHeavyMode == 1) "%.1f V".format(data.thrHeavySet / 10.0)
                          else "${data.thrHeavySet} %"
        fun row4(k1: String, v1: String, k2: String?, v2: String?): String {
            val cell2 = if (k2 != null && v2 != null)
                "<th>${esc(k2)}</th><td>${esc(v2)}</td>"
            else
                "<th></th><td></td>"
            return "<tr><th>${esc(k1)}</th><td>${esc(v1)}</td>$cell2</tr>"
        }
        val settings = buildString {
            append(row4("Echo Amp", data.echoAmp.toString(),
                       "Frequency", "%.0f kHz".format(data.freqMHz * 1000)))
            append(row4("Offset", "%.2f m".format(data.offset),
                       "Empty Distance", "%.2f m".format(data.emptyDistance)))
            append(row4("Dead Zone", "%.2f m".format(data.deadZone),
                       "Damping", data.damping.toString()))
            append(row4("Current 4mA", "%.2f m".format(data.set4mA),
                       "Current 20mA", "%.2f m".format(data.set20mA)))
            append(row4("Temperature", "%.1f °C".format(data.temperatureC),
                       "Current", "%.2f mA".format(data.currentMA)))
        }
        val echoSettings = buildString {
            append(row("Thr.Light", thrLightStr))
            append(row("Thr.Heavy", thrHeavyStr))
        }

        return """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>${esc(data.label)} Report</title>
<style>
  body { font-family: 'Segoe UI', 'Malgun Gothic', Arial, sans-serif; color: #191F28; background: #F2F4F6; margin: 0; padding: 24px; }
  .sheet { max-width: 760px; margin: 0 auto; background: #FFFFFF; border-radius: 22px; overflow: hidden; box-shadow: 0 6px 24px rgba(0,0,0,0.08); }
  .header { background: linear-gradient(135deg, #7C3AED, #3B82F6); color: #FFFFFF; padding: 28px 28px 24px; }
  .header .name { font-size: 26px; font-weight: 800; letter-spacing: -0.5px; }
  .header .dot { display: inline-block; width: 10px; height: 10px; border-radius: 50%; background: #4ADE80; margin-right: 8px; vertical-align: middle; }
  .header .sub { font-size: 13px; opacity: 0.9; margin-top: 10px; }
  .body { padding: 24px 28px 30px; }
  h2 { font-size: 17px; font-weight: 800; color: #191F28; letter-spacing: -0.3px; margin: 26px 0 10px; padding-left: 10px; border-left: 4px solid #3182F6; }
  h2.s { border-left-color: #7C3AED; }
  table { border-collapse: collapse; width: 100%; font-size: 14px; }
  th, td { padding: 11px 14px; text-align: left; border-bottom: 1px solid #F2F4F6; }
  th { color: #4E5968; font-weight: 600; background: #FAFBFC; }
  td { font-weight: 700; color: #191F28; }
  table.kv2 th { width: 45%; }
  table.kv4 th { width: 22%; }
  table.kv4 td { width: 28%; }
  tr.gh td { font-size: 11px; font-weight: 800; color: #8B95A1; letter-spacing: 0.8px; background: #FFFFFF; border-bottom: none; padding: 14px 14px 4px; text-transform: uppercase; }
  .badge { display: inline-block; font-size: 12px; font-weight: 800; padding: 4px 12px; border-radius: 999px; }
  .badge.real { color: #3182F6; background: rgba(49,130,246,0.12); }
  .badge.avg { color: #FF8C00; background: rgba(255,140,0,0.12); }
  img.wave { width: 100%; border: 1px solid #E8EBED; border-radius: 12px; margin-top: 8px; }
  @media print { body { background: #FFF; padding: 0; } .sheet { box-shadow: none; } }
</style>
</head>
<body>
  <div class="sheet">
    <div class="header">
      <div class="name"><span class="dot"></span>${esc(data.label)}</div>
      <div class="sub">Sludge Level Meter &nbsp;·&nbsp; FW ${esc(data.firmwareVersion.ifEmpty { "—" })}<br>${esc(data.timestamp)}</div>
    </div>
    <div class="body">
      <h2>Measurement</h2>
      <table class="kv2">$measurement</table>

      <table class="kv4">$settings</table>

      <h2>Echo</h2>
      <table class="kv2">$echoSettings</table>
      <div style="height:10px"></div>
      <span class="badge real">Real</span>
      <img class="wave" src="data:image/png;base64,$realImg" alt="Real waveform">
      <div style="height:14px"></div>
      <span class="badge avg">Average</span>
      <img class="wave" src="data:image/png;base64,$avgImg" alt="Average waveform">
    </div>
  </div>
</body>
</html>"""
    }
}
