cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_SYSTEM_NAME)
    set(CMAKE_SYSTEM_NAME Generic)
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR arm)
endif()
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT CMAKE_C_COMPILER)
    if(EXISTS "${MINI_LOG_GCC_JUNCTION}")
        set(CMAKE_C_COMPILER "${MINI_LOG_GCC_JUNCTION}")
    else()
        find_program(MINI_LOG_GCC_PATH arm-none-eabi-gcc)
        if(MINI_LOG_GCC_PATH)
            set(CMAKE_C_COMPILER "${MINI_LOG_GCC_PATH}")
        endif()
    endif()
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_PROJECT_NAME mini_log)
#clangd要吃长路径
set(MINI_LOG_GCC_JUNCTION "${CMAKE_CURRENT_SOURCE_DIR}/.arm-gcc/bin/arm-none-eabi-gcc.exe")
if(EXISTS "${MINI_LOG_GCC_JUNCTION}")
    set(CMAKE_C_COMPILER   "${MINI_LOG_GCC_JUNCTION}" CACHE FILEPATH "C compiler (space-free path)" FORCE)
    set(CMAKE_ASM_COMPILER "${MINI_LOG_GCC_JUNCTION}" CACHE FILEPATH "ASM compiler (space-free path)" FORCE)
else()
    find_program(MINI_LOG_GCC_PATH arm-none-eabi-gcc)
    if(MINI_LOG_GCC_PATH)
        set(CMAKE_C_COMPILER   "${MINI_LOG_GCC_PATH}" CACHE FILEPATH "C compiler" FORCE)
        set(CMAKE_ASM_COMPILER "${MINI_LOG_GCC_PATH}" CACHE FILEPATH "ASM compiler" FORCE)
    endif()
endif()

project(${CMAKE_PROJECT_NAME} C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
get_filename_component(ARM_TOOLCHAIN_ROOT "${CMAKE_C_COMPILER}/../.." ABSOLUTE)
file(GLOB ARM_GCC_INTERNAL_INC "${ARM_TOOLCHAIN_ROOT}/lib/gcc/arm-none-eabi/*/include")
set(ARM_TOOLCHAIN_SYS_INC
    ${ARM_GCC_INTERNAL_INC}
    ${ARM_TOOLCHAIN_ROOT}/arm-none-eabi/include
)
foreach(_sys_inc IN LISTS ARM_TOOLCHAIN_SYS_INC)
    if(EXISTS "${_sys_inc}")
        add_compile_options("-I${_sys_inc}")
    endif()
endforeach()

file(GLOB MINI_LOG_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer/*.c
)

set(MINI_LOG_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer
)
add_library(${CMAKE_PROJECT_NAME} STATIC
    ${MINI_LOG_SRC}
    inc/log_config.h
    inc/log_err.h
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${MINI_LOG_INC}
)
