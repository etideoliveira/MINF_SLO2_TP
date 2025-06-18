/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    appgen.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "appgen.h"
#include "Mc32DriverLcd.h"
#include "Mc32gestI2cSeeprom.h"
#include "GesPec12.h"
#include "MenuGen.h"
#include "Generateur.h"
#include "Mc32gest_SerComm.h"
#include "app.h"
#include "Mc32gestSpiDac.h"
#include "driver/tmr/drv_tmr_static.h" 

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
 */

APPGEN_DATA appgenData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
 */


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APPGEN_Initialize ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appgenData.state = APPGEN_STATE_INIT;


    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void APPGEN_Tasks ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Tasks(void) {
    static uint16_t attente = 0;


    /* Check the application's current state. */
    switch (appgenData.state) {
            /* Application's initial state. */
        case APPGEN_STATE_INIT:
        {
            // CHR ajout init drivers timers statiques
            DRV_TMR0_Initialize();
            DRV_TMR1_Initialize();
            //Initialisation du LCD
            lcd_init();
            //Allumage de la backlight du LCD
            lcd_bl_on();
            lcd_gotoxy(1, 1); // Positionne le curseur � la premi�re ligne
            printf_lcd("TP5 IpGen 24/25");
            lcd_gotoxy(1, 2); // Positionne le curseur � la deuxi�me ligne
            printf_lcd("Etienne");
            lcd_gotoxy(1, 3); // Positionne le curseur � la troisi�me ligne
            printf_lcd("Nicolas");
            //Initialisation du bus I2C
            I2C_InitMCP79411();
            // Init SPI DAC
            SPI_InitLTC2604();
            //Appel de la fonction d'initialisation du PEC12
            Pec12Init();
            //Appel de la fonction d'initialisation de gensig
            GENSIG_Initialize(&LocalParamGen);
            //Appel de la fonction changeant l'amplitude la forme et l'offset du signal
            GENSIG_UpdateSignal(&LocalParamGen);
            //Appel de la fonction changeant la fr�quence du signal
            GENSIG_UpdatePeriode(&LocalParamGen);
            //D�marrage des timers
            DRV_TMR0_Start();
            DRV_TMR1_Start();

            RemoteParamGen = LocalParamGen;
            APP_Gen_UpdateState(APPGEN_STATE_WAIT);

            break;
        }

        case APPGEN_STATE_SERVICE_TASKS:
        {
            if (appgenData.newIp) {
                attente++;

                lcd_gotoxy(6, 2);
                printf_lcd("Adresse IP");
                lcd_gotoxy(4, 3);
                lcd_put_string_ram(appgenData.str); // Affichage de l'adresse IP
                lcd_bl_on();

                if (attente >= COUNTER) {
                    appgenData.newIp = false;
                    attente = 0;
                    MENU_Initialize(&LocalParamGen);
                }

            } else {
                BSP_LEDToggle(BSP_LED_2);

                //Test si session open Putty
                if (etatIP) {

                    lcd_bl_on();
                    MENU_Execute(&RemoteParamGen, false);
                    GENSIG_UpdateSignal(&RemoteParamGen);
                    GENSIG_UpdatePeriode(&RemoteParamGen);


                } else //mettre � jour les signaux avec les param�tres locaux
                {
                    MENU_Execute(&LocalParamGen, true);
                    GENSIG_UpdateSignal(&LocalParamGen);
                    GENSIG_UpdatePeriode(&LocalParamGen);
                }
            }

            APP_Gen_UpdateState(APPGEN_STATE_WAIT);
            break;
        }

        case APPGEN_STATE_WAIT:
        {

            // nothing to do
            break;
        }
            /* TODO: implement your application state machine.*/


            /* The default state should never be executed. */
        default:
        {

            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


// *****************************************************************************
// Fonction :
//    void APP_Gen_UpdateState(APP_GEN_STATES NewState)
//
// R�sum� :
//    Met � jour l'�tat de l'application avec une nouvelle valeur.
//
// Description :
//    Cette fonction met � jour l'�tat de l'application avec la nouvelle valeur
//    fournie. La mise � jour est effectu�e directement sur la variable d'�tat
//    globale `app_genData.state`.
//
// Param�tres :
//    - NewState : Nouvelle valeur de l'�tat de l'application.
//
// Retourne :
//    - Rien (void).
// *****************************************************************************

void APP_Gen_UpdateState(APPGEN_STATES NewState) {
    // Met � jour l'�tat de l'application avec la nouvelle valeur

    appgenData.state = NewState;
}

//clear LCD

void LCD_ClearScreen() {

    lcd_ClearLine(1);
    lcd_ClearLine(2);
    lcd_ClearLine(3);
    lcd_ClearLine(4);

}
// *****************************************************************************
// Fonction :
//    void MENU_DemandeSave(void)
//
// R�sum� :
//    Affiche un message de confirmation de sauvegarde.
//
// Description :
//    Cette fonction affiche un message de confirmation de sauvegarde sur un �cran LCD.
//    Lors du premier appel, elle efface plusieurs lignes de l'�cran. Ensuite, elle
//    affiche "Sauvegarde OK" pendant un certain nombre d'appels avant de r�initialiser
//    l'�tat de sauvegarde.
//
// Param�tres :
//    - Aucun.
//
// Retourne :
//    - Rien (void).
// *****************************************************************************

void MENU_DemandeSave(void) {
    static bool compteurPremierPassage = true; // Indique si c'est le premier passage dans la fonction
    static uint8_t comptAffichageSauvegarde = 0; // Compteur pour l'affichage du message de sauvegarde

    if (compteurPremierPassage) {
        LCD_ClearScreen();
        compteurPremierPassage = false; // Marque la fin du premier passage
    } else if (comptAffichageSauvegarde < 100) {
        // Affiche "Sauvegarde OK" � une position sp�cifique sur l'�cran LCD
        lcd_gotoxy(4, 2);
        printf_lcd("Sauvegarde OK");
        comptAffichageSauvegarde++; // Incr�mente le compteur d'affichage
    } else {
        // R�initialise le compteur d'affichage et l'�tat de sauvegarde
        MENU_Initialize(&RemoteParamGen);
        comptAffichageSauvegarde = 0;
        ipSave = false;
    }
}

void APPGEN_DispNewAddress(IPV4_ADDR ipAddr) {
    appgenData.newIp = true;
    LCD_ClearScreen();
    {
        sprintf(appgenData.str, "%d.%d.%d.%d", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]); // Formate l'adresse IP en cha�ne de caract�res et la stocke dans appgenData.str
    }
}
/*******************************************************************************
 End of File
 */
