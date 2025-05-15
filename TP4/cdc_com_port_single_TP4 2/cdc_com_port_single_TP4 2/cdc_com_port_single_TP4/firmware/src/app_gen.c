/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app_gen.c

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

#include "app_gen.h"
#include "Mc32DriverLcd.h"
#include "Mc32gestI2cSeeprom.h"
#include "GesPec12.h"
#include "MenuGen.h"
#include "Generateur.h"
#include "Mc32gest_SerComm.h"
#include "app.h"
#include "Mc32gestSpiDac.h"

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

APP_GEN_DATA app_genData;

S_ParamGen LocalParamGen;
S_ParamGen RemoteParamGen;


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
    void APP_GEN_Initialize ( void )

  Remarks:
    See prototype in app_gen.h.
 */

void APP_GEN_Initialize(void) {
    /* Place the App state machine in its initial state. */
    app_genData.state = APP_GEN_STATE_INIT;
    app_genData.strRxReceived = false;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void APP_GEN_Tasks ( void )

  Remarks:
    See prototype in app_gen.h.
 */

void APP_GEN_Tasks(void) {

    /* Check the application's current state. */
    switch (app_genData.state) {
            /* Application's initial state. */
        case APP_GEN_STATE_INIT:
        {

            //Initialisation du LCD
            lcd_init();
            //Allumage de la backlight du LCD
            lcd_bl_on();
            lcd_gotoxy(1, 1); // Positionne le curseur � la premi�re ligne
            printf_lcd("TP4 USBGen 24/25");
            lcd_gotoxy(1, 2); // Positionne le curseur � la deuxi�me ligne
            printf_lcd("Etienne");
            lcd_gotoxy(1, 3); // Positionne le curseur � la troisi�me ligne
            printf_lcd("Vitor");
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

            APP_Gen_UpdateState(APP_GEN_STATE_WAIT);
            break;
        }


        case APP_GEN_STATE_SERVICE_TASKS:
        {
            //Toggle de la LED, afin de voir le temps d'interruption
            BSP_LEDToggle(BSP_LED_2);

            //Test si l'USB est branch�
            if (etatUSB) {
                if (app_genData.strRxReceived == true) //Si une trame est re�u, allumer le back light
                {
                    lcd_bl_on();
                } else if (Pec12.InactivityDuration >= INACTIVITYDURATIONMAX) //Si le Pec12 n'est pas touch� pendant 5 secondes, �teindre le back light
                {
                    lcd_bl_off();
                }

                if (FLAG_LCD) //Clear les lignes lors de la reception d'un message
                {
                    LCD_ClearScreen();
                }

                FLAG_LCD = false; //Mise du FLAG_LCD � z�ro pour ne pas clear les lignes si aucun message n'est re�u 

                MENU_Execute(&RemoteParamGen, false);
                GENSIG_UpdateSignal(&RemoteParamGen);
                GENSIG_UpdatePeriode(&RemoteParamGen);

                app_genData.strRxReceived = false;
            } else // Si l'USB n'est pas branch�, mettre � jour les signaux avec les param�tres locaux
            {
                MENU_Execute(&LocalParamGen, true);
                GENSIG_UpdateSignal(&LocalParamGen);
                GENSIG_UpdatePeriode(&LocalParamGen);
            }

            APP_Gen_UpdateState(APP_GEN_STATE_WAIT);
            break;
        }

            /* TODO: implement your application state machine.*/
        case APP_GEN_STATE_WAIT:
        {
            break;
        }

            /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}



/*******************************************************************************
 End of File
 */

// *****************************************************************************
// Fonction :
//    void APP_GEN_SaveNewStr(uint8_t* str, uint8_t strLen)
//
// R�sum� :
//    Sauvegarde une nouvelle cha�ne de caract�res re�ue.
//
// Description :
//    Cette fonction marque la r�ception d'une nouvelle cha�ne de caract�res,
//    copie cette cha�ne dans le buffer `app_genData.strRx` et s'assure qu'elle
//    est termin�e par un caract�re nul ('\0'). Si la longueur de la cha�ne
//    d�passe 31 caract�res, elle est bloqu�e � 31.
//
// Param�tres :
//    - str : Pointeur vers la cha�ne de caract�res � sauvegarder.
//    - strLen : Longueur de la cha�ne de caract�res.
//
// Retourne :
//    - Rien (void).
// *****************************************************************************

void APP_GEN_SaveNewStr(uint8_t* str, uint8_t strLen) {
    app_genData.strRxReceived = true; // Marque la r�ception d'une nouvelle cha�ne

    // Si la longueur de la cha�ne d�passe 31, elle est tronqu�e �  31
    if (strLen > 31)
        strLen = 31;

    // Copie la cha�ne dans le buffer `app_genData.strRx`
    memcpy(app_genData.strRx, str, strLen);

    // Termine la cha�ne par un caract�re nul ('\0')
    app_genData.strRx[strLen] = 0;
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

void APP_Gen_UpdateState(APP_GEN_STATES NewState) {
    // Met � jour l'�tat de l'application avec la nouvelle valeur
    app_genData.state = NewState;
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
        comptAffichageSauvegarde = 0;
        usbStatSave = false;
    }
}
//clear LCD

void LCD_ClearScreen() {
    lcd_ClearLine(1);
    lcd_ClearLine(2);
    lcd_ClearLine(3);
    lcd_ClearLine(4);
}