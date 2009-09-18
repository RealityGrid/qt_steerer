#
#  CMake build machinery for the RealityGrid Qt Steerer.
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

set(doc_NAME Steerer_User_Manual)

find_package(LATEX REQUIRED)

# run pdflatex (in dvi mode) twice on the input file.
# pdflatex is required so we can do the out of source build.
# dvi mode is required so we can use eps files.
add_custom_command(
  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${doc_NAME}.dvi
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${doc_NAME}.tex
  COMMAND ${PDFLATEX_COMPILER}
  ARGS    -interaction batchmode -output-format dvi -output-directory ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${doc_NAME}.tex
  COMMAND ${PDFLATEX_COMPILER}
  ARGS    -interaction batchmode -output-format dvi -output-directory ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${doc_NAME}.tex
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  VERBATIM
)

# convert the dvi produced above into a pdf file.
add_custom_command(
  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${doc_NAME}.pdf
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${doc_NAME}.dvi
  COMMAND ${DVIPDF_CONVERTER}
  ARGS    -q -o ${CMAKE_CURRENT_BINARY_DIR}/${doc_NAME}.pdf ${CMAKE_CURRENT_BINARY_DIR}/${doc_NAME}.dvi
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  VERBATIM
)

# add a target so that the above is 
add_custom_target(Steerer_User_Manual ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${doc_NAME}.pdf
)

# install
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${doc_NAME}.pdf
  DESTINATION ${REG_STEERER_DOC_INSTALL}
)
