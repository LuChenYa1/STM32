#include "led.h"
#include "stm32g4xx.h"                  // Device header

uint16_t LED_memery =0xff00;

void LOCK_ON_OFF(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

void LED_Init(void)
{
	GPIOC->ODR |= LED_memery;
	LOCK_ON_OFF();
}

void LED_ON(uint8_t LEDx)
{
	LED_memery &= ~(0x0100 << (LEDx - 1));
	GPIOC->ODR &= 0x00ff;
	GPIOC->ODR |= LED_memery;
	LOCK_ON_OFF();
}

void LED_OFF(uint8_t LEDx)
{
	LED_memery |= (0x0100 << (LEDx - 1));
	GPIOC->ODR &= 0x00ff;
	GPIOC->ODR |= LED_memery;
	LOCK_ON_OFF();
}

void LED_TRG(uint8_t LEDx)
{
	LED_memery ^= (0x0100 << (LEDx - 1));
	GPIOC->ODR &= 0x00ff;
	GPIOC->ODR |= LED_memery;
	LOCK_ON_OFF();
}

