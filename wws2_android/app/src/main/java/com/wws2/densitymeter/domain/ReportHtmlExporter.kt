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
        val yLabelPaint = Paint().apply {
            color = Color.parseColor("#8B95A1"); textSize = 16f
            textAlign = Paint.Align.LEFT; isAntiAlias = true
        }
        // Y축 라벨용 왼쪽 여백 (X축 하단 여백과 동일한 방식)
        val leftPad = yLabelPaint.measureText("3.0") + 12f
        val plotW = w - leftPad
        c.drawRect(leftPad, 1f, w - 1, h - 1, Paint().apply {
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
        fun xOf(idx: Float) = leftPad + plotW * idx / (n - 1).toFloat()
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

        fun hdash(y: Float, colorHex: String) = c.drawLine(leftPad, y, w, y, Paint().apply {
            color = Color.parseColor(colorHex); strokeWidth = 2.5f; pathEffect = DashPathEffect(floatArrayOf(6f, 4f), 0f)
        })
        if (reading.thrLightReal > 0) hdash(yOf(reading.thrLightReal), "#666666")
        if (reading.thrHeavyReal > 0) hdash(yOf(reading.thrHeavyReal), "#FF8C00")

        // Y-axis ticks: 1.0V / 2.0V / 3.0V (ADC 0~4095 = 0~3.3V)
        run {
            val guidePaint = Paint().apply {
                color = Color.parseColor("#408B95A1"); strokeWidth = 1f
            }
            c.drawText("V", leftPad - yLabelPaint.measureText("V") - 6f, 16f, yLabelPaint)
            for (volt in intArrayOf(1, 2, 3)) {
                val raw = (volt / 3.3f * ADC_MAX).toInt()
                val y = yOf(raw)
                c.drawLine(leftPad, y, w, y, guidePaint)
                val label = "$volt.0"
                val tw = yLabelPaint.measureText(label)
                c.drawText(label, leftPad - tw - 6f, (y + 6f).coerceIn(34f, h - 2f), yLabelPaint)
            }
        }

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
            val mw = labelPaint.measureText("m")
            c.drawText("m", w - mw / 2f - 2f, h + 22f, labelPaint)
            for (i in 0..10) {
                val v = emptyM - (emptyM / 10f) * i
                val x = leftPad + plotW * (emptyM - v) / totalRangeM
                if (x < leftPad || x > w) continue
                val label = "%.2f".format(v)
                val maxX = w - mw - 8f - labelPaint.measureText(label) / 2f
                c.drawText(label, x.coerceIn(leftPad, maxX), h + 22f, labelPaint)
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

    /** 파형 2장(Real/Avg)의 base64 PNG. Word(MHTML) export에서 별도 파트로 분리할 때 사용. */
    fun waveImagesBase64(data: ReportData): Pair<String, String> = Pair(
        bitmapToBase64Png(renderWaveformBitmap(data.realEcho)),
        bitmapToBase64Png(renderWaveformBitmap(data.avgEcho)),
    )

    fun buildHtml(data: ReportData): String {
        val (realB64, avgB64) = waveImagesBase64(data)
        return buildHtml(data, "data:image/png;base64,$realB64", "data:image/png;base64,$avgB64")
    }

    /**
     * @param forWord Word(MHTML)용 변형: Word가 div CSS background/gradient와
     * CSS img width를 무시하므로 헤더는 table+bgcolor, 이미지는 width 속성,
     * 코멘트 박스는 table 테두리로 대체한다.
     */
    fun buildHtml(data: ReportData, realSrc: String, avgSrc: String, forWord: Boolean = false): String {

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
            append(row4("Thr.Light", thrLightStr,
                       "Thr.Heavy", thrHeavyStr))
            append(row4("Offset", "%.2f m".format(data.offset),
                       "Empty", "%.2f m".format(data.emptyDistance)))
            append(row4("Dead Zone", "%.2f m".format(data.deadZone),
                       "Damping", data.damping.toString()))
            append(row4("Set 4mA", "%.2f m".format(data.set4mA),
                       "Set 20mA", "%.2f m".format(data.set20mA)))
        }

        val headerTitle = esc(data.title.ifBlank { data.label })
        // Word는 div CSS background(gradient 포함)를 무시 → table+bgcolor로 대체
        val header = if (forWord) """<table width="100%" cellpadding="0" cellspacing="0" style="border:none; border-collapse:collapse;"><tr>
      <td bgcolor="#7C3AED" style="background:#7C3AED; padding:24px 28px; border:none;">
        <span style="font-size:26px; font-weight:800; color:#FFFFFF;"><span style="color:#4ADE80;">&#9679;</span>&nbsp;$headerTitle</span><br>
        <span style="font-size:13px; color:#EDE9FE;">ENV130</span><br>
        <span style="font-size:13px; color:#EDE9FE;">${esc(data.timestamp)}</span>
      </td>
    </tr></table>"""
        else """<div class="header">
      <div class="name"><span class="dot"></span>$headerTitle</div>
      <div class="sub">ENV130<br>${esc(data.timestamp)}</div>
    </div>"""
        // Word는 CSS width:100%를 무시하고 원본 px로 그림 → A4 안에 들어오게 width 속성 지정
        val imgAttr = if (forWord) " width=\"620\"" else ""
        val commentBox = if (forWord)
            """<table width="100%" style="border-collapse:collapse;"><tr><td style="border:1.5px solid #C9CFD6; padding:14px; font-size:14px; height:60px; vertical-align:top;">${esc(data.comment)}</td></tr></table>"""
        else
            """<div class="comment">${esc(data.comment)}</div>"""

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
  table { border-collapse: collapse; width: 100%; font-size: 14px; border: 1.5px solid #C9CFD6; }
  th, td { padding: 11px 14px; text-align: left; border: 1px solid #D8DDE3; }
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
  .comment { border: 1.5px solid #C9CFD6; min-height: 90px; padding: 14px; font-size: 14px; font-weight: 600; color: #191F28; white-space: pre-wrap; }
  @media print { body { background: #FFF; padding: 0; } .sheet { box-shadow: none; } }
</style>
</head>
<body>
  <div class="sheet">
    $header
    <div class="body">
      <h2>Measurement</h2>
      <table class="kv2">$measurement</table>

      <h2>Parameter</h2>
      <table class="kv4">$settings</table>

      <h2>Echo</h2>
      <span class="badge real">Real</span>
      <img class="wave"$imgAttr src="$realSrc" alt="Real waveform">
      <div style="height:14px"></div>
      <span class="badge avg">Average</span>
      <img class="wave"$imgAttr src="$avgSrc" alt="Average waveform">

      <h2>Comment</h2>
      $commentBox
    </div>
  </div>
</body>
</html>"""
    }
}
