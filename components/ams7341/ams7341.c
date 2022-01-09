#include "ams7341.h"


static const char *TAG = "AS7341";

esp_err_t as7341_init_desc(as7341_dev_t *dev, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
    dev->i2c_dev.port = port;
    dev->i2c_dev.addr = AS7341_I2C_ADDRESS;
    dev->i2c_dev.cfg.sda_io_num = sda_gpio;
    dev->i2c_dev.cfg.scl_io_num = scl_gpio;
#if HELPER_TARGET_IS_ESP32
    dev->i2c_dev.cfg.master.clk_speed = I2C_FREQ_HZ;
#endif

    return i2c_dev_create_mutex(&dev->i2c_dev);
}

uint8_t get_bit_at_position(uint8_t byte, uint8_t position) {
    /*  00000010 
        00000001
    */
    uint8_t bitmask = 0X01;
    byte = byte >> position;
    byte = byte & bitmask;
    return byte;
}

void read_and_print_register(as7341_dev_t *dev, uint8_t reg) {
    uint8_t id;
    esp_err_t err = i2c_dev_read_reg(&dev->i2c_dev, reg, &id, 1);
    if(err != ESP_OK) {
        ESP_LOGW(TAG, "Error retrieving chip id from register");
    } else {
        // ESP_LOGI(TAG, "Found chip with ID %d", id);
        uint8_t arr[8];
        for(int i = 0; i < 8; i++) {
            uint8_t bitVal = get_bit_at_position(id, i);
            arr[i] = bitVal;
        }
        ESP_LOGI(TAG, "Bit %d.%d.%d.%d.%d.%d.%d.%d", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]);

    }
}

esp_err_t as7341_read_channel(as7341_dev_t *dev, uint8_t channel, uint16_t *data) {
    I2C_DEV_TAKE_MUTEX(&dev->i2c_dev);
    esp_err_t err_data = i2c_dev_read_reg(&dev->i2c_dev, channel, data, 2);
    if(err_data != ESP_OK) {
        ESP_LOGE(TAG, "Could not read from data register");
    } 

    I2C_DEV_GIVE_MUTEX(&dev->i2c_dev);
    return err_data;
}

esp_err_t set_again(as7341_dev_t *dev, as7341_again_t gain) {
    I2C_DEV_TAKE_MUTEX(&dev->i2c_dev);

    uint8_t gain_config = gain;
    esp_err_t err = i2c_dev_write_reg(&dev->i2c_dev, REG_CFG_1, &gain_config, 1);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Could not read from data register");
    } 

    I2C_DEV_GIVE_MUTEX(&dev->i2c_dev);
    return err;
}

esp_err_t as7341_init(as7341_dev_t *dev) 
{
    esp_err_t err;
    err = as7341_enable_power_on(dev, true);
    err = set_again(dev, AS7341_GAIN_16X);
    err = as7341_enable_spectral_measurement(dev, true);

    as7341_chip_information_t chip_info;
    err = as7341_get_chip_information(dev, &chip_info);

    ESP_LOGI(TAG, "CHIP_ID: %d, CHIP_REV_ID: %d, CHIP_AUX_ID: %d", (int)chip_info.chip_id, (int)chip_info.chip_rev_id, (int)chip_info.chip_aux_id);

    return err;
}

esp_err_t as7341_set_smux_command(as7341_dev_t *dev, as7341_smux_cmd_t command) {
    I2C_DEV_TAKE_MUTEX(&dev->i2c_dev);
    uint8_t data;
    esp_err_t err = i2c_dev_read_reg(&dev->i2c_dev, REG_ENABLE, &data, 1);
    if(err == ESP_OK) {
        uint8_t enable_spectral_measurement = 0b00000010 | data;
        err = i2c_dev_write_reg(&dev->i2c_dev, REG_ENABLE, &enable_spectral_measurement, 1);
    }
    I2C_DEV_GIVE_MUTEX(&dev->i2c_dev);
    return err;
}
esp_err_t as7341_enable_spectral_measurement(as7341_dev_t *dev, bool enable) {
    I2C_DEV_TAKE_MUTEX(&dev->i2c_dev);
    uint8_t data;
    esp_err_t err = i2c_dev_read_reg(&dev->i2c_dev, REG_ENABLE, &data, 1);
    if(err == ESP_OK) {
        uint8_t enable_spectral_cmd = enable ? 0b00000010 : 0;
        uint8_t enable_spectral_measurement = enable_spectral_cmd | data;
        err = i2c_dev_write_reg(&dev->i2c_dev, REG_ENABLE, &enable_spectral_measurement, 1);
    }
    I2C_DEV_GIVE_MUTEX(&dev->i2c_dev);
    return err;
}

esp_err_t as7341_enable_power_on(as7341_dev_t *dev, bool enable) {
    I2C_DEV_TAKE_MUTEX(&dev->i2c_dev);
    uint8_t data;
    esp_err_t err = i2c_dev_read_reg(&dev->i2c_dev, REG_ENABLE, &data, 1);
    if(err == ESP_OK) {
        uint8_t power_on_cmd = enable ? 0b00000001 : 0;
        uint8_t enable_power_on = power_on_cmd | data;
        err = i2c_dev_write_reg(&dev->i2c_dev, REG_ENABLE, &enable_power_on, 1);
    }
    I2C_DEV_GIVE_MUTEX(&dev->i2c_dev);
    return err;
}

esp_err_t as7341_get_chip_information(as7341_dev_t *dev, as7341_chip_information_t *data) {
    I2C_DEV_TAKE_MUTEX(&dev->i2c_dev);
    esp_err_t err = i2c_dev_read_reg(&dev->i2c_dev, REG_ID, &data->chip_id, 1);
    err = i2c_dev_read_reg(&dev->i2c_dev, REG_AUXID, &data->chip_aux_id, 1);
    err = i2c_dev_read_reg(&dev->i2c_dev, REG_REVID, &data->chip_rev_id, 1);
    I2C_DEV_GIVE_MUTEX(&dev->i2c_dev);
    return err;
}