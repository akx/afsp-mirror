# Microsoft Developer Studio Project File - Name="ResampAudio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ResampAudio - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ResampAudio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ResampAudio.mak" CFG="ResampAudio - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ResampAudio - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../bin/ResampAudio.exe"
# Begin Target

# Name "ResampAudio - Win32 Release"
# Begin Source File

SOURCE=..\..\audio\ResampAudio\ResampAudio.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\ResampAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSexpTime.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSfiltSpec.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSincTime.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSinterp.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSintFilt.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSKaiserLPF.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSoptions.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSratio.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSrefresh.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSresamp.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSuKaiser.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\ResampAudio\RSwriteCof.c
# End Source File
# End Target
# End Project
