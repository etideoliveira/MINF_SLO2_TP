#include "Mc32DriverLcd.h"
#include "afficheLCD.h"
#include "app.h"

void AffichageINIT(void) {
    lcd_gotoxy(1, 1);
    printf_lcd("Local Setting");
    lcd_gotoxy(1, 2);
    printf_lcd("PWM&RS232 2025");
    lcd_gotoxy(1, 3);
    printf_lcd("EDO");
    lcd_gotoxy(1, 4);
    printf_lcd("TCT");
}
