#include <i2cdev.h>
#include <esp_err.h>
#include <esp_log.h>

typedef struct 
{
    i2c_dev_t i2c_dev;
} as7341_dev_t;



// static const char *TAG = "as7341";

#define AS7341_I2C_ADDRESS          0X39
#define I2C_FREQ_HZ 1000000

#define REG_ASTATUS                 0X60
/*
#define REG_CH0_DATA_L              0X61
#define REG_CH0_DATA_H              0X62
*/
#define REG_ITIME_L                 0X63
#define REG_ITIME_M                 0X64
#define REG_ITIME_H                 0X65
/*
#define REG_CH1_DATA_L              0X66
#define REG_CH1_DATA_H              0X67
#define REG_CH2_DATA_L              0X68
#define REG_CH2_DATA_H              0X69
#define REG_CH3_DATA_L              0X6A
#define REG_CH3_DATA_H              0X6B
#define REG_CH4_DATA_L              0X6C
#define REG_CH4_DATA_H              0X6D
#define REG_CH5_DATA_L              0X6E
#define REG_CH5_DATA_H              0X6F
*/

#define REG_CH0_DATA_L              0X95
#define REG_CH0_DATA_H              0X96
#define REG_CH1_DATA_L              0X97
#define REG_CH1_DATA_H              0X98
#define REG_CH2_DATA_L              0X99
#define REG_CH2_DATA_H              0X9A
#define REG_CH3_DATA_L              0X9B
#define REG_CH3_DATA_H              0X9C
#define REG_CH4_DATA_L              0X9D
#define REG_CH4_DATA_H              0X9E
#define REG_CH5_DATA_L              0X9F
#define REG_CH5_DATA_H              0XA0

#define REG_CONFIG                  0X70
#define REG_STAT                    0X71
#define REG_EDGE                    0X72
#define REG_GPIO                    0X73
#define REG_LED                     0X74
#define REG_ENABLE                  0X80
#define REG_ATIME                   0X81
#define REG_WTIME                   0X83
#define REG_SP_TH_L_LSB             0X84
#define REG_SP_TH_L_MSB             0X85
#define REG_SP_TH_H_LSB             0X86
#define REG_SP_TH_H_MSB             0X87
#define REG_AUXID                   0X90
#define REG_REVID                   0X91
#define REG_ID                      0X92
#define REG_STATUS                  0X93
#define REG_STATUS_2                0XA3
#define REG_STATUS_3                0XA4
#define REG_STATUS_5                0XA6
#define REG_STATUS_6                0XA7
#define REG_CFG_0                   0XA9
#define REG_CFG_1                   0XAA
#define REG_CFG_6                   0XAF
#define REG_CFG_8                   0XB1
#define REG_CFG_9                   0XB2
#define REG_CFG_10                  0XB3
#define REG_CFG_12                  0XB5
#define REG_PERS                    0XBD
#define REG_GPIO_2                  0XBE
#define REG_ASTEP_L                 0XCA
#define REG_ASTEP_H                 0XCB
#define REG_AGC_GAIN_MAX            0XCF
#define REG_AZ_CONFIG               0XD6
#define REG_DF_TIME_1               0XD8
#define REG_DF_TIME_2               0XDA
#define REG_FD_CFG_0                0XD7
#define REG_FD_STATUS               0XDB
#define REG_INTENAB                 0XF9
#define REG_CONTROL                 0XFA
#define REG_FIFO_MAP                0XFC
#define REG_FIFO_LVL                0XFD
#define REG_FDATA_L                 0XFE
#define REG_FDATA_H                 0XFF


/*
* AGain 
*/
typedef enum {
  AS7341_GAIN_0_5X,
  AS7341_GAIN_1X,
  AS7341_GAIN_2X,
  AS7341_GAIN_4X,
  AS7341_GAIN_8X,
  AS7341_GAIN_16X,
  AS7341_GAIN_32X,
  AS7341_GAIN_64X,
  AS7341_GAIN_128X,
  AS7341_GAIN_256X,
  AS7341_GAIN_512X,
} as7341_again_t;

/*
* SMUX
*/
typedef enum {
  AS7341_SMUX_CMD_ROM_RESET, ///< ROM code initialization of SMUX
  AS7341_SMUX_CMD_READ,      ///< Read SMUX configuration to RAM from SMUX chain
  AS7341_SMUX_CMD_WRITE, ///< Write SMUX configuration from RAM to SMUX chain
} as7341_smux_cmd_t;

typedef struct {
    uint8_t chip_id;
    uint8_t chip_rev_id;
    uint8_t chip_aux_id;
} as7341_chip_information_t;

esp_err_t as7341_init_desc(as7341_dev_t *dev, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t as7341_free_desc(as7341_dev_t *dev);
esp_err_t as7341_init(as7341_dev_t *dev);
esp_err_t as7341_read_channel(as7341_dev_t *dev, uint8_t channel, uint16_t *data);
esp_err_t as7341_set_smux_command(as7341_dev_t *dev, as7341_smux_cmd_t command);
esp_err_t as7341_enable_spectral_measurement(as7341_dev_t *dev, bool enable);
esp_err_t as7341_enable_power_on(as7341_dev_t *dev, bool enable);
esp_err_t as7341_get_chip_information(as7341_dev_t *dev, as7341_chip_information_t *data);