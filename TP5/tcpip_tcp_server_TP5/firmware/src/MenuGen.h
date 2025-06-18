#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu  du générateur
// Traitement cyclique à 1 ms du Pec12


#include <stdint.h>
#include "DefMenuGen.h"
#include "Mc32Debounce.h"

#define FREQU_MAX 2000  // Fréwuence maximum
#define FREQU_MIN 20    // Fréquence minimum
#define AMPL_MAX 10000  // Amplitude maximum
#define AMPL_MIN 0      // Amplitude minimum
#define PAS_AMPL 100    // Pas d'amplitude
#define OFFSET_MAX 5000 // Offset maximum
#define OFFSET_MIN -5000    // Offset minimum
#define PAS_OFFSET 100 // Pas d'offset

// Énumération pour les état du menu
typedef enum {
    MENU_STATE_FORME = 0,           // État pour sélectionner la forme du signal
    MENU_STATE_FORME_VALUE = 1,     // État pour modifier la valeur de la forme
    MENU_STATE_FREQUENCE = 2,       // État pour sélectionner la fréquence
    MENU_STATE_FREQUENCE_VALUE = 3, // État pour modifier la valeur de la fréquence
    MENU_STATE_AMPLITUDE = 4,       // État pour sélectionner l'amplitude
    MENU_STATE_AMPLITUDE_VALUE = 5, // État pour modifier la valeur de l'amplitude
    MENU_STATE_OFFSET = 6,          // État pour sélectionner l'offset
    MENU_STATE_OFFSET_VALUE = 7    // État pour modifier la valeur de l'offset
} MENU_STATES;

// Fonction d'affichage des valeurs du menu
void AfficheMenu(S_ParamGen *pParam);
//Déclaration de la valeur des define

#define SELECTMODE 0
#define SELECTVALUE 1
#define MENUCOUNTERMAX 4
#define MENUCOUNTERMIN 1
#define MODEFORME 1
#define MODEFREQ 2
#define MODEAMPL 3
#define MODEOFFSET 4
#define FORMEMAX 3
#define FORMEMIN 0
#define FREQMAX 2000
#define FREQMIN 20
#define FREQSTEP 20
#define AMPLMAX 10000
#define AMPLMIN 0
#define AMPLSTEP 100
#define OFFSETMAX 5000
#define OFFSETMIN -5000
#define OFFSETSTEP 100
#define SAVEMODE 1
#define NOSAVEMODE 0
#define SAVECOUNTERMAX 50
#define SAVEDISPLAYTIME 200

//Déclaration des strucures

S_SwitchDescriptor DescrS9;
S_ParamGen OldValue;

// Prototype des fonctions 

void MENU_Initialize(S_ParamGen *pParam);
void MENU_Execute(S_ParamGen *pParam, bool local);
void MENU_SelectMode(S_ParamGen *pParam, int8_t selectModeMenuCounter);
S_ParamGen MENU_SelectValue(S_ParamGen selectValueNewValue, int8_t selectValueMenuCounter);
int8_t MENU_SaveValues(S_ParamGen *pParam);

#endif