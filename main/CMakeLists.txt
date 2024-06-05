idf_component_register(SRCS 
    "src/main.c"
    src/logging/logging_utils.c
    src/connect_wifi/connect_wifi.c
    src/http_handlers/http_handlers.c
    src/web_server/web_server.c
    src/gpio_utils/gpio_utils.c
    src/motor_control/motor_control.c
    src/task_utils/task_utils.c
    src/gpio_state/gpio_state.c
    src/encoder/encoder.c
    INCLUDE_DIRS "src"
)
