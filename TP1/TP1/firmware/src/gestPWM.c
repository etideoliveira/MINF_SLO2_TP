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
#include "driver/oc/drv_oc_static.h"


APP_DATA appData;
S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    PWMData.absSpeed = ZERO;
    PWMData.absAngle = ZERO;
    PWMData.SpeedSetting = ZERO;
    PWMData.AngleSetting = ZERO;
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
    // Variables  pour le canal 0
    static uint16_t adcValues0[10]; // Tableau pour stocker les valeurs ADC du canal 0
    static uint8_t index0 = ZERO; // Index pour le tableau du canal 0
    static uint16_t sum0 = ZERO; // Somme des valeurs pour la moyenne du canal 0
    static uint16_t lastValue0 = ZERO; // Dernière valeur lue du canal 0
    // Variables  pour le canal 1
    static uint16_t adcValues1[10]; // Tableau pour stocker les valeurs ADC du canal 1
    static uint8_t index1 = ZERO; // Index pour le tableau du canal 1
    static uint16_t sum1 = ZERO; // Somme des valeurs pour la moyenne du canal 1
    static uint16_t lastValue1 = ZERO; // Dernière valeur lue du canal 1
    // Lecture du convertisseur AD
    appData.AdcRes = BSP_ReadAllADC();
    lastValue0 = appData.AdcRes.Chan0;
    sum0 -= adcValues0[index0];
    adcValues0[index0] = lastValue0;
    sum0 += adcValues0[index0];
    index0 = (index0 + UN) % DIX;
    uint16_t averageValue0 = sum0 / DIX;

    // Calculer la valeur variant de 0 à 198
    uint16_t scaledValue0 = (averageValue0 * ORDONEEPRG) / MAXVALAD;

    // Calculer la vitesse signée variant de -99 à 99
    PWMData.SpeedSetting = scaledValue0 - OFFSETORIG;

    
    // Calculer la vitesse absolue variant de 0 à +99
    if (scaledValue0 >= OFFSETORIG) 
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
    index1 = (index1 + UN) % DIX;
    uint16_t averageValue1 = sum1 / DIX;

    // Calculer la valeur variant de 0 à 180
    PWMData.absAngle = (averageValue1 * ANGLE_ABS) / MAXVALAD;

    // Calculer l'angle signé variant de -90 à 90
    PWMData.AngleSetting = PWMData.absAngle - MAXANGLE;
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    lcd_gotoxy(1,1);
    printf_lcd("TP1 PWM 2024-2025");
    lcd_gotoxy(1,2);
    printf_lcd("SpeedSetting%4d", PWMData.SpeedSetting);
    lcd_gotoxy(1,3);
    printf_lcd("absSpeed %2d", PWMData.absSpeed);
    lcd_gotoxy(1,4);
    printf_lcd("Angle%4d", PWMData.AngleSetting);
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    STBY_HBRIDGE_W = UN;
   // Gestion de la direction du pont en H
    if (PWMData.SpeedSetting > ZERO) 
    {
        AIN1_HBRIDGE_W = UN;
        AIN2_HBRIDGE_W = ZERO;
        // Avancer
    } 
    else if (PWMData.SpeedSetting < ZERO) 
    {
        AIN1_HBRIDGE_W = ZERO;
        AIN2_HBRIDGE_W = UN;
         // Reculer
    } 
    else 
    {
        AIN1_HBRIDGE_W, AIN2_HBRIDGE_W = ZERO;
         // Arrêter
    }

    // Calcul de la valeur du nombre d'impulsions pour OC2
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, ((PWMData.absSpeed * MAXVALAD) / OFFSETORIG)*DEUX); // Calculer le rapport cyclique &Appliquer la largeur d'impulsion à OC2
    
    // Calcul de la valeur du nombre d'impulsions pour OC3
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, ((PWMData.absAngle + MAXANGLE) * MAXVALAD / ANGLE_ABS)*DEUX); //Convertir l'angle signé en largeur d'impulsion & Appliquer la largeur d'impulsion à OC3
   
} 

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    static uint8_t pwmCounter = ZERO; // Compteur pour le PWM
    // Vérifier si le compteur a atteint la période PWM
    if (pwmCounter < PWM_PERIOD_CYCLES) {
        // Comparer le compteur avec la valeur de absSpeed
        if (pwmCounter < PWMData.absSpeed) {
            BSP_LEDOn(BSP_LED_2);// Allumer LED2
        } else {
            BSP_LEDOff(BSP_LED_2);// Éteindre LED2
        }
        // Incrémenter le compteur
        pwmCounter++;
    } else {
        // Réinitialiser le compteur à la fin de la période
        pwmCounter = ZERO;
    }
}


