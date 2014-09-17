cmake_minimum_required(VERSION 3.0.0)
project(TMDSvr)

include(GNUInstallDirs)
include(tmdsvr_cfg.cmake)

configure_file(
	"${CMAKE_SOURCE_DIR}/version_config.hpp.cmake"
	"${CMAKE_SOURCE_DIR}/headers/version_config.hpp"
)

set(TMDSvr_BIN "TMDSvr-${CMAKE_SYSTEM}-${CMAKE_CXX_COMPILER_ID}.${CMAKE_BUILD_TYPE}")
set(TMDSvr_LIB "TMDSvrLib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGES_RELEASE} -DRELEASE")

set(SOURCES
 src/version.cpp
)

set(HEADERS
	headers/version.hpp
	headers/version_config.hpp
)

include_directories(
	"${CMAKE_SOURCE_DIR}/headers"
)

find_package(Boost 1.49 COMPONENTS system program_options regex REQUIRED)
find_package(OpenSSL)
find_library(Tamandua
	NAMES "tamandua-${Tamandua_VERSION}" "tamandua"
	PATHS "${CMAKE_SOURCE_DIR}/lib" "${CMAKE_LIBRARY_PATH}" "${CMAKE_FRAMEWORK_PATH}")
find_file(Tamandua_INCLUDE tamandua.hpp
	PATHS "${CMAKE_SOURCE_DIR}/headers" "${CMAKE_INCLUED_PATH}" "${CMAKE_FRAMEWORK_PATH}")

if (Tamandua STREQUAL Tamandua-NOTFOUND)
	message(FATAL_ERROR "Tamandua library not found!")
elseif(Tamandua_INCLUDE STREQUAL Tamandua_INCLUDE-NOTFOUND)
	message(FATAL_ERROR "Tamandua's header files not found!")
else()
	message(STATUS "Tamandua library found!")
endif(Tamandua STREQUAL Tamandua-NOTFOUND)

add_library(${TMDSvr_LIB} STATIC ${HEADERS} ${SOURCES})

add_executable(${TMDSvr_BIN} src/main.cpp)
target_link_libraries(${TMDSvr_BIN} "${OPENSSL_LIBRARIES}" "${Boost_LIBRARIES}" "${Tamandua}" "${TMDSvr_LIB}")
