/*
 * Commande.c
 *
 *  Created on: Oct 19, 2021
 *      Author: ericc
 */

/**
 * \file Commande.c
 * \brief Programme où sont mise les fonctions de commande.
 * \author Eric.CHASSERGUE & Yohesan J.
 * \date 19 Octobre 2021
 *
 * Programme d'où les fonctions commande sont exécutées.
 */

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "Commande.h"
#include "string.h"
#include "stdlib.h"
#include "adc.h"
#include "dma.h"

const uint8_t prompt2[]="\r\nYou@Nucleo-G431 >> ";
int alpha;
int CR = 1023;
uint16_t ADC_buffer[ADC_BUFFER_SIZE];

/**
 * \fn void affprompt(void)
 * \brief Fonction qui afficher le prompt.
 */
void affprompt(){
	HAL_UART_Transmit(&huart2, prompt2, sizeof(prompt2) , HAL_MAX_DELAY);
}


/**
 * \fn void commande(char cmd[])
 * \brief Fonction qui vérifie si la commande entrée est connue.
 * 
 * \param char cmd[] pointeur du tableau où est contenue la commande entrée
 */
void commande(char cmd[]){
	if (strcmp(cmd,"start\r")==0) {Onmotor();}
	else if (strcmp(cmd,"stop\r")==0) {Offmotor();}
	else if (strcmp(cmd,"help\r")==0) {Comhelp();}
	else if (strcmp(cmd,"pinout\r")==0) {Pinlist();}
	else if (strcmp(cmd,"courant\r")==0) {Courant();}
	else if (strncmp(cmd,"speed = ",8)==0) {Speedchange(cmd);}
	else {Badcom();}
}


/**
 * \fn void Imp(void)
 * \brief Fonction qui envoie le signal permettant de demarrer le hacheur.
 */
void Imp(){
	HAL_GPIO_WritePin(Imp_GPIO_Port, Imp_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(Imp_GPIO_Port, Imp_Pin, GPIO_PIN_RESET);
}


/**
 * \fn void Onmotor(void)
 * \brief Fonction qui allume le moteur	via la commande "start".
 */
void Onmotor(){
	alpha = 50;
	const uint8_t power_on[] = "\r\nPower ON"; 					//contenant le message d'allumage du moteur
	const uint8_t error_start[] = "\r\nLe demarrage a echoué"; 	//contenant le message d'erreur au démarrage
	const uint8_t testm[] = "\r\nLe test commence"; 			//contenant le message prévenant du démarrage du test
	const uint8_t a50[] = "\r\nalpha = 50%"; 					//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t a70[] = "\r\nalpha = 70%"; 					//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t a100[] = "\r\nalpha = 100%"; 					//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t a0[] = "\r\nalpha = 0%"; 						//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t testmt[] = "\r\nLe test est fini"; 			//contenant le message prévenant de la fin du test

	//Initialisation pour l'ADC
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, ADC_buffer, ADC_BUFFER_SIZE);

	Imp();
	HAL_UART_Transmit(&huart2, power_on, sizeof(power_on), HAL_MAX_DELAY);

	TIM1->CCR2 = (double) (alpha*CR)/100;

	affprompt();

}

/**
 * \fn void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 * \brief Fonction qui allume le moteur via un appuie sur le button utilisateur.
 * 
 * /!\ FONCTION NON FONCIONNELLE
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	const uint8_t power_on[] = "\r\nPower ON"; //message d'allumage du moteur

	Imp();
	HAL_UART_Transmit(&huart2, power_on, sizeof(power_on), HAL_MAX_DELAY);
	affprompt();
}


/**
 * \fn void Alphachange(double newalpha)
 * \brief Fonction qui change la valeur de alpha selon la valeur souhaitée.
 */
void Alphachange(int newalpha){
	while(alpha != newalpha){
		
		if(newalpha > alpha){alpha++;}
		
		else{alpha--;}
		
		TIM1->CCR2 = (double) (alpha*CR)/100;
		HAL_Delay(1000);
	}
}


