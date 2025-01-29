#include "led.h"
#include "bsp.h"

// Allumer toutes les LEDs
void AllumerToutesLesLEDs(void) {
    int i = 0;
    for (i = 0; i < NOMBRE_LED; i++) {
        BSP_LEDStateSet(i, LED_ALLUMEE); // Allumer les LEDs
    }
}

// Éteindre toutes les LEDs
void EteindreToutesLesLEDs(void) {
    int i = 0;
    for (i = 0; i < NOMBRE_LED; i++) {
        BSP_LEDStateSet(i, LED_ETEINTE); // Éteindre les LEDs
    }
}
