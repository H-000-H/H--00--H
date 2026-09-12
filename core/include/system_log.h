/**
 *@copyright SPDX-License-Identifier: Apache-2.0
 *@file system_log.h
 *@brief system log 头文件
 *@author H-000-H
 *@details
 *   system_log — 系统日志宏统一入口 (mini-log / ESP-IDF 两后端)
 *   根据 Kconfig CONFIG_SYS_LOG_USE_* 选择后端, 提供 MT_LOG_ERROR/WARN/INFO 三级宏
 *   与 MT_DRV_LOG_* 驱动日志宏。
 *
 *   非 ESP 后端走随仓库 mini-log (mini-log/inc/log.h): SPSC 环形缓冲 + 可选
 *   flash 落盘, 由 MINI_LOG_* 宏输出; ESP 后端走 esp_log.h。
 *   本文件是全仓日志宏的唯一汇聚点, 各模块请用 MT_LOG_* / MT_DRV_LOG_*,
 *   不要直接调用 mini_log_default_output()。
 */

#ifndef SYSTEM_LOG_H
#define SYSTEM_LOG_H

/* Kconfig 生成的配置 — 见 tools/genconfig.py */
#include "config.h"

#if defined(CONFIG_SYS_LOG_USE_MINI_LOG)

#include "log.h"

/* mini-log 的 MINI_LOG_x 宏不带 tag; 这里把 tag 作为前缀并入格式串,
 * 保持全仓 (tag, fmt, ...) 的既有调用约定。 */
#define MT_LOG_ERROR(tag, fmt, ...) MINI_LOG_E("[%s] " fmt, tag, ##__VA_ARGS__)
#define MT_LOG_WARN(tag, fmt, ...) MINI_LOG_W("[%s] " fmt, tag, ##__VA_ARGS__)
#define MT_LOG_INFO(tag, fmt, ...) MINI_LOG_I("[%s] " fmt, tag, ##__VA_ARGS__)

#elif defined(CONFIG_SYS_LOG_USE_ESP)

#include "esp_log.h"
#define MT_LOG_INFO ESP_LOGI
#define MT_LOG_WARN ESP_LOGW
#define MT_LOG_ERROR ESP_LOGE
#define MT_DRV_LOG_ERROR ESP_LOGE
#define MT_DRV_LOG_WARN ESP_LOGW
#define MT_DRV_LOG_INFO ESP_LOGI
#define MT_DRV_LOG_DEBUG ESP_LOGD
#define MT_DRV_LOG_VERBOSE ESP_LOGD

#else
#error "SYS_LOG backend not configured — choose one in Kconfig"
#endif

#if defined(CONFIG_SYS_LOG_USE_MINI_LOG)
/* -------------------------------------------------------------------------- */
/* 驱动日志宏 (MT_DRV_LOG_*) — 统一走 mini-log 控制台链路 */
/* -------------------------------------------------------------------------- */
#define MT_DRV_LOG_ERROR(tag, fmt, ...) MINI_LOG_E("[%s] " fmt, tag, ##__VA_ARGS__)
#define MT_DRV_LOG_WARN(tag, fmt, ...) MINI_LOG_W("[%s] " fmt, tag, ##__VA_ARGS__)
#define MT_DRV_LOG_INFO(tag, fmt, ...) MINI_LOG_I("[%s] " fmt, tag, ##__VA_ARGS__)
#define MT_DRV_LOG_DEBUG(tag, fmt, ...) MINI_LOG_D("[%s] " fmt, tag, ##__VA_ARGS__)
#define MT_DRV_LOG_VERBOSE(tag, fmt, ...) MINI_LOG_D("[%s] " fmt, tag, ##__VA_ARGS__)
#endif

#endif /* SYSTEM_LOG_H */
