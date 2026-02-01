idf_component_register(
    SRCS "hello_world_main.c"           # 组件源文件
    PRIV_REQUIRES spi_flash esp_wifi     # 内部依赖：flash 与 Wi-Fi
                 esp_event esp_netif     # 事件循环与网络接口
                 nvs_flash               # NVS 初始化
    INCLUDE_DIRS ""                     # 暴露的头文件目录（此处为空）
)