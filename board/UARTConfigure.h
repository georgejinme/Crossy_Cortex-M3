#ifndef __UARTCONFIGURE_H_
#define __UARTCONFIGURE_H_

//Uart0ͨ��FT2232Dͨ��оƬ����λ�����ӣ�������ñ
//ͨ�Ų�������Ϊ115200
extern void UART0Initial(void);
extern void UARTStringPut(unsigned long ulBase,const char *);
extern void UARTStringGet(char * data, unsigned long ulBase);

#endif
