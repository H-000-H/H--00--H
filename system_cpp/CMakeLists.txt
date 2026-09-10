# ==============================================================================
# system — 系统运行时 (看门狗、安全状态、任务管理)
# ==============================================================================
# 注意: system 组件部分源文件直接引用 FreeRTOS API.
# 当使用 RT-Thread 后端时, 这些文件仅编译通过但运行时不可用.
# 完整的多后端支持需要将这些文件重构为统一接口调用.
# ==============================================================================

# ── OS 后端选择 (默认裸机; 读 mini_tree/.config) ──
set(OS_BACKEND "BARE")
set(_MINI_TREE_CONFIG "${CMAKE_CURRENT_LIST_DIR}/../.config")
if(EXISTS "${_MINI_TREE_CONFIG}")
    file(STRINGS "${_MINI_TREE_CONFIG}" _os_choice REGEX "^CONFIG_OS_(MINI_OS|FREERTOS|RTTHREAD|BARE)=y$")
    if(_os_choice STREQUAL "CONFIG_OS_FREERTOS=y")
        set(OS_BACKEND "FREERTOS")
    elseif(_os_choice STREQUAL "CONFIG_OS_RTTHREAD=y")
        set(OS_BACKEND "RTTHREAD")
    elseif(_os_choice STREQUAL "CONFIG_OS_BARE=y")
        set(OS_BACKEND "BARE")
    endif()
endif()

add_library(system STATIC
    "src/safe_state.c"
    "src/system_init.cpp"
    "src/system_scrubber.cpp"
    "src/system_wdt.cpp"
    "src/task_manager.cpp"
)

target_include_directories(system
    PUBLIC
        "include"
    PRIVATE
        "${PROJECT_SOURCE_DIR}/hal_bus/include"
        "${PROJECT_SOURCE_DIR}/hal_inst/include"
        "${PROJECT_SOURCE_DIR}/hal_if/include"
)

target_link_libraries(system
    PUBLIC
        core
    PRIVATE
        board
)

add_dependencies(system kconfig_generate)
