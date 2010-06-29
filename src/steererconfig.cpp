/*
  The RealityGrid Steerer

  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
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

  Author: Andrew Porter
          Mark Riding
          Robert Haines
 */

/** @file SteererConfig.cpp
 *  @brief Holds configuration data for the steering client
 *  @author Mark Riding
 *  @author Andrew Porter */

#include <iostream>
#include <qfile.h>
#include <qdir.h>
#include <qstring.h>
#include <qmessagebox.h>

#include "buildconfig.h"
#include "debug.h"
#include "steererconfig.h"

using namespace std;

/** Constructor */
SteererConfig::SteererConfig(){
  mTopLevelRegistry = "";
  mKeyPassphrase = "";
  mAutoPollingOn = true;
  mPollingIntervalSecs = 0.1f;
  mShowMonParamTable = true;
  mShowSteerParamTable = true;
  mShowIOTypeTable = true;
  mShowChkTypeTable = true;

  Wipe_security_info(&mRegistrySecurity);
}

/** Destructor */
SteererConfig::~SteererConfig(){

}

/** Method loads a configuration xml file, and parses it to
 *  determine the stored configuration values.
 */
void SteererConfig::readConfig(QString fileName){

  QDomDocument doc( "steererConfigDocument" );
  QFile        configFile(fileName);
  QDomNodeList sectionNodeList;
  QDomNode     tmpNode;
  QString      flag;

  // Parse file
  if ( !configFile.open( QIODevice::ReadOnly ) ){
    cout << "Input file " << fileName.ascii() << " does not exist :-(" <<endl;
    return;
  }
  if ( !doc.setContent( &configFile ) ) {
    configFile.close();
    return;
  }
  configFile.close();

  // Root element
  QDomElement docElem = doc.documentElement();

  // Address of top-level registry
  QDomNodeList nodeList = docElem.elementsByTagName("Registry");
  if(nodeList.count() != 1){
    cout << "Failed to find Registry section in config. file" << endl;
  }
  else{
    mTopLevelRegistry = getElementAttrValue(nodeList.item(0).toElement(),
			       "address");
    REG_DBGMSG1("Top-level registry is ", mTopLevelRegistry.ascii());

    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "username");
    strncpy(mRegistrySecurity.userDN, flag.ascii(), REG_MAX_STRING_LENGTH);
    //cout << "          username is " << QString(mRegistrySecurity.userDN)
    //	 << endl;
  }

  // Polling configuration section
  nodeList = docElem.elementsByTagName("Polling");
  if(nodeList.count() != 1){
    cout << "Failed to find Polling section in config. file" << endl;
  }
  else{
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "autoPolling");
    mAutoPollingOn = (flag.contains("on") == 1);
    if(mAutoPollingOn){
      REG_DBGMSG("Automatic polling is ON");
    } else {
      REG_DBGMSG("Automatic polling is OFF");
    }

    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "pollingInterval");
    mPollingIntervalSecs = flag.toFloat();
    REG_DBGMSG1("Default fixed polling interval is ",
		mPollingIntervalSecs);
  }

  // GUI display section
  nodeList = docElem.elementsByTagName("Display");
  if(nodeList.count() != 1){
    cout << "Failed to find Display section in config. file" << endl;
  }
  else{
    // monitored parameters table
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "showMonParamTable");
    mShowMonParamTable = (flag.contains("on") == 1);

    if(mShowMonParamTable){
      REG_DBGMSG("Display of monitored params. table is ON");
    } else {
      REG_DBGMSG("Display of monitored params. table is OFF");
    }

    // steered parameters table
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "showSteerParamTable");
    mShowSteerParamTable = (flag.contains("on") == 1);
    if(mShowSteerParamTable){
      REG_DBGMSG("Display of steered params. table is ON");
    } else {
      REG_DBGMSG("Display of steered params. table is OFF");
    }

    // IOTypes table
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "showIOTypesTable");
    mShowIOTypeTable = (flag.contains("on") == 1);
    if(mShowIOTypeTable){
      REG_DBGMSG("Display of IOTypes table is ON");
    } else {
      REG_DBGMSG("Display of IOTypes table is OFF");
    }

    // ChkTypes table
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "showChkTypesTable");
    mShowChkTypeTable = (flag.contains("on") == 1);
    if(mShowChkTypeTable){
      REG_DBGMSG("Display of ChkTypes table is ON");
    } else {
      REG_DBGMSG("Display of ChkTypes table is OFF");
    }
  }

  return;
}

/** Method loads a configuration xml file, and parses it to
 *  determine the stored security configuration values.
 */
