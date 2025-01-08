/* 
 * File:   fonctions.c
 * Author: Etienne De Oliveira
 * 
 * Description : Ce fichier contient les déclarations de mes fonctions.
 * 
 */
#include "fonction.h"
#include "Mc32DriverLcd.h"
#include "Mc32DriverAdc.h"
#include "app.h"
#include "bsp.h"

/*Fonction Eteindre Leds
 Cette fonction éteint toutes les Leds
 */
//Leds
void LedOff(void)
{
    LED0_W = UN;
    LED1_W = UN;
    LED2_W = UN;
    LED3_W = UN;
    LED4_W = UN;
    LED5_W = UN;
    LED6_W = UN;
    LED7_W = UN;
}
/*Fonction Allumer les Leds
 Cette fonction allume toutes les Leds
 */
void LedOn(void)
{
    LED0_W = ZERO;
    LED1_W = ZERO;
    LED2_W = ZERO;
    LED3_W = ZERO;
    LED4_W = ZERO;
    LED5_W = ZERO;
    LED6_W = ZERO;
    LED7_W = ZERO;
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
    printf_lcd("De Oliveira");
    //init ADC
    BSP_InitADC10();
    //Leds
    LedOff();
}