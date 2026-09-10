/**
 *@copyright SPDX-License-Identifier: Apache-2.0
 *@file mini_time.h
 *@brief 毫秒时钟与延时
 *@author H-000-H
 *@details
 *   mini_time — 与操作系统无关的"时间"公共设施, 由各后端分别实现:
 *     - 裸机    : 复用 xtask 调度器 tick (WFI 忙等, 不让出)
 *     - mini-os : mini_os_get_tick + mini_os_thread_delay_ms (让出调度器)
 *     - FreeRTOS: xTaskGetTickCount + vTaskDelay
 *     - RT-Thread: rt_tick_get + rt_thread_mdelay
 *
 *   注意语义差异 (与锁无关, 但影响行为):
 *     - OS 后端的 mini_delay_ms 会**让出调度器**, 裸机版是**忙等不让出**。
 *       持锁区间内调用会让出点暴露给其他任务, 这是互斥锁必须按后端分发的根因,
 *       详见 docs 中后端切换说明。
 *     - mini_delay_us 全部为不让出的短忙等 (1-Wire / bitbang 等短时序), ISR 中可用。
 */

#ifndef MINI_TIME_H
#define MINI_TIME_H

#include "compiler_compat.h"
#include "compiler_inline.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 执行 WFI 等待中断 (低功耗忙等)
 * @note 仅屏蔽中断等待事件; 裸机延时与裸机互斥锁自旋共用。
 *       未支持的目标退化为空操作。
 */
MINI_STATIC_INLINE void mini_wfi(void)
{
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_8M_BASE__) ||                            \
    defined(__ARM_ARCH_8M_MAIN__) || defined(__CORTEX_M) || defined(__riscv)
    __asm__ volatile("wfi");
#else
    /* 未支持的目标 (含 host / x86 构建): 空操作 */
#endif
}

/**
 * @brief 获取当前时间 (毫秒, 单调递增)
 * @return 自启动以来的毫秒数
 */
uint32_t mini_time_ms(void);

/**
 * @brief 延时毫秒
 * @param[in] ms 毫秒 (0 立即返回)
 * @note OS 后端会阻塞并让出调度器; 裸机为 WFI 忙等, 期间整个系统停摆
 */
void mini_delay_ms(uint32_t ms);

/**
 * @brief 忙等微秒 (1-Wire / bitbang 等短时序)
 * @param[in] us 微秒 (0 立即返回)
 * @note 不让出调度器; ISR 中可用
 */
void mini_delay_us(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif /* MINI_TIME_H */
