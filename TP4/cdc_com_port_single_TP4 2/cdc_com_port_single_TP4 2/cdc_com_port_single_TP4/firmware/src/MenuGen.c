// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms

// Included Files 
#include <stdint.h>                   // Inclusion de la bibliothèque standard pour les types d'entiers (uint8_t, uint16_t, etc.)
#include <stdbool.h>                  // Inclusion de la bibliothèque standard pour le type booléen (true, false)
#include "MenuGen.h"                  // Inclusion du fichier d'en-tête pour la gestion du menu du générateur
#include "GesPec12.h"                 // Inclusion du fichier d'en-tête pour la gestion du PEC12 (encodeur rotatif)
#include "Mc32DriverLcd.h"            // Inclusion du fichier d'en-tête pour le driver LCD (affichage)
#include "Generateur.h"               // Inclusion du fichier d'en-tête pour le générateur de signaux
#include "bsp.h"                      // Inclusion du fichier d'en-tête pour la plateforme de développement (Board Support Package)
#include "app.h"                      // Inclusion du fichier d'en-tête pour l'application principale
#include "Mc32gestI2cSeeprom.h"
// Déclaration d'une variable globale pour stocker l'état actuel du menu
MENU_STATES MENU_DATA; // MENU_DATA est une variable qui représente l'état du menu (par exemple, forme, fréquence, amplitude, etc.)

// Structure pour le traitement du PEC12
S_Pec12_Descriptor Pec12; // Pec12 est une structure qui contient les informations nécessaires pour gérer les entrées du PEC12 (encodeur rotatif)

//----------------------------------------------------------------------------
//  MENU_Initialize
//  Initialise l'affichage du menu sur l'écran LCD avec les paramètres
//  du générateur. Cette fonction est appelée pour afficher les valeurs
//  actuelles de la forme, de la fréquence, de l'amplitude et de l'offset.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les paramètres
//                           du générateur de signaux.
//----------------------------------------------------------------------------

