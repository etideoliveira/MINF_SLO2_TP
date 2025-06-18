// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion du g�n�rateur

// Pr�vu pour signal de 40 �chantillons
// Migration sur PIC32 30.04.2014 C. Huber

#include "Generateur.h"          // Inclusion du fichier d'en-t�te pour le g�n�rateur
#include "DefMenuGen.h"          // Inclusion du fichier d'en-t�te pour le menu du g�n�rateur
#include "Mc32gestSpiDac.h"      // Inclusion du fichier d'en-t�te pour la gestion du DAC SPI
#include "system_config.h"       // Inclusion du fichier d'en-t�te pour la configuration syst�me
#include "Mc32DriverLcd.h"       // Inclusion du fichier d'en-t�te pour le driver LCD
#include "Mc32gestI2cSeeprom.h"
// Variables globales
S_ParamGen structInterGENSIG; // Structure interm�diaire des param�tres du g�n�rateur
int32_t tableauValeursSignal[MAX_ECH]; // Tableau pour stocker les valeurs du signal

//----------------------------------------------------------------------------
//  GENSIG_Initialize
//  Initialise le g�n�rateur de signaux en r�cup�rant les param�tres sauvegard�s
//  ou en les initialisant par d�faut si aucune donn�e valide n'est trouv�e.
//  
//  Param�tres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les param�tres
//                           du g�n�rateur de signaux.
//----------------------------------------------------------------------------

void GENSIG_Initialize(S_ParamGen *pParam) {
    //Lecture des donn�es dans la flash et enregistrement dans la structure 
    I2C_ReadSEEPROM(&structInterGENSIG, MCP79411_EEPROM_BEG, sizeof (S_ParamGen));
    if (structInterGENSIG.Magic == MAGIC) {
        *pParam = structInterGENSIG; // Sauvegarde des valeurs r�cup�r�es
    } else {
        pParam->Forme = Sinus; // Forme du signal par d�faut : Sinus
        pParam->Frequence = F_MIN;    // Fr�quence par d�faut : minimum
        pParam->Amplitude = UN_VOLT_AMPLITUDE; // Amplitude par d�faut : 1 Volt
        pParam->Offset = 0;           // Offset par d�faut : 0
        pParam->Magic = MAGIC;        // Valeur magique pour identifier les donn�es
    }
}

//----------------------------------------------------------------------------
//  GENSIG_UpdatePeriode
//  Met � jour la p�riode d'�chantillonnage du g�n�rateur de signaux en
//  fonction de la fr�quence sp�cifi�e dans les param�tres.
//  
//  Param�tres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les param�tres
//                           du g�n�rateur de signaux.
//----------------------------------------------------------------------------

void GENSIG_UpdatePeriode(S_ParamGen *pParam) {
    static uint16_t compteurTimer3 = 0; // Compteur pour le timer 3
    compteurTimer3 = (uint16_t)(TRANSFORMATION_VALEUR_TIMER3 / pParam->Frequence);
    PLIB_TMR_Period16BitSet(TMR_ID_3, compteurTimer3); // Mise � jour de la p�riode du timer 3
}

//----------------------------------------------------------------------------
//  GENSIG_UpdateSignal
//  Met � jour les valeurs du signal en fonction de la forme, de l'amplitude
//  et de l'offset sp�cifi�s dans les param�tres. Les valeurs sont stock�es
//  dans le tableau de valeurs du signal.
//  
//  Param�tres :
//      S_ParamGen *pParam : Pointeur vers la structure contenant les param�tres
//                           du g�n�rateur de signaux.
//----------------------------------------------------------------------------

void GENSIG_UpdateSignal(S_ParamGen *pParam) {
    int32_t nbEchantillon; // Compteur d'�chantillons
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

    amplitude = (int32_t)pParam->Amplitude; // R�cup�ration de l'amplitude
    offset = (int32_t)((pParam->Offset * INVERSION) / DEUX); // Inversion de l'effet de l'offset
    amplitude_en_mV = amplitude / MAX_ECH; // Conversion de l'amplitude saisie par l'utilisateur en mV

    // Boucle pour g�n�rer les valeurs du signal
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
            case Carre: // Cas du signal carr�
                if ((MAX_ECH / DEUX) > nbEchantillon) {
                    valBrute = MOITIE_AMPLITUDE + offset + amplitude / DEUX;
                } else {
                    valBrute = MOITIE_AMPLITUDE + offset - amplitude / DEUX;
                }
                break;
            default:
                break;
        }
        // Gestion de l'�cr�tage
        if (valBrute > MAX_AMPLITUDE) {
            valBrute = MAX_AMPLITUDE; // Limite sup�rieure
        } else if (valBrute < 0) {
            valBrute = 0; // Limite inf�rieure
        }
        // Conversion pour passer de 10000 � 65535 
        tableauValeursSignal[nbEchantillon] = (VAL_MAX_PAS * valBrute) / MAX_AMPLITUDE;
    }
}
//----------------------------------------------------------------------------
//  GENSIG_Execute
//  Envoie cycliquement chaque �chantillon au DAC (Convertisseur Num�rique-Analogique)
//  en utilisant l'interface SPI. La fonction g�re �galement le passage � l'�chantillon
//  suivant, en revenant � z�ro lorsque le dernier �chantillon est atteint.
//  
//  Param�tres :
//      Aucun
//  
//  Valeur de retour :
//      Aucun
//----------------------------------------------------------------------------

void GENSIG_Execute(void) {
    static uint16_t EchNb = 0; // D�claration d'une variable statique pour garder la trace de l'�chantillon courant

    // �criture de l'�chantillon courant dans le DAC via SPI
    SPI_WriteToDac(0, tableauValeursSignal[EchNb]);

    // Passage � l'�chantillon suivant et gestion du d�bordement
    // Incr�mente EchNb et utilise l'op�rateur modulo pour revenir � 0 lorsque MAX_ECH est atteint
    EchNb = (uint16_t)((EchNb + 1) % MAX_ECH);
}