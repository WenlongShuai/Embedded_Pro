/*******************************************************************************
Copyright 2015, STMicroelectronics International N.V.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STMicroelectronics nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/


#include "hardware_iic.h"
#include "atk_ms53l1m.h"
#include "vl53l1_i2c_platform.h"
#include "vl53l1_api.h"
#include "delay.h"

/* when not customized by application define dummy one */
#ifndef VL53L1_GetI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L1_GetI2cBus(...) (void)0
#endif

#ifndef VL53L1_PutI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L1_PutI2cBus(...) (void)0
#endif


uint8_t _I2CBuffer[8];

VL53L1_Error VL53L1_WriteMulti(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    VL53l1X_write_nbyte(ATK_MS53L1M_IIC_ADDR, index, count, pdata);

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L1_Error VL53L1_ReadMulti(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    VL53l1X_read_byte(ATK_MS53L1M_IIC_ADDR, index, count, pdata);

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}

VL53L1_Error VL53L1_WrByte(VL53L1_DEV Dev, uint16_t index, uint8_t data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    VL53l1X_write_nbyte(ATK_MS53L1M_IIC_ADDR, index, 1, &data);

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}

VL53L1_Error VL53L1_WrWord(VL53L1_DEV Dev, uint16_t index, uint16_t data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    _I2CBuffer[0] = data >> 8;
    _I2CBuffer[1] = data & 0x00FF;

    VL53l1X_write_nbyte(ATK_MS53L1M_IIC_ADDR, index, 2, _I2CBuffer);

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}

VL53L1_Error VL53L1_WrDWord(VL53L1_DEV Dev, uint16_t index, uint32_t data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    _I2CBuffer[0] = (data >> 24) & 0xFF;
    _I2CBuffer[1] = (data >> 16) & 0xFF;
    _I2CBuffer[2] = (data >> 8)  & 0xFF;
    _I2CBuffer[3] = (data >> 0 ) & 0xFF;

    VL53l1X_write_nbyte(ATK_MS53L1M_IIC_ADDR, index, 4, _I2CBuffer);

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}


VL53L1_Error VL53L1_RdByte(VL53L1_DEV Dev, uint16_t index, uint8_t *data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    VL53l1X_read_byte(ATK_MS53L1M_IIC_ADDR, index, 1, data);

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}

VL53L1_Error VL53L1_RdWord(VL53L1_DEV Dev, uint16_t index, uint16_t *data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    VL53l1X_read_byte(ATK_MS53L1M_IIC_ADDR, index, 2, _I2CBuffer);
    *data = ((uint16_t)_I2CBuffer[0] << 8) + (uint16_t)_I2CBuffer[1];

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}

VL53L1_Error VL53L1_RdDWord(VL53L1_DEV Dev, uint16_t index, uint32_t *data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    VL53l1X_read_byte(ATK_MS53L1M_IIC_ADDR, index, 4, _I2CBuffer);
    *data = ((uint32_t)_I2CBuffer[0] << 24) + ((uint32_t)_I2CBuffer[1] << 16) + ((uint32_t)_I2CBuffer[2] << 8) + (uint32_t)_I2CBuffer[3];

    VL53L1_GetI2cBus();
    VL53L1_PutI2cBus();

    return Status;
}

VL53L1_Error VL53L1_UpdateByte(VL53L1_DEV Dev, uint16_t index, uint8_t AndData, uint8_t OrData)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    uint8_t data;

    Status = VL53L1_RdByte(Dev, index, &data);

    if (Status)
    {
        goto done;
    }

    data = (data & AndData) | OrData;
    Status = VL53L1_WrByte(Dev, index, data);
done:
    return Status;
}

VL53L1_Error VL53L1_GetTickCount(uint32_t *ptick_count_ms)
{
    /* Returns current tick count in [ms] */

    VL53L1_Error status  = VL53L1_ERROR_NONE;

    //*ptick_count_ms = timeGetTime();
    *ptick_count_ms = 0;

    return status;
}

#define trace_print(level, ...) (void)0

#define trace_i2c(...) (void)0

//#define trace_print(level, ...) \
//	_LOG_TRACE_PRINT(VL53L1_TRACE_MODULE_PLATFORM, \
//	level, VL53L1_TRACE_FUNCTION_NONE, ##__VA_ARGS__)

