idf_component_register(
    SRCS ams7341.cpp
    INCLUDE_DIRS .
    REQUIRES i2cdev log esp_idf_lib_helpers
)
