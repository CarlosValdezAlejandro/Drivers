/**
  ******************************************************************************
  * @file    ADS1115.h
  * @author  Carlos Alejandro Valdez
  * @brief   ADS1115 I2C HAL module driver.
  *
  *
  ******************************************************************************
  * @attention
  *
  * MIT License

  * Copyright (c) 2023 Carlos Alejandro Valdez

  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:

  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.

  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  *
  ******************************************************************************


/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef ADS1115_H
#define ADS1115_H


#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32wlxx_hal.h"
#include "stm32wlxx_hal_i2c.h"


/* Definitions ---------------------------------------------------------------*/

/** @defgroup I2C addresses
  * @{
  */
#define ADDRESS_I2C_GND 0X48	/*!<    Default                  */
#define ADDRESS_I2C_VSS 0X49
#define ADDRESS_I2C_SDA 0X4A
#define ADDRESS_I2C_SCL 0X4B 	/*!<    It shouldn't be used     */
#define RST_REGISTERS_PWRDWN   0x06

// register map

// para mapear 8 bits
/** @defgroup All four registers are accessed by writing to the Address Pointer register
  * @{
  */
#define ADDRESS_POINTER 0X00 	/*!<    Just the last 2 bits must be modified     */
#define CONVERSION_REG  0X00
#define CONFIG_REG      0X01
#define LO_THRESH_REG   0X02
#define HI_THRESH_REG   0X03


/** @defgroup I2C instance storage
  * @brief  I2C Configuration Structure definition
  * @{
  */
typedef struct {
    I2C_HandleTypeDef *i2cHandle;
} ADS1115_I2C;


/** @defgroup 16 bits register configuration struct
  * @brief  Register Configuration Structure definition
  * @{
  */
typedef struct {
    uint8_t os        : 1;
    uint8_t mux       : 3;
    uint8_t pga       : 3;
    uint8_t mode      : 1;
    uint8_t data_rate : 3;
    uint8_t comp_mode : 1;
    uint8_t comp_pol  : 1;
    uint8_t comp_lat  : 1;
    uint8_t comp_que  : 2;
} config_register_bits_t;


/** @defgroup Address Pointer
  * @brief  Address Pointer Structure definition
  * @{
  */
typedef struct {
    uint8_t reserved            : 6;
    uint8_t reg_address_pointer : 2;
} address_pointer_reg_t;


/** @defgroup 16 bit Hi Threshold Configuration
  * @brief  Hi Threshold config
  * @{
  */
typedef struct {
    uint16_t MSB : 16;
} hi_thresh_t;


/** @defgroup 16 bit Low Threshold Configuration
  * @brief  Low Threshold config
  * @{
  */
typedef struct {
    uint16_t LSB : 16;
} lo_thresh_t;

/**
  * @}
  */


/**
  * @brief  Operational status or single-shot conversion start
  */
typedef enum {
    OS_SINGLE_CONVERSION = 0X01		/*!<     Default     */
} os_config_t;


/**
  * @brief  Input multiplexer configuration
  */
typedef enum {
    AIN0_AIN1 = 0X00,	/*!<     AINP = AIN0 and AINN = AIN1 (default)       */
    AIN0_AIN3 = 0X01,
    AIN1_AIN3 = 0X02,
    AIN2_AIN3 = 0X03,
    AIN0_GND  = 0X04,
    AIN1_GND  = 0X05,
    AIN2_GND  = 0X06,
    AIN3_GND  = 0X07,
} mux_config_t;


/**
  * @brief  Programmable gain amplifier configuration
  */
typedef enum {
    FSR_6_144 = 0x00,	/*!<     FSR = ±6.144 V               */
    FSR_4_096 = 0x01,	/*!<     FSR = ±4.096 V               */
    FSR_2_048 = 0x02,	/*!<     FSR = ±2.048 V (default)     */
    FSR_1_024 = 0x03,	/*!<     FSR = ±1.024 V               */
    FSR_0_512 = 0x04,	/*!<     FSR = ±0.512 V               */
    FSR_0_256 = 0x05,	/*!<     FSR = ±0.256 V               */
    //FSR_0_256 = 0x06,
    //FSR_0_256 = 0x07,
} pga_config_t;


/**
  * @brief  Device operating mode
  */
typedef enum {
    CONTINUOUS_CONV = 0X00,
    SINGLE_SHOT     = 0X01		/*!<     Default     */
} mode_config_t;


/**
  * @brief  Data rate
  */
typedef enum {
    SPS_8   = 0x0,
    SPS_16  = 0x1,
    SPS_32  = 0x2,
    SPS_64  = 0x3,
    SPS_128 = 0x4,	/*!<     Default     */
    SPS_250 = 0x5,
    SPS_475 = 0x6,
    SPS_860 = 0x7,
} data_rate_t;


/**
  * @brief  Comparator mode
  */
typedef enum {
    TRADITIONAL_COMP = 0X00,	/*!<     Default     */
    WINDOW_COMP      = 0X01,
} comp_mode_t;


/**
  * @brief  Comparator polarity
  */
typedef enum {
    ACTIVE_LOW  = 0X00,		/*!<     Default     */
    ACTIVE_HIGH = 0X01,
} comp_polarity_t;


/**
  * @brief  Latching comparator
  */
typedef enum {
    NONLATCHING = 0x00,		/*!<    Default      */
    LATCHING    = 0x01,
} comp_latching_t;


/**
  * @brief  Comparator queue and disable
  */
typedef enum {
    ONE_CONVERSION   = 0x00,
    TWO_CONVERSIONS  = 0x01,
    FOUR_CONVERSIONS = 0x02,
    DISABLE_         = 0x03,	/*!<     Default     */
} comp_queue_t;


/* Functions declaration *************************************************************/

void init_ADS1115 (address_pointer_reg_t *reg, ADS1115_I2C *dev, I2C_HandleTypeDef *i2cHandle);

void config_register_bits(config_register_bits_t *reg, os_config_t os, mux_config_t mux, pga_config_t pga, mode_config_t mode, data_rate_t data_rate, comp_mode_t comp_mode, comp_polarity_t comp_pol, comp_latching_t comp_lat, comp_queue_t comp_que);

void address_pointer_bits(address_pointer_reg_t *reg, uint8_t os);

uint16_t read_ADS1115(ADS1115_I2C *dev);

HAL_StatusTypeDef I2C_Write(ADS1115_I2C *dev ,uint8_t slave_address, uint8_t *data, uint16_t size);

HAL_StatusTypeDef I2C_Read(ADS1115_I2C *dev ,uint8_t slave_address , uint8_t *data, uint16_t size);



#endif
