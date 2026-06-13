package com.wws2.densitymeter.ui.component

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import com.wws2.densitymeter.ui.theme.AppColors

/** 다운로드/리포트 시작 전 제목 입력. 비워두면 기기 라벨이 사용된다. */
@Composable
fun TitleInputDialog(
    heading: String,
    confirmText: String,
    onDismiss: () -> Unit,
    onConfirm: (String) -> Unit,
) {
    var text by remember { mutableStateOf("") }
    Dialog(onDismissRequest = onDismiss) {
        Column(
            modifier = Modifier
                .clip(RoundedCornerShape(16.dp))
                .background(AppColors.White)
                .padding(20.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
        ) {
            Text(heading, fontSize = 20.sp, fontWeight = FontWeight.W700, color = AppColors.DarkText)
            Spacer(Modifier.height(16.dp))
            OutlinedTextField(
                value = text,
                onValueChange = { text = it },
                modifier = Modifier.fillMaxWidth(),
                singleLine = true,
                shape = RoundedCornerShape(8.dp),
                label = { Text("Title (optional)") },
                supportingText = { Text("Leave empty to use the device name") },
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
                    text = confirmText,
                    background = AppColors.Primary,
                    contentColor = AppColors.White,
                    modifier = Modifier.weight(1f),
                    onClick = { onConfirm(text.trim()) },
                )
            }
        }
    }
}
