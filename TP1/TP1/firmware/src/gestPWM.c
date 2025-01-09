/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2024-2025
//
//	Auteur 		: 	Etienne De Oliveira
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.5 + Harmony 5.5
//
/*--------------------------------------------------------*/



#include "gestPWM.h"
#include "system_config/default/framework/driver/tmr/drv_tmr_static.h"
#include "Mc32DriverAdc.h"
#include "app.h"
#include "Mc32DriverLcd.h"
#include "C:\microchip\harmony\v2_06\framework\peripheral\oc\processor\oc_p32mx795f512l.h"
#include "driver/oc/drv_oc_static.h"


APP_DATA appData; // D�claration d'une structure pour stocker les donn�es de l'application
S_pwmSettings PWMData; // D�claration d'une structure pour stocker les param�tres PWM

// Fonction d'initialisation du PWM
void GPWM_Initialize(S_pwmSettings *pData)
{
   // Initialisation des param�tres de vitesse et d'angle � z�ro
    PWMData.absSpeed = ZERO; // Vitesse absolue
    PWMData.absAngle = ZERO; // Angle absolu
    PWMData.SpeedSetting = ZERO; // R�glage de la vitesse
    PWMData.AngleSetting = ZERO; // R�glage de l'angle

   // Activation du pont en H pour contr�ler le moteur
    BSP_EnableHbrige();

   // D�marrage des timers et des sorties de comparaison pour le PWM
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
    DRV_TMR3_Start();
    DRV_OC0_Start();
    DRV_OC1_Start();
}

// Fonction pour obtenir les r�glages de vitesse et d'angle
void GPWM_GetSettings(S_pwmSettings *pData)	
{
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
    appData.AdcRes = BSP_ReadAllADC();   // R�cup�ration des valeurs ADC
    lastValue0 = appData.AdcRes.Chan0;   // Stockage de la derni�re valeur du canal 0
    sum0 -= adcValues0[index0];          // Soustraction de la valeur pr�c�dente du tableau
    adcValues0[index0] = lastValue0;     // Stockage de la nouvelle valeur dans le tableau
    sum0 += adcValues0[index0];          // Ajout de la nouvelle valeur � la somme
    index0 = (index0 + UN) % DIX;        // Mise � jour de l'index pour le tableau
    uint16_t averageValue0 = sum0 / DIX; // Calcul de la moyenne des valeurs du canal 0

    // Mise � l'�chelle de la valeur pour obtenir une plage de 0 � 198
    uint16_t scaledValue0 = (averageValue0 * ORDONEEPRG) / MAXVALAD;

    // Calcul de la vitesse sign�e variant de -99 � 99
    PWMData.SpeedSetting = scaledValue0 - OFFSETORIG;

    // Calcul de la vitesse absolue variant de 0 � +99
    if (scaledValue0 >= OFFSETORIG) 
    {
        PWMData.absSpeed = PWMData.SpeedSetting; // Vitesse positive
    } 
    else 
    {
        PWMData.absSpeed = abs(PWMData.SpeedSetting); // Vitesse n�gative
    }
    
    // Traitement pour le canal 1
    lastValue1 = appData.AdcRes.Chan1;   // Stockage de la derni�re valeur du canal 1
    sum1 -= adcValues1[index1];          // Soustraction de la valeur pr�c�dente du tableau
    adcValues1[index1] = lastValue1;     // Stockage de la nouvelle valeur dans le tableau
    sum1 += adcValues1[index1];          // Ajout de la nouvelle valeur � la somme
    index1 = (index1 + UN) % DIX;        // Mise � jour de l'index pour le tableau
    uint16_t averageValue1 = sum1 / DIX; // Calcul de la moyenne des valeurs du canal 1

    // Calcul de l'angle absolu variant de 0 � 180
    PWMData.absAngle = (averageValue1 * ANGLE_ABS) / MAXVALAD;

    // Calcul de l'angle sign� variant de -90 � 90
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

// Execution PWM et gestion moteur � partir des info dans structure
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
         // Arr�ter
    }

    // Calcul de la valeur du nombre d'impulsions pour OC2
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, ((PWMData.absSpeed * MAXVALAD) / OFFSETORIG)*DEUX); // Calculer le rapport cyclique &Appliquer la largeur d'impulsion � OC2
    
    // Calcul de la valeur du nombre d'impulsions pour OC3
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, ((PWMData.absAngle + MAXANGLE) * MAXVALAD / ANGLE_ABS)*DEUX); //Convertir l'angle sign� en largeur d'impulsion & Appliquer la largeur d'impulsion � OC3
   
} 

// Fonction pour ex�cuter le PWM en mode logiciel
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    static uint8_t pwmCounter = ZERO; // Compteur pour suivre le cycle PWM

    // V�rification si le compteur a atteint la p�riode d�finie pour le PWM
    if (pwmCounter < PWM_PERIOD_CYCLES) {
        // Comparaison du compteur avec la vitesse absolue
        if (pwmCounter < PWMData.absSpeed) {
            BSP_LEDOn(BSP_LED_2); // Allumer la LED2 si le compteur est inf�rieur � la vitesse
        } else {
            BSP_LEDOff(BSP_LED_2); // �teindre la LED2 sinon
        }
        // Incr�mentation du compteur pour le prochain cycle
        pwmCounter++;
    } else {
        // R�initialisation du compteur � z�ro � la fin de la p�riode PWM
        pwmCounter = ZERO;
    }
}


