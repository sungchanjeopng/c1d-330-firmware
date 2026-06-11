package com.wws2.densitymeter.ui.component

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.wws2.densitymeter.ui.theme.AppColors
import com.wws2.densitymeter.ui.theme.isWideLayout

// Tab indices are kept stable (Main=0, Echo=1, Param=3, Menu=4) so the
// ViewModel's tabIndex-based logic (heartbeat page selection, etc.) stays
// untouched; the Trend tab (index 2) is simply not offered any more.
private val tabs = listOf(
    "Main" to 0,
    "Echo" to 1,
    "Param" to 3,
    "Menu" to 4,
)

@Composable
fun BottomNavBar(
    currentIndex: Int,
    onTap: (Int) -> Unit,
) {
    val wide = isWideLayout
    Column(modifier = Modifier.fillMaxWidth()) {
        HorizontalDivider(
            thickness = 1.dp,
            color = AppColors.GrayLabel.copy(alpha = 0.5f),
        )
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 6.dp)
                .padding(top = if (wide) 8.dp else 6.dp, bottom = if (wide) 12.dp else 10.dp),
        ) {
            tabs.forEach { (label, i) ->
                val active = i == currentIndex
                Box(
                    modifier = Modifier
                        .weight(1f)
                        .clickable { onTap(i) }
                        .padding(vertical = if (wide) 10.dp else 6.dp),
                    contentAlignment = Alignment.Center,
                ) {
                    Text(
                        text = label,
                        textAlign = TextAlign.Center,
                        fontSize = if (wide) 18.sp else 15.sp,
                        fontWeight = if (active) FontWeight.W900 else FontWeight.W700,
                        color = if (active) AppColors.Primary else AppColors.GrayLabel,
                        letterSpacing = (-0.3).sp,
                        maxLines = 1,
                        softWrap = false,
                    )
                }
            }
        }
    }
}
