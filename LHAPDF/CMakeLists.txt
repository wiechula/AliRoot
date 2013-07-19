cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE LHAPDF)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/${MODULE})

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/LHAPDF/lhapdf5.5.1/include)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS AliStructFuncType.cxx)

# Fortran sources
set ( FSRCS  lhapdf5.5.1/src/alphas.f lhapdf5.5.1/src/commoninit.f lhapdf5.5.1/src/description.f lhapdf5.5.1/src/eks98.f lhapdf5.5.1/src/eksarp.f lhapdf5.5.1/src/eps08.f  lhapdf5.5.1/src/eps09.f lhapdf5.5.1/src/EVLCTEQ.f lhapdf5.5.1/src/evolution.f lhapdf5.5.1/src/inputPDF.f lhapdf5.5.1/src/LHpdflib.F lhapdf5.5.1/src/parameter.f lhapdf5.5.1/src/QCDNUM.f lhapdf5.5.1/src/QCDparams.f lhapdf5.5.1/src/Sqcdnum.f lhapdf5.5.1/src/wrapa02.f lhapdf5.5.1/src/wrapa02m.f lhapdf5.5.1/src/wrapabfkwpi.f lhapdf5.5.1/src/wrapacfgpg.f lhapdf5.5.1/src/wrapcteq5.f lhapdf5.5.1/src/wrapcteq6.f lhapdf5.5.1/src/wrapcteq6lg.f lhapdf5.5.1/src/wrapdgg.f lhapdf5.5.1/src/wrapdog.f lhapdf5.5.1/src/wrapEVLCTEQ.f lhapdf5.5.1/src/wrapevolve.f lhapdf5.5.1/src/wrapgrv.f lhapdf5.5.1/src/wrapgrvg.f lhapdf5.5.1/src/wrapgrvpi.f lhapdf5.5.1/src/wrapgsg96.f lhapdf5.5.1/src/wrapgsg.f lhapdf5.5.1/src/wraph1.f lhapdf5.5.1/src/wraplacg.f lhapdf5.5.1/src/wrapowpi.f lhapdf5.5.1/src/wrapmrst.f lhapdf5.5.1/src/wrapQCDNUM3.f lhapdf5.5.1/src/wrapQCDNUM4.f lhapdf5.5.1/src/wrapQCDNUM.f lhapdf5.5.1/src/wrapsasg.f lhapdf5.5.1/src/wrapsmrspi.f lhapdf5.5.1/src/wrapwhitg.f lhapdf5.5.1/src/wrapzeus.f lhapdf5.5.1/src/lhaglue.f lhapdf5.5.1/src/wrapUSER.f)

# C sources
set ( CSRCS  lhapdf5.5.1/src/binreloc.c )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Sources without dictionary
set ( SRCS ${SRCS} lhapdf5.5.1/src/version.cxx  lhapdf5.5.1/src/getdatapath.cxx)

# Make dictionary
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.h
  COMMAND ${ROOTCINT} 
  ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx -c -p 
  ${include_dirs}
  ${HDRS} ${MODULE}LinkDef.h
  DEPENDS ${HDRS} ${MODULE}LinkDef.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add a library to the project using the specified source files
add_library(${MODULE} SHARED ${SRCS} G__${MODULE}.cxx ${FSRCS} ${CSRCS})

# Additional compilation flags
set_target_properties(${MODULE} PROPERTIES
  COMPILE_FLAGS "-O -g -ffree-form -DDEFAULTPREFIXPATH=\\\"NONE\\\" -DDEFAULTLHAPATH=\\\"${CMAKE_SOURCE_DIR}/LHAPDF/PDFsets\\\""
  )

# System dependent: Modify the way the library is build
if(${CMAKE_SYSTEM} MATCHES Darwin)
  set_target_properties(${MODULE} PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(${CMAKE_SYSTEM} MATCHES Darwin)

# Installation
install(TARGETS ${MODULE} LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)
