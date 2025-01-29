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
    int i = 0;
    for (i = 0; i < NOMBRE_LED; i++) {
        BSP_LEDStateSet(i, UN); // Éteindre les LEDs
    }
}
/*Fonction Allumer les Leds
 Cette fonction allume toutes les Leds
 */
void LedOn(void)
{
    int i = 0;
    for (i = 0; i < NOMBRE_LED; i++) {
        BSP_LEDStateSet(i, ZERO); // Allumer les LEDs
    }
}

/*Fonction initialisation de la carte 
 Cette fonction initialise le LCD et l'ADC */
void Init(void)
{
    lcd_init();
    lcd_bl_on();
    lcd_gotoxy(1,1);
    printf_lcd("TP2 PWM&RS232 24-25");
    lcd_gotoxy(1,2);
    printf_lcd("De Oliveira");
    lcd_gotoxy(1,3);
    printf_lcd("Choulat");
    //init ADC
    BSP_InitADC10();
    //Leds
    LedOff();
}