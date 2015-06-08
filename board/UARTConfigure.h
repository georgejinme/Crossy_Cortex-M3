#ifndef __UARTCONFIGURE_H_
#define __UARTCONFIGURE_H_

//Uart0通过FT2232D通信芯片与上位机连接，无需跳帽
//通信波特率设为115200
extern void UART0Initial(void);
extern void UARTStringPut(unsigned long ulBase,const char *);

#endif
