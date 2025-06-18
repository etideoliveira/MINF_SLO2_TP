// Mc32Gest_SerComm.C
// Fonctions d'émission et de réception des messages transmis via USB CDC
// Canevas TP4 SLO2 2015-2016

#include "app.h"
#include "Mc32gest_SerComm.h"
#include "system_config.h"
#include "Mc32gestI2cSeeprom.h"
#include <string.h>


// Fonction de réception d'un message
// Met à jour les paramètres du générateur à partir du message reçu
// Format du message attendu :
// !S=TF=0200A=10000O=+5000W=0#
// !S=PF=200A=10000O=-5000W=1#

// *****************************************************************************
// Fonction :
//    bool GetMessage(int8_t *TCPReadBuffer, S_ParamGen *pParam, bool *SaveTodo)
//
// Résumé :
//    Analyse le message reçu via USB et met à jour les paramètres du générateur.
//
// Description :
//    Cette fonction extrait et met à jour les paramètres du générateur à partir
//    d'un message reçu via le port USB. Le message doit respecter un format
//    spécifique et commencer par '!'. Les différents paramètres (forme du
//    signal, fréquence, amplitude, offset et sauvegarde) sont extraits et 
//    convertis. Si une sauvegarde est demandée, les paramètres sont écrits 
//    dans la SEEPROM.
//
// Paramètres :
//    - TCPReadBuffer : Pointeur vers le tampon contenant le message reçu via USB.
//    - pParam : Pointeur vers la structure de paramètres du générateur (S_ParamGen).
//    - SaveTodo : Pointeur vers un booléen indiquant si les paramètres doivent être sauvegardés.
//
// Retourne :
//    - true si le message est correctement analysé et les paramètres mis à jour.
//    - false si le message n'est pas correct
// *****************************************************************************

bool GetMessage(uint8_t *TCPReadBuffer, S_ParamGen *pParam, bool *SaveTodo) {
    //Déclaration de variables
    char *pt_Forme = 0;
    char *pt_Frequence = 0;
    char *pt_Amplitude = 0;
    char *pt_Offset = 0;
    char *pt_Sauvegarde = 0;

    // Recherche des différents paramètres dans le buffer reçu
    pt_Forme = strstr((char*) TCPReadBuffer, "S");
    pt_Frequence = strstr((char*) TCPReadBuffer, "F");
    pt_Amplitude = strstr((char*) TCPReadBuffer, "A");
    pt_Offset = strstr((char*) TCPReadBuffer, "O");
    pt_Sauvegarde = strstr((char*) TCPReadBuffer, "W");

    // Vérifie si le message commence par '!'
    if (TCPReadBuffer[0] == '!') // On peut remplacer '!' par une constante définie
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

        // Mise à jour des paramètres à partir du message reçu
        pParam->Frequence = atoi(pt_Frequence + 2); // Décalage de 2 pour ignorer 'F='
        pParam->Amplitude = atoi(pt_Amplitude + 2); // Décalage de 2 pour ignorer 'A='
        pParam->Offset = atoi(pt_Offset + 2); // Décalage de 2 pour ignorer 'O='
        *SaveTodo = atoi(pt_Sauvegarde + 2); // Décalage de 2 pour ignorer 'W='

        // Si la sauvegarde est demandée, écrire les paramètres dans la EEPROM
        if (*SaveTodo) {
            I2C_WriteSEEPROM((uint32_t*) pParam, MCP79411_EEPROM_BEG , sizeof (S_ParamGen));
        }

        return true; //La lecture a aboutie  
    } else {
        return false; //La lecture n'a pas aboutie
    }
}

// Fonction d'envoi d'un message
// Remplit le tampon d'émission pour USB en fonction des paramètres du générateur
// Format du message envoyé :
// !S=TF=2000A=10000O=+5000WP=0#
// !S=TF=2000A=10000O=+5000WP=1# (ack de sauvegarde)

// *****************************************************************************
// Fonction :
//    void SendMessage(int8_t *TCPSendBuffer, S_ParamGen *pParam, bool Saved)
//
// Résumé :
//    Formate et envoie un message via USB avec les paramètres du générateur.
//
// Description :
//    Cette fonction construit un message formaté en chaîne de caractères avec
//    les paramètres actuels du générateur (forme du signal, fréquence, amplitude,
//    offset et état de la sauvegarde) et l'écrit dans le tampon d'envoi USB.
//
// Paramètres :
//    - TCPSendBuffer : Pointeur vers le tampon où le message formaté sera écrit.
//    - pParam : Pointeur vers la structure de paramètres du générateur (S_ParamGen).
//    - Saved : Booléen indiquant si la sauvegarde des paramètres a été effectuée.
//
// Retourne :
//    - Rien (void).
// *****************************************************************************

void SendMessage(uint8_t *TCPSendBuffer, S_ParamGen *pParam, bool Saved) {
    char *indiceFormeEnvoie;

    // Détermine la forme du signal à envoyer
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

    // Formate le message à envoyer avec les paramètres du générateur
    sprintf((char*) TCPSendBuffer, "!S=%sF=%dA=%dO=%+dWP=%d#", indiceFormeEnvoie, pParam->Frequence, pParam->Amplitude, pParam->Offset, Saved);
}