//#define trace_i2c(...) \
//	_LOG_TRACE_PRINT(VL53L1_TRACE_MODULE_NONE, \
//	VL53L1_TRACE_LEVEL_NONE, VL53L1_TRACE_FUNCTION_I2C, ##__VA_ARGS__)


VL53L1_Error VL53L1_GetTimerFrequency(int32_t *ptimer_freq_hz)
{
    *ptimer_freq_hz = 0;

    trace_print(VL53L1_TRACE_LEVEL_INFO, "VL53L1_GetTimerFrequency: Freq : %dHz\n", *ptimer_freq_hz);
    return VL53L1_ERROR_NONE;
}


VL53L1_Error VL53L1_WaitMs(VL53L1_Dev_t *pdev, int32_t wait_ms)
{
    (void)pdev;
    delay_ms(wait_ms);
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WaitUs(VL53L1_Dev_t *pdev, int32_t wait_us)
{
    (void)pdev;
    delay_ms(wait_us / 1000);
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WaitValueMaskEx(
    VL53L1_Dev_t *pdev,
    uint32_t      timeout_ms,
    uint16_t      index,
    uint8_t       value,
    uint8_t       mask,
    uint32_t      poll_delay_ms)
{
    /*
     * Platform implementation of WaitValueMaskEx V2WReg script command
     *
     * WaitValueMaskEx(
     *          duration_ms,
     *          index,
     *          value,
     *          mask,
     *          poll_delay_ms);
     */

    VL53L1_Error status         = VL53L1_ERROR_NONE;
    uint32_t     start_time_ms = 0;
    uint32_t     current_time_ms = 0;
    uint32_t     polling_time_ms = 0;
    uint8_t      byte_value      = 0;
    uint8_t      found           = 0;
#ifdef VL53L1_LOG_ENABLE
    uint8_t      trace_functions = VL53L1_TRACE_FUNCTION_NONE;
#endif

    char   register_name[VL53L1_MAX_STRING_LENGTH];

    /* look up register name */
#ifdef PAL_EXTENDED
    VL53L1_get_register_name(
        index,
        register_name);
#else
    VL53L1_COPYSTRING(register_name, "");
#endif

    /* Output to I2C logger for FMT/DFT  */

    /*trace_i2c("WaitValueMaskEx(%5d, 0x%04X, 0x%02X, 0x%02X, %5d);\n",
    		     timeout_ms, index, value, mask, poll_delay_ms); */
    trace_i2c("WaitValueMaskEx(%5d, %s, 0x%02X, 0x%02X, %5d);\n",
              timeout_ms, register_name, value, mask, poll_delay_ms);

    /* calculate time limit in absolute time */

    VL53L1_GetTickCount(&start_time_ms);

    /* remember current trace functions and temporarily disable
     * function logging
     */

#ifdef VL53L1_LOG_ENABLE
    trace_functions = VL53L1_get_trace_functions();
    VL53L1_set_trace_functions(VL53L1_TRACE_FUNCTION_NONE);
#endif

    /* wait until value is found, timeout reached on error occurred */

    while ((status == VL53L1_ERROR_NONE) &&
            (polling_time_ms < timeout_ms) &&
            (found == 0))
    {

        if (status == VL53L1_ERROR_NONE)
            status = VL53L1_RdByte(
                         pdev,
                         index,
                         &byte_value);

        if ((byte_value & mask) == value)
            found = 1;

        if (status == VL53L1_ERROR_NONE  &&
                found == 0 &&
                poll_delay_ms > 0)
            status = VL53L1_WaitMs(
                         pdev,
                         poll_delay_ms);

        /* Update polling time (Compare difference rather than absolute to
        negate 32bit wrap around issue) */
        VL53L1_GetTickCount(&current_time_ms);
        polling_time_ms = current_time_ms - start_time_ms;

    }

#ifdef VL53L1_LOG_ENABLE
    /* Restore function logging */
    VL53L1_set_trace_functions(trace_functions);
#endif

    if (found == 0 && status == VL53L1_ERROR_NONE)
        status = VL53L1_ERROR_TIME_OUT;

    return status;
}
