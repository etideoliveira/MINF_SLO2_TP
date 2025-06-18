// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion du générateur

// Prévu pour signal de 40 échantillons
// Migration sur PIC32 30.04.2014 C. Huber

#include "Generateur.h"          // Inclusion du fichier d'en-tête pour le générateur
#include "DefMenuGen.h"          // Inclusion du fichier d'en-tête pour le menu du générateur
#include "Mc32gestSpiDac.h"      // Inclusion du fichier d'en-tête pour la gestion du DAC SPI
#include "system_config.h"       // Inclusion du fichier d'en-tête pour la configuration système
#include "Mc32DriverLcd.h"       // Inclusion du fichier d'en-tête pour le driver LCD
#include "Mc32gestI2cSeeprom.h"
// Variables globales
S_ParamGen structInterGENSIG; // Structure intermédiaire des paramètres du générateur
int32_t tableauValeursSignal[MAX_ECH]; // Tableau pour stocker les valeurs du signal

//----------------------------------------------------------------------------
//  GENSIG_Initialize
//  Initialise le générateur de signaux en récupérant les paramètres sauvegardés
//  ou en les initialisant par défaut si aucune donnée valide n'est trouvée.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les paramètres
//                           du générateur de signaux.
//----------------------------------------------------------------------------

void GENSIG_Initialize(S_ParamGen *pParam) {
    //Lecture des données dans la flash et enregistrement dans la structure 
    I2C_ReadSEEPROM(&structInterGENSIG, MCP79411_EEPROM_BEG, sizeof (S_ParamGen));
    if (structInterGENSIG.Magic == MAGIC) {
        *pParam = structInterGENSIG; // Sauvegarde des valeurs récupérées
    } else {
        pParam->Forme = Sinus; // Forme du signal par défaut : Sinus
        pParam->Frequence = F_MIN;    // Fréquence par défaut : minimum
        pParam->Amplitude = UN_VOLT_AMPLITUDE; // Amplitude par défaut : 1 Volt
        pParam->Offset = 0;           // Offset par défaut : 0
        pParam->Magic = MAGIC;        // Valeur magique pour identifier les données
    }
}

//----------------------------------------------------------------------------
//  GENSIG_UpdatePeriode
//  Met à jour la période d'échantillonnage du générateur de signaux en
//  fonction de la fréquence spécifiée dans les paramètres.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les paramètres
//                           du générateur de signaux.
//----------------------------------------------------------------------------

void GENSIG_UpdatePeriode(S_ParamGen *pParam) {
    static uint16_t compteurTimer3 = 0; // Compteur pour le timer 3
    compteurTimer3 = (uint16_t)(TRANSFORMATION_VALEUR_TIMER3 / pParam->Frequence);
    PLIB_TMR_Period16BitSet(TMR_ID_3, compteurTimer3); // Mise à jour de la période du timer 3
}

//----------------------------------------------------------------------------
//  GENSIG_UpdateSignal
//  Met à jour les valeurs du signal en fonction de la forme, de l'amplitude
//  et de l'offset spécifiés dans les paramètres. Les valeurs sont stockées
//  dans le tableau de valeurs du signal.
//  
//  Paramètres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les paramètres
//                           du générateur de signaux.
//----------------------------------------------------------------------------

