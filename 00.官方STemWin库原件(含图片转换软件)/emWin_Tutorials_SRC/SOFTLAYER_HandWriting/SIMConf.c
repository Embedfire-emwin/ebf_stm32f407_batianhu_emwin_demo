/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : SIMConf.c
Purpose     : Simulation configuration for softlayers.
---------------------------END-OF-HEADER------------------------------
*/

#ifdef WIN32

#include <windows.h>

#include "LCD_SIM.h"
#include "GUI_SIM_Win32.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       SIM_X_Config
*
* Function description
*   Called during the initialization process in order to configure
*   the simulator.
*/
void SIM_X_Config() {
  SIM_GUI_SetCompositeSize(480, 272);  // Set size of composite window
  SIM_GUI_SetTransMode(1, GUI_TRANSMODE_PIXELALPHA);
  SIM_GUI_SetTransMode(2, GUI_TRANSMODE_PIXELALPHA);
  SIM_GUI_SetTransMode(3, GUI_TRANSMODE_PIXELALPHA);
  SIM_GUI_ShowDevice(1);
}

#else

void SIMConf_C(void); // Avoid empty object files
void SIMConf_C(void) {}

#endif

/*************************** End of file ****************************/
