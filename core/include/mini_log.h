/**
 *@copyright SPDX-License-Identifier: Apache-2.0
 *@file mini_log.h
 *@brief 分级日志
 *@author H-000-H
 *@details
 *   mini_log — 与操作系统无关的日志设施, 实现统一走 printf_output。
 *
 *   上层通常不直接调用本文件, 而是用 core/include/system_log.h 的
 *   SYS_LOGI/W/E 与 DRV_LOGI/W/E/D/V 宏; 那两个宏根据 Kconfig 选择
 *   mini_log 或 ESP-IDF esp_log 后端。
 *
 *   注意: 本文件不做日志级别过滤 (level 仅作接口保留);
 *   需要过滤请在 output 层或 SYS_LOG 宏层做。
 */

#ifndef MINI_LOG_H
#define MINI_LOG_H

#include "compiler_compat.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MINI_LOG_LEVEL_T_DEFINED
#define MINI_LOG_LEVEL_T_DEFINED
/**
 * @brief 日志级别
 */
typedef enum
{
    MINI_LOG_ERROR = 0, /**< 错误 */
    MINI_LOG_WARN,      /**< 告警 */
    MINI_LOG_INFO,      /**< 信息 */
    MINI_LOG_DEBUG,     /**< 调试 */
} mini_log_level_t;
#endif

/**
 * @brief 分级日志输出 (info/warn/error)
 * @param[in] level 日志级别 (MINI_LOG_*)
 * @param[in] tag 日志标签 (NULL 时使用 "drv")
 * @param[in] fmt printf 格式串 (NULL 时输出 "(null)")
 * @param[in] ... 格式化参数
 */
void mini_log(mini_log_level_t level, const char* tag, const char* fmt, ...);

/**
 * @brief 致命日志输出 (panic 前调用)
 * @param[in] fmt printf 格式串
 * @param[in] ... 格式化参数
 */
void mini_log_fatal(const char* fmt, ...) MINI_FMT_PRINTF(1, 2);

/**
 * @brief 关键断言失败日志 (含文件/行号)
 * @param[in] file 源文件名
 * @param[in] line 行号
 * @param[in] fmt printf 格式串
 * @param[in] ... 格式化参数
 */
void mini_log_critical_assert(const char* file, int line, const char* fmt, ...) MINI_FMT_PRINTF(3, 4);

#ifdef __cplusplus
}
#endif

#endif /* MINI_LOG_H */
