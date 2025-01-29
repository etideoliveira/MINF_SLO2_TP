/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2024-2025
//
//	Auteur 		: 	Etienne De Oliveira
//                  Tassilo Choulat
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.5 + Harmony 5.5
//
/*--------------------------------------------------------*/

#include "GestPWM.h"
#include "app.h"
#include "Mc32DriverLcd.h"
#include "Mc32DriverAdc.h"
#include "C:\microchip\harmony\v2_06\framework\peripheral\oc\processor\oc_p32mx795f512l.h"

// Fonction d'initialisation du PWM
void GPWM_Initialize(S_pwmSettings *pData) {
    // Initialisation des param�tres de vitesse et d'angle � z�ro
    pData->absSpeed = ZERO; // Vitesse absolue
    pData->absAngle = ZERO; // Angle absolu
    pData->SpeedSetting = ZERO; // R�glage de la vitesse
    pData->AngleSetting = ZERO; // R�glage de l'angle

   // Activation du pont en H pour contr�ler le moteur
    BSP_EnableHbrige();

   // D�marrage des timers et des sorties de comparaison pour le PWM
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
    DRV_OC0_Start();
    DRV_OC1_Start();
}

// Fonction pour obtenir les r�glages de vitesse et d'angle
void GPWM_GetSettings(S_pwmSettings *pData) {
     // D�claration de variables pour le canal 0
    static uint16_t adcValues0[10];    // Tableau pour stocker les valeurs ADC du canal 0
    static uint8_t index0 = ZERO;      // Index pour le tableau du canal 0
    static uint16_t sum0 = ZERO;       // Somme des valeurs pour calculer la moyenne du canal 0
    static uint16_t lastValue0 = ZERO; // Derni�re valeur lue du canal 0

    // D�claration de variables pour le canal 1
    static uint16_t adcValues1[10];    // Tableau pour stocker les valeurs ADC du canal 1
    static uint8_t index1 = ZERO;      // Index pour le tableau du canal 1
    static uint16_t sum1 = ZERO;       // Somme des valeurs pour calculer la moyenne du canal 1
    static uint16_t lastValue1 = ZERO; // Derni�re valeur lue du canal 1

    // Lecture des valeurs du convertisseur analogique-num�rique (ADC)
    S_ADCResults adcResults = BSP_ReadAllADC();   // R�cup�ration des valeurs ADC
    lastValue0 = adcResults.Chan0;       // Stockage de la derni�re valeur du canal 0
    sum0 -= adcValues0[index0];          // Soustraction de la valeur pr�c�dente du tableau
    adcValues0[index0] = lastValue0;     // Stockage de la nouvelle valeur dans le tableau
    sum0 += adcValues0[index0];          // Ajout de la nouvelle valeur � la somme
    index0 = (index0 + UN) % DIX;        // Mise � jour de l'index pour le tableau
    uint16_t averageValue0 = sum0 / DIX; // Calcul de la moyenne des valeurs du canal 0

    // Mise � l'�chelle de la valeur pour obtenir une plage de 0 � 198
    uint16_t scaledValue0 = (averageValue0 * ORDONEEPRG) / MAXVALAD;

    // Calcul de la vitesse sign�e variant de -99 � 99
    pData->SpeedSetting = scaledValue0 - OFFSETORIG;
    pData->absSpeed = abs(pData->SpeedSetting); // Vitesse absolue [0, 99]
    
    // Traitement pour le canal 1
    lastValue1 = adcResults.Chan1;       // Stockage de la derni�re valeur du canal 1
    sum1 -= adcValues1[index1];          // Soustraction de la valeur pr�c�dente du tableau
    adcValues1[index1] = lastValue1;     // Stockage de la nouvelle valeur dans le tableau
    sum1 += adcValues1[index1];          // Ajout de la nouvelle valeur � la somme
    index1 = (index1 + UN) % DIX;        // Mise � jour de l'index pour le tableau
    uint16_t averageValue1 = sum1 / DIX; // Calcul de la moyenne des valeurs du canal 1

    // Calcul de l'angle absolu variant de 0 � 180
    pData->absAngle = (averageValue1 * ANGLE_ABS) / MAXVALAD;

    // Calcul de l'angle sign� variant de -90 � 90
    pData->AngleSetting = pData->absAngle - MAXANGLE;
}

// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData, int Remote) {
    lcd_gotoxy(1,1);
    if(Remote == 1)
    {
        printf_lcd("**Remote Settings");   
    }
    else
    {
        printf_lcd("**Local Settings     ");
    }
    lcd_gotoxy(1,2);
    printf_lcd("SpeedSetting%4d", pData->SpeedSetting);
    lcd_gotoxy(1,3);
    printf_lcd("absSpeed %2d", pData->absSpeed);
    lcd_gotoxy(1,4);
    printf_lcd("Angle%4d", pData->AngleSetting);
}

// Execution PWM et gestion moteur � partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData) {
    static uint16_t PulseWidthOC2; // Largeur d'impulsion pour OC2 (vitesse)
    static uint16_t PulseWidthOC3; // Largeur d'impulsion pour OC3 (angle)

    // Contr�le du pont en H en fonction de la vitesse
    STBY_HBRIDGE_W = UN;
   // Gestion de la direction du pont en H
    if (pData->SpeedSetting > ZERO) 
    {
        AIN1_HBRIDGE_W = UN;
        AIN2_HBRIDGE_W = ZERO;
        // Avancer
    } 
    else if (pData->SpeedSetting < ZERO) 
    {
        AIN1_HBRIDGE_W = ZERO;
        AIN2_HBRIDGE_W = UN;
         // Reculer
    } 
    else 
    {
        AIN1_HBRIDGE_W, AIN2_HBRIDGE_W = ZERO;
         // Arr�ter
    }

    // Calcul largeur d'impulsion pour OC2 (vitesse)
    PulseWidthOC2 = (( pData->absSpeed * MAXVALAD) / OFFSETORIG)*DEUX;
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, PulseWidthOC2);

    // Calcul largeur d'impulsion pour OC3 (angle)
    PulseWidthOC3 = ( (pData->AngleSetting + MAXANGLE) *(MAX_PWM_WIDTH - MIN_PWM_WIDTH)/ ANGLE_ABS)+ MIN_PWM_WIDTH;
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, PulseWidthOC3);
}
