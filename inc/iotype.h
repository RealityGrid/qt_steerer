/*----------------------------------------------------------------------------
    Header file for IOType class for QT steerer GUI.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the authors, nor the University of
    Manchester offer any warranties or representations, nor do they
    accept any liabilities with respect to this software.

    This program must not be used for commmercial gain without the
    written permission of the authors.
    
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    email:  csar-advice@cfs.ac.uk.
    Tel:    +44 161 275 6824/5997
    Fax:    +44 161 275 6040    
    
    Date          Version    Updates                            Author
    ----          -------    -------                            ------
    03.10.2002      0.1                                         S Ramsden

---------------------------------------------------------------------------*/

#ifndef __IOTYPE_H__
#define __IOTYPE_H__

class IOType
{

public:
  IOType(int aId, int aIOTypeType, int aAutoFlag = false);
  ~IOType();

  void printIOType() const;

  int getId() const;
  int getType() const;
  int getRowIndex() const;
  int getFrequency() const;
  bool getAutoSupportedFlag() const;
 
  int  getAndResetFrequency();

  void setIndex(const int aIndex);
  bool validateAndSetFrequency(const int aFreq);
  void setNullFrequency();

private:

  const int mId;
  const int mType;
  int	mRowIndex;
  int	mNewFrequency;

  bool	mAutoSupportedFlag;
};

#endif
