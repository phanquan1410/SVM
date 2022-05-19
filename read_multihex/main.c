   
 #include "main.h"
 #include "stdio.h"
// #include <stdint.h>
 #include <string.h>
// #include <limits.h> 
 #include <stdlib.h>

///////////TIM7 ////////////
void TIM7_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = 63;
	TIM_TimeBaseInitStructure.TIM_Period = 999;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM7, ENABLE);
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_DAC2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_SetCounter(TIM7, 0);
	SET_BIT(TIM7->EGR, TIM_EGR_UG);                    //Update registers when compleate setup
	SET_BIT(TIM7->CR1, TIM_CR1_CEN);                   //Enable counter clock
} 
//////////////////////////////////////////////////// 
// 
void UART2_Configure(void)
	{
	// define configuration
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef	UART_InitStructure;
  NVIC_InitTypeDef NVIC1_InitStructure;
	//enable uart and gpio clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // RCC-> APB1ENR |= (1<<17); enable uart2 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);// RCC->AHBENR |= (1<<17); enable gpioa clock
  
	//configure tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//configure rx
	GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
		
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_7);//config pin AF
	//uart config
	USART_DeInit(USART2);
	UART_InitStructure.USART_BaudRate = 9600;
	UART_InitStructure.USART_WordLength = USART_WordLength_8b;
	UART_InitStructure.USART_StopBits =	USART_StopBits_1;
	UART_InitStructure.USART_Parity = USART_Parity_No;
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, & UART_InitStructure);

	
	
	//USART_ClearFlag(USART2, USART_FLAG_Update);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	
	NVIC1_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC1_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
	NVIC1_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC1_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC1_InitStructure);

  USART_Cmd(USART2,ENABLE);
	}

///////////send string with uart////////////////////
void UART_SendString(USART_TypeDef *USARTx,char *Str)
{
    while(*Str){        
			  USART_SendData(USARTx, *Str);
			  while(USART_GetFlagStatus(USARTx, USART_ISR_TXE) == 0){};
        Str++;
    }
}
///////////////////////////////////////////////////


/////////////////nivc uart///////////////////
uint8_t Data_USART2[8];
uint8_t Data  = 0;
uint8_t Data_Str[8];
uint16_t count = 0;
uint32_t i;
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != 0)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		Data = USART2->RDR;
		Data_USART2[count] = Data;
		count++;	
		
		//sprintf(msg, "%d ",count);
		//UART_SendString(USART2, msg );
	}
}

/////////////////////////////////////////////////////////

///////////////nivc timer//////////
uint16_t x=0; 
void TIM7_DAC2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7, TIM_IT_Update) != 0)
   {
		 
		TIM_ClearITPendingBit(TIM7 , TIM_IT_Update);	
		x++;
   }
}

/////////////////////////////////

/////////
//int myAtoi(const char* str)
//{
//    int sign = 1, base = 0, i = 0;    
//    // if whitespaces then ignore.
//    while (str[i] == ' ')
//    {
//      i++;
//    }    
//    // sign of number
//    if (str[i] == '-' || str[i] == '+')
//    {
//      sign = 1 - 2 * (str[i++] == '-');
//    }   
//    // checking for valid input
//    while (str[i] >= '0' && str[i] <= '9')
//    {
//        // handling overflow test case
//      if (base > INT_MAX / 10
//         || (base == INT_MAX / 10
//          && str[i] - '0' > 7))
//      {
//        if (sign == 1)
//				{
//          return INT_MAX;
//        }
//				else
//				{
//          return INT_MIN;
//        }
//        base = 10 * base + (str[i++] - '0');
//			}
//    return base * sign;
//		}
//	}

/////////



