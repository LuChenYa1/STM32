//#include "LED.h"

//uint16_t LED_Memery = 0xff00;

//void LOCK_ON_OFF(void)
//{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
//}

//void LED_Init()
//{
//	GPIOC->ODR |= LED_Memery;
//	LOCK_ON_OFF();
//}

//void LED_ON(uint8_t LEDx)
//{
//	LED_Memery = LED_Memery & ~(0x0100 << (LEDx -1));
//	GPIOC -> ODR &= 0x00ff;
//	GPIOC -> ODR |= LED_Memery;
//	LOCK_ON_OFF();
//}

//void LED_OFF(uint8_t LEDx)
//{
//	LED_Memery = LED_Memery | (0x0100 << (LEDx -1));
//	GPIOC -> ODR &= 0x00ff;
//	GPIOC -> ODR |= LED_Memery;
//	LOCK_ON_OFF();
//}

//void LED_TRG(uint8_t LEDx)
//{
//	LED_Memery = LED_Memery ^ (0x0100 << (LEDx -1));
//	GPIOC -> ODR &= 0x00ff;
//	GPIOC -> ODR |= LED_Memery;
//	LOCK_ON_OFF();
//}

#include "LED.h"

uint16_t LED_Memery = 0xff00;

void LOCK_ON_OFF(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

void LED_Init(void)
{
	GPIOC->ODR |= LED_Memery;
	LOCK_ON_OFF();
}

void LED_ON(uint8_t LEDx)
{
	LED_Memery &= ~(0x0100 << (LEDx - 1));
	GPIOC->ODR &= 0x00ff;
	GPIOC->ODR |= LED_Memery;
	LOCK_ON_OFF();
}

void LED_OFF(uint8_t LEDx)
{
	LED_Memery |= (0x0100 << (LEDx - 1));
	GPIOC->ODR &= 0x00ff;
	GPIOC->ODR |= LED_Memery;
	LOCK_ON_OFF();
}

void LED_TRG(uint8_t LEDx)
{
	LED_Memery ^= (0x0100 << (LEDx - 1));
	GPIOC->ODR &= 0x00ff;
	GPIOC->ODR |= LED_Memery;
	LOCK_ON_OFF();
}

