#
#  The RealityGrid Steerer
#
#  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
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

set(inc_dir ${PROJECT_SOURCE_DIR}/inc)
set(res_dir ${PROJECT_SOURCE_DIR}/res)

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
  ${inc_dir}/application.h
  ${inc_dir}/attachform.h
  ${inc_dir}/attachsockets.h
  ${inc_dir}/chkptform.h
  ${inc_dir}/chkptvariableform.h
  ${inc_dir}/configform.h
  ${inc_dir}/controlform.h
  ${inc_dir}/historyplot.h
  ${inc_dir}/historysubplot.h
  ${inc_dir}/iotypetable.h
  ${inc_dir}/parametertable.h
  ${inc_dir}/steerermainwindow.h
  ${inc_dir}/table.h
)

# header files and definitions
include_directories(${inc_dir})

# moc the include files
qt4_wrap_cpp(QT_MOC_SRCS ${steerer_MOCS})

# add some apple specific stuff
set(GUI_TYPE)
set(EXE_NAME steerer)
if(APPLE AND STEERER_BUILD_BUNDLE)
  # set gui type and app name
  set(GUI_TYPE MACOSX_BUNDLE)
  set(EXE_NAME Steerer)

  # add icons to the build
  set(steerer_SRCS ${steerer_SRCS} ${res_dir}/realitygrid.icns)
  set(MACOSX_BUNDLE_ICON_FILE ${res_dir}/realitygrid.icns)
  set_source_files_properties(${res_dir}/realitygrid.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  # set bundle info strings
  set(MACOSX_BUNDLE_INFO_STRING "${REG_STEERER_VERSION}, (c) 2002-2010, The University of Manchester, UK.")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${REG_STEERER_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "(c) 2002-2010, The University of Manchester, UK.")
endif(APPLE AND STEERER_BUILD_BUNDLE)

# and some windows specific stuff
if(WIN32 AND NOT CYGWIN)
  # supress braindead warnings
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif(WIN32 AND NOT CYGWIN)

# build the executable
add_executable(${EXE_NAME} ${GUI_TYPE} ${steerer_SRCS} ${QT_MOC_SRCS})
target_link_libraries(${EXE_NAME}
  ${REG_LINK_LIBRARIES}
  ${QWT_LIBRARIES}
  ${QT_LIBRARIES}
  ${REG_EXTERNAL_LIBS}
)

# install
install(
  TARGETS ${EXE_NAME}
  RUNTIME DESTINATION ${STEERER_BIN_INSTALL}
  BUNDLE  DESTINATION ${STEERER_BIN_INSTALL}
)
