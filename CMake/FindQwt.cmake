#
#  CMake script to find a Qwt installation.
#
#  (C) Copyright 2004-2009, University of Manchester, United Kingdom,
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

# QWT_FOUND - system has Qwt
# QWT_INCLUDE_DIR - where to find qwt.h
# QWT_LIBRARIES - the libraries to link against to use Qwt
# QWT_LIBRARY - where to find the Qwt library (not for general use)

if(NOT QT4_FOUND)
  include(FindQt4)
endif(NOT QT4_FOUND)

set(QWT_FOUND NO)

if(QT4_FOUND)
  find_path(QWT_INCLUDE_DIR qwt.h
    /usr/local/qwt/include
    /usr/local/include
    /usr/include/qwt-qt4
    /usr/include/qwt
    /usr/include
  )

  set(QWT_NAMES ${QWT_NAMES} qwt-qt4 qwt libqwt-qt4 libqwt)
  find_library(QWT_LIBRARY
    NAMES ${QWT_NAMES}
    PATHS /usr/local/qwt/lib /usr/local/lib /usr/lib
  )

  if(QWT_LIBRARY)
    set(QWT_LIBRARIES ${QWT_LIBRARY})
    set(QWT_FOUND YES)

  endif(QWT_LIBRARY)
endif(QT4_FOUND)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(QWT DEFAULT_MSG QWT_LIBRARIES)

mark_as_advanced(QWT_INCLUDE_DIR QWT_LIBRARY)
