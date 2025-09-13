#include "DMA.h"


/* 缓冲区定义 */
uint8_t I2C1_TxBuffer[256];
uint8_t I2C1_RxBuffer[256];
uint8_t I2C1_TransferComplete = 0;
uint8_t I2C1_TransferError = 0;

/**
 * 初始化I2C1和相关DMA通道
 */
void I2C1_DMA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 使能相关时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    /* 配置I2C1引脚: PB6-SCL, PB7-SDA */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* 将PB6和PB7连接到I2C1 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
    
    /* 配置I2C1 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;  // 作为主设备时不需要设置自身地址
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;  // 100kHz标准模式
    I2C_Init(I2C1, &I2C_InitStructure);
    
    /* 配置DMA发送通道 - DMA1 Stream6 Channel1 */
    DMA_DeInit(DMA1_Stream6);
    DMA_InitStructure.DMA_Channel = DMA_Channel_1;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)I2C1_TxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  // 内存到外设
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // 正常模式，非循环
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
    
    /* 配置DMA接收通道 - DMA1 Stream0 Channel1 */
    DMA_DeInit(DMA1_Stream0);
    DMA_InitStructure.DMA_Channel = DMA_Channel_1;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)I2C1_RxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  // 外设到内存
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_Init(DMA1_Stream0, &DMA_InitStructure);
    
    /* 使能DMA中断 */
    DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);  // 发送完成中断
    DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);  // 接收完成中断
    DMA_ITConfig(DMA1_Stream6, DMA_IT_TE, ENABLE);  // 发送错误中断
    DMA_ITConfig(DMA1_Stream0, DMA_IT_TE, ENABLE);  // 接收错误中断
    
    /* 配置DMA中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能I2C1的DMA请求 */
    I2C_DMACmd(I2C1, ENABLE);
    
    /* 使能I2C1 */
    I2C_Cmd(I2C1, ENABLE);
}

/**
 * 检查I2C总线状态并在必要时恢复
 * @return 0表示总线正常，1表示需要恢复
 */
uint8_t I2C1_CheckBusState(void)
{
    uint32_t timeout = 10000;
    
    /* 检查I2C总线是否繁忙 */
    if(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        /* 尝试发送停止信号 */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* 等待总线空闲 */
        timeout = 10000;
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        {
            if(timeout-- == 0)
            {
                /* 总线恢复失败，可能需要重置I2C外设 */
                return 1;
            }
        }
    }
    
    return 0;
}

/**
 * 重置I2C外设
 */
void I2C1_Reset(void)
{
    /* 禁用I2C1 */
    I2C_Cmd(I2C1, DISABLE);
    
    /* 禁用DMA通道 */
    DMA_Cmd(DMA1_Stream6, DISABLE);
    DMA_Cmd(DMA1_Stream0, DISABLE);
    
    /* 重置I2C外设 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    
    /* 重新初始化I2C和DMA */
    I2C1_DMA_Init();
}

/**
 * 使用DMA发送数据到I2C从设备
 * @param slaveAddr 从机地址(7位)
 * @param data 要发送的数据缓冲区
 * @param size 要发送的数据字节数
 * @return 0表示成功，非0表示错误
 */
uint8_t I2C1_DMA_SendData(uint8_t slaveAddr, uint8_t *data, uint16_t size)
{
    uint32_t timeout = 10000;
    
    /* 检查总线状态并在必要时恢复 */
    if(I2C1_CheckBusState() != 0)
    {
        /* 总线恢复失败，重置I2C外设 */
        I2C1_Reset();
        return 6;  // 总线恢复失败
    }
    
    /* 复制数据到发送缓冲区 */
    for(uint16_t i = 0; i < size; i++)
    {
        I2C1_TxBuffer[i] = data[i];
    }
    
    /* 配置DMA发送缓冲区大小 */
    DMA1_Stream6->NDTR = size;
    
    /* 清除传输状态标志 */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* 发送起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* 等待发送起始信号完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 2;  // 起始信号超时
        }
    }
    
    /* 发送从机地址+写操作 */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter);
    
    /* 等待地址发送完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // 地址发送超时，可能无ACK
        }
    }
    
    /* 使能DMA发送 */
    DMA_Cmd(DMA1_Stream6, ENABLE);
    
    /* 等待DMA传输完成 */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* 发生超时，停止传输 */
            DMA_Cmd(DMA1_Stream6, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 4;  // DMA传输超时
        }
    }
    
    if(I2C1_TransferError)
    {
        return 5;  // DMA传输错误
    }
    
    return 0;  // 传输成功
}

