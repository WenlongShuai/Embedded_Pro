#ifndef __STM32F10X_H__
#define __STM32F10X_H__

#define PERIPH_BASE 	((unsigned int)0x40000000)
#define APB1_BASE   	PERIPH_BASE
#define APB2_BASE 		(PERIPH_BASE+0x10000)
#define AHB_BASE  		(PERIPH_BASE+0x20000)

#define RCC_BASE  		(AHB_BASE+0x1000)
#define RCC_AHBENR  	*((unsigned int*)(RCC_BASE+0x14))
#define RCC_APB2ENR  	*((unsigned int*)(RCC_BASE+0x18))
#define RCC_APB1ENR  	*((unsigned int*)(RCC_BASE+0x1C))

#define GPIOA_BASE  	(APB2_BASE+0x800)
#define GPIOB_BASE  	(APB2_BASE+0xC00)
#define GPIOC_BASE  	(APB2_BASE+0x1000)
#define GPIOD_BASE  	(APB2_BASE+0x1400)
#define GPIOE_BASE  	(APB2_BASE+0x1800)
#define GPIOF_BASE  	(APB2_BASE+0x1C00)
#define GPIOG_BASE  	(APB2_BASE+0x2000)

#if 0
#define GPIOA_CRL  		*((unsigned int*)(GPIOA_BASE+0x00))
#define GPIOA_CRH  		*((unsigned int*)(GPIOA_BASE+0x04))
#define GPIOA_IDR  		*((unsigned int*)(GPIOA_BASE+0x08))
#define GPIOA_ODR  		*((unsigned int*)(GPIOA_BASE+0x0C))
#define GPIOA_BSRR 		*((unsigned int*)(GPIOA_BASE+0x10))
#define GPIOA_BRR  		*((unsigned int*)(GPIOA_BASE+0x14))
#define GPIOA_LCKR 		*((unsigned int*)(GPIOA_BASE+0x18))

#define GPIOB_CRL  		*((unsigned int*)(GPIOB_BASE+0x00))
#define GPIOB_CRH  		*((unsigned int*)(GPIOB_BASE+0x04))
#define GPIOB_IDR  		*((unsigned int*)(GPIOB_BASE+0x08))
#define GPIOB_ODR  		*((unsigned int*)(GPIOB_BASE+0x0C))
#define GPIOB_BSRR 		*((unsigned int*)(GPIOB_BASE+0x10))
#define GPIOB_BRR  		*((unsigned int*)(GPIOB_BASE+0x14))
#define GPIOB_LCKR 		*((unsigned int*)(GPIOB_BASE+0x18))

#define GPIOC_CRL  		*((unsigned int*)(GPIOC_BASE+0x00))
#define GPIOC_CRH  		*((unsigned int*)(GPIOC_BASE+0x04))
#define GPIOC_IDR  		*((unsigned int*)(GPIOC_BASE+0x08))
#define GPIOC_ODR  		*((unsigned int*)(GPIOC_BASE+0x0C))
#define GPIOC_BSRR 		*((unsigned int*)(GPIOC_BASE+0x10))
#define GPIOC_BRR  		*((unsigned int*)(GPIOC_BASE+0x14))
#define GPIOC_LCKR 		*((unsigned int*)(GPIOC_BASE+0x18))

#define GPIOD_CRL  		*((unsigned int*)(GPIOD_BASE+0x00))
#define GPIOD_CRH  		*((unsigned int*)(GPIOD_BASE+0x04))
#define GPIOD_IDR  		*((unsigned int*)(GPIOD_BASE+0x08))
#define GPIOD_ODR  		*((unsigned int*)(GPIOD_BASE+0x0C))
#define GPIOD_BSRR 		*((unsigned int*)(GPIOD_BASE+0x10))
#define GPIOD_BRR  		*((unsigned int*)(GPIOD_BASE+0x14))
#define GPIOD_LCKR 		*((unsigned int*)(GPIOD_BASE+0x18))

#define GPIOE_CRL  		*((unsigned int*)(GPIOE_BASE+0x00))
#define GPIOE_CRH  		*((unsigned int*)(GPIOE_BASE+0x04))
#define GPIOE_IDR  		*((unsigned int*)(GPIOE_BASE+0x08))
#define GPIOE_ODR  		*((unsigned int*)(GPIOE_BASE+0x0C))
#define GPIOE_BSRR 		*((unsigned int*)(GPIOE_BASE+0x10))
#define GPIOE_BRR  		*((unsigned int*)(GPIOE_BASE+0x14))
#define GPIOE_LCKR 		*((unsigned int*)(GPIOE_BASE+0x18))

#define GPIOF_CRL  		*((unsigned int*)(GPIOF_BASE+0x00))
#define GPIOF_CRH  		*((unsigned int*)(GPIOF_BASE+0x04))
#define GPIOF_IDR  		*((unsigned int*)(GPIOF_BASE+0x08))
#define GPIOF_ODR  		*((unsigned int*)(GPIOF_BASE+0x0C))
#define GPIOF_BSRR 		*((unsigned int*)(GPIOF_BASE+0x10))
#define GPIOF_BRR  		*((unsigned int*)(GPIOF_BASE+0x14))
#define GPIOF_LCKR 		*((unsigned int*)(GPIOF_BASE+0x18))

#define GPIOG_CRL  		*((unsigned int*)(GPIOG_BASE+0x00))
#define GPIOG_CRH  		*((unsigned int*)(GPIOG_BASE+0x04))
#define GPIOG_IDR  		*((unsigned int*)(GPIOG_BASE+0x08))
#define GPIOG_ODR  		*((unsigned int*)(GPIOG_BASE+0x0C))
#define GPIOG_BSRR 		*((unsigned int*)(GPIOG_BASE+0x10))
#define GPIOG_BRR  		*((unsigned int*)(GPIOG_BASE+0x14))
#define GPIOG_LCKR 		*((unsigned int*)(GPIOG_BASE+0x18))

#else

typedef struct GPIO_RGE
{
	unsigned int CRL;
	unsigned int CRH;
	unsigned int IDR;
	unsigned int ODR;
	unsigned int BSRR;
	unsigned int BRR;
	unsigned int LCKR;
}GPIOx_REG;

#define GPIOA  ((GPIOx_REG *)GPIOA_BASE)
#define GPIOB  ((GPIOx_REG *)GPIOB_BASE)
#define GPIOC  ((GPIOx_REG *)GPIOC_BASE)
#define GPIOD  ((GPIOx_REG *)GPIOD_BASE)
#define GPIOE  ((GPIOx_REG *)GPIOE_BASE)
#define GPIOF  ((GPIOx_REG *)GPIOF_BASE)
#define GPIOG  ((GPIOx_REG *)GPIOG_BASE)

#endif


#endif
