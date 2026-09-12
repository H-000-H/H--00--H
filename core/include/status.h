/**
 *@copyright SPDX-License-Identifier: Apache-2.0
 *@file status.h
 *@brief status 头文件
 *@author H-000-H
 *@details
 *   status.h — 栈公共状态码与指针错误编码 (层无关)
 *   HAL / bus / VFS / 统一接口共用。HAL 不得依赖 VFS.h；需要错误码时包含本头。
 *
 *   ── 编号策略 (自持编号, 不依赖 <errno.h>, 跨工具链数值稳定) ──
 *     0              成功
 *     -1  .. -63     通用/栈层段    本头全部 MINI_ERR_*; -28..-63 预留扩容
 *     -64 .. -127    子系统段       net / fs / ota / log / system 私有码
 *     -128.. -255    驱动/板级段    drivers / board / 产品私有码
 *   幅度上限 MINI_ERR_MAX(255), 以兼容 ERR_PTR 的指针编码。
 *   子系统/驱动段私有码用 MINI_ERR_BUILD(mag) 构造, 幅度落在对应段内;
 *   用 MINI_ERR_SECTOR_OF() / MINI_ERR_IS_SUBSYS() / MINI_ERR_IS_DRIVER() 判归属。
 *
 *   ── 命名空间边界 (互不混用, 跨边界必须显式翻译) ──
 *     MINI_ERR_* / MINI_OK   本头, 栈内唯一通用命名空间
 *     MINI_OS_ERR_*          lib/mini-os 内核 API; 数值与本头逐位对齐, 零转换互转
 *     NET_ERR_* / NET_OK     net 上层协议包装层私有 (负 errno 语义), 自成一套
 *     BUFF_*                 algorithm/buffer 私有 (包装 errno)
 *     MINI_LOG_ERR_*         mini-log 私有, 仅 mini-log 内部使用
 *   ⚠ 不同命名空间的码不可直接按数值比较, 在层边界处翻译 (见 net/port/net_error.h)。
 */

#ifndef STATUS_H
#define STATUS_H

#include "compiler_inline.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MINI_ERR_MAX 255 /**< 错误码幅度上限, ERR_PTR 编码上限 */

/**
 * @brief 错误码类型 (0 成功, 负数失败)
 * @note 负值顺序与 lib/mini-os/inc/err.h 的后备分支逐位对齐, 保证两分支等价
 * @note 返回错误码的函数应把返回类型写成 mt_err_t; 参数类型保持 int,
 *       以兼容 C++ 侧 (enum -> int 隐式可行, int -> enum 需显式转换)
 */
typedef enum mt_err
{
    MINI_OK = 0, /**< 成功 */

    /* ── 通用/栈层段 [-1..-27] ── */
    MINI_ERR_INVAL    = -1,  /**< 无效参数 */
    MINI_ERR_ISR      = -2,  /**< 中断上下文非法调用 */
    MINI_ERR_NOMEM    = -3,  /**< 内存不足 */
    MINI_ERR_IO       = -4,  /**< 物理 IO 错误 */
    MINI_ERR_BUSY     = -5,  /**< 设备忙 */
    MINI_ERR_AGAIN    = -6,  /**< 重试 */
    MINI_ERR_NOSPC    = -7,  /**< 无剩余空间/通道 */
    MINI_ERR_TIMEOUT  = -8,  /**< 锁获取/操作超时 */
    MINI_ERR_HW_FATAL = -9,  /**< 硬件物理故障, 不可恢复 */
    MINI_ERR_DEFER    = -10, /**< 依赖未就绪, 稍后重试 */
    MINI_ERR_NODEV    = -11, /**< 设备已拆除或不存在 */
    MINI_ERR_NOTSUPP  = -12, /**< 操作不支持/未实现 */
    MINI_ERR_STATE    = -13, /**< 状态非法/时序错误 */

    /* ── 扩容 [-14..-27] ── */
    MINI_ERR_RANGE    = -14, /**< 数值/索引越界 */
    MINI_ERR_NOENT    = -15, /**< 目标不存在 (节点/键/表项) */
    MINI_ERR_EXIST    = -16, /**< 目标已存在 */
    MINI_ERR_NOTINIT  = -17, /**< 尚未初始化 */
    MINI_ERR_NODATA   = -18, /**< 无数据可读 (非错误性为空) */
    MINI_ERR_CORRUPT  = -19, /**< 数据损坏/完整性校验失败 */
    MINI_ERR_CRC      = -20, /**< 校验和不匹配 (CRC/LRC) */
    MINI_ERR_PARITY   = -21, /**< 帧/奇偶错误 */
    MINI_ERR_OVERFLOW = -22, /**< 计数/缓冲溢出 */
    MINI_ERR_NOTREADY = -23, /**< 未就绪 (上电/标定/预热未完成) */
    MINI_ERR_CANCELED = -24, /**< 操作被取消/中止 */
    MINI_ERR_PERM     = -25, /**< 无权限 */
    MINI_ERR_PROTO    = -26, /**< 协议违例 */
    MINI_ERR_AUTH     = -27  /**< 认证/鉴权失败 */

    /* [-28..] 预留: 通用扩容, 不分配给子系统 */
} mt_err_t;

