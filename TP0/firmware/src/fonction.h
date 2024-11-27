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
    #include "app.h"
//Prototype des fonctions
void Init(void);
void LedOn(void);
void LedOff(void);
void Chenillard(void);
void AdcPot(void);

#ifdef __cplusplus
}
#endif

#endif 