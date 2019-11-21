#ifndef _AT25DF081_C_
#define _AT25DF081_C_

#include "AT25DF081.h"

void spi_pre_transfer_callback(spi_transaction_t *t)
{
#ifdef at25df081_led
    gpio_set_level(at25df081_led_pin, 1);
#endif
    return;
}

void spi_after_transfer_callback(spi_transaction_t *t)
{
#ifdef at25df081_led
    gpio_set_level(at25df081_led_pin, 0);
#endif
    return;
}

esp_err_t at25df081_transfer(at25df081_spist_t *ts)
{
    esp_err_t ret = ESP_OK;
    if(!at25df081_flag){
        ESP_LOGE(at25df081_tag, "Flash not initialized!");
        return ESP_ERR_NOT_FOUND;
    }
    ret = gpio_set_level(at25df081_cs_pin, 0);
    if (ts->tx_size == 0 && ts->rx_size == 0)
        return ESP_FAIL;
    if (ts == NULL)
        return ESP_FAIL;
    if (ts->tx_size != 0 && ts->tx_buffer != NULL)
    {
        spi_transaction_t t = (spi_transaction_t){
            .length = ts->tx_size * 8,
            .tx_buffer = ts->tx_buffer,
        };
        ret = spi_device_transmit(at25df081_bus, &t);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    }
    if (ts->rx_size != 0 && ts->rx_buffer != NULL)
    {
        spi_transaction_t t = (spi_transaction_t){
            .rxlength = ts->rx_size * 8,
            .rx_buffer = ts->rx_buffer,
        };
        ret = spi_device_transmit(at25df081_bus, &t);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    }
    ret = gpio_set_level(at25df081_cs_pin, 1);
    return ret;
}

