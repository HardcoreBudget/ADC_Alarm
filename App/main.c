/*
 * main.c
 *
 *  Created on: Oct 17, 2023
 *      Author: Hazim Emad
 */
#include "../Library/STD_types.h"
#include "../Library/Bit_Math.h"
#include "../MCAL/DIO/Header/DIO_Interface.h"
#include "../MCAL/GIE/Header/GIE_Interface.h"
#include "../HAL/LCD/Header/LCD_Interface.h"
#include "../MCAL/ADC/Header/ADC_Interface.h"
#include "../MCAL/EXTI/Header/EXTI_Interface.h"
#include "util/delay.h"

#define LDR_THRESHOLD  3
#define LM35_THRESHOLD  28

void ADC_Chain();
void LDR_Alarm();
void LM35_Alarm();
u8 current_ADC = 0;
f32 readings[2];
int main(){
	DIO_voidInit();
	DIO_voidSetPinValue(DIO_PortD,DIO_PIN2,High);
	DIO_voidSetPinValue(DIO_PortD,DIO_PIN3,High);
	ADC_voidInit(0);
	ADC_voidEnable();
	ADC_voidInterrputEnable();
	EXTI0_VID_SET_CALL_BACK(LDR_Alarm);
	EXTI1_VID_SET_CALL_BACK(LM35_Alarm);
	EXTI0_voidInit();
	EXTI1_voidInit();
	EXTI0_voidEnable();
	EXTI1_voidEnable();
	LCD_voidInit();
	GIE_voidEnable();
	ADC_voidSetCallback(ADC_Chain);
	ADC_voidStartInterruptConversion(current_ADC);
	while(1){
		DIO_voidSetPinValue(DIO_PortB,DIO_PIN6,High);
		DIO_voidSetPinValue(DIO_PortB,DIO_PIN7,High);
		DIO_voidSetPinValue(DIO_PortC,DIO_PIN5,Low);
		_delay_ms(100);
		if(readings[0]/1000 > LDR_THRESHOLD){
			DIO_voidSetPinValue(DIO_PortB,DIO_PIN6,Low);
			DIO_voidSetPinValue(DIO_PortC,DIO_PIN5,High);
		}
		else{
			LCD_voidClearLine(0);
		}
		if(readings[1]/10 > LM35_THRESHOLD){
			DIO_voidSetPinValue(DIO_PortB,DIO_PIN7,Low);
			DIO_voidSetPinValue(DIO_PortC,DIO_PIN5,High);
		}
		else{
			LCD_voidClearLine(1);
		}

		ADC_voidEnable();
		ADC_voidStartInterruptConversion(current_ADC);
	}
	return 0;
}

void ADC_Chain(){
	ADC_voidDisable();
	readings[current_ADC] = ADC_u16ReadADCInMV();
	current_ADC++;
	if(current_ADC > 1){
		current_ADC = 0;
	}
}

void LDR_Alarm(){
	LCD_voidGoToPosition(0,0);
	LCD_voidWriteString((u8*) "LDR: ");
	LCD_voidWriteFloatData(readings[0]/1000);
}

void LM35_Alarm(){
	LCD_voidGoToPosition(1,0);
	LCD_voidWriteString((u8*) "Temp: ");
	LCD_voidWriteFloatData(readings[1]/10);
}
