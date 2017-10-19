/**
  ******************************************************************************
  * File Name          : pwm.c
  * Description        : Adds a commands to virtual serial port to initialize and read pwm.
  * Modified on	       : June 12, 2017
  *****************************************************************************
*/
//include files
#include <stdint.h>
#include <stdio.h>
#include "stm32f3xx_hal.h"
#include "common.h"

//Declaring global variables
TIM_HandleTypeDef tim1;
TIM_OC_InitTypeDef sConfig;
GPIO_InitTypeDef GPIO_InitStruct; 

// FUNCTION      : adcInit
// DESCRIPTION   : This function initializes pwm channels.
// PARAMETERS    : 
//	mode	: checks if the command is passed through in vcp
// RETURNS       : NOTHING
void pwmInit(int mode)
{
	//checks if such command exists
	if(mode != CMD_INTERACTIVE) {
    	return;
  	}

	//Initialize Timer
	__TIM1_CLK_ENABLE();
	tim1.Instance = TIM1;
	tim1.Init.Prescaler = 72;
	tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim1.Init.Period = 1000;
	tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim1.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&tim1);
	HAL_TIM_Base_Start(&tim1);

	//configure gpio pin for pwm channels
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    	GPIO_InitStruct.Pull = GPIO_NOPULL;
    	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    	GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	

	//configure user pussh button 
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Initialize PWM
	sConfig.OCMode = TIM_OCMODE_PWM1;
	sConfig.Pulse = 500;
	sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfig.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfig.OCFastMode = TIM_OCFAST_DISABLE;
	sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfig.OCNIdleState =TIM_OCNIDLESTATE_RESET;

	//config three channels
	HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_3);
}

// FUNCTION      : pwm
// DESCRIPTION   : This function reads specified pwm channels.
// PARAMETERS    : 
//	mode	: checks if the command is passed through in vcp
// RETURNS       : NOTHING
void pwm(int mode)
{
	uint32_t duty,channel;
	int rc;

	//checks if such command exists
	if(mode != CMD_INTERACTIVE) {
		return;
  	}
	//fetch user inputs
	rc = fetch_uint32_arg(&channel);
  	if(rc) {
    		printf("Missing Channel\n");
    		return;
 	}

	//fetch user inputs
	rc = fetch_uint32_arg(&duty);
  	if(rc) {
    		printf("Missing value for duty cycle\n");
    		return;
 	}

	//reset all pwm channels
	HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_3);

	//Configure the selected channel 
	if(channel == 1) {
		channel = TIM_CHANNEL_1;
		TIM1->CCR1 = duty*1024/100;
	}
	else if(channel == 2) {
		channel = TIM_CHANNEL_2;
		TIM1->CCR2 = duty*1024/100;
	}
	else if(channel == 3) {
		channel = TIM_CHANNEL_3;
		TIM1->CCR3 = duty*1024/100;
	}
	else {
		printf("No channel found");
	}
	
	//start pwm
	HAL_TIM_PWM_Start(&tim1,channel);
	
} 

// FUNCTION      : pwmBreathe
// DESCRIPTION   : This function lets LED to breathe.
// PARAMETERS    : 
//	mode	: checks if the command is passed through in vcp
// RETURNS       : NOTHING
void pwmBreathe(int mode)
{
	int count=0;
	int flag;
	uint32_t value,channel;

	//reset all pwm channels
	HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_3);

	//fetch user inputs
	fetch_uint32_arg(&channel);
	fetch_uint32_arg(&value);

	//Configure the selected channel 
	//for channel 1
	if(channel == 1) {
		while(1) {
			if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0 ) {		//if button not pushed.....pwm starts
				//set and reset flag at extreme values
				if(count == 0) 
					flag = 0;
				if(count == value) 
					flag = 1;
		
				if(flag == 0) {						//duty cycle goes from 0% to 100%
					TIM1->CCR1 = count;
					HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_1);
					HAL_Delay(2);
					count ++;
				}
				if(flag == 1) {						//duty cycle goes from 100% to 0%
					TIM1->CCR1 = count;
					HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_1);
					HAL_Delay(2);
					count --;
				}
			}
			else {
				HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_1);			//stop pwm when button is pressed
				return;
			}
		}
	}
	//for channel 2
	else if(channel == 2) {
		while(1) {
			if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0 ) {		//if button not pushed.....pwm starts
				//set and reset flag at extreme values
				if(count == 0) 
					flag = 0;
				if(count == value) 
					flag = 1;
		
				if(flag == 0) {						//duty cycle goes from 0% to 100%
					TIM1->CCR2 = count;
					HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_2);
					HAL_Delay(2);
					count ++;
				}
				if(flag == 1) {						//duty cycle goes from 100% to 0%
					TIM1->CCR2 = count;
					HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_2);
					HAL_Delay(2);
					count --;
				}
			}
			else {
				HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_2);			//stop pwm when button is pressed
				return;
			}
		}
	}
	//for channel 3
	else if(channel == 3) {
		while(1) {
			if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0 ) {		//if button not pushed.....pwm starts
				//set and reset flag at extreme values	
				if(count == 0) 
					flag = 0;
				if(count == value) 
					flag = 1;
		
				if(flag == 0) {						//duty cycle goes from 0% to 100%
					TIM1->CCR3 = count;
					HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_3);
					HAL_Delay(2);
					count ++;
				}
				if(flag == 1) {						//duty cycle goes from 100% to 0%
					TIM1->CCR3 = count;
					HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_3);
					HAL_Delay(2);
					count --;
				}
			}
			else {
				HAL_TIM_PWM_Stop(&tim1,TIM_CHANNEL_3);			//stop pwm when button is pressed
				return;
			}
		}

	}
	else {
		printf("No channel found");
	}

} 


//add commands to virtual communication port
ADD_CMD("pwminit",pwmInit,"		Initializes PWM channels")
ADD_CMD("pwm",pwm,"<channel> <value> 	Reads PWM channels")
ADD_CMD("pwmbreathe",pwmBreathe,"<channel> <value> 	LED breathe")
