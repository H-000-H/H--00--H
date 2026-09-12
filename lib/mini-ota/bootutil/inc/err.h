/**
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @file: err.h
 * @brief: 全项目统一错误码（0 表示成功，负数为具体失败原因）TODO暂时这套吧现在错误码有点多了有时间统一一下
 * 用法: int rc = image_read_payload(...); if (rc != ERR_OK) { log("%s", err_str(rc)); }
 */
#ifndef ERR_H
#define ERR_H

#ifdef __cplusplus
extern "C" {
#endif

#define ERR_OK                0

/* ---------- 通用 ---------- */
#define ERR_ARG             (-1)  /**< 入参非法：空指针、不支持的模式、长度不对齐 */
#define ERR_TOO_SMALL       (-2)  /**< 数据长度不足：小于该布局/字段所需的最小长度 */
#define ERR_OVERFLOW        (-3)  /**< 长度计算溢出 */
#define ERR_NOT_SUPPORTED     (-4)  /**< 功能未编译进来（如未开启加密支持时的 SHA/GCM/CBC） */
#define ERR_UNSUPPORTED     (-4)  /**< 功能未编译进来（如未开启加密支持时的 SHA/GCM/CBC） */
#define ERR_BUF_TOO_SMALL   (-5)  /**< 输出缓冲区容量不足 */

/* ---------- 校验/加解密 ---------- */
#define ERR_CRC_MISMATCH    (-6)  /**< CRC32 不一致 */
#define ERR_HASH_MISMATCH   (-7)  /**< SHA-256 摘要不一致 */
#define ERR_AUTH_FAILED     (-8)  /**< 认证失败：GCM tag 或 HMAC 不匹配 */
#define ERR_DECRYPT_FAILED  (-9)  /**< 解密失败（底层算法库返回错误） */
#define ERR_HASH_FAILED     (-10) /**< 摘要/HMAC 计算失败（底层算法库返回错误） */
#define ERR_PADDING         (-11) /**< PKCS#7 填充非法（CBC 去填充时校验） */
#define ERR_OTA_OPEN     (-12) /**< OTA 未开启 */
#define ERR_TRANSMIT     (-13) /**<传输错误 */
#define ERR_OTA_STATE    (-14) /**< OTA 持久化状态缺失或损坏（无有效记录/校验不过） */
#define ERR_INVAL    (-15)  /**< 参数非法 */
/**
 * @brief 错误码转可读字符串
 * @param err [in] 本文件定义的错误码
 * @return 静态字符串，不会为 NULL
 */
const char *err_str(int err);

#ifdef __cplusplus
}
#endif

#endif /* ERR_H */
