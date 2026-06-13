package com.wws2.densitymeter.domain

import android.content.Context
import com.wws2.densitymeter.model.InterfaceEchoReading
import com.wws2.densitymeter.model.ReportData
import org.json.JSONArray
import org.json.JSONObject
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

/** ReportResult 화면을 나중에 다시 열 수 있도록 ReportData 스냅샷을 JSON으로 저장/복원. */
object ReportSnapshotStore {

    fun dir(context: Context) = File(context.filesDir, "report_snapshots")

    fun save(context: Context, data: ReportData): File {
        val name = data.title.ifBlank { data.label }
        val safeName = name.replace(Regex("[\\\\/:*?\"<>|]"), "_")
        val stamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).format(Date())
        val file = File(dir(context).apply { mkdirs() }, "Report_${safeName}_$stamp.json")
        file.writeText(toJson(data).toString())
        return file
    }

    fun list(context: Context): List<File> =
        dir(context).listFiles { f -> f.extension.equals("json", ignoreCase = true) }
            ?.sortedByDescending { it.lastModified() } ?: emptyList()

    fun load(file: File): ReportData? = try {
        fromJson(JSONObject(file.readText()))
    } catch (e: Exception) {
        null
    }

    /** 기존 스냅샷 파일 갱신 (comment 수정 등). */
    fun update(file: File, data: ReportData) {
        try {
            file.writeText(toJson(data).toString())
        } catch (_: Exception) {
        }
    }

    private fun toJson(d: ReportData) = JSONObject().apply {
        put("title", d.title)
        put("comment", d.comment)
        put("deviceId", d.deviceId); put("label", d.label)
        put("firmwareVersion", d.firmwareVersion); put("timestamp", d.timestamp)
        put("lightLevel", d.lightLevel); put("heavyLevel", d.heavyLevel)
        put("temperatureC", d.temperatureC); put("currentMA", d.currentMA)
        put("freqMHz", d.freqMHz); put("offset", d.offset)
        put("emptyDistance", d.emptyDistance); put("deadZone", d.deadZone)
        put("set4mA", d.set4mA); put("set20mA", d.set20mA); put("damping", d.damping)
        put("thrLightSet", d.thrLightSet); put("thrLightMode", d.thrLightMode)
        put("thrHeavySet", d.thrHeavySet); put("thrHeavyMode", d.thrHeavyMode)
        put("echoAmp", d.echoAmp); put("relay", d.relay)
        d.realEcho?.let { put("realEcho", echoToJson(it)) }
        d.avgEcho?.let { put("avgEcho", echoToJson(it)) }
    }

    private fun fromJson(o: JSONObject) = ReportData(
        title = o.optString("title"),
        comment = o.optString("comment"),
        deviceId = o.optString("deviceId"),
        label = o.optString("label"),
        firmwareVersion = o.optString("firmwareVersion"),
        timestamp = o.optString("timestamp"),
        lightLevel = o.optDouble("lightLevel", 0.0),
        heavyLevel = o.optDouble("heavyLevel", 0.0),
        temperatureC = o.optDouble("temperatureC", 0.0),
        currentMA = o.optDouble("currentMA", 0.0),
        freqMHz = o.optDouble("freqMHz", 0.0),
        offset = o.optDouble("offset", 0.0),
        emptyDistance = o.optDouble("emptyDistance", 0.0),
        deadZone = o.optDouble("deadZone", 0.0),
        set4mA = o.optDouble("set4mA", 0.0),
        set20mA = o.optDouble("set20mA", 0.0),
        damping = o.optInt("damping"),
        thrLightSet = o.optInt("thrLightSet"),
        thrLightMode = o.optInt("thrLightMode"),
        thrHeavySet = o.optInt("thrHeavySet"),
        thrHeavyMode = o.optInt("thrHeavyMode"),
        echoAmp = o.optInt("echoAmp"),
        relay = o.optInt("relay"),
        realEcho = o.optJSONObject("realEcho")?.let { echoFromJson(it) },
        avgEcho = o.optJSONObject("avgEcho")?.let { echoFromJson(it) },
    )

    private fun echoToJson(e: InterfaceEchoReading) = JSONObject().apply {
        put("lightLevel", e.lightLevel); put("heavyLevel", e.heavyLevel)
        put("deadzone", e.deadzone); put("empty", e.empty)
        put("thrLightDist", e.thrLightDist); put("thrHeavyDist", e.thrHeavyDist)
        put("thrLightReal", e.thrLightReal); put("thrHeavyReal", e.thrHeavyReal)
        put("thrLightSet", e.thrLightSet); put("thrHeavySet", e.thrHeavySet)
        put("thrLightMode", e.thrLightMode); put("thrHeavyMode", e.thrHeavyMode)
        put("echoAmp", e.echoAmp); put("statusCh", e.statusCh)
        put("temperature", e.temperature)
        put("wave", JSONArray(e.wave))
    }

    private fun echoFromJson(o: JSONObject): InterfaceEchoReading {
        val waveArr = o.optJSONArray("wave") ?: JSONArray()
        return InterfaceEchoReading(
            lightLevel = o.optDouble("lightLevel", 0.0),
            heavyLevel = o.optDouble("heavyLevel", 0.0),
            deadzone = o.optInt("deadzone"),
            empty = o.optInt("empty"),
            thrLightDist = o.optInt("thrLightDist"),
            thrHeavyDist = o.optInt("thrHeavyDist"),
            thrLightReal = o.optInt("thrLightReal"),
            thrHeavyReal = o.optInt("thrHeavyReal"),
            thrLightSet = o.optInt("thrLightSet"),
            thrHeavySet = o.optInt("thrHeavySet"),
            thrLightMode = o.optInt("thrLightMode"),
            thrHeavyMode = o.optInt("thrHeavyMode"),
            echoAmp = o.optInt("echoAmp"),
            statusCh = o.optInt("statusCh"),
            temperature = o.optInt("temperature"),
            wave = (0 until waveArr.length()).map { waveArr.optInt(it) },
        )
    }
}
