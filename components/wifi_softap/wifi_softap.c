#include <string.h>
#include <stdlib.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"

#include "wifi_softap.h"

static const char *TAG = "wifi_softap";

static esp_err_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

static uint8_t select_best_channel(void)
{
    wifi_scan_config_t scan_cfg = {0};
    uint16_t ap_count = 0;
    int channel_counts[14] = {0};
    uint8_t best_channel = 1;
    int best_count = 0x7fffffff;
    const uint8_t preferred[] = {1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13};

    esp_err_t ret = esp_wifi_scan_start(&scan_cfg, true);
    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "scan start failed (%s), fallback to channel 1", esp_err_to_name(ret));
        return 1;
    }

    ret = esp_wifi_scan_get_ap_num(&ap_count);
    if (ret != ESP_OK || ap_count == 0)
    {
        ESP_LOGW(TAG, "scan get ap num failed (%s), fallback to channel 1", esp_err_to_name(ret));
        return 1;
    }

    wifi_ap_record_t *ap_list = (wifi_ap_record_t *)calloc(ap_count, sizeof(wifi_ap_record_t));
    if (!ap_list)
    {
        ESP_LOGW(TAG, "alloc ap list failed, fallback to channel 1");
        return 1;
    }

    ret = esp_wifi_scan_get_ap_records(&ap_count, ap_list);
    if (ret != ESP_OK)
    {
        free(ap_list);
        ESP_LOGW(TAG, "scan get ap records failed (%s), fallback to channel 1", esp_err_to_name(ret));
        return 1;
    }

    for (uint16_t i = 0; i < ap_count; ++i)
    {
        uint8_t ch = ap_list[i].primary;
        if (ch >= 1 && ch <= 13)
        {
            channel_counts[ch]++;
        }
    }
    free(ap_list);

    for (size_t i = 0; i < sizeof(preferred); ++i)
    {
        uint8_t ch = preferred[i];
        if (channel_counts[ch] < best_count)
        {
            best_count = channel_counts[ch];
            best_channel = ch;
        }
    }

    ESP_LOGI(TAG, "auto channel selected: %d (min AP count=%d)", best_channel, best_count);
    return best_channel;
}

esp_err_t app_wifi_softap_start(const char *ssid, const char *password, uint8_t channel, uint8_t max_conn)
{
    ESP_RETURN_ON_FALSE(ssid && ssid[0] != '\0', ESP_ERR_INVALID_ARG, TAG, "SSID cannot be empty");
    ESP_RETURN_ON_FALSE(channel == 0 || (channel >= 1 && channel <= 13),
                        ESP_ERR_INVALID_ARG, TAG, "channel must be 1-13 or 0 for auto");

    ESP_ERROR_CHECK(init_nvs());

    // 初始化网络栈与事件循环（重复调用也安全）
    esp_err_t ret = esp_netif_init();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        return ret;
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        return ret;
    }

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint8_t selected_channel = channel;
    if (channel == 0)
    {
        ESP_LOGI(TAG, "auto channel scan enabled");
        esp_err_t scan_ret = esp_wifi_set_mode(WIFI_MODE_STA);
        if (scan_ret == ESP_OK)
        {
            scan_ret = esp_wifi_start();
        }

        if (scan_ret == ESP_OK)
        {
            selected_channel = select_best_channel();
        }
        else
        {
            ESP_LOGW(TAG, "scan setup failed (%s), fallback to channel 1", esp_err_to_name(scan_ret));
            selected_channel = 1;
        }

        esp_wifi_stop(); // best effort，忽略返回值
    }

    wifi_config_t wifi_config = {0};
    strlcpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = strlen(ssid);
    wifi_config.ap.channel = selected_channel;
    wifi_config.ap.max_connection = max_conn;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.pmf_cfg.required = false;

    if (password && strlen(password) >= 8)
    {
        strlcpy((char *)wifi_config.ap.password, password, sizeof(wifi_config.ap.password));
    }
    else
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "SoftAP started, SSID:%s password:%s channel:%d max_conn:%d",
             ssid, password ? password : "<open>", selected_channel, max_conn);
    return ESP_OK;
}
