/*----------------------------------------------------------------------------
    Header file for Parameter class for QT steerer GUI.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
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

    Initial version by: S Ramsden, 03.10.2002
    
---------------------------------------------------------------------------*/


#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include "parameterhistory.h"

class QTable;

class Parameter
{

public:

  Parameter(int aId, int aType, bool aSteerable);
  ~Parameter();
  
  void printParameter(QTable *) const;  

  int getId() const;
  int getType() const;
  int getRowIndex() const;
  bool isPresent() const;
  bool isRegistered() const;
  bool isSteerable() const;

  bool checkRegistered();

  void setIndex(int aIndex);
  void setIsPresent();
  void unRegister();

  ParameterHistory  mParamHist;
  
private:
  const	bool	mSteerable;
  bool		mRegisteredFlag;
  bool		mPresentFlag;

  int		mRowIndex;
  const int	mId;
  const int	mType;

};


#endif


