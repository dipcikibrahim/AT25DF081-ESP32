#ifndef _AT25DF081_H_
#define _AT25DF081_H_

#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#define at25df081_tag "AT25DF081 spi_external_flash"
//#define at25df081_led

#define at25df081_initial_addr 0x0
#define at25df081_pagesize 0x0100
#define at25df081_final_addr 0xFFFFF

typedef struct {
    int host;
    int sck_pin;
    int mosi_pin;
    int miso_pin;
    int cs_pin;
#ifdef at25df081_led
    int led_pin
#endif
    int clk_freq_hz;
    int max_tx_size;
    int queue_size;
} at25df081_spi_conf_t;

typedef struct {
    size_t tx_size;
    uint8_t tx_buffer[512];
    size_t rx_size;
    uint8_t rx_buffer[512];
} at25df081_spist_t;
// Transmit structure of the library.

typedef enum at25df081_make_list{
    at25df081_make_readid,
    at25df081_make_wenable,
    at25df081_make_wdisable,
    at25df081_make_chiperase,
    at25df081_make_reset,
    at25df081_make_globalunprotect,
    at25df081_make_globalprotect,
    at25df081_make_write_status1,
    at25df081_make_waitready,
    at25df081_make_read_status,
    at25df081_make_transmit,
}at25df081_make_list_t;

// Instruction Codes
#define at25df081_cmd_wenable 0x06
#define at25df081_cmd_wdisable 0x04

#define at25df081_cmd_page_program 0x02 // 1 to 256 bytes
#define at25df081_cmd_read_array 0x0B

#define at25df081_cmd_chip_erase 0xC7

#define at25df081_cmd_be_4kb 0x20
#define at25df081_cmd_be_32kb 0x52
#define at25df081_cmd_be_64kb 0xD8

#define at25df081_cmd_reset_flash 0xF0
#define at25df081_cmd_read_mdid 0x9F
#define at25df081_cmd_globalunprotect 0x00
#define at25df081_cmd_globalprotect 0x3C
#define at25df081_cmd_unprotect_sector 0x39
#define at25df081_cmd_protect_sector 0x36
#define at25df081_cmd_write_status1 0x01

#define at25df081_readybit_mask (1<<0)
#define at25df081_webit_mask (1<<1)
#define at25df081_epsbit_mask (1<<5)

#define at25df081_cmd_readstatus 0x05
#define dummy_data 0

static spi_device_handle_t at25df081_bus;
uint8_t at25df081_flag = 0x00;
int at25df081_cs_pin;
#ifdef at25df081_led
    int at25df081_led_pin;
#endif

esp_err_t at25df081_init(at25df081_spi_conf_t *cfg);
// Initialize function of the flash.
void spi_pre_transfer_callback(spi_transaction_t *t);
// Previous transfer callback of the ESP32 spi interface.
void spi_after_transfer_callback(spi_transaction_t *t);
// After transfer callback of the ESP32 spi interface.
esp_err_t at25df081_make(at25df081_make_list_t command, at25df081_spist_t *ts);
// Command shortcut function of the flash functions.
esp_err_t at25df081_transfer(at25df081_spist_t *ts);
// Transfer function of the flash.
esp_err_t at25df081_pp(uint32_t addr, uint8_t *data, size_t len, uint32_t *fn_addr); 
// Page program sequence, addr should be first byte of the page want to be programmed.
esp_err_t at25df081_read(uint32_t addr, uint8_t *data, size_t len);
// Read function of the flash.

#include "AT25DF081.c"

#endif
