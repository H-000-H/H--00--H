/**
 *@copyright SPDX-License-Identifier: Apache-2.0
 *@file mini_panic.h
 *@brief Panic 与关键断言
 *@author H-000-H
 *@details
 *   mini_panic — fail-fast 语义的公共设施, 与操作系统无关:
 *     输出致命原因 -> 调用板级硬件安全关断 -> 驻留死循环,
 *     等待外部硬件看门狗复位。
 *
 *   两个宏的区别:
 *     MINI_PANIC(fmt, ...)              无条件 panic (调用方已判定致命)
 *     MINI_CRITICAL_ASSERT(cond, fmt...) 条件断言 (cond 为假才 panic)
 *
 *   板级接口 (板级必须提供强符号):
 *     system_safety_hardware_shutdown(reason) — 硬件安全关断, 实现在板级
 *     safety_hardware_shutdown(void)          — weak, 板级未覆盖时 trap
 *     mini_panic_interlock(void)              — weak, 板级可覆盖 (喂狗/切断执行器)
 */

#ifndef MINI_PANIC_H
#define MINI_PANIC_H

#include "compiler_compat.h"
#include "log.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief panic 互锁 (weak, 默认空实现, 板级可覆盖)
 * @note 板级可在此喂硬件看门狗、切断执行器供电
 */
void mini_panic_interlock(void);

/**
 * @brief 硬件安全关断 (weak, 默认 trap, 板级可覆盖)
 */
void safety_hardware_shutdown(void);

/**
 * @brief 板级硬件安全关断 (必须由板级强制实现)
 * @param[in] reason 关断原因 (用于日志/黑匣子)
 */
void system_safety_hardware_shutdown(const char* reason);

#ifdef __cplusplus
}
#endif

/**
 * @brief Panic
 * @param fmt 格式化字符串
 * @param ... 可变参数
 * @details 输出致命原因 -> 板级硬件安全关断 -> 驻留死循环等看门狗复位
 */
#undef MINI_PANIC
#define MINI_PANIC(fmt, ...)                                                                                                                         \
    do                                                                                                                                               \
    {                                                                                                                                                \
        mini_log_default_output("[FATAL ERROR] " fmt "\r\n", ##__VA_ARGS__);                                                                        \
        system_safety_hardware_shutdown("MINI_PANIC");                                                                                               \
        while (1)                                                                                                                                    \
        {                                                                                                                                            \
            ;                                                                                                                                        \
        }                                                                                                                                            \
    } while (0)

/**
 * @brief 关键断言
 * @param[in] cond 条件
 * @param[in] fmt 格式化字符串
 * @param ... 可变参数
 * @details cond 为假时: 输出关键原因 -> 板级硬件安全关断 -> 驻留死循环等看门狗复位
 */
#undef MINI_CRITICAL_ASSERT
#define MINI_CRITICAL_ASSERT(cond, fmt, ...)                                                                                                         \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if (!(cond))                                                                                                                                 \
        {                                                                                                                                            \
            mini_log_default_output("[1 FAILED] %s:%d: " fmt "\r\n", __FILE__, __LINE__, ##__VA_ARGS__);                                            \
            system_safety_hardware_shutdown("MINI_CRITICAL_ASSERT");                                                                                 \
            while (1)                                                                                                                                \
            {                                                                                                                                        \
                ;                                                                                                                                    \
            }                                                                                                                                        \
        }                                                                                                                                            \
    } while (0)

#endif /* MINI_PANIC_H */
