# Microsoft Developer Studio Project File - Name="PQevalAudio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PQevalAudio - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PQevalAudio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PQevalAudio.mak" CFG="PQevalAudio - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PQevalAudio - Win32 Release" (based on "Win32 (x86) Console Application")
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
# ADD CPP /nologo /W3 /GX /O2 /I "../../include" /I "../../audio/PQevalAudio" /I "../../audio/PQevalAudio/CB" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../bin/PQevalAudio.exe"
# Begin Target

# Name "PQevalAudio - Win32 Release"
# Begin Group "CB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\CB\PQCB.h
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\CB\PQgroupCB.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\CB\PQspreadCB.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\CB\PQtimeSpread.c
# End Source File
# End Group
# Begin Group "MOV"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQframeMOV.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQloudTest.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQmovBW.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQmovEHS.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQmovModDiffB.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQmovNLoudB.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQmovNMRB.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQmovPD.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQprtMOV.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\MOV\PQprtMOVCi.c
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Misc\PQdataBoundary.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Misc\PQgenTables.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Misc\PQinitFmem.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Misc\PQoptions.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Misc\PQreadChan.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Misc\PQtableFn.c
# End Source File
# End Group
# Begin Group "Patt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Patt\PQadapt.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Patt\PQloud.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\Patt\PQmodPatt.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\PQeval.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\PQevalAudio.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\PQevalAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\PQnNet.c
# End Source File
# Begin Source File

SOURCE=..\..\audio\PQevalAudio\PQnNet.h
# End Source File
# End Target
# End Project
