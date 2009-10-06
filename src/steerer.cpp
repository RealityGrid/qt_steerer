/*
  The RealityGrid Steerer

  Copyright (c) 2002-2009, University of Manchester, United Kingdom.
  All rights reserved.

  This software is produced by Research Computing Services, University
  of Manchester as part of the RealityGrid project and associated
  follow on projects, funded by the EPSRC under grants GR/R67699/01,
  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
  EP/F00561X/1.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of The University of Manchester nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  Author: Mark Riding
          Andrew Porter
          Sue Ramsden
          Robert Haines
 */

/** @file steerer.cpp
    @brief The QT steerer GUI main.
    @author Sue Ramsden
    @author Mark Riding
    @author Andrew Porter */
#include <qapplication.h>
#include <signal.h>

#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"

#include "exception.h"
#include "steerermainwindow.h"

#ifndef WIN32
#include <QCleanlooksStyle>
#else
#include <qwindowsstyle.h>
#endif

#include <signal.h>
#include <new>

#ifndef WIN32
#include <X11/Xlib.h>
#endif

/** @file steerer.cpp
    @brief The main source file for the steering client */

//file scope global pointer pointing at this SteererMainWindow object; need this to
//perform clean up.  
SteererMainWindow* gSteererMainWindowSelfPtr;

// exception handler function called when new fails
#ifndef WIN32
void failedNewHandler()
#else
int failedNewHandler(size_t size)
#endif
{
	cerr << "Run out of memory - steerer quitting..." << endl;
	delete gSteererMainWindowSelfPtr;
	if (Steerer_finalize() != REG_SUCCESS) {
	  REG_DBGEXCP("Steerer_finalize failed");
	}
	exit(0);
}

extern "C" void signalHandler(int aSignal)
{
  
  // caught one signal - ignore all others now as going to quit and do not
  // want the quit process to be interrupted and restarted...
  signal(SIGINT, SIG_IGN);	//ctrl-c
  signal(SIGTERM, SIG_IGN);	//kill (note cannot (and should not) catch kill -9)  
  signal(SIGSEGV, SIG_IGN);
  signal(SIGILL, SIG_IGN);
  signal(SIGABRT, SIG_IGN);
  signal(SIGFPE, SIG_IGN);

  switch(aSignal)
  {
    case SIGINT:
      cout << "Interrupt signal received (signal " << aSignal << ")" << endl;
      break;
      
    case SIGTERM:
      cout << "Kill signal received (signal " << aSignal << ")" << endl;
      break;
      
    case SIGSEGV:
      cout << "Illegal Access caught (signal " << aSignal << ")" << endl;
      break;

    case  SIGILL:
      cout << "Illegal Exception caught (signal " << aSignal << ")" << endl;
      break;

      // note: abort called if exception not caught (and hence calls terminate)
    case SIGABRT:
      cout << "Abort signal caught (signal " << aSignal << ")" << endl;
      break;

    case SIGFPE:
      cout << "Arithmetic Exception caught (signal " << aSignal << ")" << endl;
      break;

    default:
      cout << "Signal caught (signal " << aSignal << ")" << endl;

  }

  cout << "Steerer quitting..." << endl;
  delete gSteererMainWindowSelfPtr;
  if (Steerer_finalize() != REG_SUCCESS) {
    REG_DBGEXCP("Steerer_finalize failed");
  }
  exit(0);
}



int main( int argc, char ** argv )
{
  // Print out command-line arguments
  //cout << argc << endl;
  //for (int i=0; i<argc; i++){
  //  cout << argv[i] << " ";
  //}
  //cout << endl;

  // Was getting XSync errors when trying to save screenshots to file.
  // This seems to fix it.
#ifndef WIN32
  XInitThreads();
#endif

  SteererMainWindow *lSteererMainWindow = kNULL;
  gSteererMainWindowSelfPtr = kNULL;

  signal(SIGINT, signalHandler);	//ctrl-c
  signal(SIGTERM, signalHandler);	//kill (note cannot (and should not) catch kill -9)  
  signal(SIGSEGV, signalHandler);
  signal(SIGILL, signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGFPE, signalHandler);

#ifndef WIN32
  std::set_new_handler( failedNewHandler );
#else
  _set_new_handler( failedNewHandler );
#endif

  int result = 1;

  // output some ReG library constants
  REG_DBGLOG("ReG steering library info:");
  REG_DBGLOG1("REG_MAX_NUM_FILES", REG_MAX_NUM_FILES);
  REG_DBGLOG1("REG_MAX_NUM_STR_CMDS", REG_MAX_NUM_STR_CMDS);
  REG_DBGLOG1("REG_MAX_NUM_STR_PARAMS", REG_MAX_NUM_STR_PARAMS);
  REG_DBGLOG1("REG_MAX_STRING_LENGTH",REG_MAX_STRING_LENGTH);
  REG_DBGLOG1("REG_MAX_LINE_LEN ",REG_MAX_LINE_LEN );
  REG_DBGLOG1("REG_MAX_MSG_SIZE",REG_MAX_MSG_SIZE);

  #ifdef UNICORE_DEMO
    REG_DBGLOG("UNICORE_DEMO defined");
  #else
    REG_DBGLOG("UNICORE_DEMO not defined\n");
  #endif

  REG_DBGLOG("Steerer starting...\n");

  // note no need to lock mutex  here as commsthread not running
  
  // initialise steerer via library
  if (Steerer_initialize() == REG_SUCCESS)		//ReG library
  {
    QApplication lApp( argc, argv );

    /* Don't set the window style explicitly - use system default
    //lApp.setFont(QFont("Times", 10));
#ifndef WIN32
    lApp.setStyle(new QMotifPlusStyle);
#else
    lApp.setStyle(new QWindowsStyle);
#endif
    */

    // MR Check to see if we were supplied with an SGS as a command line arg
    // if so - we've been started from the QT launcher, so go directly to the
    // main Steering window without using the Grid Attach dialog.
    if (argc == 2)
      lSteererMainWindow = new SteererMainWindow(true, argv[1]);
    else
      lSteererMainWindow = new SteererMainWindow();
    gSteererMainWindowSelfPtr = lSteererMainWindow;
    lApp.setMainWidget( lSteererMainWindow );
    
    lSteererMainWindow->show();
    
    lApp.connect( &lApp, SIGNAL( lastWindowClosed() ), &lApp, SLOT( quit() ) );
    result = lApp.exec();
    
    delete lSteererMainWindow;

    REG_DBGLOG("Steerer quitting.");

    if (Steerer_finalize() != REG_SUCCESS) {
      REG_DBGEXCP("Steerer_finalize failed");
    }

    return result;

  }
  else
  {
    cerr << "Quitting steerer...\n";

    cerr << "Library call Steerer_initialize failed\n";
    cerr << "Check environment variables and restart steerer\n" << endl;
    return result;
  }

}


