# Microsoft Developer Studio Project File - Name="libAO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libAO - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libAO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libAO.mak" CFG="libAO - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libAO - Win32 Release" (based on "Win32 (x86) Static Library")
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
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x1009
# ADD RSC /l 0x1009
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libAO.lib"
# Begin Target

# Name "libAO - Win32 Release"
# Begin Source File

SOURCE=..\..\libAO\AOdecFI.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOdecFO.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOdecHelp.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOdecOpt.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOnFrame.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOsetDformat.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOsetFIopt.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOsetFOopt.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOsetFormat.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOsetFtype.c
# End Source File
# Begin Source File

SOURCE=..\..\libAO\AOstdin.c
# End Source File
# End Target
# End Project
