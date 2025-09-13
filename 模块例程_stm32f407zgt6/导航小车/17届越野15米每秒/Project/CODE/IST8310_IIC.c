/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				SEEKFREE_IIC.c
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.32.4 or MDK 5.28
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					���߶��壺
* 					------------------------------------
* 					ģ��ܽ�			��Ƭ���ܽ�
* 					SCL					�鿴 SEEKFREE_SCL �궨��
* 					SDA					�鿴 SEEKFREE_SDA �궨��
* 					------------------------------------
********************************************************************************************************************/
//�شż�IST8310��ģ��IICͨ��������IST8310��IIC��������ɵ�������ϸ΢���
#include "IST8310_IIC.h"

//�ڲ����ݶ���
//uint8 IIC_ad_main;																	// �����ӵ�ַ
//uint8 IIC_ad_sub;																	// �����ӵ�ַ
//uint8 *IIC_buf;																		// ����|�������ݻ�����
//uint8 IIC_num;																		// ����|�������ݸ���

static uint16 simiic_delay_time_IST=10;													// ICM�ȴ�����Ӧ����Ϊ100

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC��ʱ ʱ������
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
//static void simiic_delay_set(uint16 time)
//{
//	simiic_delay_time = time;														// ICM�ȴ�����Ӧ����Ϊ100
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC��ʱ ʱ������
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void simiic_delay_IST(void)
{
	uint16 delay_time;
	delay_time = simiic_delay_time_IST;
	while(delay_time--);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC��ʼ�ź�
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void simiic_start_IST(void)
{
	SDA1_IST();
	SCL1_IST();
	simiic_delay_IST();
	SDA0_IST();
	simiic_delay_IST();
	SCL0_IST();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IICֹͣ�ź�
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void simiic_stop_IST(void)
{
	SDA0_IST();
	SCL0_IST();
	simiic_delay_IST();
	SCL1_IST();
	simiic_delay_IST();
	SDA1_IST();
	simiic_delay_IST();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC ACK�ź�
// @param		ack_dat			��Ӧ�� (����ack:SDA=0��no_ack:SDA=0)
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void simiic_sendack_IST(unsigned char ack_dat)
{
	SCL0_IST();
	simiic_delay_IST();
	if(ack_dat)
		SDA0_IST();
	else
		SDA1_IST();

	SCL1_IST();
	simiic_delay_IST();
	SCL0_IST();
	simiic_delay_IST();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC�ȴ�Ӧ��
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static int sccb_waitack_IST(void)
{
	SCL0_IST();
//	DIR_IN();
	simiic_delay_IST();

	SCL1_IST();
	simiic_delay_IST();

	if(SDA_IST)																			// Ӧ��Ϊ�ߵ�ƽ���쳣��ͨ��ʧ��
	{
//		DIR_OUT();
		SCL0_IST();
		return 0;
	}
//	DIR_OUT();
	SCL0_IST();
	simiic_delay_IST();
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC �ֽڷ��� �������մ�Ӧ�� �����Ǵ�Ӧ��λ
// @param		c				����c(����������Ҳ���ǵ�ַ)
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void send_ch_IST(uint8 c)
{
	uint8 i = 8;
	while(i--)
	{
	if(c & 0x80)
		SDA1_IST();																		// SDA �������
	else
		SDA0_IST();
	c <<= 1;
	simiic_delay_IST();
	SCL1_IST();																			// SCL ���ߣ��ɼ��ź�
	simiic_delay_IST();
	SCL0_IST();																			// SCL ʱ��������
	}
	sccb_waitack_IST();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC �ֽڽ��� �˳���Ӧ���|��Ӧ����|ʹ��
// @param		ack_x			Ӧ��
// @return		uint8			����
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static uint8 read_ch_IST(uint8 ack_x)
{
	uint8 i;
	uint8 c;
	c=0;
	SCL0_IST();
	simiic_delay_IST();
	SDA1_IST();             
//	DIR_IN();																		// ��������Ϊ���뷽ʽ
	for(i=0;i<8;i++)
	{
		simiic_delay_IST();
		SCL0_IST();																		// ��ʱ����Ϊ�ͣ�׼����������λ
		simiic_delay_IST();
		SCL1_IST();																		// ��ʱ����Ϊ�ߣ�ʹ��������������Ч
		simiic_delay_IST();
		c<<=1;
		if(SDA_IST)
			c+=1;																	// ������λ�������յ����ݴ�c
	}
//	DIR_OUT();
	SCL0_IST();
	simiic_delay_IST();
	simiic_sendack_IST(ack_x);

	return c;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IICд���ݵ��豸�Ĵ�������
// @param		dev_add			�豸��ַ(����λ��ַ)
// @param		reg				�Ĵ�����ַ
// @param		dat				д�������
// @return		void						
// @since		v1.0
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void simiic_write_reg_IST(uint8 dev_add, uint8 reg, uint8 dat)
{
	simiic_start_IST();
	send_ch_IST( (dev_add<<1) | 0x00);													// ����������ַ��дλ
	send_ch_IST( reg );																	// ���ʹӻ��Ĵ�����ַ
	send_ch_IST( dat );																	// ������Ҫд�������
	simiic_stop_IST();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IICд������ݵ��豸�Ĵ�������
// @param		dev_add			�豸��ַ(����λ��ַ)
// @param		reg				�Ĵ�����ַ
// @param		*dat			д�������
// @param		num				д���ֽ�����
// @return		void						
// @since		v1.0
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void simiic_write_regs_IST(uint8 dev_add, uint8 reg, uint8 *dat, uint8 num)
{
	simiic_start_IST();
	send_ch_IST( (dev_add<<1) | 0x00);													// ����������ַ��дλ
	send_ch_IST( reg );																	// ���ʹӻ��Ĵ�����ַ
	while(num--)
	{
		send_ch_IST( *dat++ );															// ������Ҫд�������
	}
	simiic_stop_IST();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC���豸�Ĵ�����ȡ����
// @param		dev_add			�豸��ַ(����λ��ַ)
// @param		reg				�Ĵ�����ַ
// @param		type			ѡ��ͨ�ŷ�ʽ��IIC  ���� SCCB
// @return		uint8			���ؼĴ���������
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 simiic_read_reg_IST(uint8 dev_add, uint8 reg, IIC_type_IST type)
{
	uint8 dat;
	simiic_start_IST();
	send_ch_IST( (dev_add<<1) | 0x00);													// ����������ַ��дλ
	send_ch_IST( reg );																	// ���ʹӻ��Ĵ�����ַ
	if(type == SCCB_IST)simiic_stop_IST();

	simiic_start_IST();
	send_ch_IST( (dev_add<<1) | 0x01);													// ����������ַ�Ӷ�λ
	dat = read_ch_IST(SEEKFREE_NACK_IST);													// ��ȡ����
	simiic_stop_IST();

	return dat;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC��ȡ���ֽ�����
// @param		dev_add			�豸��ַ(����λ��ַ)
// @param		reg				�Ĵ�����ַ
// @param		dat_add			���ݱ���ĵ�ַָ��
// @param		num				��ȡ�ֽ�����
// @param		type			ѡ��ͨ�ŷ�ʽ��IIC  ���� SCCB
// @return		uint8			���ؼĴ���������
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void simiic_read_regs_IST(uint8 dev_add, uint8 reg, uint8 *dat_add, uint8 num, IIC_type_IST type)
{
	simiic_start_IST();
	send_ch_IST( (dev_add<<1) | 0x00);													//����������ַ��дλ
	send_ch_IST( reg );																	//���ʹӻ��Ĵ�����ַ
	if(type == SCCB_IST)simiic_stop_IST();

	simiic_start_IST();
	send_ch_IST( (dev_add<<1) | 0x01);													//����������ַ�Ӷ�λ
	while(--num)
	{
		*dat_add = read_ch_IST(SEEKFREE_ACK_IST);											//��ȡ����
		dat_add++;
	}
	*dat_add = read_ch_IST(SEEKFREE_NACK_IST);												//��ȡ����
	simiic_stop_IST();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC�˿ڳ�ʼ��
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void simiic_init_IST(void)
{
	gpio_init(SEEKFREE_SCL_IST, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	gpio_init(SEEKFREE_SDA_IST, GPO, GPIO_HIGH, GPO_OPEN_DTAIN);
}
