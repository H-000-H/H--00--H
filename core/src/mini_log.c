/**
 *@copyright SPDX-License-Identifier: Apache-2.0
 *@file mini_log.c
 *@brief 分级日志实现
 *@author H-000-H
 *@details
 *   统一经 printf_output 输出, 与 RTOS 后端无关。
 *
 *   本文件是全仓唯一允许调用 stdio 输出 (vprintf / my_printf_output) 的
 *   源文件之一, 故定义 ALLOW_STDIO_OUTPUT 豁免 poison 层。
 */

#define ALLOW_STDIO_OUTPUT

#include "mini_log.h"

#include "compiler_compat.h"
#include "printf_output.h"
#include <stdarg.h>
#include <stdio.h>

#include "compiler_compat_poison.h"

void mini_log(mini_log_level_t level, const char* tag, const char* fmt, ...)
{
    MINI_UNUSED_PARAM(level);
    if (!fmt)
        fmt = "(null)";

    va_list args;
    va_start(args, fmt);
    my_printf_output("[%s] ", tag ? tag : "drv");
    vprintf(fmt, args);
    my_printf_output("\n");
    va_end(args);
}

void mini_log_fatal(const char* fmt, ...)
{
    if (!fmt)
        fmt = "(null)";

    va_list args;
    va_start(args, fmt);
    my_printf_output("\r\n[FATAL ERROR] ");
    vprintf(fmt, args);
    my_printf_output("\r\n");
    va_end(args);
}

void mini_log_critical_assert(const char* file, int line, const char* fmt, ...)
{
    if (!fmt)
        fmt = "(null)";

    va_list args;
    va_start(args, fmt);
    my_printf_output("\r\n[1 FAILED] %s:%d: ", file ? file : "?", line);
    vprintf(fmt, args);
    my_printf_output("\r\n");
    va_end(args);
}
