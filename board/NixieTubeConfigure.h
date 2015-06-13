#ifndef	__NIXIETUBECONFIGURE_H_
#define __NIXIETUBECONFIGURE_H_

extern char NixieTube[]; 	//最高位代表两个点
							//1-dp1；2-dp2;3-all;0-none
							//低4位对应4个米字管
//可显示字符，即'0'~'9','a'~'z','A'~'Z','+','-','/','|','\'
extern unsigned char volatile LEDSerial;

extern unsigned char volatile NixieTubeSlice;	//数码管时间片
extern unsigned char volatile NixieTubeSegment;
extern unsigned char volatile NixieTubeSelected1;
extern unsigned char volatile NixieTubeSelected2;

extern void NixieTubeCoding(void);

#endif
