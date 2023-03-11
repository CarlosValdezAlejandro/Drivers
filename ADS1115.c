/**
  ******************************************************************************
  * @file    ADS1115.c
  * @author  Carlos Alejandro Valdez
  * @brief   ADS1115 I2C HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Analogic Digital Converter ADS1115 device:
  *           + Setting and initialization
  *           + Data reading
  *           + Comparator threshold setting (not developing yet)
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

  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]
    (#) Include library "ADS1115.h" inside USER CODE includes.

	(#) Declare structures inside private variables of USER CODE:

		(##) ADS1115_I2C (stores the memory address of the I2C port declaration)
		(##) config_register_bits_t (stores the ADS1115 configuration such as MUX, PGA, MODE, DATA RATE, etc.)
	 	(##) address_pointer_reg_t (configures/selects the register address pointer: conversion register,
	 		 configuration register, lo_threshold register, hi_threshold register)

	(#) After initializing the I2C peripheral, the ADS1115 should be initialized using the functions config_register_bits and init_ADS1115 in this order.
		The config_register_bits function receives the config_register_bits_t instance along with the converter's operating configurations.
		The init_ADS1115 function receives the address_pointer_reg instance, the ADS1115_I2C instance, and the I2C_HandleTypeDef instance.

	(#) The device address is configured by default to 0x48 in the .c file but can be changed in the main.c file using the already declared
		variable (uint8_t) address.

	(#) To read the ADC values, there is a function called read_ADS1115 which returns a 16-bit value and requires the ADS1115_I2C instance to be passed.

	[..]
	The library uses the stm32wlxx_hal_i2c library and utilizes the I2C transmission and reception functions HAL_I2C_Master_Receive and HAL_I2C_Master_Transmit.
	Both functions are blocking functions.

	[..]
	(@) All other necessary information can be found in the ADS1115 Texas Instrument Datasheet.


  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "ADS1115.h"

/* Variables -----------------------------------------------------------------*/

extern uint8_t  address = ADDRESS_I2C_GND;
static uint16_t config_bits = 0;
static uint8_t  address_pointer = 0;


/* Functions -----------------------------------------------------------------*/

/**
  * @brief Bit Configuration of the chip configuration register.
  * @param reg Direction to the struct that contains the bit description.
  * @param os Conversion type.
  * @param mux Multiplexer configuration.
  * @param pga PGA configuration.
  * @param mode Conversion mode.
  * @param data_rate Data rate.
  * @param comp_mode Comparator mode.
  * @param comp_pol Comparator polarity.
  * @param comp_lat Latching comparator.
  * @param comp_que	Comparator queue and disable.
  */
void config_register_bits(config_register_bits_t *reg, os_config_t os, mux_config_t mux, pga_config_t pga,
						  mode_config_t mode, data_rate_t data_rate, comp_mode_t comp_mode, comp_polarity_t comp_pol,
						  comp_latching_t comp_lat, comp_queue_t comp_que)
{
    reg->os = os;
    reg->mux = mux;
    reg->pga = pga;
    reg->mode = mode;
    reg->data_rate = data_rate;
    reg->comp_mode = comp_mode;
    reg->comp_pol = comp_pol;
    reg->comp_lat = comp_lat;
    reg->comp_que = comp_que;

    config_bits |= (reg->os        & 0x01) << 15;
    config_bits |= (reg->mux       & 0x07) << 12;
    config_bits |= (reg->pga       & 0x07) <<  9;
    config_bits |= (reg->mode      & 0x01) <<  8;
    config_bits |= (reg->data_rate & 0x07) <<  5;
    config_bits |= (reg->comp_mode & 0x01) <<  4;
    config_bits |= (reg->comp_pol  & 0x01) <<  3;
    config_bits |= (reg->comp_lat  & 0x01) <<  2;
    config_bits |= (reg->comp_que  & 0x03) <<  0;
}


/**
  * @brief to access the four registers.
  * @param reg Direction to the struct that contains the register address description.
  * @param reg_address_pointer Which register you want to access.
  */
void address_pointer_bits(address_pointer_reg_t *reg, uint8_t reg_address_pointer)
{
    reg->reserved = 0x00;
    reg->reg_address_pointer = reg_address_pointer & 0x03;

    address_pointer |= (reg->reserved ) << 6;
    address_pointer |= reg->reg_address_pointer;
}


/**
  * @brief Write data through I2C port to the ADS1115.
  * @param dev Direction of the struct that contains the I2C instance.
  * @param slave_address ADS1115 address selected.
  * @param data Data to be transmitted.
  * @param size Size in bytes to be transmitted.
  * @retval HAL state.
  */
HAL_StatusTypeDef I2C_Write(ADS1115_I2C *dev ,uint8_t slave_address, uint8_t *data, uint16_t size)
{
  HAL_StatusTypeDef status;

  status = HAL_I2C_Master_Transmit(dev->i2cHandle, (slave_address << 1), data, size, HAL_MAX_DELAY);

  return status;
}


/**
  * @brief Read data through I2C port to the ADS1115.
  * @param dev Direction of the struct that contains the I2C instance.
  * @param slave_address ADS1115 address selected.
  * @param data Data to be received.
  * @param size Size in bytes to be received.
  * @retval HAL state.
  */
HAL_StatusTypeDef I2C_Read(ADS1115_I2C *dev ,uint8_t slave_address , uint8_t *data, uint16_t size)
{
	HAL_StatusTypeDef status;

    status = HAL_I2C_Master_Receive(dev->i2cHandle, ((slave_address << 1) | 0x01), data, size, HAL_MAX_DELAY);

	return status;
}


/**
  * @brief ADS1115 initialization.
  * @param reg Address pinter register instance direction.
  * @param dev ADS1115_I2C instance direction.
  * @param i2cHandle I2C_HandleTypeDef instance direction.
  */
void init_ADS1115 (address_pointer_reg_t *reg, ADS1115_I2C *dev, I2C_HandleTypeDef *i2cHandle)
{
    uint8_t data[3]={0,0,0};

    dev->i2cHandle = i2cHandle;

    address_pointer_bits(reg, CONFIG_REG);

    data[0] = CONFIG_REG & 0xff;
    data[1] = (config_bits >> 8) & 0xff;
    data[2] = config_bits & 0xff;

    I2C_Write(dev,address, data, 3);
}


/**
  * @brief Return the ADS1115 16 bit ADC lecture.
  * @param dev ADS1115_I2C instance direction.
  * @retval 16 bit lecture.
  */
uint16_t read_ADS1115(ADS1115_I2C *dev)
{
    uint8_t  read[2];

    I2C_Write(dev,address, CONVERSION_REG, 1);

    I2C_Read(dev,address, read, 2);

    return (uint16_t)(read[0] << 8) | (uint16_t)(read[1] & 0xff);
    
}
