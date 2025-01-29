#ifndef GestPWM_H
#define GestPWM_H
/*--------------------------------------------------------*/
// GestPWM.h
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2016-2017
//
//	Auteur 		: 	C. HUBER
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
//  Modification : 1.12.2023 SCA : enleve decl. PWMData extern
//    
/*--------------------------------------------------------*/

#include <stdint.h> // Inclusion des d�finitions de types standards

/*--------------------------------------------------------*/
// D�finition des constantes
/*--------------------------------------------------------*/
#define ZERO 0
#define UN 1
#define DEUX 2
#define DIX 10
#define PWM_PERIOD_CYCLES 100 // 3.5 ms / 35 �s
#define ORDONEEPRG 198
#define OFFSETORIG 99
#define MAXVALAD 1023
#define DIVISION 100
#define ANGLE_ABS 180
#define MAXANGLE 90

// Limites pour le signal PWM (en microsecondes)
#define MIN_PWM_WIDTH 1000    // Largeur minimale (1000 �s pour un angle de -90�)
#define MAX_PWM_WIDTH 2750   // Largeur maximale (2750 �s pour un angle de +90�)

/*--------------------------------------------------------*/
// D�claration des types et structures
/*--------------------------------------------------------*/

// Structure contenant les param�tres PWM
typedef struct {
    uint8_t absSpeed;    // vitesse 0 � 99
    uint8_t absAngle;    // Angle  0 � 180
    int8_t SpeedSetting; // consigne vitesse -99 � +99
    int8_t AngleSetting; // consigne angle  -90 � +90
} S_pwmSettings;

extern S_pwmSettings PWMData, PWMDataToSend;
void GPWM_Initialize(S_pwmSettings *pData);

// Ces 4 fonctions ont pour param�tre un pointeur sur la structure S_pwmSettings.
void GPWM_GetSettings(S_pwmSettings *pData);	// Obtention vitesse et angle
void GPWM_DispSettings(S_pwmSettings *pData, int Remote);	// Affichage
void GPWM_ExecPWM(S_pwmSettings *pData);		// Execution PWM et gestion moteur.

#endif // GestPWM_H
