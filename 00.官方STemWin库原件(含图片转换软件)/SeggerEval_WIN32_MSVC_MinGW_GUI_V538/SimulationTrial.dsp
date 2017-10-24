# Microsoft Developer Studio Project File - Name="SimulationTrial" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SimulationTrial - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SimulationTrial.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SimulationTrial.mak" CFG="SimulationTrial - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SimulationTrial - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SimulationTrial - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SimulationTrial - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Output\Release"
# PROP Intermediate_Dir "Output\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /O1 /I "GUI\Include" /I "Config" /D "NDEBUG" /D "TARGET_1375_C8_137X" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib shell32.lib winmm.lib libcmt.lib ws2_32.lib /nologo /subsystem:windows /pdb:none /map /machine:I386 /nodefaultlib /out:"Exe/GUISimulation.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "SimulationTrial - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Output\Debug"
# PROP Intermediate_Dir "Output\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "GUI\Include" /I "Config" /D "_DEBUG" /D "TARGET_1375_C8_137X" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib shell32.lib winmm.lib libcmt.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"Exe/GUISimulationDebug.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SimulationTrial - Win32 Release"
# Name "SimulationTrial - Win32 Debug"
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Application\Background_480x272.c
# End Source File
# Begin Source File

SOURCE=.\Application\BackgroundWindow_480x272.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_BootLoader_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_Cloud_SSL_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_Compress_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_Connectivity_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_Cpp_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_FS_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_GUI_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_JLink_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_OS_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Icon_Production_64x64.c
# End Source File
# Begin Source File

SOURCE=.\Application\Separator_218x21.c
# End Source File
# Begin Source File

SOURCE=.\Application\SWIPELIST_Demo.c
# End Source File
# End Group
# Begin Group "Config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Config\GUIConf.c
# End Source File
# Begin Source File

SOURCE=.\Config\GUIConf.h
# End Source File
# Begin Source File

SOURCE=.\Config\LCDConf.c
# End Source File
# Begin Source File

SOURCE=.\Config\LCDConf.h
# End Source File
# Begin Source File

SOURCE=.\Config\SIMConf.c
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GUI\Include\BUTTON.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\CALENDAR.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\CHECKBOX.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\CHOOSECOLOR.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\CHOOSEFILE.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\DIALOG.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\DIALOG_Intern.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\DROPDOWN.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\EDIT.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\EDIT_Private.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\FRAMEWIN.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\Global.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GRAPH.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_ARRAY.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_ConfDefaults.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_Debug.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_FontIntern.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_HOOK.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_Private.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_SetOrientation.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_SetOrientationCX.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_SIM_Win32.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_Type.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_Version.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\GUI_VNC.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\HEADER.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\ICONVIEW.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\IMAGE.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\KNOB.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LCD.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LCD_ConfDefaults.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LCD_Private.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LCD_Protected.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LCD_SIM.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LISTBOX.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LISTVIEW.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\LISTWHEEL.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\MENU.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\MESSAGEBOX.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\MULTIEDIT.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\MULTIPAGE.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\PROGBAR.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\RADIO.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\SCROLLBAR.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\SIM.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\SIM_GetRegions.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\SLIDER.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\SPINBOX.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\TEXT.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\TREEVIEW.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\WIDGET.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\WM.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\WM_GUI.h
# End Source File
# Begin Source File

SOURCE=.\GUI\Include\WM_Intern.h
# End Source File
# End Group
# Begin Group "Library"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GUI\Library\GUI.lib
# End Source File
# End Group
# End Group
# Begin Group "Simulation"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=.\Simulation\Simulation.res
# End Source File
# Begin Source File

SOURCE=.\Simulation\WinMain.c
# End Source File
# Begin Source File

SOURCE=.\Simulation\GUISim.lib
# End Source File
# End Group
# End Target
# End Project
