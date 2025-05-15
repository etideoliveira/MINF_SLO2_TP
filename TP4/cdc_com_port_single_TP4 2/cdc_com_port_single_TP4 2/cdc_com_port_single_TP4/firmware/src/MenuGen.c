// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du g�n�rateur
// Traitement cyclique � 10 ms

// Included Files 
#include <stdint.h>                   // Inclusion de la biblioth�que standard pour les types d'entiers (uint8_t, uint16_t, etc.)
#include <stdbool.h>                  // Inclusion de la biblioth�que standard pour le type bool�en (true, false)
#include "MenuGen.h"                  // Inclusion du fichier d'en-t�te pour la gestion du menu du g�n�rateur
#include "GesPec12.h"                 // Inclusion du fichier d'en-t�te pour la gestion du PEC12 (encodeur rotatif)
#include "Mc32DriverLcd.h"            // Inclusion du fichier d'en-t�te pour le driver LCD (affichage)
#include "Generateur.h"               // Inclusion du fichier d'en-t�te pour le g�n�rateur de signaux
#include "bsp.h"                      // Inclusion du fichier d'en-t�te pour la plateforme de d�veloppement (Board Support Package)
#include "app.h"                      // Inclusion du fichier d'en-t�te pour l'application principale
#include "Mc32gestI2cSeeprom.h"
// D�claration d'une variable globale pour stocker l'�tat actuel du menu
MENU_STATES MENU_DATA; // MENU_DATA est une variable qui repr�sente l'�tat du menu (par exemple, forme, fr�quence, amplitude, etc.)

// Structure pour le traitement du PEC12
S_Pec12_Descriptor Pec12; // Pec12 est une structure qui contient les informations n�cessaires pour g�rer les entr�es du PEC12 (encodeur rotatif)

//----------------------------------------------------------------------------
//  MENU_Initialize
//  Initialise l'affichage du menu sur l'�cran LCD avec les param�tres
//  du g�n�rateur. Cette fonction est appel�e pour afficher les valeurs
//  actuelles de la forme, de la fr�quence, de l'amplitude et de l'offset.
//  
//  Param�tres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les param�tres
//                           du g�n�rateur de signaux.
//----------------------------------------------------------------------------

