/* 
 * File:   fonctions.h
 * Author: Etienne De Oliveira
 * 
 * Description : Ce fichier contient les prototypes de mes fonctions.
 * 
 */
#ifndef _FONCTION_H    /* Guard against multiple inclusion */
#define _FONCTION_H

#ifdef __cplusplus
extern "C" {
#endif

#define HUIT 8

//Prototype des fonctions
/*Fonction initialisation de la carte 
 Cette fonction initialise le LCD et l'ADC */
void Init(void);

/*Fonction Allumer les Leds
 Cette fonction allume toutes les Leds */
void LedOn(void);

/*Fonction Eteindre Leds
 Cette fonction éteint toutes les Leds */
void LedOff(void);

/*Fonction Chenillard
 Cette fonction fait le chenillard des 8 Leds */
void Chenillard(void);

/*Fonction ADC 
 Cette fonction lis l'ADC, pour obtenir la valeur des potentiomètres.
 Ensuite on affiche la valeur sur le LCD. */
void AdcPot(void);

#ifdef __cplusplus
}
#endif

#endif 