/**
 * \fn void Speedchange(void)
 * \brief Fonction qui change la vitesse du moteur via la commande "speed = <alpha>"
 */
void Speedchange(char cmd[]){
	int alphavoulue;
	alphavoulue = atoi(&cmd[8]);
	
	if(alphavoulue>100){alphavoulue = 100;}
	Alphachange(alphavoulue);
	
	affprompt();
}


/**
 * \fn void Offmotor(void)
 * \brief Fonction qui éteind le moteur via la commande "stop"
 */
void Offmotor(){
	const uint8_t power_off[] = "\r\nPower OFF"; //message d'extinction du moteur

	Alphachange(50);
	HAL_UART_Transmit(&huart2, power_off, sizeof(power_off), HAL_MAX_DELAY);
	affprompt();
}


/**
 * \fn void Badcom(void)
 * \brief Gestion d'erreur pour les commandes non reconnues.
 */
void Badcom(){
	const uint8_t not_found[] = "\r\nCommand not found"; //message de commande non reconnue

	HAL_UART_Transmit(&huart2, not_found, sizeof(not_found), HAL_MAX_DELAY);
	affprompt();
}


/**
 * \fn void Comhelp(void)
 * \brief Fonction qui affiche les commandes utilisable via la commande "help"
 */
void Comhelp(){
	const uint8_t help[] = "\r\nCommandes disponible :\r\nhelp : affiche toutes les commandes disponibles\r\npinout : affiche toutes les broches connectees et leur fonction\r\nstart : allume l'etage de puissance du moteur\r\nstop : eteint l'etage de puissance du moteur\r\nspeed = <alpha> : fait tourner le moteur a la vitesse voulue";

	HAL_UART_Transmit(&huart2, help, sizeof(help), HAL_MAX_DELAY);
	affprompt();
}

/**
 * \fn void Pinlist(void)
 * \brief Fonction qui affiche la liste des pin branchés et leurs fonctions via la commande "pinout"
 *
 * /!\ A REMPLIRE
 */
void Pinlist(){
	const uint8_t pinout[] = "\r\nPin 9 : 5V\r\nPin 12 : TIM1_CH2N (PB0)\r\nPin 13 : TIM1_CH1 (PC0)\r\nPin 16 : Capteur de courant de la phase jaune (PA1)\r\nPin 18 : GND\r\nPin 19 : 5V\r\nPin 27 : GND\r\nPin 30 : TIM1_CH2 (PC1)\r\nPin 31 : TIM1_CH1N (PA11)\r\nPin 33 : Reception de l'inpultion pour le reset (PA0)\r\nPin 35 : Capteur de courant de la phase rouge\r\nPin 36 : GND\r\nPin 37 : 5V";

	HAL_UART_Transmit(&huart2, pinout, sizeof(pinout), HAL_MAX_DELAY);
	affprompt();
}

/**
 * \fn void Pinlist(void)
 * \brief Fonction permettant de recuperer les données sur le courant.
 * 
 * /!\ FONCTION NON TERMINEE
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	int j;
	//HAL_ADC_Stop_DMA(&hadc1);
	for(j=0;j<ADC_BUFFER_SIZE;j++){
		ADC_buffer[j] = __LL_ADC_CALC_DATA_TO_VOLTAGE( __VREFANALOG_VOLTAGE__, ADC_buffer[j],  LL_ADC_RESOLUTION_12B);
	}
}

/**
 * \fn void Pinlist(void)
 * \brief Fonction qui affiche la valeur du courant dans le shell
 * 
 * /!\ FONCTION NON TERMINEE
 */
void Courant(){
	int j;
	char msg[30];
	
	for(j=0;j<ADC_BUFFER_SIZE;j++){
		sprintf(msg,"\r\nADC_buffer[%d] = %f", j, (float) ADC_buffer[j]);
		HAL_UART_Transmit(&huart2, msg, sizeof(msg) , HAL_MAX_DELAY);
	}
	
	affprompt();
}
