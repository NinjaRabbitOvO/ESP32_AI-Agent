/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>              // 标准输入输出
#include <string.h>             // 字符串工具（计算长度）
#include <inttypes.h>           // 整数格式化宏
#include "sdkconfig.h"          // 自动生成的配置宏
#include "freertos/FreeRTOS.h"  // FreeRTOS 核心定义
#include "freertos/task.h"      // FreeRTOS 任务 API
#include "esp_chip_info.h"      // 芯片信息查询
#include "esp_flash.h"          // Flash 相关 API
#include "esp_system.h"         // 系统级功能（重启、内存）
#include "esp_event.h"          // 事件循环
#include "esp_netif.h"          // 网络接口初始化
#include "esp_wifi.h"           // Wi-Fi 驱动
#include "nvs_flash.h"          // NVS 存储初始化
#include "esp_log.h"            // 日志输出
#include "esp_err.h"            // 错误码定义

#define WIFI_SSID "ESP32-S3"       // SoftAP SSID
#define WIFI_PASS "2452460803"     // SoftAP 密码
#define WIFI_CHANNEL 1              // SoftAP 信道
#define WIFI_MAX_CONN 4             // 允许的最大并发 STA 数

static const char *TAG = "app";

// 初始化 NVS，Wi-Fi 驱动依赖于此
static void init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

// 配置为 SoftAP 模式，开启 Wi-Fi
static void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());                    // 初始化 TCP/IP 栈
    ESP_ERROR_CHECK(esp_event_loop_create_default());     // 创建默认事件循环
    esp_netif_create_default_wifi_ap();                   // 创建默认 AP 接口

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = WIFI_CHANNEL,
            .password = WIFI_PASS,
            .max_connection = WIFI_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
    };

    // 密码长度小于 8 时切换为开放网络（此处密码满足要求，仅作为保护）
    if (strlen(WIFI_PASS) < 8)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "SoftAP started, SSID:%s password:%s channel:%d", WIFI_SSID, WIFI_PASS, WIFI_CHANNEL);
}

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_INFO); // 设置默认日志级别
    printf("Hello world!\n");             // 启动后先打印欢迎语

    init_nvs();          // 确保 NVS 可用
    wifi_init_softap();  // 开启 SoftAP

    /* 打印芯片信息 */
    esp_chip_info_t chip_info; // 保存芯片特性
    uint32_t flash_size;       // 保存 flash 大小（字节）
    esp_chip_info(&chip_info); // 读取芯片信息到结构体
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100; // 主版本号（高两位）
    unsigned minor_rev = chip_info.revision % 100; // 次版本号（低两位）
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed"); // 读取 flash 大小失败时直接返回
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size()); // 输出历史最小空闲堆

    // SoftAP 运行后保持任务存活，避免示例自动重启
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}