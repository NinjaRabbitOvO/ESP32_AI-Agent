idf_component_register(SRCS "hello_world_main.c"   # 组件源文件
                       PRIV_REQUIRES spi_flash     # 仅内部依赖 spi_flash 组件
                       INCLUDE_DIRS "")            # 暴露的头文件目录（此处为空）
