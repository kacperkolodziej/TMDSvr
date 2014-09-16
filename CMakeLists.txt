cmake_minimum_required(VERSION 3.0.0)
project(TMDSvr)

# include(FindTamandua.cmake)
include(tmdsvr_cfg.cmake)

configure_file(
	"${CMAKE_SOURCE_DIR}/version_config.hpp.cmake"
	"${CMAKE_SOURCE_DIR}/headers/version_config.hpp"
)

find_package(Boost 1.49 COMPONENTS system program_options REQUIRED)
find_package(OpenSSL)


