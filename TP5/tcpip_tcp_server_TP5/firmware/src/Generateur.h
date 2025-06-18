#ifndef Generateur_h
#define Generateur_h

// TP3 MenuGen 2016
// C. HUBER  03.02.2016
// Fichier Generateur.h
// Prototypes des fonctions du générateur  de signal

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "DefMenuGen.h"

#define MAX_ECH 100 // Nombre maximum d'échantillons pour le signal
#define MOITIE_ECH 50 // Moitié du nombre maximum d'échantillons
#define QUART_ECH 25 // Quart du nombre maximum d'échantillons
#define VAL_MAX_PAS 65535 // Valeur maximale de conversion pour le DAC (16 bits)
#define TRANSFORMATION_VALEUR_TIMER3 800000 // Valeur de fréquence du timer 3 (en Hz)
#define MAX_AMPLITUDE 10000 // Amplitude maximale du signal (en mV)
#define MOITIE_AMPLITUDE 5000 // Moitié de l'amplitude maximale (en mV)
#define PAS_AMPL 100 // Pas d'amplitude (en mV)
#define OFFSET_MAX 5000 // Offset maximum (en mV)
#define OFFSET_MIN -5000 // Offset minimum (en mV)
#define PAS_OFFSET 100 // Pas d'offset (en mV)
#define INVERSION -1 // Facteur d'inversion pour l'offset
#define F_MIN 20 // Fréquence minimale (en Hz)
#define F_MAX 2000 // Fréquence maximale (en Hz)
#define PAS_F 20 // Pas de fréquence (en Hz)
#define UN_VOLT_AMPLITUDE 1000 // Amplitude correspondant à 1 Volt (en mV)
#define DEUX 2 // Constante représentant la valeur 2
#define QUATRE 4 // Constante représentant la valeur 4
#define DEUX_SECONDES 200 // Constante représentant 2 secondes (en ms)

//----------------------------------------------------------------------------
//  GENSIG_Initialize
//  Initialise le générateur de signaux avec des paramètres sauvegardés
//  ou par défaut.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers les paramètres du générateur.
//----------------------------------------------------------------------------

void GENSIG_Initialize(S_ParamGen *pParam);

//----------------------------------------------------------------------------
//  GENSIG_UpdatePeriode
//  Met à jour la période d'échantillonnage en fonction de la fréquence.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers les paramètres du générateur.
//----------------------------------------------------------------------------

void GENSIG_UpdatePeriode(S_ParamGen *pParam);

//----------------------------------------------------------------------------
//  GENSIG_UpdateSignal
//  Met à jour les valeurs du signal (forme, amplitude, offset).
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers les paramètres du générateur.
//----------------------------------------------------------------------------

void GENSIG_UpdateSignal(S_ParamGen *pParam);

//----------------------------------------------------------------------------
//  GENSIG_Execute
//  Envoie cycliquement chaque échantillon au DAC.
//  À appeler dans l'interruption du Timer 3.
//----------------------------------------------------------------------------

void GENSIG_Execute(void);

#endif
