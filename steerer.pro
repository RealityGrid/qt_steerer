# /*----------------------------------------------------------------------------
#  .pro file used by QT to generate Makefile
# 
#  (C) Copyright 2002, 2004, University of Manchester, United Kingdom,
#  all rights reserved.
#
#  This software is produced by the Supercomputing, Visualization and
#  e-Science Group, Manchester Computing, University of Manchester
#  as part of the RealityGrid project (http://www.realitygrid.org),
#  funded by the EPSRC under grants GR/R67699/01 and GR/R67699/02.
#
#  LICENCE TERMS
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS MATERIAL IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
#  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
#  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
#  CORRECTION.
#
#  Authors........: Mark Riding, Andrew Porter, Sue Ramsden
#    
#---------------------------------------------------------------------------*/



TEMPLATE = app

TARGET      = steerer

## debug modes DBG_VERSION DBG_CONS
DEFINES     += DBG_VERSION
DEFINES     += WITH_OPENSSL
CONFIG      += qt thread
INCLUDEPATH = ${REG_STEER_HOME}/include ./inc ${REG_XML_INCDIR} ${REG_QWT_INCDIR}
LIBS        += -L${REG_STEER_HOME}/lib32 -lReG_Steer
#LIBS        += -lReG_Steer_Utils -lssl -lcrypto
LIBS        += -L${REG_XML_LIBDIR} -lxml2
LIBS        += -L${REG_QWT_LIBDIR} -lqwt
MOC_DIR      = moc
OBJECTS_DIR  = obj

# Input
HEADERS += inc/application.h \	
           inc/attachform.h \
           inc/chkptform.h \
           inc/chkptvariableform.h \
           inc/commsthread.h \
           inc/configform.h \
           inc/controlform.h \
           inc/debug.h \
           inc/exception.h \
           inc/iotype.h \
           inc/iotypetable.h \
           inc/historyplot.h \
           inc/parameter.h \
           inc/parameterhistory.h \
           inc/parametertable.h \
           inc/steererconfig.h \
           inc/steerermainwindow.h \
           inc/table.h \
           inc/types.h \
           inc/utility.h \
           inc/historysubplot.h
SOURCES += src/application.cpp \
           src/attachform.cpp \
           src/chkptform.cpp \
           src/chkptvariableform.cpp \
           src/commsthread.cpp \
           src/configform.cpp \
           src/controlform.cpp \
           src/exception.cpp \
           src/iotype.cpp \
           src/iotypetable.cpp \
           src/historyplot.cpp \
           src/parameter.cpp \
           src/parameterhistory.cpp \
           src/parametertable.cpp \
           src/steerer.cpp \
           src/steererconfig.cpp \
           src/steerermainwindow.cpp \
           src/table.cpp \
           src/utility.cpp \
           src/historysubplot.cpp