/* ── 分段边界 (幅度, 均为正整数) ── */
#define MINI_ERR_SECTOR_COMMON_FIRST 1U
#define MINI_ERR_SECTOR_COMMON_LAST 63U
#define MINI_ERR_SECTOR_SUBSYS_FIRST 64U
#define MINI_ERR_SECTOR_SUBSYS_LAST 127U
#define MINI_ERR_SECTOR_DRIVER_FIRST 128U
#define MINI_ERR_SECTOR_DRIVER_LAST MINI_ERR_MAX

/**
 * @brief 由幅度构造子系统/驱动段私有码
 * @param[in] mag 幅度, 必须落在所属段内 (子系统 64..127, 驱动 128..255)
 * @return 负的错误码
 */
#define MINI_ERR_BUILD(mag) (-(int)(mag))

/* ── 子系统段分片 (幅度基准; 每片 16 码) ──
 * 子系统私有码走本段, 不要另立 errno 风格的独立命名空间 —— 同一段内便于用
 * MINI_ERR_IS_SUBSYS() 判归属, 且数值跨工具链稳定。 */
#define MINI_ERR_SUBSYS_NET_BASE 64U  /**< 64..79    net 协议/传输层 */
#define MINI_ERR_SUBSYS_FS_BASE 80U   /**< 80..95    fs 文件系统 (预留) */
#define MINI_ERR_SUBSYS_OTA_BASE 96U  /**< 96..111   ota 升级/引导 (预留) */
#define MINI_ERR_SUBSYS_LOG_BASE 112U /**< 112..127  log 日志后端 */

/**
 * @brief 构造子系统私有码
 * @param[in] base 子系统段基准幅度 (MINI_ERR_SUBSYS_*_BASE)
 * @param[in] idx 段内偏移 (0..15)
 * @return 负的错误码, 归属 MINI_ERR_SECTOR_SUBSYS
 * @code
 *   #define NET_ERR_INVAL MINI_ERR_SUBSYS(MINI_ERR_SUBSYS_NET_BASE, 0)
 * @endcode
 */
#define MINI_ERR_SUBSYS(base, idx) (-(int)((base) + (unsigned)(idx)))

/**
 * @brief 取错误码幅度 (绝对值)
 * @param[in] err 错误码
 * @return 幅度; 非负入参原样返回
 * @note 用无符号回绕避免 INT_MIN 取负的未定义行为
 */
#define MINI_ERR_MAGNITUDE(err) ((unsigned)0 - (unsigned)(err))

/**
 * @brief 错误码所属段
 */
typedef enum mt_err_sector
{
    MINI_ERR_SECTOR_INVALID = 0, /**< 非负, 或幅度超出 MINI_ERR_MAX */
    MINI_ERR_SECTOR_COMMON  = 1, /**< [-1..-63]    通用/栈层 */
    MINI_ERR_SECTOR_SUBSYS  = 2, /**< [-64..-127]  子系统私有 */
    MINI_ERR_SECTOR_DRIVER  = 3  /**< [-128..-255] 驱动/板级私有 */
} mt_err_sector_t;

