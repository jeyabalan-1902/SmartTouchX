#ifndef FLASH_CMD_H
#define FLASH_CMD_H

#include "esp_app.h"
#include "http.h"
#include "crc32.h"
#include "uart_config.h"


#define FLASH_BASE_ADDRESS              0x08008000

#define FLASH_ERASE_BASE_SECTOR         2
#define FLASH_ERASE_SECTORS             3

#define Flash_HAL_OK                    0x00
#define Flash_HAL_ERROR                 0x01
#define Flash_HAL_BUSY                  0x02
#define Flash_HAL_TIMEOUT               0x03
#define Flash_HAL_INV_ADDR              0x04

// BL Commands
#define COMMAND_BL_GET_CID              0x51
#define COMMAND_BL_GO_TO_RESET          0x52
#define COMMAND_BL_FLASH_ERASE          0x53
#define COMMAND_BL_MEM_WRITE            0x54

// Command Lengths
#define COMMAND_BL_GET_CID_LEN          6
#define COMMAND_BL_GO_TO_RESET_LEN      6
#define COMMAND_BL_FLASH_ERASE_LEN      8
#define COMMAND_BL_MEM_WRITE_BASE_LEN   7 

#define BL_ACK                          0xA5
#define BL_NACK                         0x7F

esp_err_t send_sync_command(void);
esp_err_t send_get_cid_command(void);
esp_err_t send_flash_erase_command(uint8_t sector, uint8_t num_sectors);
esp_err_t send_mem_write_command(uint32_t base_address, uint8_t *data, uint8_t length);
esp_err_t send_go_reset();
esp_err_t flash_downloaded_firmware(void);


#endif