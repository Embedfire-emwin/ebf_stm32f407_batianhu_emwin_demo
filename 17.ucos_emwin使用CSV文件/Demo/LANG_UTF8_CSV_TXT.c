/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.18 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LANG_UTF8_CSV_TXT.c
Purpose     : Sample application which shows how language resource files
              can be used to display different languages using UTF-8.

              If executed in the emWin simulation (WIN32) environment,
              the application creates a CSV file and a text file in the
              same location the executable lies in. These files then
              are used to display texts written in different languages.

              If  executed on target hardware the application uses the
              arrays containing the file data directly.
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#ifndef SKIP_TEST

/*********************************************************************
*
*       Includes
*
**********************************************************************
*/
#ifdef WIN32
  #include <windows.h>   // File handling
#endif

#include <stdio.h>       // Definition of NULL
#include "GUI.h"
#include "GUI_Debug.h"
#include "includes.h"
#include "GUICSV_Port.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define HEIGHT_HEADLINE_1  40
#define HEIGHT_HEADLINE_2  20
#define HEIGHT_TOP         (HEIGHT_HEADLINE_1 + HEIGHT_HEADLINE_2)
#define NUM_LANGUAGES      3
#define NUM_ITEMS          5
#define NUM_FILES          2
#define BORDER_SIZE        2
#define XSIZE_MAX          480
#define YSIZE_MAX          360
#define XSIZE_MIN          320
#define YSIZE_MIN          240

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 10)


/*********************************************************************
*
*       Language context
*
**********************************************************************
*/
typedef struct {
  const GUI_FONT * pFontHeadline;
  const GUI_FONT * pFontBody;
} LANG_CONTEXT;

static LANG_CONTEXT _Languages[] = {
  { &GUI_Font16B_1,                &GUI_Font16_1             },
  { &GUI_Font16B_1,                &GUI_Font16_1             },
  { &FONT_XINSONGTI_19, &FONT_XINSONGTI_19 }
};

