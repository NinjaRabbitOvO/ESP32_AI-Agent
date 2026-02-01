idf_component_register(
    SRCS "hello_world_main.c"           # 组件源文件
    REQUIRES wifi_softap                 # 依赖自定义 SoftAP 组件
    INCLUDE_DIRS ""                     # 暴露的头文件目录（此处为空）
)