int main(void)
 {
	char msg[100];
	char msg1[100];
	int tem_now = 20;
	int now;
  int t = 0;
	uint8_t Active = 0;
	uint16_t e;
	uint8_t Done = 0; 
	//TIM7_Config();
	SystemCoreClockUpdate();
	SysTick_init();
	UART2_Configure();

	 
	
	while(1)
	{
			if (count == 8)
			{
				if (Data_USART2[0] == 0xAA && 
						Data_USART2[7] == 0x55 && 
						Data_USART2[1] + Data_USART2[2] == 0xFF && 
						Data_USART2[3] + Data_USART2[4] == 0xFF && 
						Data_USART2[5] + Data_USART2[6] == 0xFF)
				{
					UART_SendString(USART2,"\nright form\n");
					Done = 1;

				}else 
				{
					UART_SendString(USART2,"\nwrong form\n");
					Done = 0;
					if (count == 8)
					{
						count = 0;
					}
				}			
			}
		
			if (Done == 1 )
			{
				if(Data_USART2[3] <= 0x3C && Data_USART2[3] >0x00 )
				{
					sprintf(msg,"slot position: %d \n", Data_USART2[3]);
					UART_SendString(USART2,msg);
					for (i=0;i<8;i++){
						Data_USART2[i] = 0;
					}
					if (count == 8)
					{
						count = 0;
						Done = 0;
					}
				}
				if(Data_USART2[3] == 0xDC)
				{
					UART_SendString(USART2," temperature code right \n");
				  for (i=0;i<8;i++){
						Data_USART2[i] = 0;
					}
					if (count == 8)
					{
						count = 0;
						Done = 0;
					}
				}
				if(Data_USART2[3] == 0xCF)
				{
					sprintf(msg,"%d", Data_USART2[5]);
					UART_SendString(USART2,msg);
					t = atoi(msg);
					now = tem_now;
					while( tem_now > t)
					{
						tem_now -- ;
						Active = 1;
					}
					
					while(tem_now < t )
					{
						tem_now ++ ;
						Active = 1;
					}
			
					if (tem_now == t )
					{
						sprintf(msg,"temperature: %d \n", tem_now);
						UART_SendString(USART2,msg);
					}else{
						UART_SendString(USART2," something wrong \n");
					}

				for (i=0;i<8;i++){
						Data_USART2[i] = 0;
					}
					if (count == 8)
					{
						count = 0;
						Done = 0;
					}
				}
				if(Data_USART2[3] == 0xD1)
				{
					if(Data_USART2[5] > 0x00)
					{
						UART_SendString(USART2," time defrost code right \n");
						sprintf(msg,"time: %d min\n", Data_USART2[5]);
						UART_SendString(USART2,msg);
					}
					for (i=0;i<8;i++){
						Data_USART2[i] = 0;
					}
					if (count == 8)
					{
						count = 0;
						Done = 0;
					}
				}
				if(Data_USART2[3] == 0xD2)
				{
					if(Data_USART2[5] > 0x00)
					{
						UART_SendString(USART2," time cooler work code right \n");
						sprintf(msg,"time: %d min\n", Data_USART2[5]);
						UART_SendString(USART2,msg);
					}
					for (i=0;i<8;i++){
						Data_USART2[i] = 0;
					}
					if (count == 8)
					{
						count = 0;
						Done = 0;
					}
				}
				if(Data_USART2[3] == 0xD3)
				{
					if(Data_USART2[5] > 0x00)
					{
						UART_SendString(USART2," time cooler rest code right \n");
						sprintf(msg,"time: %d min\n", Data_USART2[5]);
						UART_SendString(USART2,msg);
					}
					for (i=0;i<8;i++){
						Data_USART2[i] = 0;
					}
					if (count == 8)
					{
						count = 0;
						Done = 0;
					}
				}
				if(Data_USART2[3] == 0xDD)
				{
					UART_SendString(USART2," led set code right \n");
					for (i=0;i<8;i++){
						Data_USART2[i] = 0;
					}
					if (count == 8)
					{
						count = 0;
						Done = 0;
					}
				}
			}
			

			if (Active == 1)
			{
				if (now > t)
				{	
					UART_SendString(USART2,"cooler on\n");
					Active = 0;
				}else if(now < t)
				{
					UART_SendString(USART2,"cooler off\n");
					Active = 0;
				}else{
					UART_SendString(USART2,"something wrong\n");
					Delay_ms(1000);
				}
			}

	}
}