void MENU_Initialize(S_ParamGen *pParam) {
    LCD_ClearScreen(); // Efface l'écran LCD pour préparer l'affichage
    lcd_gotoxy(1, 1); // Positionne le curseur à la première ligne
    printf_lcd(" Forme : %10c", pParam->Forme); // Affiche la forme du signal
    lcd_gotoxy(1, 2); // Positionne le curseur à la deuxième ligne
    printf_lcd(" Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fréquence en Hz
    lcd_gotoxy(1, 3); // Positionne le curseur à la troisième ligne
    printf_lcd(" Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude en mV
    lcd_gotoxy(1, 4); // Positionne le curseur à la quatrième ligne
    printf_lcd(" Offset [mV] = %5d", pParam->Offset); // Affiche l'offset en mV
}

//----------------------------------------------------------------------------
//  MENU_Execute
//  Exécute le menu, permettant de naviguer et de modifier les paramètres
//  du générateur. Cette fonction est appelée cycliquement depuis l'application.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers les paramètres du générateur.
//----------------------------------------------------------------------------

void MENU_Execute(S_ParamGen *pParam, bool local) {
    // Variables locales
    static int16_t Frequence_Selection = F_MIN; // Sélection de la fréquence initialisée à la valeur minimale
    static int16_t Amplitude_Selection = UN_VOLT_AMPLITUDE; // Sélection de l'amplitude initialisée à 1 Volt
    static int16_t Offset_Selection = 0; // Sélection de l'offset initialisée à 0
    static uint8_t CompteurForme = 0; // Compteur pour la forme du signal
    static bool firstTimeIn = 1; // Indicateur pour l'initialisation
    static int8_t isSaved = NOSAVEMODE; //Variable déterminant si l'utilisateur est en train d'enregistrer les valeurs ou non


    if (local) {
        // Initialisation de l'affichage
        // Efface l'écran LCD la première fois
        if (firstTimeIn) {
            MENU_Initialize(pParam); // Appelle la fonction d'initialisation
            firstTimeIn = 0; // Met à jour l'indicateur pour éviter de réinitialiser à chaque appel
        }
        //Appel de la fonction de debounce pour le bouton de sauvegarde S9
        DoDebounce(&DescrS9, S_OK);

        //Est-ce que S9 a été pressé?
        if (DebounceIsPressed(&DescrS9)) {
            //Nous alluomons la backlight du LCD au cas ou celle-ci serait éteinte (en cas d'inactivité)
            lcd_bl_on();
            //Indication d'une activité de l'utilisateur
            Pec12.NoActivity = false;
            Pec12.InactivityDuration = 0;
            //Est-ce que l'utilisateur est déjà dans le mode de sauvegarde des données?
            if (isSaved == NOSAVEMODE) {
                //Remise à zéro du flag du bouton S9
                DebounceClearPressed(&DescrS9);
                //effacer l'affichage du LCD
                LCD_ClearScreen();
                //Affichage du mode de sauvegarde du système 
                lcd_gotoxy(1, 2);
                printf_lcd("    Sauvegarde ?");
                lcd_gotoxy(1, 3);
                printf_lcd("    (appui long)");
                //Le programme passe en mode de sauvegarde des données
                isSaved = SAVEMODE;
            }
        }
        if (isSaved == NOSAVEMODE) {

            switch (MENU_DATA) {
                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FORME:
                    // Gestion de l'affichage de la forme du signal
                    lcd_gotoxy(1, 1); // Positionne le curseur à la première ligne
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
                            printf_lcd("*Forme : Carre      "); // Affiche la forme carré
                            break;
                    }

                    // Gestion de la sortie de l'état
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_FREQUENCE; // Passe à l'état de fréquence
                        lcd_gotoxy(1, 1);
                        printf_lcd(" "); // Efface la ligne
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_OFFSET; // Passe à l'état d'offset
                        lcd_gotoxy(1, 1);
                        printf_lcd(" "); // Efface la ligne
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_FORME_VALUE; // Passe à l'état de sélection de forme
                        lcd_ClearLine(1); // Efface la ligne
                    }
                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FORME_VALUE:
                    // Gestion de l'affichage de la sélection de forme
                    lcd_gotoxy(1, 1); // Positionne le curseur à la première ligne
                    switch (CompteurForme) {
                        case 0:
                            printf_lcd("?Forme : Sinus       "); // Affiche la sélection de forme
                            break;
                        case 1:
                            printf_lcd("?Forme : Triangle    "); // Affiche la sélection de forme
                            break;
                        case 2:
                            printf_lcd("?Forme : DentDeScie"); // Affiche la sélection de forme
                            break;
                        case 3:
                            printf_lcd("?Forme : Carre      "); // Affiche la sélection de forme
                            break;
                    }

                    // Gestion de la sélection de forme
                    if (Pec12IsMinus()) {
                        CompteurForme++; // Incrémente le compteur de forme
                        if (CompteurForme == QUATRE) {
                            CompteurForme = 0; // Retourne au début
                        }
                    } else if (Pec12IsPlus()) {
                        if (CompteurForme == 0) {
                            CompteurForme = QUATRE; // Retourne à la fin
                        }
                        CompteurForme--; // Décrémente le compteur de forme
                    }

                    // Gestion de la sortie de l'état
                    if (Pec12IsOK()) {
                        // Validation de la valeur choisie
                        switch (CompteurForme) {
                            case 0:
                                pParam->Forme = Sinus; // Définit la forme sinus
                                break;
                            case 1:
                                pParam->Forme = Triangle; // Définit la forme triangle
                                break;
                            case 2:
                                pParam->Forme = DentDeScie; // Définit la forme dent de scie
                                break;
                            case 3:
                                pParam->Forme = Carre; // Définit la forme carré
                                break;
                        }

                        // Envoie les nouvelles valeurs au générateur
                        GENSIG_UpdateSignal(pParam);
                        MENU_DATA = MENU_STATE_FORME; // Retourne au menu précédent
                        lcd_ClearLine(1); // Efface la ligne
                    } else if (Pec12IsESC()) {
                        // Récupère l'ancienne valeur
                        CompteurForme = pParam->Forme; // Rétablit l'ancienne forme
                        MENU_DATA = MENU_STATE_FORME; // Retourne au menu précédent
                        lcd_ClearLine(1); // Efface la ligne
                    }
                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FREQUENCE:
                    // Gestion de l'affichage de la fréquence
                    lcd_gotoxy(1, 2); // Positionne le curseur à la deuxième ligne
                    printf_lcd("*Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fréquence

                    // Gestion de la sortie de l'état
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Passe à l'état d'amplitude
                        lcd_gotoxy(1, 2);
                        printf_lcd(" Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fréquence
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_FORME; // Passe à l'état de forme
                        lcd_ClearLine(1); // Efface la ligne
                        lcd_gotoxy(1, 2);
                        printf_lcd(" Freq [Hz]   = %5d", pParam->Frequence); // Affiche la fréquence
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_FREQUENCE_VALUE; // Passe à l'état de sélection de fréquence
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_FREQUENCE_VALUE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 2); // Positionne le curseur à la deuxième ligne
                    printf_lcd("?Freq [Hz]   = %5d", Frequence_Selection); // Affiche la fréquence sélectionnée

                    // Gestion de l'augmentation de la fréquence
                    if (Pec12IsMinus()) {
                        Frequence_Selection += PAS_F; // Incrémente la fréquence par le pas défini
                        if (Frequence_Selection > F_MAX) { // Vérifie si la fréquence dépasse la valeur maximale
                            Frequence_Selection = F_MIN; // Si oui, retourne à la valeur minimale
                        }
                    }// Gestion de la diminution de la fréquence
                    else if (Pec12IsPlus()) {
                        Frequence_Selection -= PAS_F; // Décrémente la fréquence par le pas défini
                        if (Frequence_Selection < F_MIN) { // Vérifie si la fréquence est inférieure à la valeur minimale
                            Frequence_Selection = F_MAX; // Si oui, retourne à la valeur maximale
                        }
                    }

                    // Gestion de la sortie de l'état
                    // Si la sélection est validée
                    if (Pec12IsOK()) {
                        // On valide la valeur choisie
                        pParam->Frequence = Frequence_Selection; // Met à jour la fréquence dans les paramètres

                        // On envoie les nouvelles valeurs au générateur
                        GENSIG_UpdatePeriode(pParam); // Met à jour la période d'échantillonnage du générateur

                        // On retourne au menu précédent
                        MENU_DATA = MENU_STATE_FREQUENCE; // Retourne à l'état de fréquence
                    }// Si la sélection n'est pas validée
                    else if (Pec12IsESC()) {
                        // On reprend l'ancienne valeur
                        Frequence_Selection = pParam->Frequence; // Rétablit l'ancienne fréquence

                        // On retourne au menu précédent
                        MENU_DATA = MENU_STATE_FREQUENCE; // Retourne à l'état de fréquence
                    }

                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_AMPLITUDE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 3); // Positionne le curseur à la troisième ligne
                    printf_lcd("*Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude actuelle

                    // Gestion de la sortie de l'état
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_OFFSET; // Passe à l'état d'offset
                        lcd_gotoxy(1, 3); // Positionne le curseur à la troisième ligne
                        printf_lcd(" Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_FREQUENCE; // Passe à l'état de fréquence
                        lcd_gotoxy(1, 3); // Positionne le curseur à la troisième ligne
                        printf_lcd(" Ampl [mV]   = %5d", pParam->Amplitude); // Affiche l'amplitude
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_AMPLITUDE_VALUE; // Passe à l'état de sélection d'amplitude
                    }
                    break;

                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_AMPLITUDE_VALUE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 3); // Positionne le curseur à la troisième ligne
                    printf_lcd("?Ampl [mV]   = %5d", Amplitude_Selection); // Affiche la sélection d'amplitude

                    // Gestion de l'augmentation de l'amplitude
                    if (Pec12IsMinus()) {
                        Amplitude_Selection += PAS_AMPL; // Incrémente l'amplitude par le pas défini
                        if (Amplitude_Selection > MAX_AMPLITUDE) { // Vérifie si l'amplitude dépasse la valeur maximale
                            Amplitude_Selection = 0; // Si oui, retourne à 0
                        }
                    }// Gestion de la diminution de l'amplitude
                    else if (Pec12IsPlus()) {
                        Amplitude_Selection -= PAS_AMPL; // Décrémente l'amplitude par le pas défini
                        if (Amplitude_Selection < 0) { // Vérifie si l'amplitude est inférieure à 0
                            Amplitude_Selection = MAX_AMPLITUDE; // Si oui, retourne à la valeur maximale
                        }
                    }

                    // Gestion de la sortie de l'état
                    // Si la sélection est validée
                    if (Pec12IsOK()) {
                        // On valide la valeur choisie
                        pParam->Amplitude = Amplitude_Selection; // Met à jour l'amplitude dans les paramètres

                        // On envoie les nouvelles valeurs au générateur
                        GENSIG_UpdateSignal(pParam); // Met à jour le signal du générateur

                        // On retourne au menu précédent
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Retourne à l'état d'amplitude
                    }// Si la sélection n'est pas validée
                    else if (Pec12IsESC()) {
                        // On reprend l'ancienne valeur
                        Amplitude_Selection = pParam->Amplitude; // Rétablit l'ancienne amplitude

                        // On retourne au menu précédent
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Retourne à l'état d'amplitude
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_OFFSET:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 4); // Positionne le curseur à la quatrième ligne
                    printf_lcd("*Offset [mV] = %5d", pParam->Offset); // Affiche l'offset actuel

                    // Gestion de la sortie de l'état
                    if (Pec12IsMinus()) {
                        MENU_DATA = MENU_STATE_FORME; // Passe à l'état de forme
                        lcd_ClearLine(1); // Efface la ligne
                        lcd_gotoxy(1, 4); // Positionne le curseur à la quatrième ligne
                        printf_lcd(" Offset [mV] = %5d", pParam->Offset); // Affiche l'offset
                    } else if (Pec12IsPlus()) {
                        MENU_DATA = MENU_STATE_AMPLITUDE; // Passe à l'état d'amplitude
                        lcd_gotoxy(1, 4); // Positionne le curseur à la quatrième ligne
                        printf_lcd(" Offset [mV] = %5d", pParam->Offset); // Affiche l'offset
                    } else if (Pec12IsOK()) {
                        MENU_DATA = MENU_STATE_OFFSET_VALUE; // Passe à l'état de sélection d'offset
                    }
                    break;

                    //////////////////////////////////////////////////////////////////////////////
                case MENU_STATE_OFFSET_VALUE:
                    // Gestion de l'affichage
                    lcd_gotoxy(1, 4); // Positionne le curseur à la quatrième ligne
                    printf_lcd("?Offset [mV] = %5d", Offset_Selection); // Affiche la sélection d'offset

                    // Gestion de l'augmentation de l'offset
                    if (Pec12IsMinus()) {
                        if (Offset_Selection < OFFSET_MAX) { // Vérifie si l'offset est inférieur à la valeur maximale
                            Offset_Selection += PAS_OFFSET; // Incrémente l'offset par le pas défini
                        }
                    }// Gestion de la diminution de l'offset
                    else if (Pec12IsPlus()) {
                        if (Offset_Selection > OFFSET_MIN) { // Vérifie si l'offset est supérieur à la valeur minimale
                            Offset_Selection -= PAS_OFFSET; // Décrémente l'offset par le pas défini
                        }
                    }

                    // Gestion de la sortie de l'état
                    // Si la sélection est validée
                    if (Pec12IsOK()) {
                        // On valide la valeur choisie
                        pParam->Offset = Offset_Selection; // Met à jour l'offset dans les paramètres

                        // On envoie les nouvelles valeurs au générateur
                        GENSIG_UpdateSignal(pParam); // Met à jour le signal du générateur

                        // On retourne au menu précédent
                        MENU_DATA = MENU_STATE_OFFSET; // Retourne à l'état d'offset
                    }// Si la sélection n'est pas validée
                    else if (Pec12IsESC()) {
                        // On reprend l'ancienne valeur
                        Offset_Selection = pParam->Offset; // Rétablit l'ancienne valeur d'offset

                        // On retourne au menu précédent
                        MENU_DATA = MENU_STATE_OFFSET; // Retourne à l'état d'offset
                    }
                    break;

            }

        } else {
            //Appel de la fonction enregistrant les valeurs dans la mémoire flash
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
                printf_lcd("#Forme : Carre      "); // Affiche la forme carré
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
 * @brief Enregistre les valeurs des paramètres dans la mémoire non volatile (NVM) après confirmation de l'utilisateur.
 * 
 * Cette fonction enregistre les valeurs des paramètres dans la NVM lorsque l'utilisateur confirme l'action en appuyant 
 * sur un bouton spécifique pendant une certaine durée. Elle fournit un retour visuel sur un écran (LCD) pour les 
 * opérations de sauvegarde réussies ou annulées.
 * 
 * @param  pParam Pointeur vers la structure S_ParamGen contenant les valeurs des paramètres.
 * @param  saveValuesMenuCounter Compteur pour naviguer dans le menu des valeurs à sauvegarder.
 * 
 * @return État du mode de sauvegarde (SAVEMODE si en mode de sauvegarde, NOSAVEMODE sinon).
 */

int8_t MENU_SaveValues(S_ParamGen *pParam) {
    //Déclaration des variables internes à la fonction 
    static uint8_t saveCounter = 0; //Variable comptant le temps durant lequel l'utilisateur appuie sur S9
    static uint8_t saveMode = false; //Variable indiquant si l'utilisateur a choisi de sauvegarder les valeurs ou non
    static uint8_t saveDisplayCounter = 0; //Variable comptant le temps durant lequel la confirmation de la sauvegarde doit être affiché

    //Si l'utilisateur n'a pas encore confirmé ou annulé la sauvegarde
    if (saveMode == false) {
        //Est-ce que S9 est pressé?
        if (DebounceIsPressed(&DescrS9)) {
            //Incrémentation du compteur du temps de pression sur S9
            saveCounter++;
            //Détection d'un flanc sur le bouton S9
            if (DebounceGetInput(&DescrS9)) {
                //Remise à zéro du flag du bouton S9
                DebounceClearPressed(&DescrS9);
                //Est-ce que le bouton S9 a été pressé plus de 500ms?
                if (saveCounter >= SAVECOUNTERMAX) {
                    //Enregistrement des valeurs de pParam dans la mémoire flash
                    I2C_WriteSEEPROM((uint32_t*) pParam, MCP79411_EEPROM_BEG, sizeof (S_ParamGen));
                    //Effacement des lignes d'affichage de la sauvegarde sur le LCD
                    lcd_ClearLine(2);
                    lcd_ClearLine(3);
                    //Remise à zéro du compteur de pression sur S9
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
                    //Remise à zéro du compteur de pression sur S9
                    saveCounter = 0;
                    //Changement du mode de la sauvegarde afin d'afficher la l'annulation de l'enregistrement des valeurs
                    saveMode = true;
                    //Affichage de la confirmation de la sauvegarde
                    lcd_gotoxy(1, 2);
                    printf_lcd(" Sauvegarde ANNULEE");
                }
            }
        }
        //Retour de l'état de l'état de la sauvegarde
        return (SAVEMODE);
    } else if ((saveMode != false) && (saveDisplayCounter < SAVEDISPLAYTIME)) {
        //Incrémentation du compteur de l'affichage de la sauvagarde
        saveDisplayCounter++;
        //Retour de l'état de l'état de la sauvegarde
        return (SAVEMODE);
    } else {
        //Effacement de la ligne affichant si la valeur a été sauvegardée ou non
        //lcd_ClearLine(2);
        //Remise à zéro du compteur de l'affichage de la sauvagarde
        saveDisplayCounter = 0;
        //Remise à zéro du mode de la sauvegarde
        saveMode = false;
        MENU_Initialize(pParam);
        //Retour de l'état de l'état de la sauvegarde
        return (NOSAVEMODE);
    }
}