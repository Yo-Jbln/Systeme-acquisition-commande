/*
 * Commande.h
 *
 *  Created on: Oct 19, 2021
 *      Author: ericc
 */

#define ADC_BUFFER_SIZE 2
#define __VREFANALOG_VOLTAGE__ 3300

void commande(char []);
void Onmotor(void);
void Offmotor(void);
void Badcom(void);
void Comhelp(void);
void Pinlist(void);
void affprompt(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void Imp(void);
void Speedchange(char []);
void Alphachange(int);
void Courant(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
