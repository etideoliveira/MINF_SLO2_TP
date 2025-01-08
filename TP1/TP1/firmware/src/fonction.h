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
    
#define ZERO 0
#define UN 1
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

#ifdef __cplusplus
}
#endif

#endif 