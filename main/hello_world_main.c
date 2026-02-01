/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>              // 标准输入输出
#include <inttypes.h>           // 整数格式化宏
#include "sdkconfig.h"          // 自动生成的配置宏
#include "freertos/FreeRTOS.h"  // FreeRTOS 核心定义
#include "freertos/task.h"      // FreeRTOS 任务 API
#include "esp_chip_info.h"      // 芯片信息查询
#include "esp_flash.h"          // Flash 相关 API
#include "esp_system.h"         // 系统级功能（重启、内存）
#include "esp_log.h"            // 日志输出

#include "wifi_softap.h"        // SoftAP 组件接口

static const char *TAG = "app";

static void print_chip_info(void)
{
    esp_chip_info_t chip_info;
    uint32_t flash_size = 0;

    esp_chip_info(&chip_info);
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

    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK)
    {
        printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
               (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    }
    else
    {
        printf("Get flash size failed\n");
    }

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    printf("Hello world!\n");

    // 启动 SoftAP：SSID=ESP32-S3，密码=2452460803，信道 1，最多 4 个连接
    ESP_ERROR_CHECK(wifi_softap_start("ESP32-S3", "2452460803", 1, 4));

    // 打印芯片信息
    print_chip_info();

    // 保持任务存活，SoftAP 持续运行
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}