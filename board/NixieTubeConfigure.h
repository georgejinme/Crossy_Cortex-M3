#ifndef	__NIXIETUBECONFIGURE_H_
#define __NIXIETUBECONFIGURE_H_

extern char NixieTube[]; 	//���λ����������
							//1-dp1��2-dp2;3-all;0-none
							//��4λ��Ӧ4�����ֹ�
//����ʾ�ַ�����'0'~'9','a'~'z','A'~'Z','+','-','/','|','\'
extern unsigned char volatile LEDSerial;

extern unsigned char volatile NixieTubeSlice;	//�����ʱ��Ƭ
extern unsigned char volatile NixieTubeSegment;
extern unsigned char volatile NixieTubeSelected1;
extern unsigned char volatile NixieTubeSelected2;

extern void NixieTubeCoding(void);

#endif
