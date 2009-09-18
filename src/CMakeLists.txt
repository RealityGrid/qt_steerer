#
#  CMake build machinery for the RealityGrid Qt Steerer.
#
#  (C) Copyright 2004-2008, University of Manchester, United Kingdom,
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
#  Author.........: Robert Haines
#----------------------------------------------------------------------

set(steerer_SRCS
  application.cpp
  attachform.cpp
  chkptform.cpp
  chkptvariableform.cpp
  commsthread.cpp
  configform.cpp
  controlform.cpp
  exception.cpp
  historyplot.cpp
  historysubplot.cpp
  iotype.cpp
  iotypetable.cpp
  logo.cpp
  parameter.cpp
  parameterhistory.cpp
  parametertable.cpp
  steererconfig.cpp
  steerer.cpp
  steerermainwindow.cpp
  table.cpp
  utility.cpp
)

set(steerer_MOCS
  ${PROJECT_SOURCE_DIR}/inc/application.h
  ${PROJECT_SOURCE_DIR}/inc/attachform.h
  ${PROJECT_SOURCE_DIR}/inc/chkptform.h
  ${PROJECT_SOURCE_DIR}/inc/chkptvariableform.h
  ${PROJECT_SOURCE_DIR}/inc/configform.h
  ${PROJECT_SOURCE_DIR}/inc/controlform.h
  ${PROJECT_SOURCE_DIR}/inc/historyplot.h
  ${PROJECT_SOURCE_DIR}/inc/historysubplot.h
  ${PROJECT_SOURCE_DIR}/inc/iotypetable.h
  ${PROJECT_SOURCE_DIR}/inc/parametertable.h
  ${PROJECT_SOURCE_DIR}/inc/steerermainwindow.h
  ${PROJECT_SOURCE_DIR}/inc/table.h
)

# header files and definitions
include_directories(${PROJECT_SOURCE_DIR}/inc)

# moc the include files
qt4_wrap_cpp(QT_MOC_SRCS ${steerer_MOCS})

# build the executable
add_executable(steerer ${steerer_SRCS} ${QT_MOC_SRCS})
target_link_libraries(steerer
  ${REG_LINK_LIBRARIES}
  ${QWT_LIBRARIES} 
  ${QT_LIBRARIES}
  ${REG_EXTERNAL_LIBS}
)

# install
install(TARGETS steerer RUNTIME DESTINATION ${REG_STEERER_BIN_INSTALL})
