#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu du générateur
// Traitement cyclique à 1 ms du PEC12

#include <stdbool.h> // Inclusion de la bibliothèque pour les types booléens
#include <stdint.h>  // Inclusion de la bibliothèque pour les types d'entiers
#include "DefMenuGen.h" // Inclusion du fichier d'en-tête pour les définitions du menu

// *****************************************************************************
/* Application states

  Summary:
    Menu states enumeration

  Description:
    This enumeration defines the valid menu states. These states
    determine the behavior of the application at various times.
 */

// Définition des états du menu
typedef enum {
    MENU_STATE_FORME = 0,           // État pour sélectionner la forme du signal
    MENU_STATE_FORME_VALUE = 1,     // État pour modifier la valeur de la forme
    MENU_STATE_FREQUENCE = 2,       // État pour sélectionner la fréquence
    MENU_STATE_FREQUENCE_VALUE = 3, // État pour modifier la valeur de la fréquence
    MENU_STATE_AMPLITUDE = 4,       // État pour sélectionner l'amplitude
    MENU_STATE_AMPLITUDE_VALUE = 5, // État pour modifier la valeur de l'amplitude
    MENU_STATE_OFFSET = 6,          // État pour sélectionner l'offset
    MENU_STATE_OFFSET_VALUE = 7,    // État pour modifier la valeur de l'offset
    MENU_STATE_SAVE = 8              // État pour sauvegarder les paramètres
} MENU_STATES;

//----------------------------------------------------------------------------
//  MENU_Initialize
//  Initialise l'affichage du menu sur l'écran LCD avec les paramètres
//  du générateur.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers les paramètres du générateur.
//----------------------------------------------------------------------------

void MENU_Initialize(S_ParamGen *pParam);

//----------------------------------------------------------------------------
//  MENU_Execute
//  Exécute le menu, permettant de naviguer et de modifier les paramètres
//  du générateur. Cette fonction est appelée cycliquement depuis l'application.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers les paramètres du générateur.
//----------------------------------------------------------------------------

void MENU_Execute(S_ParamGen *pParam);

#endif 