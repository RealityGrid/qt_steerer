/*----------------------------------------------------------------------------
  The QT steerer GUI main.

  (C) Copyright 2002, 2004, University of Manchester, United Kingdom,
  all rights reserved.

  This software is produced by the Supercomputing, Visualization and
  e-Science Group, Manchester Computing, University of Manchester
  as part of the RealityGrid project (http://www.realitygrid.org),
  funded by the EPSRC under grants GR/R67699/01 and GR/R67699/02.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS MATERIAL IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
  CORRECTION.

  Authors........: Mark Riding, Andrew Porter, Sue Ramsden
   
---------------------------------------------------------------------------*/


#include <qapplication.h>
#include <signal.h>

#include "types.h"
#include "debug.h"
#include "ReG_Steer_Steerside.h"

#include "exception.h"
#include "steerermainwindow.h"

#include <qmotifplusstyle.h>

#include <signal.h>
#include <new.h>


//file scope global pointer pointing at this SteererMainWindow object; need this to
//perform clean up.  
SteererMainWindow* gSteererMainWindowSelfPtr;

// exception handler function called when new fails
void failedNewHandler()
{
	cerr << "Run out of memory - steerer quitting..." << endl;
	delete gSteererMainWindowSelfPtr;
	if (Steerer_finalize() != REG_SUCCESS)
	  DBGEXCP("Steerer_finalize failed");
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
  if (Steerer_finalize() != REG_SUCCESS)
    DBGEXCP("Steerer_finalize failed");
  exit(0);
}



int main( int argc, char ** argv )
{
  // ignore any ctrl-c or kill until SteererMainWindow init sets up signal handlers
  ///signal( SIGINT, SIG_IGN );
  ///signal( SIGTERM, SIG_IGN );

  // Print out command-line arguments
  //cout << argc << endl;
  //for (int i=0; i<argc; i++){
  //  cout << argv[i] << " ";
  //}
  //cout << endl;

  SteererMainWindow *lSteererMainWindow = kNULL;
  gSteererMainWindowSelfPtr = kNULL;

  signal(SIGINT, signalHandler);	//ctrl-c
  signal(SIGTERM, signalHandler);	//kill (note cannot (and should not) catch kill -9)  
  signal(SIGSEGV, signalHandler);
  signal(SIGILL, signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGFPE, signalHandler);
  
  set_new_handler( failedNewHandler );

  int result = 1;

  // output some ReG library constants
  DBGLOG("ReG steering library info:");
  DBGLOG1("REG_MAX_NUM_FILES", REG_MAX_NUM_FILES);
  DBGLOG1("REG_MAX_NUM_STR_CMDS", REG_MAX_NUM_STR_CMDS);
  DBGLOG1("REG_MAX_NUM_STR_PARAMS", REG_MAX_NUM_STR_PARAMS);
  DBGLOG1("REG_MAX_STRING_LENGTH",REG_MAX_STRING_LENGTH);
  DBGLOG1("REG_MAX_LINE_LEN ",REG_MAX_LINE_LEN );
  DBGLOG1("REG_MAX_MSG_SIZE",REG_MAX_MSG_SIZE);

  #ifdef UNICORE_DEMO
    DBGLOG("UNICORE_DEMO defined");
  #else
    DBGLOG("UNICORE_DEMO not defined\n");
  #endif

  DBGLOG("Steerer starting...\n");

  // note no qApp->lock need here as commsthread not running
  
  // initialise steerer via library
  if (Steerer_initialize() == REG_SUCCESS)		//ReG library
  {
    QApplication lApp( argc, argv );
    lApp.setFont(QFont("Times", 10));  //SMR XXX default is Helvetica on errol
    lApp.setStyle(new QMotifPlusStyle);

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

    DBGLOG("Steerer quitting.");

    if (Steerer_finalize() != REG_SUCCESS)		//ReG library
      DBGEXCP("Steerer_finalize failed");

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


