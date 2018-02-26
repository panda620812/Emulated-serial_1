#ifndef	_ADC_H_
#define	_ADC_H_

void ADC_GPIO_Configuration(void);
void ADC_Configuration(void);
void ADC_Initializes(void);
uint32_t ADC_Check(void);
void DMA_Configuration(void);
void filter(void);
u16 GetVolt(u16 advalue);
#endif
