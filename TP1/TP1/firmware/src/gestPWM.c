/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2016-2017
//
//	Auteur 		: 	C. HUBER
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
/*--------------------------------------------------------*/



#include "gestPWM.h"
#include "system_config/default/framework/driver/tmr/drv_tmr_static.h"
#include "Mc32DriverAdc.h"
#include "app.h"
#include "Mc32DriverLcd.h"
#include "C:\microchip\harmony\v2_06\framework\peripheral\oc\processor\oc_p32mx795f512l.h"


APP_DATA appData;
S_pwmSettings PWMData;      // pour les settings

// Variables globales pour le canal 0
uint16_t adcValues0[10]; // Tableau pour stocker les valeurs ADC du canal 0
uint8_t index0 = 0; // Index pour le tableau du canal 0
uint16_t sum0 = 0; // Somme des valeurs pour la moyenne du canal 0
uint16_t lastValue0 = 0; // Dernière valeur lue du canal 0

// Variables globales pour le canal 1
uint16_t adcValues1[10]; // Tableau pour stocker les valeurs ADC du canal 1
uint8_t index1 = 0; // Index pour le tableau du canal 1
uint16_t sum1 = 0; // Somme des valeurs pour la moyenne du canal 1
uint16_t lastValue1 = 0; // Dernière valeur lue du canal 1

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    PWMData.absSpeed = 0;
    PWMData.absAngle = 0;
    PWMData.SpeedSetting = 0;
    PWMData.AngleSetting = 0;
   // Init état du pont en H
    BSP_EnableHbrige();
   // lance les timers et OC
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
    DRV_TMR3_Start();
    DRV_OC0_Start();
    DRV_OC1_Start();
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
    // Lecture du convertisseur AD
    appData.AdcRes = BSP_ReadAllADC();
    lastValue0 = appData.AdcRes.Chan0;
    sum0 -= adcValues0[index0];
    adcValues0[index0] = lastValue0;
    sum0 += adcValues0[index0];
    index0 = (index0 + 1) % 10;
    uint16_t averageValue0 = sum0 / 10;

    // Calculer la valeur variant de 0 à 198
    uint16_t scaledValue0 = (averageValue0 * 198) / 1023;

    // Calculer la vitesse signée variant de -99 à 99
    PWMData.SpeedSetting = scaledValue0 - 99;

    // Calculer la vitesse absolue variant de 0 à +99
    if (scaledValue0 >= 99) 
    {
        PWMData.absSpeed = PWMData.SpeedSetting ;
    } else 
    {
        PWMData.absSpeed = abs(PWMData.SpeedSetting);
    }

    // Traitement pour le 2ème AD (canal 1)
    lastValue1 = appData.AdcRes.Chan1;
    sum1 -= adcValues1[index1];
    adcValues1[index1] = lastValue1;
    sum1 += adcValues1[index1];
    index1 = (index1 + 1) % 10;
    uint16_t averageValue1 = sum1 / 10;

    // Calculer la valeur variant de 0 à 180
    PWMData.absAngle = (averageValue1 * 180) / 1023;

    // Calculer l'angle signé variant de -90 à 90
    PWMData.AngleSetting = PWMData.absAngle - 90;
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    lcd_ClearLine(1);
    lcd_ClearLine(2);
    lcd_ClearLine(4);
    lcd_gotoxy(1,1);
    printf_lcd("TP1 PWM 2024-2025");
    lcd_gotoxy(1,2);
    printf_lcd("SpeedSetting %2d", PWMData.SpeedSetting);
    lcd_gotoxy(1,3);
    printf_lcd("absSpeed %2d", PWMData.absSpeed);
    lcd_gotoxy(1,4);
    printf_lcd("Angle %2d", PWMData.AngleSetting);
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
   // Gestion de la direction du pont en H
    if (PWMData.SpeedSetting > 0) 
    {
        AIN1_HBRIDGE_R, STBY_HBRIDGE_R = 1;
        AIN2_HBRIDGE_R = 0;
        // Avancer
    } else if (PWMData.SpeedSetting < 0) 
    {
        AIN2_HBRIDGE_R, STBY_HBRIDGE_R = 1;
        AIN1_HBRIDGE_R = 0;
         // Reculer
    } else 
    {
        STBY_HBRIDGE_R = 1;
        AIN1_HBRIDGE_R, AIN2_HBRIDGE_R = 0;
         // Arrêter
    }

    // Calcul de la valeur du nombre d'impulsions pour OC2
    uint16_t oc2PulseWidth = (PWMData.absSpeed * 1023) / 99; // Calculer le rapport cyclique
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, oc2PulseWidth); // Appliquer la largeur d'impulsion à OC2
    //PLIB_OC_PulseWidth16BitSet(2,oc2PulseWidth);
    // Calcul de la valeur du nombre d'impulsions pour OC3
    uint16_t oc3PulseWidth = (PWMData.absAngle + 90) * 1023 / 180; // Convertir l'angle signé en largeur d'impulsion
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, oc3PulseWidth); // Appliquer la largeur d'impulsion à OC3
   // PLIB_OC_PulseWidth16BitSet(3,oc3PulseWidth);
} 

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    
}


