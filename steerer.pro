# /*----------------------------------------------------------------------------
#     .pro file used by QT to generate Makefile
# 
#    (C)Copyright 2002 The University of Manchester, United Kingdom,
#    all rights reserved.
#
#    This software is produced by the Supercomputing, Visualization &
#    e-Science Group, Manchester Computing, the Victoria University of
#    Manchester as part of the RealityGrid project.
#
#    This software has been tested with care but is not guaranteed for
#    any particular purpose. Neither the copyright holder, nor the
#    University of Manchester offer any warranties or representations,
#    nor do they accept any liabilities with respect to this software.
#
#    This software must not be used for commercial gain without the
#    written permission of the authors.
#    
#    This software must not be redistributed without the written
#    permission of the authors.
#
#    Permission is granted to modify this software, provided any
#    modifications are made freely available to the original authors.
# 
#    Supercomputing, Visualization & e-Science Group
#    Manchester Computing
#    University of Manchester
#    Manchester M13 9PL
#    
#    WWW:    http://www.sve.man.ac.uk  
#    email:  sve@man.ac.uk
#    Tel:    +44 161 275 6095
#    Fax:    +44 161 275 6800    
#
#    Initial version by: S Ramsden, 03.10.2002
#    
#---------------------------------------------------------------------------*/



TEMPLATE = app

TARGET      = steerer

## debug modes DBG_VERSION DBG_CONS
DEFINES     += DBG_VERSION
CONFIG      += qt thread
INCLUDEPATH = ${REG_STEER_HOME}/include ./inc ${REG_XML_INCDIR} ${HOME}/gsoap-linux-2.2.3
LIBS        += -L${REG_STEER_HOME}/lib32 -lReG_Steer
#LIBS        += -L${REG_STEER_HOME}/expat/xmlparse -lexpat
LIBS        += -L${REG_XML_LIBDIR} -lxml2 
LIBS        += -L${HOME}/gsoap-linux-2.2.3 -lgsoap
MOC_DIR      = moc
OBJECTS_DIR  = obj

# Input
HEADERS += inc/application.h \	
           inc/attachform.h \
           inc/chkptform.h \
           inc/commsthread.h \
           inc/configform.h \
           inc/controlform.h \
           inc/debug.h \
           inc/exception.h \
           inc/iotype.h \
           inc/iotypetable.h \
           inc/parameter.h \
           inc/parametertable.h \
           inc/steerermainwindow.h \
           inc/table.h \
           inc/types.h \
           inc/utility.h
SOURCES += src/application.cpp \
           src/attachform.cpp \
           src/chkptform.cpp \
           src/commsthread.cpp \
           src/configform.cpp \
           src/controlform.cpp \
           src/exception.cpp \
           src/iotype.cpp \
           src/iotypetable.cpp \
           src/parameter.cpp \
           src/parametertable.cpp \
           src/steerer.cpp \
           src/steerermainwindow.cpp \
           src/table.cpp \
           src/utility.cpp
