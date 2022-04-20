#include "stm32f0xx.h"
#include "lcd.h"

void init_lcd_spi(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	//set as outputs
	GPIOB->MODER &= ~0x30c30000; //0011 0000 1100 0011 0000 0000 0000 0000
	GPIOB->MODER |= 0x10410000;  //0001 0000 0100 0001 0000 0000 0000 0000

	//set logic high
	GPIOB->ODR |= 0x4900; //0100 1001 0000 0000

	//set to alternative function 0
	GPIOB->AFR[0] &= ~0xf0f000 ;//1111 0000 1111 0000 0000 0000

	//"""Enable the SPI channel."""
	RCC->APB1ENR |= 1<<14;

	//"""Ensure that the CR1_SPE bit is clear. Many of the bits set in the control registers require that the SPI channel is not enabled."""
	SPI2->CR1 &= ~(1<<6);

	//"""Configure the interface for a 8-bit word size."""
	SPI2->CR2 = SPI_CR2_DS_0 |SPI_CR2_DS_1 |SPI_CR2_DS_2;

	//"""Configure the SPI channel to be in master mode."""
	SPI2->CR1 |= (1<<2);

	//Set the SSM and SSI bits
	SPI2->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM;

	SPI2->CR1 |= (1<<6);

}

void setup_buttons(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    //set as outputs
    GPIOC->MODER &= ~0xff00; //1111 1111 0000 0000
    GPIOC->MODER |= 0x5500;//0101 0101 0000 0000

    //set open drain
    GPIOC->OTYPER |= 0xf0;//1111 0000

    //set as inputs
    GPIOC->MODER &= ~0xff; //1111 1111

    //enable internal pullup resistors
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;

}

void basic_drawing(void);
void move_ball(void);

int main(void)
{
    setup_buttons();
    LCD_Setup(); // this will call init_lcd_spi()
    basic_drawing();
    move_ball();
}

