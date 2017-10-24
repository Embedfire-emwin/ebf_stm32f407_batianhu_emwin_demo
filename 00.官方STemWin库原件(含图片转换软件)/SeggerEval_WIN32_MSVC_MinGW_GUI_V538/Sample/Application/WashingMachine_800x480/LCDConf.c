#include <stddef.h>
#include <stdlib.h>

#include "GUI.h"

/*********************************************************************
*
*       Typedefs
*
**********************************************************************
*/
typedef struct {
  int NumBuffers;
  const GUI_DEVICE_API * pDriver;
  const LCD_API_COLOR_CONV * pColorConv;
  int xSize, ySize;
  int xPos, yPos;
} INIT_LAYER;

typedef struct {
  int NumLayers;
  INIT_LAYER aLayer[GUI_NUM_LAYERS];
} INIT_APP;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
//
// Array for initializing layers for applications. Do not change order
//
static INIT_APP _InitApp = {
  //
  // WashingMachine
  //
  4,  // Number of layers
  {
    { 3, GUIDRV_WIN32, GUICC_8888, 800, 480,   0,   0, },
    { 3, GUIDRV_WIN32, GUICC_8888, 760, 400,  22,  30, },
    { 3, GUIDRV_WIN32, GUICC_8888, 776, 173,  12, 153, },
    { 3, GUIDRV_WIN32, GUICC_8888, 151,  40, 625, 325, },
  }
};

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCDCONF_GetLayerPos
*/
void LCDCONF_GetLayerPos(int LayerIndex, int * pxPos, int * pyPos) {
  if (LayerIndex < _InitApp.NumLayers) {
    if (pxPos) {
      *pxPos = _InitApp.aLayer[LayerIndex].xPos;
    }
    if (pyPos) {
      *pyPos = _InitApp.aLayer[LayerIndex].yPos;
    }
  }
}

/*********************************************************************
*
*       LCD_X_Config
*/
void LCD_X_Config(void) {
  int i;
  GUI_DEVICE * pDevice;

  //
  // Multiple buffer configuration, should be the first thing here in that routine
  //
  for (i = 0; i < _InitApp.NumLayers; i++) {
    if (_InitApp.aLayer[i].NumBuffers > 1) {
      GUI_MULTIBUF_ConfigEx(i, _InitApp.aLayer[i].NumBuffers);
    }
  }
  //
  // Set display driver and color conversion for layers
  //
  for (i = 0; i < _InitApp.NumLayers; i++) {
    pDevice = GUI_DEVICE_CreateAndLink(_InitApp.aLayer[i].pDriver, _InitApp.aLayer[i].pColorConv, 0, i);
    if (pDevice == NULL) {
      while (1); // Error
    }
    if (LCD_GetSwapXYEx(i)) {
      LCD_SetSizeEx (i, _InitApp.aLayer[i].ySize, _InitApp.aLayer[i].xSize);
      LCD_SetVSizeEx(i, _InitApp.aLayer[i].ySize, _InitApp.aLayer[i].xSize);
    } else {
      LCD_SetSizeEx (i, _InitApp.aLayer[i].xSize, _InitApp.aLayer[i].ySize);
      LCD_SetVSizeEx(i, _InitApp.aLayer[i].xSize, _InitApp.aLayer[i].ySize);
    }
    LCD_SetPosEx(i, _InitApp.aLayer[i].xPos, _InitApp.aLayer[i].yPos);
    LCD_SetVisEx(i, 1);
  }
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  return 0;
}
/*************************** End of file ****************************/
