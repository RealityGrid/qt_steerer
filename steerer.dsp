# Microsoft Developer Studio Project File - Name="steerer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=steerer - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "steerer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "steerer.mak" CFG="steerer - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "steerer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "steerer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "steerer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "obj"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GX /O1 /I "C:\work\ReG\cvsreg\reg_steer_lib\include" /I "inc" /I "C:\work\ReG\cvsreg\reg_steer_lib\win32\include" /I "C:\work\ReG\qwt-0.4.1\include" /I "$(QTDIR)\include" /I "C:\work\ReG\cvsreg\reg_qt_steerer" /I "moc\\" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "DBG_VERSION" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 libxml2_a.lib iconv_a.lib oncrpcms.lib reg_steer_lib.lib "qt-mt.lib" "qtmain.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "C:\work\RealityGrid\qwt-0.4.1\Release\qwt.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" delayimp.lib /nologo /subsystem:windows /machine:IX86 /libpath:"C:\work\ReG\cvsreg\reg_steer_lib\release" /libpath:"C:\work\ReG\qwt-0.4.1\Release" /libpath:"C:\work\ReG\cvsreg\reg_steer_lib\win32\rpc\bin" /libpath:"$(QTDIR)\lib" /libpath:"C:\work\ReG\cvsreg\reg_steer_lib\win32\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "C:\work\ReG\cvsreg\reg_steer_lib\include" /I "inc" /I "C:\work\ReG\cvsreg\reg_steer_lib\win32\include" /I "C:\work\ReG\qwt-0.4.1\include" /I "$(QTDIR)\include" /I "C:\work\ReG\cvsreg\reg_qt_steerer" /I "moc\\" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "DBG_VERSION" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 libxml2_a.lib iconv_a.lib oncrpcms.lib reg_steer_lib.lib "qt-mt333.lib" "qtmain.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "C:\work\ReG\qwt-0.4.1\Release\qwt.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" /nologo /subsystem:windows /debug /machine:IX86 /nodefaultlib:"MSVCRT.lib" /pdbtype:sept /libpath:"C:\work\ReG\cvsreg\reg_steer_lib\debug" /libpath:"C:\work\ReG\qwt-0.4.1\Debug" /libpath:"C:\work\ReG\cvsreg\reg_steer_lib\win32\rpc\bin" /libpath:"$(QTDIR)\lib" /libpath:"C:\work\ReG\cvsreg\reg_steer_lib\win32\lib"

!ENDIF 

# Begin Target

# Name "steerer - Win32 Release"
# Name "steerer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=src\application.cpp
# End Source File
# Begin Source File

SOURCE=src\attachform.cpp
# End Source File
# Begin Source File

SOURCE=src\chkptform.cpp
# End Source File
# Begin Source File

SOURCE=src\chkptvariableform.cpp
# End Source File
# Begin Source File

SOURCE=src\commsthread.cpp
# End Source File
# Begin Source File

SOURCE=src\configform.cpp
# End Source File
# Begin Source File

SOURCE=src\controlform.cpp
# End Source File
# Begin Source File

SOURCE=src\exception.cpp
# End Source File
# Begin Source File

SOURCE=src\historyplot.cpp
# End Source File
# Begin Source File

SOURCE=src\iotype.cpp
# End Source File
# Begin Source File

SOURCE=src\iotypetable.cpp
# End Source File
# Begin Source File

SOURCE=src\parameter.cpp
# End Source File
# Begin Source File

SOURCE=src\parameterhistory.cpp
# End Source File
# Begin Source File

SOURCE=src\parametertable.cpp
# End Source File
# Begin Source File

SOURCE=src\steerer.cpp
# End Source File
# Begin Source File

SOURCE=src\steerermainwindow.cpp
# End Source File
# Begin Source File

SOURCE=src\table.cpp
# End Source File
# Begin Source File

SOURCE=src\utility.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=inc\application.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__APPLI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\application.h...
InputPath=inc\application.h

"moc\moc_application.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\application.h -o moc\moc_application.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__APPLI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\application.h...
InputPath=inc\application.h

"moc\moc_application.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\application.h -o moc\moc_application.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\attachform.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__ATTAC="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\attachform.h...
InputPath=inc\attachform.h