/**
 * 使用DMA从I2C从设备接收数据
 * @param slaveAddr 从机地址(7位)
 * @param data 存储接收数据的缓冲区
 * @param size 要接收的数据字节数
 * @return 0表示成功，非0表示错误
 */
uint8_t I2C1_DMA_ReceiveData(uint8_t slaveAddr, uint8_t regAddress, uint8_t *data, uint16_t size)
{
    uint32_t timeout = 10000;
    
    /* 检查总线状态并在必要时恢复 */
    if(I2C1_CheckBusState() != 0)
    {
        /* 总线恢复失败，重置I2C外设 */
        I2C1_Reset();
        return 6;  // 总线恢复失败
    }
    
    /* 配置DMA接收缓冲区大小 */
    DMA1_Stream0->NDTR = size;
    
    /* 清除传输状态标志 */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* 发送起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* 等待发送起始信号完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 2;  // 起始信号超时
        }
    }
    
    /* 发送从机地址+读操作 */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Receiver);
    
    /* 等待地址发送完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // 地址发送超时，可能无ACK
        }
    }
		
		I2C_SendData(I2C1, regAddress);
		
		/* 等待地址发送完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // 地址发送超时，可能无ACK
        }
    }
    
    /* 使能DMA接收 */
    DMA_Cmd(DMA1_Stream0, ENABLE);
    
    /* 等待DMA传输完成 */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* 发生超时，停止传输 */
            DMA_Cmd(DMA1_Stream0, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 4;  // DMA传输超时
        }
    }
    
    if(I2C1_TransferError)
    {
        return 5;  // DMA传输错误
    }
    
    /* 复制接收到的数据到用户缓冲区 */
    for(uint16_t i = 0; i < size; i++)
    {
        data[i] = I2C1_RxBuffer[i];
    }
    
    return 0;  // 传输成功
}

/**
 * 使用DMA先发送数据再接收数据（组合操作）
 * @param slaveAddr 从机地址(7位)
 * @param txData 要发送的数据缓冲区
 * @param txSize 要发送的数据字节数
 * @param rxData 存储接收数据的缓冲区
 * @param rxSize 要接收的数据字节数
 * @return 0表示成功，非0表示错误
 */