void SteererConfig::readSecurityConfig(QString fileName){

  QFile configFile(fileName);

  if(!configFile.exists() ||
     (Get_security_config(fileName.ascii(), &mRegistrySecurity)
      != REG_SUCCESS) ){
    QMessageBox::critical( NULL, "Error with security configuration file",
			   "File "+fileName+" does not exist\n"
			   "or cannot be parsed.\n\n",
			   QMessageBox::Ok, 0, 0 );
    return;
  }
  return;
}

//----------------------------------------------------------------------

QString SteererConfig::getElementAttrValue(QDomElement elem, QString name)
{
  QDomNode     tmpNode;
  QDomNodeList nodeList = elem.elementsByTagName(name);

  if(nodeList.count() != 1){
    //cout << "Failed to find " << name << " in config. file" << endl;
  }
  else{
    tmpNode = nodeList.item(0).attributes().namedItem("value");
    if(!tmpNode.isNull()){
      return tmpNode.nodeValue();
    }
  }
  return "";
}

/** Create an xml encoding of the current configuration
 *  and save the file to disk
 */
void SteererConfig::writeConfig(QString fileName){

  /*
  QDomDocument *doc = new QDomDocument();

  // Get root node
  QDomElement root = doc->createElement("LaunchSimulation");
  doc->appendChild(root);

  // Where to launch job
  QDomElement eMachine = doc->createElement("TargetHostname");
  root.appendChild(eMachine);

  QDomText tMachineName = doc->createTextNode(mTargetMachine->mName);
  eMachine.appendChild(tMachineName);

  // What the jobmanager is called
  QDomElement eJobMgr = doc->createElement("TargetHostJobManager");
  root.appendChild(eJobMgr);

  QDomText tJobMgr = doc->createTextNode(mTargetMachine->mJobManager);
  eJobMgr.appendChild(tJobMgr);

  // Wall-clock time of job in minutes
  QDomElement eRunTime = doc->createElement("RunTime");
  root.appendChild(eRunTime);

  QDomText tRunTime = doc->createTextNode(QString::number(mTimeToRun));
  eRunTime.appendChild(tRunTime);

  // Checkpoint GSH
  QDomElement eChkGSH;
  QDomText tChkGSH;
  if(restart || migration){
    eChkGSH = doc->createElement("CheckPointGSH");
    root.appendChild(eChkGSH);

    tChkGSH = doc->createTextNode(currentCheckpointGSH);
    eChkGSH.appendChild(tChkGSH);
  }

  // No. of processors
  QDomElement eNumProc = doc->createElement("NoProcessors");
  root.appendChild(eNumProc);

  QDomText tNumProc = doc->createTextNode(QString::number(mNumberProcessors));
  eNumProc.appendChild(tNumProc);

  // Input file(s) for simulation
  QDomElement eInputFiles = doc->createElement("SimulationInputFiles");
  root.appendChild(eInputFiles);

  QDomElement eFile;
  QDomText tFile;
  if(mAppToLaunch->mHasInputFile){

    eFile =  doc->createElement("File");
    eInputFiles.appendChild(eFile);

    // Read input file
    QFile file(mInputFileName);
    file.open( IO_ReadOnly );
    QByteArray fileData = file.readAll();
    file.close();

    // Encode input file as base64
    QCString fileDataB64 = QCodecs::base64Encode(fileData, true);

    tFile = doc->createTextNode(fileDataB64);
    eFile.appendChild(tFile);
  }

  // Pull out GSH of SGS and use to create job ID too
  QString outputFile("ReGJob.");
  QDomText tSGS;
  if(mAppToLaunch->mNumInputs == 0){
    tSGS = doc->createTextNode(simulationGSH);
    outputFile.append(simulationGSH.right(simulationGSH.length() - simulationGSH.findRev('/') - 1));
  }
  else{
    tSGS = doc->createTextNode(visualizationGSH);
    outputFile.append(visualizationGSH.right(visualizationGSH.length() - visualizationGSH.findRev('/') - 1));
  }

  // Name of file to write job stdout to
  QDomElement eStdOut = doc->createElement("SimulationSTDOUTfile");
  root.appendChild(eStdOut);
  QDomText tStdOut = doc->createTextNode(outputFile+".stdout");
  eStdOut.appendChild(tStdOut);

  // Name of file to write job stderr to
  QDomElement eStdErr = doc->createElement("SimulationSTDERRfile");
  root.appendChild(eStdErr);
  QDomText tStdErr = doc->createTextNode(outputFile+".stderr");
  eStdErr.appendChild(tStdErr);

  // GSH of associated SGS
  QDomElement eSGS = doc->createElement("ReGSGSAddress");
  root.appendChild(eSGS);
  eSGS.appendChild(tSGS);
  */
  return;
}
