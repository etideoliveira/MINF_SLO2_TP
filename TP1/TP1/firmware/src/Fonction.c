/* 
 * File:   fonctions.c
 * Author: Etienne De Oliveira
 * 
 * Description : Ce fichier contient les d�clarations de mes fonctions.
 * 
 */
#include "fonction.h"
#include "Mc32DriverLcd.h"
#include "Mc32DriverAdc.h"
#include "app.h"
#include "bsp.h"

/*Fonction Eteindre Leds
 Cette fonction �teint toutes les Leds
 */
//Leds
void LedOff(void)
{
    LED0_W = 1;
    LED1_W = 1;
    LED2_W = 1;
    LED3_W = 1;
    LED4_W = 1;
    LED5_W = 1;
    LED6_W = 1;
    LED7_W = 1;
}
/*Fonction Allumer les Leds
 Cette fonction allume toutes les Leds
 */
void LedOn(void)
{
    LED0_W = 0;
    LED1_W = 0;
    LED2_W = 0;
    LED3_W = 0;
    LED4_W = 0;
    LED5_W = 0;
    LED6_W = 0;
    LED7_W = 0;
}

/*Fonction initialisation de la carte 
 Cette fonction initialise le LCD et l'ADC */
void Init(void)
{
            lcd_init();
            lcd_bl_on();
            lcd_gotoxy(1,1);
            printf_lcd("TP1 PWM 2024-2025");
            lcd_gotoxy(1,2);
            printf_lcd("De Oliveira Etienne");
            //init ADC
            BSP_InitADC10();
            //Leds
            LedOff();;
}