cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE MICROCERN)
include_directories( ${CMAKE_SOURCE_DIR}/${MODULE})


# Fortran sources
set ( FSRCS  cltou.F gamma.F gamma64.F sortzv.F ufill.F fint.F abend.F cfill.F mtlprt.F mtlset.F kerset.F gauss.F getenvf.F gauss64.F)

# C sources
set ( CSRCS  geteni.c fchput.c fchtak.c dummies.c)

# Add a library to the project using the specified source files
add_library(${MODULE} SHARED ${FSRCS} ${CSRCS})

# Additional compilation flags
set_target_properties(${MODULE} PROPERTIES
  COMPILE_FLAGS "-O -g"
  )

# System dependent: Modify the way the library is build
if(${CMAKE_SYSTEM} MATCHES Darwin)
  set_target_properties(${MODULE} PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(${CMAKE_SYSTEM} MATCHES Darwin)

# Installation
install(TARGETS ${MODULE} LIBRARY DESTINATION lib)

