

#ifndef CONFIG_H_
#define CONFIG_H_


#define F_CPU				8000000UL
#define SERIAL_BAUD			19200U

#define	CH1					OCR1B
#define CH2					OCR2A
#define CH3					OCR0A
#define CH4					OCR0B

#define VOLTMONITOR1		5	//input voltage channel
#define VOLTMONITOR2		4	//FET driver voltage
#define CURRMONITOR			3	//output current
#define NTC1				2	//onboard ntc
#define NTC2				1	//external ntc

#define LIGHTSENSOR			1

#define CRITICALVOLTAGE				10500
#define WORKRESTOREVOLTAGE			11000
#define LIGHTTARGETLEVEL			255
#define INCRSTEPTIME				20
#define MAXAUTOLIGHTTIME			1000
#define AMBIENTLIGHTTRASHOLVAL		100
#define PCB_CRITICAL_TEMP			650
#define PCB_RESTORE_TEMP			450


#endif 







	