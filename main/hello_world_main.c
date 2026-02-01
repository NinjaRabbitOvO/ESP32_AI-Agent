/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>              // 标准输入输出
#include <inttypes.h>           // 提供整数格式化宏
#include "sdkconfig.h"          // 自动生成的配置宏
#include "freertos/FreeRTOS.h"  // FreeRTOS 核心定义
#include "freertos/task.h"      // FreeRTOS 任务 API
#include "esp_chip_info.h"      // 芯片信息查询
#include "esp_flash.h"          // Flash 相关 API
#include "esp_system.h"         // 系统级功能（重启、内存）

void app_main(void)
{
    printf("Hello world!\n");  // 启动后先打印欢迎语

    /* 打印芯片信息 */
    esp_chip_info_t chip_info;  // 用于保存芯片特性
    uint32_t flash_size;        // 保存 flash 大小（字节）
    esp_chip_info(&chip_info);  // 读取芯片信息到结构体
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;  // 主版本号（高两位）
    unsigned minor_rev = chip_info.revision % 100;  // 次版本号（低两位）
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");  // 读取 flash 大小失败时直接返回
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());  // 输出历史最小空闲堆

    for (int i = 10; i >= 0; i--) {  // 倒计时 10 秒后重启
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // 延时 1 秒（FreeRTOS 节拍）
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();  // 触发软件重启
}
