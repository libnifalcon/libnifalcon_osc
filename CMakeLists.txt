######################################################################################
# CMake directives
######################################################################################

#Require 2.6 or higher. 
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

#See if we've got our cmake modules checked out. If not, advise to do so.
IF(EXISTS "${CMAKE_SOURCE_DIR}/../compily_buildd/cmake")
  SET(BUILDSYS_CMAKE_DIR "${CMAKE_SOURCE_DIR}/../compily_buildd/cmake")
  MESSAGE(STATUS "Using outside compily_buildd directory ${BUILDSYS_CMAKE_DIR}")
ELSEIF(EXISTS "${CMAKE_SOURCE_DIR}/compily_buildd/cmake")
  SET(BUILDSYS_CMAKE_DIR "${CMAKE_SOURCE_DIR}/compily_buildd/cmake")
  MESSAGE(STATUS "Using compily_buildd git submodule ${BUILDSYS_CMAKE_DIR}")
ELSE(EXISTS "${CMAKE_SOURCE_DIR}/compily_buildd/cmake")
  MESSAGE(FATAL_ERROR 
    "Cannot find compily_buildd directory for np labs project compilation functions.\n"
    "Make sure you've either put the compily_buildd directory in the same root as your repository directory, or that you've used the compily_buildd submodule (git submodule update --init).\n"
    "NP Labs build repository is available at git://github.com/qdot/compily_buildd.git"
    )
ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/../compily_buildd/cmake")

LIST(APPEND CMAKE_MODULE_PATH ${BUILDSYS_CMAKE_DIR})

INCLUDE( ${BUILDSYS_CMAKE_DIR}/BuildSysCMakeLib.cmake )

#setting link directory policy

IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

######################################################################################
# Project declaration and options
######################################################################################

#Project declaration

PROJECT(LIBNIFALCON_OSC)
INITIALIZE_BUILD()

#Common Options

OPTION_FAST_MATH(ON)
OPTION_ARCH_OPTS(ON)
OPTION_GPROF(OFF)

######################################################################################
# Project specific package finding
######################################################################################

#Find boost, used all over the place
SET(Boost_USE_MULTITHREADED ON)

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
IF(NOT Boost_FOUND)
  MESSAGE(FATAL_ERROR "libnifalcon_osc requires a minimum of the Boost 1.33 headers to build")
ENDIF(NOT Boost_FOUND)

FIND_PACKAGE(libnifalcon REQUIRED)
FIND_PACKAGE(liblo REQUIRED)

######################################################################################
# Project specific globals
######################################################################################

#library definitions

SET(LIBNIFALCON_OSC_MAJOR_VERSION 1)
SET(LIBNIFALCON_OSC_MINOR_VERSION 0)
SET(LIBNIFALCON_OSC_BUILD_VERSION 0)

# BUILDSYS_CPACK_INFO(
#   NAME "libnifalcon"
#   MAJOR_VERSION ${LIBNIFALCON_MAJOR_VERSION}
#   MINOR_VERSION ${LIBNIFALCON_MINOR_VERSION}
#   BUILD_VERSION ${LIBNIFALCON_BUILD_VERSION}
#   VENDOR "Nonpolynomial Labs"
#   DESCRIPTION "Open Source Driver for the Novint Falcon Haptic Controller"
#   )
# INCLUDE(CPack)

SET(LIBNIFALCON_OSC_VERSION ${LIBNIFALCON_MAJOR_VERSION}.${LIBNIFALCON_MINOR_VERSION}.${LIBNIFALCON_BUILD_VERSION})

#If we build libusb staticly on apple, we need the proper frameworks
IF(BUILD_STATIC AND APPLE)
  LIST(APPEND LIBNIFALCON_OSC_EXE_LINK_LIBS "-framework IOKit" "-framework CoreFoundation")
ENDIF(BUILD_STATIC AND APPLE)

INCLUDE_DIRECTORIES(${LIBNIFALCON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${LIBLO_INCLUDE_DIRS})

LIST(APPEND LIBNIFALCON_OSC_EXE_LINK_LIBS 
  ${LIBNIFALCON_CLI_BASE_LIBRARY}
  ${LIBNIFALCON_LIBRARY}
  ${LIBLO_LIBRARY}
  )

SET(SRCS src/falcon_osc.cpp)

BUILDSYS_BUILD_EXE(
  NAME falcon_osc
  SOURCES "${SRCS}" 
  CXX_FLAGS FALSE
  LINK_LIBS "${LIBNIFALCON_OSC_EXE_LINK_LIBS}" 
  LINK_FLAGS FALSE 
  DEPENDS FALSE
  SHOULD_INSTALL TRUE
  )

