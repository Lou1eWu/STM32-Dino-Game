#include "stm32f10x.h"
#include "Delay.h"
#include "Key.h"

#define KEY_TIME_DOUBLE 10 // ??????(ms)
#define KEY_TIME_LONG 100 // ??????(ms) ??2000?????1000???
#define KEY_TIME_REPEAT 50 // ??????(ms)


#define KEY_PRESSED				1
#define KEY_UNPRESSED			0

uint8_t Key_Flag[KEY_COUNT];

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11 | GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
	{
//		Delay_ms(50);
//		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
//		Delay_ms(50);
		KeyNum=1;
	}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)
	{
//		Delay_ms(50);
//		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)
//		Delay_ms(50);
		KeyNum=2;
	}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0)
	{
//		Delay_ms(50);
//		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)
//		Delay_ms(50);
		KeyNum=3;
	}	
	return KeyNum;
}

uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
	if (Key_Flag[n] & Flag)
	{
		if (Flag != KEY_HOLD)
		{
			Key_Flag[n] &= ~Flag;
		}
		return 1;
	}
	return 0;
}

uint8_t Key_GetState(uint8_t n)//����keytick��
{
	if (n == KEY_1)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
		{
			return KEY_PRESSED;
		}
	}
	else if (n == KEY_2)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		{
			return KEY_PRESSED;
		}
	}
	else if (n == KEY_3)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
		{
			return KEY_PRESSED;
		}
	}
	else if (n == KEY_4)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1)
		{
			return KEY_PRESSED;
		}
	}
	return KEY_UNPRESSED;
}

void Key_Tick(void)
{
	static uint8_t Count, i;
	static uint8_t CurrState[KEY_COUNT], PrevState[KEY_COUNT];
	static uint8_t S[KEY_COUNT];
	static uint16_t Time[KEY_COUNT];
	
	for (i = 0; i < KEY_COUNT; i ++)
	{
		if (Time[i] > 0)
		{
			Time[i] --;
		}
	}
	
	Count ++;
	if (Count >= 1)
	{
		Count = 0;
		
		for (i = 0; i < KEY_COUNT; i ++)
		{
			PrevState[i] = CurrState[i];
			CurrState[i] = Key_GetState(i);
			
			if (CurrState[i] == KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_HOLD;
			}
			else
			{
				Key_Flag[i] &= ~KEY_HOLD;
			}
			
			if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED)
			{
				Key_Flag[i] |= KEY_DOWN;
			}
			
			if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_UP;
			}
			
			if (S[i] == 0)
			{
				if (CurrState[i] == KEY_PRESSED)
				{
					Time[i] = KEY_TIME_LONG;
					S[i] = 1;
				}
			}
			else if (S[i] == 1)
			{
				if (CurrState[i] == KEY_UNPRESSED)
				{
					Time[i] = KEY_TIME_DOUBLE;
					S[i] = 2;
				}
				else if (Time[i] == 0)
				{
					Time[i] = KEY_TIME_REPEAT;
					Key_Flag[i] |= KEY_LONG;
					S[i] = 4;
				}
			}
			else if (S[i] == 2)
			{
				if (CurrState[i] == KEY_PRESSED)
				{
					Key_Flag[i] |= KEY_DOUBLE;
					S[i] = 3;
				}
				else if (Time[i] == 0)
				{
					Key_Flag[i] |= KEY_SINGLE;
					S[i] = 0;
				}
			}
			else if (S[i] == 3)
			{
				if (CurrState[i] == KEY_UNPRESSED)
				{
					S[i] = 0;
				}
			}
			else if (S[i] == 4)
			{
				if (CurrState[i] == KEY_UNPRESSED)
				{
					S[i] = 0;
				}
				else if (Time[i] == 0)
				{
					Time[i] = KEY_TIME_REPEAT;
					Key_Flag[i] |= KEY_REPEAT;
					S[i] = 4;
				}
			}
		}
	}
}


