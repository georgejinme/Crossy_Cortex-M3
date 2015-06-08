#ifndef	__GPIODRIVERCONFIGURE_H__
#define __GPIODRIVERCONFIGURE_H__

#define LED_0			0
#define LED_1			1
#define LED_ALL			2

#define KEY_PRESS		1
#define KEY_LEFT		2
#define KEY_RIGHT		3
#define KEY_UP			4
#define KEY_DOWN		5

#define LED0_PIN		GPIO_PIN_3
#define LED0_BASE		GPIO_PORTF_BASE
#define LED1_PIN		GPIO_PIN_2
#define LED1_BASE		GPIO_PORTF_BASE

#define KEY_PRESS_PIN	GPIO_PIN_5
#define KEY_PRESS_BASE	GPIO_PORTE_BASE
#define KEY_LEFT_PIN	GPIO_PIN_4
#define KEY_LEFT_BASE	GPIO_PORTB_BASE
#define KEY_RIGHT_PIN	GPIO_PIN_6
#define KEY_RIGHT_BASE	GPIO_PORTB_BASE
#define KEY_UP_PIN		GPIO_PIN_4
#define KEY_UP_BASE		GPIO_PORTE_BASE
#define KEY_DOWN_PIN	GPIO_PIN_1
#define KEY_DOWN_BASE	GPIO_PORTF_BASE

#define UART0RX_PIN		GPIO_PIN_0
#define UART0TX_PIN		GPIO_PIN_1
#define UART0_PIN_BASE	GPIO_PORTA_BASE

extern void GPIOInitial(void);
extern void LEDOn(unsigned char LEDNum); 
extern void LEDOff(unsigned char LEDNum);
extern void LEDOverturn(unsigned char LEDNum);
extern unsigned char KeyPress(unsigned char KeyNum);
#endif