esp_err_t at25df081_make(at25df081_make_list_t command, at25df081_spist_t *ts)
{
    esp_err_t ret = ESP_OK;
    if(!at25df081_flag){
        ESP_LOGE(at25df081_tag, "Flash not initialized!");
        return ESP_ERR_NOT_FOUND;
    }
    at25df081_spist_t ts_temp;
    switch (command)
    {
    case at25df081_make_readid:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_read_mdid;
        ts_temp.rx_size = 4;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ESP_LOGI(at25df081_tag, "Device ID: %x %x %x %x", ts_temp.rx_buffer[0], ts_temp.rx_buffer[1], ts_temp.rx_buffer[2], ts_temp.rx_buffer[3]);
        break;
    case at25df081_make_wenable:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_wenable;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        break;
    case at25df081_make_wdisable:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_wdisable;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        break;
    case at25df081_make_chiperase:; // Approx. Chip Erase Time: 16 sec. Max: 28 sec.
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_wenable;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_chip_erase;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        break;
    case at25df081_make_reset:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_wenable;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_reset_flash;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        break;
    case at25df081_make_globalunprotect:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_wenable;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ts_temp.tx_size = 2;
        ts_temp.tx_buffer[0] = at25df081_cmd_write_status1;
        ts_temp.tx_buffer[1] = at25df081_cmd_globalunprotect;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        break;
    case at25df081_make_globalprotect:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_wenable;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ts_temp.tx_size = 2;
        ts_temp.tx_buffer[0] = at25df081_cmd_write_status1;
        ts_temp.tx_buffer[1] = at25df081_cmd_globalprotect;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        break;
    case at25df081_make_write_status1:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_write_status1;
        ts_temp.rx_size = 0;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        break;
    case at25df081_make_waitready:;
        size_t thick = 100;
        do
        {
            vTaskDelay(1 / portTICK_PERIOD_MS);
            ts_temp.tx_size = 1;
            ts_temp.tx_buffer[0] = at25df081_cmd_readstatus;
            ts_temp.rx_size = 2;
            ret = at25df081_transfer(&ts_temp);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
            ts_temp.rx_buffer[0] = ts_temp.rx_buffer[0] & at25df081_readybit_mask;
            thick--;
        } while (ts_temp.rx_buffer[0] == 1 && thick);
        break;
    case at25df081_make_read_status:;
        ts_temp.tx_size = 1;
        ts_temp.tx_buffer[0] = at25df081_cmd_readstatus;
        ts_temp.rx_size = 2;
        ret = at25df081_transfer(&ts_temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ESP_LOGI(at25df081_tag, "Readed Status: %x %x", ts_temp.rx_buffer[0], ts_temp.rx_buffer[1]);
        break;
    case at25df081_make_transmit:;
        if(ts != NULL){
            ts_temp.tx_size = ts->tx_size;
            ts_temp.rx_size = ts->rx_size;
            if(ts->tx_size != 0)
                memcpy(ts_temp.tx_buffer, ts->tx_buffer, (ts->tx_size * sizeof(uint8_t)));
            if(ts->rx_size != 0)
                memcpy(ts_temp.rx_buffer, ts->rx_buffer, (ts->rx_size * sizeof(uint8_t)));
            ret = at25df081_transfer(&ts_temp);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        }else{
            ret = ESP_ERR_INVALID_ARG;
        }
        break;
    default:;
        ret = ESP_ERR_INVALID_ARG;
        break;
    }
    if(ts != NULL){
        if(ts_temp.tx_size != 0)
            memcpy(ts->tx_buffer, ts_temp.tx_buffer, (ts_temp.tx_size * sizeof(uint8_t)));
        if(ts_temp.rx_size != 0)
            memcpy(ts->rx_buffer, ts_temp.rx_buffer, (ts_temp.rx_size * sizeof(uint8_t)));
    }
    return ret;
}

esp_err_t at25df081_init(at25df081_spi_conf_t *cfg)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = cfg->miso_pin,
        .mosi_io_num = cfg->mosi_pin,
        .sclk_io_num = cfg->sck_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = cfg->max_tx_size * 8,
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_NATIVE_PINS,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = cfg->clk_freq_hz,
        .mode = 0,
        .queue_size = cfg->queue_size,
        .pre_cb = spi_pre_transfer_callback,
        .post_cb = spi_after_transfer_callback,
        .address_bits = 0,
        .command_bits = 0,
        .dummy_bits = 0,
        .spics_io_num = -1,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };
    ret = spi_bus_initialize(cfg->host, &buscfg, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(at25df081_tag, "SPI Flash initialization error! %s", esp_err_to_name(ret));
        return ret;
    }
    ret = spi_bus_add_device(cfg->host, &devcfg, &at25df081_bus);
    if (ret != ESP_OK)
    {
        ESP_LOGE(at25df081_tag, "SPI Flash initialization error! %s", esp_err_to_name(ret));
        return ret;
    }
    at25df081_cs_pin = cfg->cs_pin;
    gpio_pad_select_gpio(at25df081_cs_pin);
    gpio_set_direction(at25df081_cs_pin, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(at25df081_cs_pin, 1);
#ifdef at25df081_led
    at25df081_led_pin = cfg->led_pin;
    gpio_pad_select_gpio(at25df081_led_pin);
    gpio_set_direction(at25df081_led_pin, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(at25df081_led_pin, 0);
#endif
    at25df081_flag = 0x01;
    at25df081_spist_t ts;
    ret = at25df081_make(at25df081_make_readid, &ts);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    if(ts.rx_buffer[0] != 0x1F && ts.rx_buffer[1] != 0x45){
        ESP_LOGE(at25df081_tag, "Flash not found!");
        ret = ESP_ERR_NOT_FOUND;
        at25df081_flag = 0x00;
        return ret;
    }
    ret = at25df081_make(at25df081_make_globalunprotect, NULL);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    ret = at25df081_make(at25df081_make_read_status, NULL);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    return ret;
}

esp_err_t at25df081_pp(uint32_t addr, uint8_t *data, size_t len, uint32_t *fn_addr)
{
    esp_err_t ret = ESP_OK;
    if(!at25df081_flag){
        ESP_LOGE(at25df081_tag, "Flash not initialized!");
        return ESP_ERR_NOT_FOUND;
    }
    // Check the final adress if its exceed memory limit
    if((addr + len - 1) > at25df081_final_addr){
        ESP_LOGE(at25df081_tag, "Element exceeded memory limit of the device!");
        return ESP_ERR_INVALID_ARG;
    }
    // Check the memory area if it's already written or free
    uint8_t * control_temp = (uint8_t *) malloc(len * sizeof(uint8_t));
    memset(control_temp, 0x00, len);
    ret = at25df081_read(addr, control_temp, len);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    size_t chk = len;
    do{
        ret = (control_temp[chk - 1] != 0xFF) ? (ESP_ERR_NO_MEM) : (ESP_OK);
        if(ret != ESP_OK)
            break;
        chk--;
    }while(chk);
    free(control_temp);
    if(ret != ESP_OK){
        ESP_LOGE(at25df081_tag, "Memory area is not empty !");
        return ret;
    }
    // Write operation
    at25df081_spist_t temp;
    size_t written_len = 0;
    uint32_t w_addr = addr;
    do
    {
        // Check the write enabled register
        ret = at25df081_make(at25df081_make_waitready, NULL);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ret = at25df081_make(at25df081_make_read_status, &temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        while (!(temp.rx_buffer[0] & at25df081_webit_mask))
        {
            ret = at25df081_make(at25df081_make_waitready, NULL);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
            ret = at25df081_make(at25df081_make_wenable, NULL);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
            ret = at25df081_make(at25df081_make_read_status, &temp);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        }
        // Unprotect the adress sector
        temp.tx_size = 4;
        temp.tx_buffer[0] = at25df081_cmd_unprotect_sector;
        temp.tx_buffer[1] = (addr >> 16) & 0xFF;
        temp.tx_buffer[2] = (addr >> 8) & 0xFF;
        temp.tx_buffer[3] = addr & 0xFF;
        temp.rx_size = 0;
        ret = at25df081_make(at25df081_make_transmit, &temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);

        ret = at25df081_make(at25df081_make_waitready, NULL);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ret = at25df081_make(at25df081_make_read_status, &temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        while (!(temp.rx_buffer[0] & at25df081_webit_mask))
        {
            ret = at25df081_make(at25df081_make_waitready, NULL);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
            ret = at25df081_make(at25df081_make_wenable, NULL);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
            ret = at25df081_make(at25df081_make_read_status, &temp);
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        }

        // Transmit write element with operation code and adress bits.
        temp.tx_size = ((len - written_len) > at25df081_pagesize) ? (at25df081_pagesize + 4) : (len - written_len + 4);
        temp.tx_buffer[0] = at25df081_cmd_page_program;
        temp.tx_buffer[1] = (w_addr >> 16) & 0xFF;
        temp.tx_buffer[2] = (w_addr >> 8) & 0xFF;
        temp.tx_buffer[3] = w_addr & 0xFF;
        memcpy(temp.tx_buffer + 4, data + written_len, temp.tx_size);
        temp.rx_size = 0;
        written_len += temp.tx_size - 4;
        w_addr += at25df081_pagesize;
        *fn_addr = w_addr;
        ret = at25df081_make(at25df081_make_transmit, &temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        // Check the status register for previous operation.
        ret = at25df081_make(at25df081_make_waitready, NULL);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        ret = at25df081_make(at25df081_make_read_status, &temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        if(temp.rx_buffer[0] & at25df081_epsbit_mask){
            ESP_LOGE(at25df081_tag, "Programming error! Aborted!");
            ret = ESP_ERR_FLASH_OP_FAIL;
            break;
        }
    }while(written_len < len);

    return ret;
}

esp_err_t at25df081_read(uint32_t addr, uint8_t *buffer, size_t len)
{
    esp_err_t ret = ESP_OK;
    if(!at25df081_flag){
        ESP_LOGE(at25df081_tag, "Flash not initialized!");
        return ESP_ERR_NOT_FOUND;
    }
    ret = at25df081_make(at25df081_make_waitready, NULL);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    at25df081_spist_t temp;
    uint32_t r_addr = addr;
    size_t readed_len = 0;
    do{
        temp.tx_size = 5;
        temp.tx_buffer[0] = at25df081_cmd_read_array;
        temp.tx_buffer[1] = (r_addr >> 16) & 0xFF;
        temp.tx_buffer[2] = (r_addr >> 8) & 0xFF;
        temp.tx_buffer[3] = r_addr & 0xFF;
        temp.tx_buffer[4] = dummy_data;
        temp.rx_size = ((len - readed_len) > at25df081_pagesize) ? (at25df081_pagesize) : (len - readed_len);
        ret = at25df081_make(at25df081_make_transmit, &temp);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        if(ret != ESP_OK){
            ESP_LOGE(at25df081_tag, "Read error! Aborted!");
            ret = ESP_ERR_FLASH_OP_FAIL;
            break;
        }
        memcpy(buffer + readed_len, temp.rx_buffer, temp.rx_size);
        readed_len += temp.rx_size;
        r_addr += at25df081_pagesize;
    }while(readed_len < len);
    return ret;
}

#endif