uint8_t I2C1_DMA_SendThenReceive(uint8_t slaveAddr, uint8_t *txData, uint16_t txSize, uint8_t *rxData, uint16_t rxSize)
{
    uint32_t timeout = 10000;
    uint8_t status;
    
    /* 检查总线状态并在必要时恢复 */
    if(I2C1_CheckBusState() != 0)
    {
        /* 总线恢复失败，重置I2C外设 */
        I2C1_Reset();
        return 6;  // 总线恢复失败
    }
    
    /* 复制发送数据到缓冲区 */
    for(uint16_t i = 0; i < txSize; i++)
    {
        I2C1_TxBuffer[i] = txData[i];
    }
    
    /* 配置DMA发送缓冲区大小 */
    DMA1_Stream6->NDTR = txSize;
    
    /* 清除传输状态标志 */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* 发送起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* 等待发送起始信号完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 2;  // 起始信号超时
        }
    }
    
    /* 发送从机地址+写操作 */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter);
    
    /* 等待地址发送完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // 地址发送超时，可能无ACK
        }
    }
    
    /* 使能DMA发送 */
    DMA_Cmd(DMA1_Stream6, ENABLE);
    
    /* 等待DMA发送完成 */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* 发生超时，停止传输 */
            DMA_Cmd(DMA1_Stream6, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 4;  // DMA传输超时
        }
    }
    
    if(I2C1_TransferError)
    {
        return 5;  // DMA传输错误
    }
    
    /* 重置传输状态标志，准备接收 */
    I2C1_TransferComplete = 0;
    I2C1_TransferError = 0;
    
    /* 配置DMA接收缓冲区大小 */
    DMA1_Stream0->NDTR = rxSize;
    
    /* 发送重复起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* 等待发送起始信号完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 7;  // 重复起始信号超时
        }
    }
    
    /* 发送从机地址+读操作 */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Receiver);
    
    /* 等待地址发送完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 8;  // 读地址发送超时
        }
    }
    
    /* 使能DMA接收 */
    DMA_Cmd(DMA1_Stream0, ENABLE);
    
    /* 等待DMA接收完成 */
    timeout = 100000;
    while(!I2C1_TransferComplete && !I2C1_TransferError)
    {
        if(timeout-- == 0)
        {
            /* 发生超时，停止传输 */
            DMA_Cmd(DMA1_Stream0, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 9;  // DMA接收超时
        }
    }
    
    if(I2C1_TransferError)
    {
        return 10;  // DMA接收错误
    }
    
    /* 复制接收到的数据到用户缓冲区 */
    for(uint16_t i = 0; i < rxSize; i++)
    {
        rxData[i] = I2C1_RxBuffer[i];
    }
    
    return 0;  // 传输成功
}

/**
 * DMA1 Stream6 中断服务函数 - I2C1发送完成
 */
void DMA1_Stream6_IRQHandler(void)
{
    /* 检查DMA传输完成中断标志 */
    if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6))
    {
        /* 清除中断标志 */
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
        
        /* 禁用DMA发送 */
        DMA_Cmd(DMA1_Stream6, DISABLE);
        
        /* 设置发送完成标志 */
        I2C1_TransferComplete = 1;
    }
    
    /* 检查DMA传输错误中断标志 */
    if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF6))
    {
        /* 清除中断标志 */
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TEIF6);
        
        /* 禁用DMA发送 */
        DMA_Cmd(DMA1_Stream6, DISABLE);
        
        /* 发送停止信号 */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* 设置错误标志 */
        I2C1_TransferError = 1;
    }
}

/**
 * DMA1 Stream0 中断服务函数 - I2C1接收完成
 */
void DMA1_Stream0_IRQHandler(void)
{
    /* 检查DMA传输完成中断标志 */
    if(DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0))
    {
        /* 清除中断标志 */
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
        
        /* 禁用DMA接收 */
        DMA_Cmd(DMA1_Stream0, DISABLE);
        
        /* 发送停止信号 */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* 设置接收完成标志 */
        I2C1_TransferComplete = 1;
    }
    
    /* 检查DMA传输错误中断标志 */
    if(DMA_GetITStatus(DMA1_Stream0, DMA_IT_TEIF0))
    {
        /* 清除中断标志 */
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TEIF0);
        
        /* 禁用DMA接收 */
        DMA_Cmd(DMA1_Stream0, DISABLE);
        
        /* 发送停止信号 */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* 设置错误标志 */
        I2C1_TransferError = 1;
    }
}

/**
 * 检查I2C1连接状态
 */
uint8_t I2C1_CheckConnection(uint8_t slaveAddr)
{
    uint32_t timeout = 10000;
    
    /* 检查总线状态并在必要时恢复 */
    if(I2C1_CheckBusState() != 0)
    {
        /* 总线恢复失败，重置I2C外设 */
        I2C1_Reset();
        return 1;  // 总线恢复失败
    }
    
    /* 发送起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    
    /* 等待起始信号发送完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(timeout-- == 0)
            return 2;  // 起始信号超时
    }
    
    /* 发送从机地址+写操作 */
    I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter);
    
    /* 等待地址发送完成 */
    timeout = 10000;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(timeout-- == 0)
        {
            /* 发送停止信号 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 3;  // 地址发送超时，可能无ACK
        }
    }
    
    /* 发送停止信号 */
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    return 0;  // 连接成功
}
