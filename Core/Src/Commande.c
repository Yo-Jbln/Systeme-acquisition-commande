/*
 * Commande.c
 *
 *  Created on: Oct 19, 2021
 *      Author: ericc
 */

/**
 * \file Commande.c
 * \brief Programme où sont mise les fonctions de commande.
 * \author Eric.CHASSERGUE
 * \date 6 septembre 2007
 *
 * Programme d'où les fonctions commande sont exécutées.
 *
 */

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "Commande.h"
#include "string.h"
#include "stdlib.h"

const uint8_t prompt2[]="\r\n<user>@Nucleo-G431 >> ";

/**
 * \fn void commande(char cmd[])
 * \brief Fonction qui vérifie si la commande entrée est connue.
 *
 * \param char cmd[] pointeur du tableau où est contennue la commande entrée
 */
void commande(char cmd[]){
	if (strcmp(cmd,"start\r")==0) {Onmotor();}
	else if (strcmp(cmd,"stop\r")==0) {Offmotor();}
	else if (strcmp(cmd,"help\r")==0) {Comhelp();}
	else if (strcmp(cmd,"pinout\r")==0) {Pinlist();}
	//else if (strncmp(cmd,"speed = ",8)==0) {Speedchange(cmd);}
	else {Badcom();}
}

/**
 * \fn void affprompt(void)
 * \brief Fonction qui afficher un prompt.
 */
void affprompt(){
	HAL_UART_Transmit(&huart2, prompt2, sizeof(prompt2) , HAL_MAX_DELAY);
}

/**
 * \fn void Onmotor(void)
 * \brief Fonction qui allume le moteur via la commande "start" depuis le shell.
 */
void Onmotor(){
	double alpha = 0.5;
	int CR = 1023;
	int i;
	const uint8_t power_on[] = "\r\nPower ON"; 					//contenant le message d'allumage du moteur
	const uint8_t error_start[] = "\r\nLe demarrage a echouer"; //contenant le message d'erreur au démarrage
	const uint8_t testm[] = "\r\nLe test commence"; 			//contenant le message prévenant du démarrage du test
	const uint8_t a50[] = "\r\nalpha = 50%"; 					//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t a70[] = "\r\nalpha = 70%"; 					//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t a100[] = "\r\nalpha = 100%"; 					//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t a0[] = "\r\nalpha = 0%"; 						//contenant le message qui indique une nouvelle valeur pour alpha
	const uint8_t testmt[] = "\r\nLe test commence"; 			//contenant le message prévenant de la fin du test



	Imp();
	HAL_UART_Transmit(&huart2, power_on, sizeof(power_on), HAL_MAX_DELAY);


	//Test
	HAL_UART_Transmit(&huart2, testm, sizeof(testm), HAL_MAX_DELAY);
	//TIM1->CCR1 = CR;

	//alpha = 50%
	TIM1->CCR2 = alpha*CR;
	HAL_UART_Transmit(&huart2, a50, sizeof(a50), HAL_MAX_DELAY);
	HAL_Delay(1000);

	//alpha = 70%
	for(i =0;i<20;i++){
		alpha = alpha + 0.01;
		TIM1->CCR2 = alpha*CR;
		HAL_Delay(1000);
	}
	//alpha = 0.7;
	//TIM1->CCR2 = alpha*CR;
	HAL_UART_Transmit(&huart2, a70, sizeof(a70), HAL_MAX_DELAY);
	HAL_Delay(1000);

	//alpha = 100%
	for(i =0;i<30;i++){
			alpha = alpha + 0.01;
			TIM1->CCR2 = alpha*CR;
			HAL_Delay(1000);
		}
	//alpha = 1.0;
	//TIM1->CCR2 = alpha*CR;
	HAL_UART_Transmit(&huart2, a100, sizeof(a100), HAL_MAX_DELAY);
	HAL_Delay(1000);

	//alpha = 0%
	for(i =100;i=0;i--){
		alpha = alpha - 0.01;
			TIM1->CCR2 = alpha*CR;
			HAL_Delay(1000);
		}
	//alpha = 0.0;
	//TIM1->CCR2 = alpha*CR;
	HAL_UART_Transmit(&huart2, a0, sizeof(a0), HAL_MAX_DELAY);
	HAL_Delay(1000);

	for(i =0;i<50;i++){
		alpha = alpha + 0.01;
				TIM1->CCR2 = alpha*CR;
				HAL_Delay(1000);
			}

	HAL_UART_Transmit(&huart2, testmt, sizeof(testmt), HAL_MAX_DELAY);
	affprompt();

}

/**
 * \fn void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 * \brief Fonction qui allume le moteur via un appuie sur le button utilisateur.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	const uint8_t power_on[] = "\r\nPower ON"; //contenant le message d'allumage du moteur

		Imp();
		HAL_UART_Transmit(&huart2, power_on, sizeof(power_on), HAL_MAX_DELAY);
		affprompt();
}

void Imp(){
	HAL_GPIO_WritePin(Imp_GPIO_Port, Imp_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(Imp_GPIO_Port, Imp_Pin, GPIO_PIN_RESET);
}

/**
 * \fn void Offmotor(void)
 * \brief Fonction qui éteind le moteur.
 */
void Offmotor(){
	const uint8_t power_off[] = "\r\nPower OFF"; //contenant le message d'extinction du moteur

	HAL_UART_Transmit(&huart2, power_off, sizeof(power_off), HAL_MAX_DELAY);
	affprompt();
}

/**
 * \fn void Badcom(void)
 * \brief Fonction qui est appelé quand aucunes commandes n'est reconnues.
 */
void Badcom(){
	const uint8_t not_found[] = "\r\nCommand not found"; //contenant le message du commande non reconnue

	HAL_UART_Transmit(&huart2, not_found, sizeof(not_found), HAL_MAX_DELAY);
	affprompt();
}

/**
 * \fn void Comhelp(void)
 * \brief Fonction qui affiche toutes les commandes connue.
 */
void Comhelp(){
	const uint8_t help[] = "\r\nCommandes disponible\r\nhelp : affiche toutes les commandes disponibles\r\npinout : affiche toutes les broches connectees et leur fonction\r\nstart : allume l etage de puissance du moteur\r\nstop : eteind l etage de puissance du moteur"; //contenant le message d'aide
	HAL_UART_Transmit(&huart2, help, sizeof(help), HAL_MAX_DELAY);
	affprompt();
}

/**
 * \fn void Pinlist(void)
 * \brief Fonction qui affiche la liste des pin branchés et leurs fonctions.
 */
void Pinlist(){
	const uint8_t pinout[] = "\r\nliste des pin a remplir"; //contenant la liste des pin utilisées
	HAL_UART_Transmit(&huart2, pinout, sizeof(pinout), HAL_MAX_DELAY);
	affprompt();
}

/**
 * \fn void Speedchange(void)
 * \brief Fonction qui change la vitesse du moteur.
 */
void Speedchange(char cmd[]){
	const char sep[] = " ";
	char *newspeed;
	int i = 0;
	int speed;
	int maxspeed = 1500;
	double alpha;

	for(i=0;i<3;i++){
		newspeed = strtok(cmd,sep);
	}
	speed = atoi(newspeed);
	if(speed>maxspeed){speed = maxspeed;}
	alpha = (double)speed/maxspeed;
	TIM1->CCR2 = alpha*1023;

}
