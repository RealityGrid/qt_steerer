/*----------------------------------------------------------------------------
    (C)Copyright 2003 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the copyright holder, nor the
    University of Manchester offer any warranties or representations,
    nor do they accept any liabilities with respect to this software.

    This software must not be used for commercial gain without the
    written permission of the authors.
    
    This software must not be redistributed without the written
    permission of the authors.

    Permission is granted to modify this software, provided any
    modifications are made freely available to the original authors.
 
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL
    
    WWW:    http://www.sve.man.ac.uk  
    email:  sve@man.ac.uk
    Tel:    +44 161 275 6095
    Fax:    +44 161 275 6800    
---------------------------------------------------------------------------*/

/** @file SteererConfig.cpp
 *  @brief Holds configuration data for the steering client 
 *  @author Mark Riding 
 *  @author Andrew Porter */

#include <iostream>
#include <qfile.h>
#include <qdir.h>
#include <qstring.h>
#include <qmessagebox.h>
#include "debug.h"
#include "steererconfig.h"
#include "ReG_Steer_Utils.h"

using namespace std;

/** Constructor */
SteererConfig::SteererConfig(){
  mTopLevelRegistry = "";
  mKeyPassphrase = "";
  mAutoPollingOn = true;
  mPollingIntervalSecs = 0.1;
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
  if ( !configFile.open( IO_ReadOnly ) ){
    cout << "Input file " << configFile.name() << " does not exist :-(" <<endl;
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
    REG_DBGMSG1("Top-level registry is ", mTopLevelRegistry);

    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "username");
    strncpy(mRegistrySecurity.userDN, flag.ascii(), REG_MAX_STRING_LENGTH);
    cout << "          username is " << QString(mRegistrySecurity.userDN) 
	 << endl;
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
    cout << "Failed to find " << name << " in config. file" << endl;
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

  QDomDocument *doc = new QDomDocument();

  /*
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
