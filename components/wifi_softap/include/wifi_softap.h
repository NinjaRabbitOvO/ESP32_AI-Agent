#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 启动 SoftAP 模式。
 *
 * @param ssid      要广播的 SSID 字符串。
 * @param password  密码（>=8 字符时使用 WPA/WPA2，不满足则自动转为开放网络）。
 * @param channel   信道号（1-13）。
 * @param max_conn  允许连接的最大 STA 数量。
 *
 * @return ESP_OK 成功，其它错误码见 esp_err.h
 */
esp_err_t wifi_softap_start(const char *ssid, const char *password, uint8_t channel, uint8_t max_conn);

#ifdef __cplusplus
}
#endif