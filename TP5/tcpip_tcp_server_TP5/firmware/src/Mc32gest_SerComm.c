// Mc32Gest_SerComm.C
// Fonctions d'�mission et de r�ception des messages transmis via USB CDC
// Canevas TP4 SLO2 2015-2016

#include "app.h"
#include "Mc32gest_SerComm.h"
#include "system_config.h"
#include "Mc32gestI2cSeeprom.h"
#include <string.h>


// Fonction de r�ception d'un message
// Met � jour les param�tres du g�n�rateur � partir du message re�u
// Format du message attendu :
// !S=TF=0200A=10000O=+5000W=0#
// !S=PF=200A=10000O=-5000W=1#

// *****************************************************************************
// Fonction :
//    bool GetMessage(int8_t *TCPReadBuffer, S_ParamGen *pParam, bool *SaveTodo)
//
// R�sum� :
//    Analyse le message re�u via USB et met � jour les param�tres du g�n�rateur.
//
// Description :
//    Cette fonction extrait et met � jour les param�tres du g�n�rateur � partir
//    d'un message re�u via le port USB. Le message doit respecter un format
//    sp�cifique et commencer par '!'. Les diff�rents param�tres (forme du
//    signal, fr�quence, amplitude, offset et sauvegarde) sont extraits et 
//    convertis. Si une sauvegarde est demand�e, les param�tres sont �crits 
//    dans la SEEPROM.
//
// Param�tres :
//    - TCPReadBuffer : Pointeur vers le tampon contenant le message re�u via USB.
//    - pParam : Pointeur vers la structure de param�tres du g�n�rateur (S_ParamGen).
//    - SaveTodo : Pointeur vers un bool�en indiquant si les param�tres doivent �tre sauvegard�s.
//
// Retourne :
//    - true si le message est correctement analys� et les param�tres mis � jour.
//    - false si le message n'est pas correct
// *****************************************************************************

bool GetMessage(uint8_t *TCPReadBuffer, S_ParamGen *pParam, bool *SaveTodo) {
    //D�claration de variables
    char *pt_Forme = 0;
    char *pt_Frequence = 0;
    char *pt_Amplitude = 0;
    char *pt_Offset = 0;
    char *pt_Sauvegarde = 0;

    // Recherche des diff�rents param�tres dans le buffer re�u
    pt_Forme = strstr((char*) TCPReadBuffer, "S");
    pt_Frequence = strstr((char*) TCPReadBuffer, "F");
    pt_Amplitude = strstr((char*) TCPReadBuffer, "A");
    pt_Offset = strstr((char*) TCPReadBuffer, "O");
    pt_Sauvegarde = strstr((char*) TCPReadBuffer, "W");

    // V�rifie si le message commence par '!'
    if (TCPReadBuffer[0] == '!') // On peut remplacer '!' par une constante d�finie
    {
        // Identification de la forme du signal
        switch (pt_Forme[2])
        {
            case 'T':
                pParam->Forme = Triangle;
                break;
            case 'S':
                pParam->Forme = Sinus;
                break;
            case 'C':
                pParam->Forme = Carre;
                break;
            case 'D':
                pParam->Forme = DentDeScie;
                break;
            default:
                break;
        }

        // Mise � jour des param�tres � partir du message re�u
        pParam->Frequence = atoi(pt_Frequence + 2); // D�calage de 2 pour ignorer 'F='
        pParam->Amplitude = atoi(pt_Amplitude + 2); // D�calage de 2 pour ignorer 'A='
        pParam->Offset = atoi(pt_Offset + 2); // D�calage de 2 pour ignorer 'O='
        *SaveTodo = atoi(pt_Sauvegarde + 2); // D�calage de 2 pour ignorer 'W='

        // Si la sauvegarde est demand�e, �crire les param�tres dans la EEPROM
        if (*SaveTodo) {
            I2C_WriteSEEPROM((uint32_t*) pParam, MCP79411_EEPROM_BEG , sizeof (S_ParamGen));
        }

        return true; //La lecture a aboutie  
    } else {
        return false; //La lecture n'a pas aboutie
    }
}

// Fonction d'envoi d'un message
// Remplit le tampon d'�mission pour USB en fonction des param�tres du g�n�rateur
// Format du message envoy� :
// !S=TF=2000A=10000O=+5000WP=0#
// !S=TF=2000A=10000O=+5000WP=1# (ack de sauvegarde)

// *****************************************************************************
// Fonction :
//    void SendMessage(int8_t *TCPSendBuffer, S_ParamGen *pParam, bool Saved)
//
// R�sum� :
//    Formate et envoie un message via USB avec les param�tres du g�n�rateur.
//
// Description :
//    Cette fonction construit un message format� en cha�ne de caract�res avec
//    les param�tres actuels du g�n�rateur (forme du signal, fr�quence, amplitude,
//    offset et �tat de la sauvegarde) et l'�crit dans le tampon d'envoi USB.
//
// Param�tres :
//    - TCPSendBuffer : Pointeur vers le tampon o� le message format� sera �crit.
//    - pParam : Pointeur vers la structure de param�tres du g�n�rateur (S_ParamGen).
//    - Saved : Bool�en indiquant si la sauvegarde des param�tres a �t� effectu�e.
//
// Retourne :
//    - Rien (void).
// *****************************************************************************

void SendMessage(uint8_t *TCPSendBuffer, S_ParamGen *pParam, bool Saved) {
    char *indiceFormeEnvoie;

    // D�termine la forme du signal � envoyer
    switch (pParam->Forme) {
        case Triangle:
            indiceFormeEnvoie = "T";
            break;
        case Sinus:
            indiceFormeEnvoie = "S";
            break;
        case Carre:
            indiceFormeEnvoie = "C";
            break;
        case DentDeScie:
            indiceFormeEnvoie = "D";
            break;
        default:
            break;
    }

    // Formate le message � envoyer avec les param�tres du g�n�rateur
    sprintf((char*) TCPSendBuffer, "!S=%sF=%dA=%dO=%+dWP=%d#", indiceFormeEnvoie, pParam->Frequence, pParam->Amplitude, pParam->Offset, Saved);
}