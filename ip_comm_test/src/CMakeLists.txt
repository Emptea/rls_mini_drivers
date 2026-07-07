cmake_minimum_required(VERSION 3.16)
project(ip_comm_test)
find_package(PIP REQUIRED)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_version(${PROJECT_NAME}
	MAJOR 0
	MINOR 0
	REVISION 1
	BUILD ${BUILD_NUMBER}
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_version.h")
set_deploy_property(${PROJECT_NAME}
	LABEL "${PROJECT_NAME}"
	FULLNAME "rls_mini"
	COMPANY "SM5"
	INFO "")
make_rc(${PROJECT_NAME} out_RC)

file(GLOB CPPS "*.cpp")
file(GLOB HDRS "*.h")
# pip_code_model(CCM_PROTOCOL "protocol_rls_mini.h" OPTIONS "-SEMs" NAME "ccm_protocol")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} ${CPPS} ${CS} ${HDRS} ${out_RC})
target_link_libraries(${PROJECT_NAME} PIP::Console PIP::IOUtils)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
deploy_target(${PROJECT_NAME}
	DEPLOY_DIR ${CMAKE_INSTALL_PREFIX}/bin
	DESTINATION ${CMAKE_INSTALL_PREFIX}/release)

# need these include and link directories for the build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

