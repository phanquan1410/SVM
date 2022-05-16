 #include "main.h"
 #include "stdio.h"
 #include "math.h"

 
 void ADC1_Init(ADC_TypeDef* ADCx)
{
	/*--------------------------------------------------------------------------*/
	ADC_InitTypeDef ADC_Init_Struct;
  ADC_CommonInitTypeDef ADC_Common_Struct;	
	
	/*--------------------------------------------------------------------------*/
	// ADC begin and clock config /
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);                           //ADC clock prescaler
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);               //Enable ADC clock if ADC clock sync AHB 
	ADC_VoltageRegulatorCmd(ADCx, ENABLE);                            //ADC vol regulator
	Delay_10(2);
	
	//	CLEAR_BIT(ADCx->CR, ADC_CR_ADCALDIF);                             //Select calibration mode
	//	SET_BIT(ADCx->CR, ADC_CR_ADCAL);                                  //Start calibration
	//	while(READ_BIT(ADCx->CR, ADC_CR_ADCAL));                          //Wairt process
	/*--------------------------------------------------------------------------*/
	
	// ADC common mode config /
	ADC_Common_Struct.ADC_Clock = ADC_Clock_AsynClkMode;              //ADC clock source select 
	ADC_Common_Struct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA mode select for dual ADC
	ADC_Common_Struct.ADC_DMAMode = ADC_DMAMode_Circular;             //DMA mdoe select for single ADC               
	ADC_Common_Struct.ADC_Mode = ADC_Mode_Independent;                //ADC mode dual or single
	ADC_Common_Struct.ADC_TwoSamplingDelay = 2;                       //Delay between 2 sample in anternate trigger mode
	ADC_CommonInit(ADCx, &ADC_Common_Struct);
	
	/*--------------------------------------------------------------------------*/
	// ADC module enable /
	ADC_Cmd(ADCx, ENABLE);
	while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_RDY));
	
	/*--------------------------------------------------------------------------*/
	// ADC1 init /
	ADC_Init_Struct.ADC_AutoInjMode = ADC_AutoInjec_Disable;                          //Auto injected group conversion mode
	ADC_Init_Struct.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;          //Regular group continuous conversion mode
	ADC_Init_Struct.ADC_DataAlign = ADC_DataAlign_Right;                              //ADC data type
	ADC_Init_Struct.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_13;         //External trigger event select
	ADC_Init_Struct.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;       //External trigger edge select 
	ADC_Init_Struct.ADC_NbrOfRegChannel = 1;                                          //Number of conversion channel in regular group
	ADC_Init_Struct.ADC_OverrunMode = ADC_OverrunMode_Disable;                        //ADC result overrun mode
	ADC_Init_Struct.ADC_Resolution = ADC_Resolution_12b;                              //ADC resolution
	ADC_Init(ADCx, &ADC_Init_Struct);
	
	ADC_AutoDelayCmd(ADCx, DISABLE);                                                    //This mode used conjungtion with 
	                                                                 
	ADC_RegularChannelConfig(ADCx, ADC_Channel_1, 1, ADC_SampleTime_19Cycles5);  
}

 void UART2_Configure(void)
	{
	// define configuration
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef	UART_InitStructure;

	//enable uart and gpio clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // RCC-> APB1ENR |= (1<<17); enable uart2 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);// RCC->AHBENR |= (1<<17); enable gpioa clock
  
	//configure tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//configure rx
	GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
		
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_7);//config pin AF
	//uart config
	UART_InitStructure.USART_BaudRate = 9600;
	UART_InitStructure.USART_WordLength = USART_WordLength_8b;
	UART_InitStructure.USART_StopBits =	USART_StopBits_1;
	UART_InitStructure.USART_Parity = USART_Parity_No;
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, & UART_InitStructure);

	USART_Cmd(USART2,ENABLE);

}

  // send a string
void UART_SendString(USART_TypeDef *USARTx,char *Str)
{
    while(*Str){        
			  USART_SendData(USARTx, *Str);
			  while(USART_GetFlagStatus(USARTx, USART_ISR_TXE) == 0){};
        Str++;
    }
}

int main(void)
{
	char msg[100];
	char msg1[100];
	int i;
	float V_res,I,R,T1,C,T2;
	uint16_t V,V_average; // vol from sensor
	SysTick_init();
	ADC1_Init(ADC1);
  SystemCoreClockUpdate();
  UART2_Configure();
  
		
	
while (1)
  {	
	for(i = 0; i<200; i++ ){
		ADC_StartConversion(ADC1);
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){};//Processing the conversion
		V = ADC_GetConversionValue(ADC1);
		V_average = V_average + V; 
	}
	V_res = 5.0 - V_average*3.3/4096;
	I = V_res/470;
	R = (V*3.3/4096)/I; //with 5V input

//	T1 = 1/(3.9083 * pow(10,-3) + 5.775 * pow(10,-7) * log(R) + 
//				(-4.183 * pow(10,-12)) * (pow(log(R),3)));
//	T1 = 1/(50.47364289686 + (-15.8542661516) * log(R) + 
//			(2319595390404 * pow(10,-3)) * (pow(log(R),3)));
		T1 = ((R/100) -1 )/(3.9083 * pow(10,-3));
//	T2 = 1/(3.9083 * pow(10,-3) + 5.775 * pow(10,-7) * log(R));
	
		
//	sprintf(msg, "\n %f %f %f %f %f %f \n",V_res,I,R,T1,C,T2);	
//	UART_SendString(USART2,msg);
//	Delay_ms(500);
//		
//	sprintf(msg, "adc %d \n",V);
//	sprintf(msg, "V res %f \n",V_res);
//	sprintf(msg, "intensity %f \n",I);
		sprintf(msg, "temperature %f \n",T1);	
		UART_SendString(USART2,msg);
		Delay_ms(500);		
	}
 } 