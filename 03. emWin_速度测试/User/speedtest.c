/**
  ******************************************************************************
  * @file    speedtest.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emWin速度测试Demo
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "GUI.h"
#include "DIALOG.h"
#include "stdio.h"

static const GUI_COLOR _aColor[8] = {
  0x000000, 
  0x0000FF, 
  0x00FF00, 
  0x00FFFF, 
  0xFF0000, 
  0xFF00FF, 
  0xFFFF00, 
  0xFFFFFF
};

U32 Stop_Test = 0;

/**
  * @brief  Return pixels per second rate
  * @param  None
  * @retval U32
  */
static U32 _GetPixelsPerSecond(void) {
  GUI_COLOR Color, BkColor;
  U32 x0, y0, x1, y1, xSize, ySize;
  I32 t, t0;
  U32 Cnt, PixelsPerSecond, PixelCnt;
  
  /* Find an area which is not obstructed by any windows */
  xSize   = LCD_GetXSize();
  ySize   = LCD_GetYSize();
  Cnt     = 0;
  x0      = 0;
  x1      = xSize - 1;
  y0      = 65;
  y1      = ySize - 60 - 1;
  Color   = GUI_GetColor();
  BkColor = GUI_GetBkColor();
  GUI_SetColor(BkColor);
  
  /* Repeat fill as often as possible in 100 ms */
  t0 = GUI_GetTime();
  do {
    GUI_FillRect(x0, y0, x1, y1);
    Cnt++;
    t = GUI_GetTime();    
  } while ((t - (t0 + 100)) <= 0);
  
  /* Compute result */
  t -= t0;
  PixelCnt = (x1 - x0 + 1) * (y1 - y0 + 1) * Cnt;
  PixelsPerSecond = PixelCnt / t * 1000;   
  GUI_SetColor(Color);
  return PixelsPerSecond;
}

/**
  * @brief  Run the speed test
  * @param  None
  * @retval int
  */
int Run_SpeedTest(void) {
  int      TimeStart, i;
  U32      PixelsPerSecond;
  unsigned aColorIndex[8];
  int      xSize, ySize, vySize;
  GUI_RECT Rect, ClipRect;
  xSize  = LCD_GetXSize();
  ySize  = LCD_GetYSize();
  vySize = LCD_GetVYSize();
#if GUI_SUPPORT_CURSOR
  GUI_CURSOR_Hide();
#endif
  if (vySize > ySize)
  {
    ClipRect.x0 = 0;
    ClipRect.y0 = 0;
    ClipRect.x1 = xSize;
    ClipRect.y1 = ySize;
    GUI_SetClipRect(&ClipRect);
  }
  
  Stop_Test = 0;
  
  for (i = 0; i< 8; i++)
  {
    aColorIndex[i] = GUI_Color2Index(_aColor[i]);
  }  
  TimeStart = GUI_GetTime();
  for (i = 0; ((GUI_GetTime() - TimeStart) < 5000) &&( Stop_Test == 0); i++)
  {
    GUI_SetColorIndex(aColorIndex[i&7]);
    
    /* Calculate random positions */
    Rect.x0 = (GUI_GetTime()/(i%25)) % xSize - xSize / 2;
    Rect.y0 = (GUI_GetTime()/(i%25)) % ySize - ySize / 2;
    Rect.x1 = Rect.x0 + 20 + (GUI_GetTime()/(i%25)) % xSize;
    Rect.y1 = Rect.y0 + 20 + (GUI_GetTime()/(i%25)) % ySize;

    GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);
    
    /* Clip rectangle to visible area and add the number of pixels (for speed computation) */
    if (Rect.x1 >= xSize)
    {
      Rect.x1 = xSize - 1;
    }
    
    if (Rect.y1 >= ySize)
    {
      Rect.y1 = ySize - 1;
    }
    
    if (Rect.x0 < 0 )
    {
      Rect.x0 = 0;
    }
    
    if (Rect.y1 < 0)
    {
      Rect.y1 = 0;
    }
    
    GUI_Exec();
    
    /* Allow short breaks so we do not use all available CPU time ... */
  }
  PixelsPerSecond = _GetPixelsPerSecond();
  GUI_SetClipRect(NULL);
  return PixelsPerSecond;
}

void SpeedTest(void)
{
	char temp[50]={0};
	int cpu_speed = 0;
	Stop_Test = 0;
	cpu_speed = Run_SpeedTest();
	
	GUI_Clear();
	sprintf(temp, "%d  Pixels/s ", cpu_speed); 
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_32B_ASCII);
	GUI_DispStringHCenterAt(temp,LCD_GetXSize()/2,LCD_GetYSize()/2-16);
}
