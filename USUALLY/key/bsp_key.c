/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：key.c
 * 描述    ：按键应用函数库         
 * 实验平台：野火STM32开发板
 * 硬件连接：-------------------------
 *          | PA0(IPU)  - key1        |
 *          | PC13(IPD) - key2        |
 *          |                         |
 *           -------------------------
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "bsp_key.h" 

 /*
 * 函数名： Delay
 * 描述  ：不精确的延时
 * 输入  ：延时参数，
 * 输出  ：无
 */
static void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

 /*
 * 函数名：Key_GPIO_Config
 * 描述  ：配置按键用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键端口（PC13,PA0）的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
}

/**
  * @brief   检测是否有按键按下
  * @param   具体的端口和端口位
  *		@arg GPIOx: x可以是（A...G） 
  *		@arg GPIO_PIN 可以是GPIO_PIN_x（x可以是1...16）
  *   @arg Down_state:按键按下时的电平，1为高电平，0为低电平
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state)
{			
	/*检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state ) 
	{	   
		/*延时消抖*/
		Key_Delay(10000);		
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state )  
		{	 
			/*等待按键释放 */
			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state);   
			return 	KEY_ON;	 
		}
		else
			return KEY_OFF;
	}
	else
		return KEY_OFF;
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE****/
