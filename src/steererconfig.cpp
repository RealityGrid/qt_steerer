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
#include "steererconfig.h"

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
  mTopLevelRegistry =  getElementAttrValue(docElem, "topLevelRegistry");
  cout << "Top-level registry is " << mTopLevelRegistry << endl;

  // Polling configuration section
  QDomNodeList nodeList = docElem.elementsByTagName("Polling");
  if(nodeList.count() != 1){
    cout << "Failed to find Polling section in config. file" << endl;
  }
  else{
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "autoPolling");
    mAutoPollingOn = (flag.contains("on") == 1);
    if(mAutoPollingOn){
      cout << "Automatic polling is ON" << endl;
    } else {
      cout << "Automatic polling is OFF" << endl;
    }

    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "pollingInterval");
    mPollingIntervalSecs = flag.toFloat();
    cout << "Default fixed polling interval is " << 
      mPollingIntervalSecs << endl;
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
      cout << "Display of monitored params. table is ON" << endl;
    } else {
      cout << "Display of monitored params. table is OFF" << endl;
    }

    // steered parameters table
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "showSteerParamTable");
    mShowSteerParamTable = (flag.contains("on") == 1);
    if(mShowSteerParamTable){
      cout << "Display of steered params. table is ON" << endl;
    } else {
      cout << "Display of steered params. table is OFF" << endl;
    }

    // IOTypes table
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "showIOTypesTable");
    mShowIOTypeTable = (flag.contains("on") == 1);
    if(mShowIOTypeTable){
      cout << "Display of IOTypes table is ON" << endl;
    } else {
      cout << "Display of IOTypes table is OFF" << endl;
    }

    // ChkTypes table
    flag = getElementAttrValue(nodeList.item(0).toElement(),
			       "showChkTypesTable");
    mShowChkTypeTable = (flag.contains("on") == 1);
    if(mShowChkTypeTable){
      cout << "Display of ChkTypes table is ON" << endl;
    } else {
      cout << "Display of ChkTypes table is OFF" << endl;
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
