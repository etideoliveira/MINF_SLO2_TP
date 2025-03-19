#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu du g�n�rateur
// Traitement cyclique � 1 ms du PEC12

#include <stdbool.h> // Inclusion de la biblioth�que pour les types bool�ens
#include <stdint.h>  // Inclusion de la biblioth�que pour les types d'entiers
#include "DefMenuGen.h" // Inclusion du fichier d'en-t�te pour les d�finitions du menu

// *****************************************************************************
/* Application states

  Summary:
    Menu states enumeration

  Description:
    This enumeration defines the valid menu states. These states
    determine the behavior of the application at various times.
 */

// D�finition des �tats du menu
typedef enum {
    MENU_STATE_FORME = 0,           // �tat pour s�lectionner la forme du signal
    MENU_STATE_FORME_VALUE = 1,     // �tat pour modifier la valeur de la forme
    MENU_STATE_FREQUENCE = 2,       // �tat pour s�lectionner la fr�quence
    MENU_STATE_FREQUENCE_VALUE = 3, // �tat pour modifier la valeur de la fr�quence
    MENU_STATE_AMPLITUDE = 4,       // �tat pour s�lectionner l'amplitude
    MENU_STATE_AMPLITUDE_VALUE = 5, // �tat pour modifier la valeur de l'amplitude
    MENU_STATE_OFFSET = 6,          // �tat pour s�lectionner l'offset
    MENU_STATE_OFFSET_VALUE = 7,    // �tat pour modifier la valeur de l'offset
    MENU_STATE_SAVE = 8              // �tat pour sauvegarder les param�tres
} MENU_STATES;

//----------------------------------------------------------------------------
//  MENU_Initialize
//  Initialise l'affichage du menu sur l'�cran LCD avec les param�tres
//  du g�n�rateur.
//  
//  Param�tres :
//      S_ParamGen *pParam : Pointeur vers les param�tres du g�n�rateur.
//----------------------------------------------------------------------------

void MENU_Initialize(S_ParamGen *pParam);

//----------------------------------------------------------------------------
//  MENU_Execute
//  Ex�cute le menu, permettant de naviguer et de modifier les param�tres
//  du g�n�rateur. Cette fonction est appel�e cycliquement depuis l'application.
//  
//  Param�tres :
//      S_ParamGen *pParam : Pointeur vers les param�tres du g�n�rateur.
//----------------------------------------------------------------------------

void MENU_Execute(S_ParamGen *pParam);

#endif 