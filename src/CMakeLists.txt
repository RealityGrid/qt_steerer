#
#  The RealityGrid Steerer
#
#  Copyright (c) 2002-2009, University of Manchester, United Kingdom.
#  All rights reserved.
#
#  This software is produced by Research Computing Services, University
#  of Manchester as part of the RealityGrid project and associated
#  follow on projects, funded by the EPSRC under grants GR/R67699/01,
#  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
#  EP/F00561X/1.
#
#  LICENCE TERMS
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#    * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
#    * Neither the name of The University of Manchester nor the names
#      of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written
#      permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
#  Author: Robert Haines

set(steerer_SRCS
  application.cpp
  attachform.cpp
  attachsockets.cpp
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
  ${PROJECT_SOURCE_DIR}/inc/attachsockets.h
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