/**
 * @brief 判定错误码所属段
 * @param[in] err 错误码 (MINI_OK / MINI_ERR_*)
 * @return 所属段; 成功码或越界码返回 MINI_ERR_SECTOR_INVALID
 */
MINI_STATIC_INLINE mt_err_sector_t MINI_ERR_SECTOR_OF(int err)
{
    unsigned mag;

    if (err >= 0)
        return MINI_ERR_SECTOR_INVALID;

    mag = MINI_ERR_MAGNITUDE(err);
    if (mag > MINI_ERR_MAX)
        return MINI_ERR_SECTOR_INVALID;
    if (mag <= MINI_ERR_SECTOR_COMMON_LAST)
        return MINI_ERR_SECTOR_COMMON;
    if (mag <= MINI_ERR_SECTOR_SUBSYS_LAST)
        return MINI_ERR_SECTOR_SUBSYS;
    return MINI_ERR_SECTOR_DRIVER;
}

/** @brief 是否为通用/栈层段错误码 */
MINI_STATIC_INLINE bool MINI_ERR_IS_COMMON(int err) { return MINI_ERR_SECTOR_OF(err) == MINI_ERR_SECTOR_COMMON; }
/** @brief 是否为子系统段私有错误码 */
MINI_STATIC_INLINE bool MINI_ERR_IS_SUBSYS(int err) { return MINI_ERR_SECTOR_OF(err) == MINI_ERR_SECTOR_SUBSYS; }
/** @brief 是否为驱动/板级段私有错误码 */
MINI_STATIC_INLINE bool MINI_ERR_IS_DRIVER(int err) { return MINI_ERR_SECTOR_OF(err) == MINI_ERR_SECTOR_DRIVER; }

/**
 * @brief 错误码转可读字符串 
 * @param[in] err MINI_OK / MINI_ERR_* 码
 * @return 常量字符串; 未知码返回 "MINI_ERR_UNKNOWN"
 * @note 无动态分配、无静态缓冲, 线程与 ISR 安全; 实现见 core/src/status.c,
 *       未被引用时由链接器整体丢弃, 不用字符串表的构建零开销。
 */
const char* MINI_ERR_TO_STR(int err);

#define MINI_IRQ_ENTRY_BOTTOM (0X01U)   /* 中断下部入口标识 */
#define MINI_IRQ_ENTRY_NOBOTTOM (0X00U) /* 中断上部入口标识 */

/* 指针的特殊处理 */
extern const char ERR_SECTION_BASE;
#define ERR_BASE ((uintptr_t)&ERR_SECTION_BASE)

MINI_STATIC_INLINE void* ERR_PTR(int err)
{
    unsigned mag = MINI_ERR_MAGNITUDE(err); /* MINI_OK(0) 得 0, 与旧行为一致 */

    if (mag > MINI_ERR_MAX)
        mag = MINI_ERR_MAGNITUDE(MINI_ERR_INVAL);

    return (void*)(ERR_BASE + (uintptr_t)mag);
}

/**
 * @brief 从 ERR_PTR 指针还原错误码 (与 ERR_PTR 互逆)
 * @param[in] PTR ERR_PTR 返回的指针
 * @return 负的错误码 (MINI_ERR_*)
 */
MINI_STATIC_INLINE mt_err_t PTR_ERR(const void* PTR) { return (mt_err_t)(-(int)(((uintptr_t)PTR) - ERR_BASE)); }

/**
 * @brief 判断指针是否为 ERR_PTR 编码的错误指针
 * @param[in] ptr 待判断指针
 * @return 错误指针返回 true
 */
MINI_STATIC_INLINE bool IS_ERR(const void* ptr) { return (uintptr_t)ptr >= ERR_BASE; }

/**
 * @brief 判断指针为 NULL 或 ERR_PTR 编码的错误指针
 * @param[in] ptr 待判断指针
 * @return NULL 或错误指针返回 true
 */
MINI_STATIC_INLINE bool IS_ERR_OR_NULL(const void* ptr) { return (ptr == NULL) || IS_ERR(ptr); }

#endif /* STATUS_H */
