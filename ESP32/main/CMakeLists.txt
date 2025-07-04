idf_component_register(SRCS "esp_app.c"
                            "src/crc32.c"
                            "src/flash_cmd.c"
                            "src/http.c"
                            "src/mqtt.c"
                            "src/uart_config.c"
                            "src/wifi.c"
                            "src/update_esp.c"
                            "src/http_server.c"
                            "src/gpio.c"
                            "src/nvs.c"
                            "src/ble.c"
                            "secret/mqtt_config.c"
                    INCLUDE_DIRS "inc" "secret"
                    EMBED_TXTFILES ${project_dir}/main/server_certs/ca_cert.pem
                    REQUIRES mbedtls nvs_flash esp_wifi esp_event mqtt json esp_http_server esp_driver_uart esp_driver_gpio
                    PRIV_REQUIRES app_update esp_http_client esp_https_ota protocomm bt)