void MENU_Initialize(S_ParamGen *pParam) {
    LCD_ClearScreen(); // Efface l'�cran LCD pour pr�parer l'affichage
    lcd_gotoxy(1, 1); // Positionne le curseur � la premi�re ligne
    printf_lcd(" Forme : %10c", pParam->Forme); // Affiche la forme du signal
    lcd_gotoxy(1, 2); // Positionne le curseur � la deuxi�me ligne
    printf_lcd(" Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fr�quence en Hz
    lcd_gotoxy(1, 3); // Positionne le curseur � la troisi�me ligne
    printf_lcd(" Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude en mV
    lcd_gotoxy(1, 4); // Positionne le curseur � la quatri�me ligne
    printf_lcd(" Offset [mV] = %5d", pParam->Offset); // Affiche l'offset en mV
}

//----------------------------------------------------------------------------
//  MENU_Execute
//  Ex�cute le menu, permettant de naviguer et de modifier les param�tres
//  du g�n�rateur. Cette fonction est appel�e cycliquement depuis l'application.
//  
//  Param�tres :
//      S_ParamGen *pParam : Pointeur vers les param�tres du g�n�rateur.
//----------------------------------------------------------------------------

void MENU_Execute(S_ParamGen *pParam, bool local) {
    // Variables locales
    static int16_t Frequence_Selection = F_MIN; // S�lection de la fr�quence initialis�e � la valeur minimale
    static int16_t Amplitude_Selection = UN_VOLT_AMPLITUDE; // S�lection de l'amplitude initialis�e � 1 Volt
    static int16_t Offset_Selection = 0; // S�lection de l'offset initialis�e � 0
    static uint8_t CompteurForme = 0; // Compteur pour la forme du signal
    static bool firstTimeIn = 1; // Indicateur pour l'initialisation
    static int8_t isSaved = NOSAVEMODE; //Variable d�terminant si l'utilisateur est en train d'enregistrer les valeurs ou non


    if (local) {
        // Initialisation de l'affichage
        // Efface l'�cran LCD la premi�re fois
        if (firstTimeIn) {
            MENU_Initialize(pParam); // Appelle la fonction d'initialisation
            firstTimeIn = 0; // Met � jour l'indicateur pour �viter de r�initialiser � chaque appel
        }
        //Appel de la fonction de debounce pour le bouton de sauvegarde S9
        DoDebounce(&DescrS9, S_OK);

        //Est-ce que S9 a �t� press�?
        if (DebounceIsPressed(&DescrS9)) {
            //Nous alluomons la backlight du LCD au cas ou celle-ci serait �teinte (en cas d'inactivit�)
            lcd_bl_on();
            //Indication d'une activit� de l'utilisateur
            Pec12.NoActivity = false;
            Pec12.InactivityDuration = 0;
            //Est-ce que l'utilisateur est d�j� dans le mode de sauvegarde des donn�es?
            if (isSaved == NOSAVEMODE) {
                //Remise � z�ro du flag du bouton S9
                DebounceClearPressed(&DescrS9);
                //effacer l'affichage du LCD
                LCD_ClearScreen();
                //Affichage du mode de sauvegarde du syst�me 
                lcd_gotoxy(1, 2);
                printf_lcd("    Sauvegarde ?");
                lcd_gotoxy(1, 3);
                printf_lcd("    (appui long)");
                //Le programme passe en mode de sauvegarde des donn�es
                isSaved = SAVEMODE;
            }
        }
        if (isSaved == NOSAVEMODE) {

            switch (MENU_DATA) {
                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FORME:
                    // Gestion de l'affichage de la forme du signal
                    lcd_gotoxy(1, 1); // Positionne le curseur � la premi�re ligne
                    switch (pParam->Forme) {
                        case 0:
                            printf_lcd("*Forme : Sinus       "); // Affiche la forme sinus
                            break;
                        case 1:
                            printf_lcd("*Forme : Triangle    "); // Affiche la forme triangle
                            break;
                        case 2:
                            printf_lcd("*Forme : DentDeScie"); // Affiche la forme dent de scie
                            break;
                        case 3:
                            printf_lcd("*Forme : Carre      "); // Affiche la forme carr�
                            break;
                    }

                    // Gestion de la sortie de l'�tat
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_FREQUENCE; // Passe � l'�tat de fr�quence
                        lcd_gotoxy(1, 1);
                        printf_lcd(" "); // Efface la ligne
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_OFFSET; // Passe � l'�tat d'offset
                        lcd_gotoxy(1, 1);
                        printf_lcd(" "); // Efface la ligne
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_FORME_VALUE; // Passe � l'�tat de s�lection de forme
                        lcd_ClearLine(1); // Efface la ligne
                    }
                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FORME_VALUE:
                    // Gestion de l'affichage de la s�lection de forme
                    lcd_gotoxy(1, 1); // Positionne le curseur � la premi�re ligne
                    switch (CompteurForme) {
                        case 0:
                            printf_lcd("?Forme : Sinus       "); // Affiche la s�lection de forme
                            break;
                        case 1:
                            printf_lcd("?Forme : Triangle    "); // Affiche la s�lection de forme
                            break;
                        case 2:
                            printf_lcd("?Forme : DentDeScie"); // Affiche la s�lection de forme
                            break;
                        case 3:
                            printf_lcd("?Forme : Carre      "); // Affiche la s�lection de forme
                            break;
                    }

                    // Gestion de la s�lection de forme
                    if (Pec12IsMinus()) {
                        CompteurForme++; // Incr�mente le compteur de forme
                        if (CompteurForme == QUATRE) {
                            CompteurForme = 0; // Retourne au d�but
                        }
                    } else if (Pec12IsPlus()) {
                        if (CompteurForme == 0) {
                            CompteurForme = QUATRE; // Retourne � la fin
                        }
                        CompteurForme--; // D�cr�mente le compteur de forme
                    }

                    // Gestion de la sortie de l'�tat
                    if (Pec12IsOK()) {
                        // Validation de la valeur choisie
                        switch (CompteurForme) {
                            case 0:
                                pParam->Forme = Sinus; // D�finit la forme sinus
                                break;
                            case 1:
                                pParam->Forme = Triangle; // D�finit la forme triangle
                                break;
                            case 2:
                                pParam->Forme = DentDeScie; // D�finit la forme dent de scie
                                break;
                            case 3:
                                pParam->Forme = Carre; // D�finit la forme carr�
                                break;
                        }

                        // Envoie les nouvelles valeurs au g�n�rateur
                        GENSIG_UpdateSignal(pParam);
                        MENU_DATA = MENU_STATE_FORME; // Retourne au menu pr�c�dent
                        lcd_ClearLine(1); // Efface la ligne
                    } else if (Pec12IsESC()) {
                        // R�cup�re l'ancienne valeur
                        CompteurForme = pParam->Forme; // R�tablit l'ancienne forme
                        MENU_DATA = MENU_STATE_FORME; // Retourne au menu pr�c�dent
                        lcd_ClearLine(1); // Efface la ligne
                    }
                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FREQUENCE:
                    // Gestion de l'affichage de la fr�quence
                    lcd_gotoxy(1, 2); // Positionne le curseur � la deuxi�me ligne
                    printf_lcd("*Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fr�quence

                    // Gestion de la sortie de l'�tat
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Passe � l'�tat d'amplitude
                        lcd_gotoxy(1, 2);
                        printf_lcd(" Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fr�quence
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_FORME; // Passe � l'�tat de forme
                        lcd_ClearLine(1); // Efface la ligne
                        lcd_gotoxy(1, 2);
                        printf_lcd(" Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fr�quence
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_FREQUENCE_VALUE; // Passe � l'�tat de s�lection de fr�quence
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FREQUENCE_VALUE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 2); // Positionne le curseur � la deuxi�me ligne
                    printf_lcd("?Freq [Hz]   = %5d", Frequence_Selection); // Affiche la fr�quence s�lectionn�e

                    // Gestion de l'augmentation de la fr�quence
                    if (Pec12IsMinus()) {
                        Frequence_Selection += PAS_F; // Incr�mente la fr�quence par le pas d�fini
                        if (Frequence_Selection > F_MAX) { // V�rifie si la fr�quence d�passe la valeur maximale
                            Frequence_Selection = F_MIN; // Si oui, retourne � la valeur minimale
                        }
                    }// Gestion de la diminution de la fr�quence
                    else if (Pec12IsPlus()) {
                        Frequence_Selection -= PAS_F; // D�cr�mente la fr�quence par le pas d�fini
                        if (Frequence_Selection < F_MIN) { // V�rifie si la fr�quence est inf�rieure � la valeur minimale
                            Frequence_Selection = F_MAX; // Si oui, retourne � la valeur maximale
                        }
                    }

                    // Gestion de la sortie de l'�tat
                    // Si la s�lection est valid�e
                    if (Pec12IsOK()) {
                        // On valide la valeur choisie
                        pParam->Frequence = Frequence_Selection; // Met � jour la fr�quence dans les param�tres

                        // On envoie les nouvelles valeurs au g�n�rateur
                        GENSIG_UpdatePeriode(pParam); // Met � jour la p�riode d'�chantillonnage du g�n�rateur

                        // On retourne au menu pr�c�dent
                        MENU_DATA = MENU_STATE_FREQUENCE; // Retourne � l'�tat de fr�quence
                    }// Si la s�lection n'est pas valid�e
                    else if (Pec12IsESC()) {
                        // On reprend l'ancienne valeur
                        Frequence_Selection = pParam->Frequence; // R�tablit l'ancienne fr�quence

                        // On retourne au menu pr�c�dent
                        MENU_DATA = MENU_STATE_FREQUENCE; // Retourne � l'�tat de fr�quence
                    }

                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_AMPLITUDE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 3); // Positionne le curseur � la troisi�me ligne
                    printf_lcd("*Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude actuelle

                    // Gestion de la sortie de l'�tat
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_OFFSET; // Passe � l'�tat d'offset
                        lcd_gotoxy(1, 3); // Positionne le curseur � la troisi�me ligne
                        printf_lcd(" Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_FREQUENCE; // Passe � l'�tat de fr�quence
                        lcd_gotoxy(1, 3); // Positionne le curseur � la troisi�me ligne
                        printf_lcd(" Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_AMPLITUDE_VALUE; // Passe � l'�tat de s�lection d'amplitude
                    }
                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_AMPLITUDE_VALUE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 3); // Positionne le curseur � la troisi�me ligne
                    printf_lcd("?Ampl [mV]   = %5d", Amplitude_Selection); // Affiche la s�lection d'amplitude

                    // Gestion de l'augmentation de l'amplitude
                    if (Pec12IsMinus()) {
                        Amplitude_Selection += PAS_AMPL; // Incr�mente l'amplitude par le pas d�fini
                        if (Amplitude_Selection > MAX_AMPLITUDE) { // V�rifie si l'amplitude d�passe la valeur maximale
                            Amplitude_Selection = 0; // Si oui, retourne � 0
                        }
                    }// Gestion de la diminution de l'amplitude
                    else if (Pec12IsPlus()) {
                        Amplitude_Selection -= PAS_AMPL; // D�cr�mente l'amplitude par le pas d�fini
                        if (Amplitude_Selection < 0) { // V�rifie si l'amplitude est inf�rieure � 0
                            Amplitude_Selection = MAX_AMPLITUDE; // Si oui, retourne � la valeur maximale
                        }
                    }

                    // Gestion de la sortie de l'�tat
                    // Si la s�lection est valid�e
                    if (Pec12IsOK()) {
                        // On valide la valeur choisie
                        pParam->Amplitude = Amplitude_Selection; // Met � jour l'amplitude dans les param�tres

                        // On envoie les nouvelles valeurs au g�n�rateur
                        GENSIG_UpdateSignal(pParam); // Met � jour le signal du g�n�rateur

                        // On retourne au menu pr�c�dent
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Retourne � l'�tat d'amplitude
                    }// Si la s�lection n'est pas valid�e
                    else if (Pec12IsESC()) {
                        // On reprend l'ancienne valeur
                        Amplitude_Selection = pParam->Amplitude; // R�tablit l'ancienne amplitude

                        // On retourne au menu pr�c�dent
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Retourne � l'�tat d'amplitude
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_OFFSET:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 4); // Positionne le curseur � la quatri�me ligne
                    printf_lcd("*Offset [mV] = %5d", pParam->Offset); // Affiche l'offset actuel

                    // Gestion de la sortie de l'�tat
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_FORME; // Passe � l'�tat de forme
                        lcd_ClearLine(1); // Efface la ligne
                        lcd_gotoxy(1, 4); // Positionne le curseur � la quatri�me ligne
                        printf_lcd(" Offset [mV] = %5d", pParam->Offset); // Affiche l'offset
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Passe � l'�tat d'amplitude
                        lcd_gotoxy(1, 4); // Positionne le curseur � la quatri�me ligne
                        printf_lcd(" Offset [mV] = %5d", pParam->Offset); // Affiche l'offset
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_OFFSET_VALUE; // Passe � l'�tat de s�lection d'offset
                    }
                    break;

                    //////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_OFFSET_VALUE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 4); // Positionne le curseur � la quatri�me ligne
                    printf_lcd("?Offset [mV] = %5d", Offset_Selection); // Affiche la s�lection d'offset

                    // Gestion de l'augmentation de l'offset
                    if (Pec12IsMinus()) {
                        if (Offset_Selection < OFFSET_MAX) { // V�rifie si l'offset est inf�rieur � la valeur maximale
                            Offset_Selection += PAS_OFFSET; // Incr�mente l'offset par le pas d�fini
                        }
                    }// Gestion de la diminution de l'offset
                    else if (Pec12IsPlus()) {
                        if (Offset_Selection > OFFSET_MIN) { // V�rifie si l'offset est sup�rieur � la valeur minimale
                            Offset_Selection -= PAS_OFFSET; // D�cr�mente l'offset par le pas d�fini
                        }
                    }

                    // Gestion de la sortie de l'�tat
                    // Si la s�lection est valid�e
                    if (Pec12IsOK()) {
                        // On valide la valeur choisie
                        pParam->Offset = Offset_Selection; // Met � jour l'offset dans les param�tres

                        // On envoie les nouvelles valeurs au g�n�rateur
                        GENSIG_UpdateSignal(pParam); // Met � jour le signal du g�n�rateur

                        // On retourne au menu pr�c�dent
                        MENU_DATA = MENU_STATE_OFFSET; // Retourne � l'�tat d'offset
                    }// Si la s�lection n'est pas valid�e
                    else if (Pec12IsESC()) {
                        // On reprend l'ancienne valeur
                        Offset_Selection = pParam->Offset; // R�tablit l'ancienne valeur d'offset

                        // On retourne au menu pr�c�dent
                        MENU_DATA = MENU_STATE_OFFSET; // Retourne � l'�tat d'offset
                    }
                    break;

            }

        } else {
            //Appel de la fonction enregistrant les valeurs dans la m�moire flash
            isSaved = MENU_SaveValues(&LocalParamGen);
        }

    } else {
        if (usbStatSave)
        {
            MENU_DemandeSave();
        }
        else{
        lcd_gotoxy(1, 1);
        switch (pParam->Forme) {
            case 0:
                printf_lcd("#Forme : Sinus       "); // Affiche la forme sinus
                break;
            case 1:
                printf_lcd("#Forme : Triangle    "); // Affiche la forme triangle
                break;
            case 2:
                printf_lcd("#Forme : DentDeScie"); // Affiche la forme dent de scie
                break;
            case 3:
                printf_lcd("#Forme : Carre      "); // Affiche la forme carr�
                break;
        }
        lcd_gotoxy(1, 2);
        printf_lcd("#Freq [Hz]   = %5d", pParam->Frequence);
        lcd_gotoxy(1, 3);
        printf_lcd("#Ampl [mV]   = %5d", pParam->Amplitude);
        lcd_gotoxy(1, 4);
        printf_lcd("#Offset [mV] = %5d", pParam->Offset);
    }
    }
    ////////////////////////////////////////////////////////////////////////////////

    // Nettoyage des flags du PEC12
    Pec12ClearESC(); // Efface le flag ESC
    Pec12ClearMinus(); // Efface le flag Minus
    Pec12ClearOK(); // Efface le flag OK
    Pec12ClearPlus(); // Efface le flag Plus

}

/**
 * Function name :MENU_SaveValues
 * @author Vitor & Etienne
 * @date 21.03.2025
 *
 * @brief Enregistre les valeurs des param�tres dans la m�moire non volatile (NVM) apr�s confirmation de l'utilisateur.
 * 
 * Cette fonction enregistre les valeurs des param�tres dans la NVM lorsque l'utilisateur confirme l'action en appuyant 
 * sur un bouton sp�cifique pendant une certaine dur�e. Elle fournit un retour visuel sur un �cran (LCD) pour les 
 * op�rations de sauvegarde r�ussies ou annul�es.
 * 
 * @param  pParam Pointeur vers la structure S_ParamGen contenant les valeurs des param�tres.
 * @param  saveValuesMenuCounter Compteur pour naviguer dans le menu des valeurs � sauvegarder.
 * 
 * @return �tat du mode de sauvegarde (SAVEMODE si en mode de sauvegarde, NOSAVEMODE sinon).
 */

int8_t MENU_SaveValues(S_ParamGen *pParam) {
    //D�claration des variables internes � la fonction 
    static uint8_t saveCounter = 0; //Variable comptant le temps durant lequel l'utilisateur appuie sur S9
    static uint8_t saveMode = false; //Variable indiquant si l'utilisateur a choisi de sauvegarder les valeurs ou non
    static uint8_t saveDisplayCounter = 0; //Variable comptant le temps durant lequel la confirmation de la sauvegarde doit �tre affich�

    //Si l'utilisateur n'a pas encore confirm� ou annul� la sauvegarde
    if (saveMode == false) {
        //Est-ce que S9 est press�?
        if (DebounceIsPressed(&DescrS9)) {
            //Incr�mentation du compteur du temps de pression sur S9
            saveCounter++;
            //D�tection d'un flanc sur le bouton S9
            if (DebounceGetInput(&DescrS9)) {
                //Remise � z�ro du flag du bouton S9
                DebounceClearPressed(&DescrS9);
                //Est-ce que le bouton S9 a �t� press� plus de 500ms?
                if (saveCounter >= SAVECOUNTERMAX) {
                    //Enregistrement des valeurs de pParam dans la m�moire flash
                    I2C_WriteSEEPROM((uint32_t*) pParam, MCP79411_EEPROM_BEG, sizeof (S_ParamGen));
                    //Effacement des lignes d'affichage de la sauvegarde sur le LCD
                    lcd_ClearLine(2);
                    lcd_ClearLine(3);
                    //Remise � z�ro du compteur de pression sur S9
                    saveCounter = 0;
                    //Changement du mode de la sauvegarde afin d'afficher la confiamation de l'enregistrement des valeurs
                    saveMode = true;
                    //Affichage de la confirmation de la sauvegarde
                    lcd_gotoxy(1, 2);
                    printf_lcd("    Sauvegarde OK");
                    
                } else {
                    //Effacement des lignes d'affichage de la sauvegarde sur le LCD
                    lcd_ClearLine(2);
                    lcd_ClearLine(3);
                    //Remise � z�ro du compteur de pression sur S9
                    saveCounter = 0;
                    //Changement du mode de la sauvegarde afin d'afficher la l'annulation de l'enregistrement des valeurs
                    saveMode = true;
                    //Affichage de la confirmation de la sauvegarde
                    lcd_gotoxy(1, 2);
                    printf_lcd(" Sauvegarde ANNULEE");
                }
            }
        }
        //Retour de l'�tat de l'�tat de la sauvegarde
        return (SAVEMODE);
    } else if ((saveMode != false) && (saveDisplayCounter < SAVEDISPLAYTIME)) {
        //Incr�mentation du compteur de l'affichage de la sauvagarde
        saveDisplayCounter++;
        //Retour de l'�tat de l'�tat de la sauvegarde
        return (SAVEMODE);
    } else {
        //Effacement de la ligne affichant si la valeur a �t� sauvegard�e ou non
        //lcd_ClearLine(2);
        //Remise � z�ro du compteur de l'affichage de la sauvagarde
        saveDisplayCounter = 0;
        //Remise � z�ro du mode de la sauvegarde
        saveMode = false;
        MENU_Initialize(pParam);
        //Retour de l'�tat de l'�tat de la sauvegarde
        return (NOSAVEMODE);
    }
}