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

APP_DATA appData;
S_pwmSettings PWMData;      // pour les settings

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
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
    // Lecture du convertisseur AD
    appData.AdcRes = BSP_ReadAllADC();
    // conversion
    
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    
}


