#ifndef __BSP_LED_H__
#define __BSP_LED_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* ���Ͷ��� ------------------------------------------------------------------*/
typedef enum
{
  LED_OFF = 0,
  LED_ON,
}LEDState_TypeDef;
#define IS_LED_STATE(STATE)           (((STATE) == LED_OFF) || ((STATE) == LED_ON))

/* �궨�� --------------------------------------------------------------------*/
#define LED1                          (uint8_t)0x01
#define LED2                          (uint8_t)0x02
#define LED3                          (uint8_t)0x04
#define IS_LED_TYPEDEF(LED)           (((LED) == LED1) || ((LED) == LED2) || ((LED) == LED3))

/*
 * ���º궨�����ݸ�������Ӳ��ϢϢ��أ���Ҫ�鿴�������·ԭ��ͼ������ȷ��д��
 * ���磬��ԭ��ͼ����LED1�ƽ���stm32f103оƬ��PB0�����ϣ������й�LED1�ĺ궨��
 * ������GPIOB��GPIO_Pin_0��صģ�����ר�Ű���Щ�뿪����Ӳ����ص����ݶ���Ϊ�꣬
 * ������޸Ļ�����ֲ����ǳ����㡣
 */

#define LED1_RCC_CLOCKGPIO            RCC_APB2Periph_GPIOB
#define LED1_GPIO_PIN                 GPIO_Pin_0
#define LED1_GPIO                     GPIOB


#define LED2_RCC_CLOCKGPIO            RCC_APB2Periph_GPIOG
#define LED2_GPIO_PIN                 GPIO_Pin_6
#define LED2_GPIO                     GPIOG


#define LED3_RCC_CLOCKGPIO            RCC_APB2Periph_GPIOG
#define LED3_GPIO_PIN                 GPIO_Pin_7
#define LED3_GPIO                     GPIOG

			
#if 0         /* ���ñ�׼�⺯������ */
#define LED1_ON                       GPIO_SetBits(LED1_GPIO,LED1_GPIO_PIN)
#define LED1_OFF                      GPIO_ResetBits(LED1_GPIO,LED1_GPIO_PIN)
#define LED1_TOGGLE                   {LED1_GPIO->ODR ^=LED1_GPIO_PIN;}  //�����ת

#define LED2_ON                       GPIO_SetBits(LED2_GPIO,LED2_GPIO_PIN)
#define LED2_OFF                      GPIO_ResetBits(LED2_GPIO,LED2_GPIO_PIN)
#define LED2_TOGGLE                   {LED2_GPIO->ODR ^=LED2_GPIO_PIN;}  //�����ת

#define LED3_ON                       GPIO_SetBits(LED3_GPIO,LED3_GPIO_PIN)
#define LED3_OFF                      GPIO_ResetBits(LED3_GPIO,LED3_GPIO_PIN)
#define LED3_TOGGLE                   {LED3_GPIO->ODR ^=LED3_GPIO_PIN;}  //�����ת

#else       /* ֱ�Ӳ����Ĵ������� */

#define LED1_ON                       {LED1_GPIO->BSRR=LED1_GPIO_PIN;}    //����͵�ƽ
#define LED1_OFF                      {LED1_GPIO->BRR=LED1_GPIO_PIN;}   //����ߵ�ƽ
#define LED1_TOGGLE                   {LED1_GPIO->ODR ^=LED1_GPIO_PIN;}  //�����ת

#define LED2_ON                       {LED2_GPIO->BSRR=LED2_GPIO_PIN;}    //����͵�ƽ
#define LED2_OFF                      {LED2_GPIO->BRR=LED2_GPIO_PIN;}   //����ߵ�ƽ
#define LED2_TOGGLE                   {LED2_GPIO->ODR ^=LED2_GPIO_PIN;}  //�����ת

#define LED3_ON                       {LED3_GPIO->BSRR=LED3_GPIO_PIN;}    //����͵�ƽ
#define LED3_OFF                      {LED3_GPIO->BRR=LED3_GPIO_PIN;}   //����ߵ�ƽ
#define LED3_TOGGLE                   {LED3_GPIO->ODR ^=LED3_GPIO_PIN;}  //�����ת

#endif


/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void LED_GPIO_Init(void);
void LEDx_StateSet(uint8_t LEDx,LEDState_TypeDef state);
#endif  // __BSP_LED_H__

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
