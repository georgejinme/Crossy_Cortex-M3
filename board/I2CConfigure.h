#ifndef	__I2CCONFIGURE_H_
#define __I2CCONFIGURE_H_

#define TUBE_SEG_I2CADDR		0x18
#define TUBE_SEL1_I2CADDR		0x19
#define TUBE_SEL2_I2CADDR		0x1a
#define LED_I2CADDR				0x1c

#define PCA9557_REG_INPUT		0x00
#define	PCA9557_REG_OUTPUT		0x01
#define PCA9557_REG_PolInver	0x02
#define PCA9557_REG_CONFIG		0x03		

//I2C主机模式下，本机I2C0总线初始化
extern void I2C0MasterInitial(void);
extern void I2CMasterSpeedSet(unsigned long ulBase, 
							  unsigned long ulSpeed);

extern unsigned char I2CMasterTransmit_Burst_2Bytes		//突发发送2Bytes（1位子地址，1位数据）
							 (unsigned long ulBase,		//发送完成后才会退出
					 		  unsigned char ucSla,		//从机地址
							  unsigned char ucAddr,		//从机子地址
							  unsigned char ucData);	//传送的1个数据位
extern void I2C0DeviceRefresh(void);

#endif