"moc\moc_attachform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\attachform.h -o moc\moc_attachform.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__ATTAC="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\attachform.h...
InputPath=inc\attachform.h

"moc\moc_attachform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\attachform.h -o moc\moc_attachform.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\chkptform.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__CHKPT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\chkptform.h...
InputPath=inc\chkptform.h

"moc\moc_chkptform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\chkptform.h -o moc\moc_chkptform.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__CHKPT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\chkptform.h...
InputPath=inc\chkptform.h

"moc\moc_chkptform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\chkptform.h -o moc\moc_chkptform.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\chkptvariableform.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__CHKPTV="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\chkptvariableform.h...
InputPath=inc\chkptvariableform.h

"moc\moc_chkptvariableform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\chkptvariableform.h -o moc\moc_chkptvariableform.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__CHKPTV="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\chkptvariableform.h...
InputPath=inc\chkptvariableform.h

"moc\moc_chkptvariableform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\chkptvariableform.h -o moc\moc_chkptvariableform.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\commsthread.h
# End Source File
# Begin Source File

SOURCE=inc\configform.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__CONFI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\configform.h...
InputPath=inc\configform.h

"moc\moc_configform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\configform.h -o moc\moc_configform.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__CONFI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\configform.h...
InputPath=inc\configform.h

"moc\moc_configform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\configform.h -o moc\moc_configform.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\controlform.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__CONTR="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\controlform.h...
InputPath=inc\controlform.h

"moc\moc_controlform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\controlform.h -o moc\moc_controlform.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__CONTR="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\controlform.h...
InputPath=inc\controlform.h

"moc\moc_controlform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\controlform.h -o moc\moc_controlform.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\debug.h
# End Source File
# Begin Source File

SOURCE=inc\exception.h
# End Source File
# Begin Source File

SOURCE=inc\historyplot.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__HISTO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\historyplot.h...
InputPath=inc\historyplot.h

"moc\moc_historyplot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\historyplot.h -o moc\moc_historyplot.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__HISTO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\historyplot.h...
InputPath=inc\historyplot.h

"moc\moc_historyplot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\historyplot.h -o moc\moc_historyplot.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\iotype.h
# End Source File
# Begin Source File

SOURCE=inc\iotypetable.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__IOTYP="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\iotypetable.h...
InputPath=inc\iotypetable.h

"moc\moc_iotypetable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\iotypetable.h -o moc\moc_iotypetable.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__IOTYP="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\iotypetable.h...
InputPath=inc\iotypetable.h

"moc\moc_iotypetable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\iotypetable.h -o moc\moc_iotypetable.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\parameter.h
# End Source File
# Begin Source File

SOURCE=inc\parameterhistory.h
# End Source File
# Begin Source File

SOURCE=inc\parametertable.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__PARAM="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\parametertable.h...
InputPath=inc\parametertable.h

"moc\moc_parametertable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\parametertable.h -o moc\moc_parametertable.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__PARAM="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\parametertable.h...
InputPath=inc\parametertable.h

"moc\moc_parametertable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\parametertable.h -o moc\moc_parametertable.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\steerermainwindow.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__STEER="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\steerermainwindow.h...
InputPath=inc\steerermainwindow.h

"moc\moc_steerermainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\steerermainwindow.h -o moc\moc_steerermainwindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__STEER="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\steerermainwindow.h...
InputPath=inc\steerermainwindow.h

"moc\moc_steerermainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\steerermainwindow.h -o moc\moc_steerermainwindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\table.h

!IF  "$(CFG)" == "steerer - Win32 Release"

USERDEP__TABLE="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\table.h...
InputPath=inc\table.h

"moc\moc_table.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\table.h -o moc\moc_table.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "steerer - Win32 Debug"

USERDEP__TABLE="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing inc\table.h...
InputPath=inc\table.h

"moc\moc_table.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc inc\table.h -o moc\moc_table.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=inc\types.h
# End Source File
# Begin Source File

SOURCE=inc\utility.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=moc\moc_application.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_attachform.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_chkptform.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_chkptvariableform.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_configform.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_controlform.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_historyplot.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_iotypetable.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_parametertable.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_steerermainwindow.cpp
# End Source File
# Begin Source File

SOURCE=moc\moc_table.cpp
# End Source File
# End Group
# End Target
# End Project