/*********************************************************************
*
*       _GetData
*/
#ifdef WIN32
static int _GetData(void * pVoid, const U8 ** ppData, unsigned NumBytes, U32 Off) {
  RES_FILE_NAME * pFileRes;
  HANDLE          hFile;
  DWORD           NumBytesRead;
  char            acDrive[_MAX_DRIVE];
  char            acPath[_MAX_PATH];
  char            acDir[_MAX_DIR];
  U8            * pData;

  pData    = (U8 *)*ppData;
  pFileRes = (RES_FILE_NAME *)pVoid;
  GetModuleFileName(NULL, acPath, FILENAME_MAX);
  _splitpath(acPath, acDrive, acDir, NULL, NULL);
  _makepath(acPath, acDrive, acDir, pFileRes->pFileName, pFileRes->pExt);
  hFile = CreateFile(acPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (SetFilePointer(hFile, Off, 0, FILE_BEGIN) == 0xFFFFFFFF) {
    CloseHandle(hFile);
    return 0;
  }
  if (!ReadFile(hFile, pData, NumBytes, &NumBytesRead, 0)) {
    CloseHandle(hFile);
    return 0;
  }
  if (NumBytesRead > NumBytes) {
    CloseHandle(hFile);
    return 0;
  }
  CloseHandle(hFile);
  return NumBytesRead;
}
#endif

/*********************************************************************
*
*       _CreateLanguageResources
*/
#ifdef WIN32
static void _CreateLanguageResources(void) {
  HANDLE hFile;
  char   acFileName[_MAX_FNAME];
  char   acDrive[_MAX_DRIVE];
  char   acPath[_MAX_PATH];
  char   acDir[_MAX_DIR];
  char   acExt[_MAX_EXT];
  U32    NumBytesWritten;
  U8     FileCnt;

  GetModuleFileName(NULL, acPath, FILENAME_MAX);
  _splitpath(acPath, acDrive, acDir, acFileName, acExt);
  for (FileCnt = 0; FileCnt < NUM_FILES; FileCnt++) {
    sprintf(acFileName, _ResFileName[FileCnt].pFileName);
    sprintf(acExt,      _ResFileName[FileCnt].pExt);
    _makepath(acPath, acDrive, acDir, acFileName, acExt);
    hFile = CreateFile(acPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
      GUI_DEBUG_ERROROUT("File could not be created.");
    }
    WriteFile(hFile, _ResFileData[FileCnt].pData, _ResFileData[FileCnt].NumBytes, &NumBytesWritten, NULL);
    CloseHandle(hFile);
    if (_ResFileData[FileCnt].NumBytes != NumBytesWritten) {
      GUI_DEBUG_ERROROUT("File was not written properly.");
    }
  }
}
#endif

/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  const char * pText;
  GUI_RECT     Rect;
  U16          xSizeScreen;
  U16          ySizeScreen;
  U16          xSizeItem;
  U16          ySizeItem;
  U16          xSize;
  U16          ySize;
  U16          xOff;
  U16          yOff;
  U8           NumLanguagesCSV;
  U8           ItemCnt;
  U8           LangCnt;

  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  GUI_UC_SetEncodeUTF8();
  xSizeScreen = LCD_GetXSize();
  ySizeScreen = LCD_GetYSize();
  if ((xSizeScreen < XSIZE_MIN) || (ySizeScreen < YSIZE_MIN)) {
    GUI_DEBUG_ERROROUT("Inaccurate display size.");
  }
  //
  // Stretch the sample up to a size of #define XSIZE_MAX * YSIZE_MAX.
  // Above that display size the sample is centered on the according axis.
  //
  if (xSizeScreen > XSIZE_MAX) {
    xOff  = (xSizeScreen - XSIZE_MAX) / 2;
    xSize = XSIZE_MAX;
  } else {
    xOff  = 0;
    xSize = xSizeScreen;
  }
  if (ySizeScreen > YSIZE_MAX) {
    yOff  = (ySizeScreen - YSIZE_MAX) / 2;
    ySize = YSIZE_MAX;
  } else {
    yOff  = 0;
    ySize = ySizeScreen;
  }
#ifdef WIN32
  _CreateLanguageResources();
#endif
  //
  // Draw background
  //
  xSizeItem = xSize / NUM_LANGUAGES - 6;
  ySizeItem = (ySize - HEIGHT_TOP) / NUM_ITEMS;
  GUI_SetBkColor(GUI_DARKBLUE);
  GUI_Clear();
  GUI_DrawGradientV(0, HEIGHT_TOP, xSizeScreen - 1, ySizeScreen - 1, GUI_DARKBLUE, GUI_DARKGRAY);
  //
  // Draw frames
  //
  Rect.x0 = xOff + BORDER_SIZE;
  Rect.x1 = xOff + xSizeItem * 2 - BORDER_SIZE;
  Rect.y0 = yOff + HEIGHT_TOP + BORDER_SIZE;
  Rect.y1 = ySizeScreen - yOff - BORDER_SIZE - 1;
  GUI_DrawRectEx(&Rect);
  GUI_DrawHLine(Rect.y0 + ySizeItem, Rect.x0, Rect.x1);
  Rect.x0 = xOff + xSizeItem * 2 + BORDER_SIZE;
  Rect.x1 = xSizeScreen - xOff - BORDER_SIZE;
  GUI_DrawRectEx(&Rect);
  GUI_DrawHLine(Rect.y0 + ySizeItem, Rect.x0, Rect.x1);
  //
  // Display headlines
  //
  GUI_SetFont(&GUI_Font20B_ASCII);
  GUI_SetTextMode(GUI_TM_TRANS);
  Rect.x0 = xOff;
  Rect.x1 = xOff + xSize - 1;
  Rect.y0 = yOff;
  Rect.y1 = yOff + HEIGHT_HEADLINE_1 - 1;
  GUI_DispStringInRect("emWin - Language Resources", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  GUI_SetFont(&GUI_Font13B_ASCII);
  Rect.x1 = xOff + xSizeItem * 2 - 1;
  Rect.y0 = yOff + HEIGHT_HEADLINE_1;
  Rect.y1 = yOff + HEIGHT_TOP    - 1;
  GUI_DispStringInRect("CSV file", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  Rect.x0 = Rect.x1;
  Rect.x1 = xSizeScreen - xOff - 1;
  GUI_DispStringInRect("Text file", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  //
  // Load resource files
  //
  GUI_LANG_SetMaxNumLang(NUM_LANGUAGES);
	Create_LANG_CSV();
//#ifdef WIN32
//  NumLanguagesCSV = GUI_LANG_LoadCSVEx(_GetData, &_ResFileName[0]);
//  GUI_LANG_LoadTextEx(_GetData, &_ResFileName[1], NumLanguagesCSV);
//#else
//  NumLanguagesCSV = GUI_LANG_LoadCSV((unsigned char*)_acGUI_LANG_CSV_Ger_Eng, sizeof(_acGUI_LANG_CSV_Ger_Eng));
//  GUI_LANG_LoadText((unsigned char*)_acGUI_LANG_TXT_Jap, sizeof(_acGUI_LANG_TXT_Jap), NumLanguagesCSV);
//#endif
  //
  // Display the resource text in the calculated rectangle
  // using the according font for headline and body.
  //
  for (LangCnt = 0; LangCnt < NUM_LANGUAGES; LangCnt++) {
    Rect.x0 = xOff + LangCnt * xSizeItem + ((LangCnt == (NUM_LANGUAGES - 1)) ? 5 : 0);
    Rect.x1 = (LangCnt == (NUM_LANGUAGES - 1)) ? xSize - 1 : Rect.x0 + xSizeItem;
    for (ItemCnt = 0; ItemCnt < NUM_ITEMS; ItemCnt++) {
      pText   = GUI_LANG_GetTextEx(ItemCnt, LangCnt);
      Rect.y0 = yOff + HEIGHT_TOP + (ItemCnt * ySizeItem);
      Rect.y1 = Rect.y0 + ySizeItem;
      if (ItemCnt == 0) {
        GUI_SetFont(_Languages[LangCnt].pFontHeadline);
      } else {
        GUI_SetFont(_Languages[LangCnt].pFontBody);
      }
      GUI_DispStringInRect(pText, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
  }
  while (1) {
    GUI_Delay(100);
  }
}

#endif

/*************************** End of file ****************************/