void GENSIG_UpdateSignal(S_ParamGen *pParam) {
    int32_t nbEchantillon; // Compteur d'échantillons
    int32_t amplitude_en_mV; // Amplitude en millivolts
    int32_t amplitude; // Amplitude
    int32_t offset; // Offset
    int32_t valBrute = 0; // Valeur brute du signal

    const float tableauSinus[MAX_ECH] = {
        0.000, 0.063, 0.127, 0.189, 0.251, 0.312, 0.372, 0.430, 0.486, 0.540,
        0.593, 0.643, 0.690, 0.734, 0.776, 0.814, 0.850, 0.881, 0.909, 0.934,
        0.955, 0.972, 0.985, 0.994, 0.999, 1.000, 0.997, 0.990, 0.979, 0.964,
        0.945, 0.923, 0.896, 0.866, 0.833, 0.796, 0.756, 0.713, 0.667, 0.619,
        0.568, 0.514, 0.459, 0.402, 0.343, 0.283, 0.221, 0.159, 0.096, 0.033,
        -0.031, -0.094, -0.157, -0.219, -0.281, -0.341, -0.400, -0.457, -0.513,
        -0.566, -0.617, -0.666, -0.712, -0.755, -0.795, -0.832, -0.865, -0.895,
        -0.922, -0.944, -0.963, -0.978, -0.990, -0.997, -1.000, -0.999, -0.994,
        -0.985, -0.972, -0.955, -0.935, -0.910, -0.882, -0.851, -0.816, -0.777,
        -0.736, -0.692, -0.644, -0.595, -0.542, -0.488, -0.432, -0.374, -0.314,
        -0.253, -0.191, -0.129, -0.066, -0.002
    };

    amplitude = (int32_t)pParam->Amplitude; // Récupération de l'amplitude
    offset = (int32_t)((pParam->Offset * INVERSION) / DEUX); // Inversion de l'effet de l'offset
    amplitude_en_mV = amplitude / MAX_ECH; // Conversion de l'amplitude saisie par l'utilisateur en mV

    // Boucle pour générer les valeurs du signal
    for (nbEchantillon = 0; nbEchantillon < MAX_ECH; nbEchantillon++) {
        switch (pParam->Forme) {
            case Sinus: // Cas du signal sinus
                valBrute = MOITIE_AMPLITUDE + offset + tableauSinus[nbEchantillon] * amplitude / DEUX;
                break;
            case Triangle: // Cas du signal triangle
                if ((MAX_ECH / DEUX) > nbEchantillon) {
                    valBrute = MOITIE_AMPLITUDE + offset + (amplitude_en_mV * (DEUX * (nbEchantillon - QUART_ECH)));
                } else {
                    valBrute = MOITIE_AMPLITUDE + offset + (amplitude_en_mV * (MAX_ECH - DEUX * (nbEchantillon - QUART_ECH)));
                }
                break;
            case DentDeScie: // Cas du signal dent de scie
                valBrute = MOITIE_AMPLITUDE + (offset - ((nbEchantillon - MOITIE_ECH) * amplitude_en_mV));
                break;
            case Carre: // Cas du signal carré
                if ((MAX_ECH / DEUX) > nbEchantillon) {
                    valBrute = MOITIE_AMPLITUDE + offset + amplitude / DEUX;
                } else {
                    valBrute = MOITIE_AMPLITUDE + offset - amplitude / DEUX;
                }
                break;
            default:
                break;
        }
        // Gestion de l'écrêtage
        if (valBrute > MAX_AMPLITUDE) {
            valBrute = MAX_AMPLITUDE; // Limite supérieure
        } else if (valBrute < 0) {
            valBrute = 0; // Limite inférieure
        }
        // Conversion pour passer de 10000 à 65535 
        tableauValeursSignal[nbEchantillon] = (VAL_MAX_PAS * valBrute) / MAX_AMPLITUDE;
    }
}
//----------------------------------------------------------------------------
//  GENSIG_Execute
//  Envoie cycliquement chaque échantillon au DAC (Convertisseur Numérique-Analogique)
//  en utilisant l'interface SPI. La fonction gère également le passage à l'échantillon
//  suivant, en revenant à zéro lorsque le dernier échantillon est atteint.
//  
//  Paramètres :
//      Aucun
//  
//  Valeur de retour :
//      Aucun
//----------------------------------------------------------------------------

void GENSIG_Execute(void) {
    static uint16_t EchNb = 0; // Déclaration d'une variable statique pour garder la trace de l'échantillon courant

    // Écriture de l'échantillon courant dans le DAC via SPI
    SPI_WriteToDac(0, tableauValeursSignal[EchNb]);

    // Passage à l'échantillon suivant et gestion du débordement
    // Incrémente EchNb et utilise l'opérateur modulo pour revenir à 0 lorsque MAX_ECH est atteint
    EchNb = (uint16_t)((EchNb + 1) % MAX_ECH);
}