idf_component_register(
    SRCS "hello_world_main.c"           # 组件源文件
    REQUIRES
        wifi_softap                     # 自定义 SoftAP 组件
        spi_flash                       # 使用 esp_flash.h
        esp_system                      # 芯片信息与系统 API
        log                             # 日志组件
    INCLUDE_DIRS ""                    # 暴露的头文件目录（此处为空）
)