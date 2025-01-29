/******************************************************************************
 * Fichier : Mc32Gest_RS232.c
 * Objet   : Gestion de la communication série (RS232) avec CRC et FIFO
 *           - Initialisation des FIFOs de réception et d'émission
 *           - Lecture/écriture de messages structurés (vitesse, angle, CRC)
 *           - Gestion d'interruptions (erreurs, RX, TX)
 *
 * Auteur  : CHR / SCA 
 * Année   : 2017-2018 (Mise à jour des commentaires : 2023)
 * Eleve   : EDA;TCT / Année 2025
 *****************************************************************************/

#include <xc.h>
#include <sys/attribs.h>
#include "system_config/default/system_definitions.h"
#include "bsp.h"
#include "GesFifoTh32.h"
#include "Mc32gest_RS232.h"
#include "gestPWM.h"
#include "Mc32CalCrc16.h"

/*----------------------------------------------------------------------------*/
/*                  Descripteurs de FIFO (Réception et Émission)              */
/*----------------------------------------------------------------------------*/
S_fifo descrFifoRX; /**< Descripteur du FIFO de réception (RX).            */
S_fifo descrFifoTX; /**< Descripteur du FIFO d'émission (TX).             */

/*----------------------------------------------------------------------------*/
/*                          Initialisation FIFO et RTS                         */
/*----------------------------------------------------------------------------*/

/**
 * @brief Initialise les deux FIFOs (RX et TX) et positionne la ligne RTS.
 *        - FIFO RX et FIFO TX sont alloués localement.
 *        - RTS = 1 pour interdire l'émission distante (flow control).
 */
void InitFifoComm(void) {
    /* Allocation mémoire pour les deux FIFOs */
    static int8_t fifoRX[FIFO_RX_SIZE];
    static int8_t fifoTX[FIFO_TX_SIZE];

    /* Initialisation des FIFOs (RX et TX) */
    InitFifo(&descrFifoRX, FIFO_RX_SIZE, fifoRX, ZERO);
    InitFifo(&descrFifoTX, FIFO_TX_SIZE, fifoTX, ZERO);

    /* RTS = 1 => on bloque l'émission du périphérique distant */
    RS232_RTS = UN;
}

/*----------------------------------------------------------------------------*/
/*            Lecture d'un message complet depuis le FIFO de réception        */
/*----------------------------------------------------------------------------*/

/**
 * @brief Récupère un message complet (STX, Speed, Angle, CRC) dans le FIFO RX.
 *        Met à jour la structure PWM si le message est valide (CRC correct).
 *
 * @param[in,out] pData : Pointeur vers la structure S_pwmSettings
 *                        (vitesse, angle, etc.)
 * @return
 *         - 0 : Aucun message valide reçu => mode local
 *         - 1 : Message valide reçu => mode remote
 */
int GetMessage(S_pwmSettings *pData)
{
    //Variables
    StruMess RxMess;
    static int commStatus = ZERO;  //Etat de connexion
    static uint8_t nbrCycles = ZERO;   //Compteur de cycles
    uint8_t NbCharToRead;       //Nbr de caractere a lire
    int8_t RxC;                 //Reception caractere du fifo
    uint16_t ValCRC16 = 0xFFFF; //Pour le calcul du CRC
    
    // Traitement de réception à introduire ICI
    // Lecture et décodage fifo réception
    // ...
    
    //Lecture du nbr de caractere
    NbCharToRead = GetReadSize(&descrFifoRX);
    
    // Si NbCharToRead >= taille message alors traite (5 octets)
    if (NbCharToRead >= MESS_SIZE)
    {
        //Début du mesage
        GetCharFromFifo(&descrFifoRX, &RxC);
        
        //Si le carctere  = 0xAA
        if (RxC == STX_code)  
        {
            //Stockage de Start
            RxMess.Start = RxC;
            
            //Stockage de la vitesse
            GetCharFromFifo(&descrFifoRX, &RxC);         
            RxMess.Speed = RxC;
            
            //Stockage de l'angle
            GetCharFromFifo(&descrFifoRX, &RxC);
            RxMess.Angle = RxC;
            
            //Stockage du CRC MSB
            GetCharFromFifo(&descrFifoRX, &RxC);
            RxMess.MsbCrc = RxC;
            
            //Stockage du CRC LSB
            GetCharFromFifo(&descrFifoRX, &RxC);
            RxMess.LsbCrc = RxC;
            
            //Calcul du CRC sur tout le message
            ValCRC16 = updateCRC16(ValCRC16, STX_code);
            ValCRC16 = updateCRC16(ValCRC16, RxMess.Speed);
            ValCRC16 = updateCRC16(ValCRC16, RxMess.Angle);
            ValCRC16 = updateCRC16(ValCRC16, RxMess.MsbCrc);
            ValCRC16 = updateCRC16(ValCRC16, RxMess.LsbCrc);
            
            //Si le CRC ne vaut pas 0, aucun champs serra mis à jour
            if (ValCRC16 == ZERO) 
            {
                //Indicateur de reception à 1
                commStatus = UN; 
                
                //Remet a 0 le nbr de cycle
                nbrCycles = ZERO; 
                
                //Mettre à jour les champs dans la structure
                pData->SpeedSetting = RxMess.Speed;
                pData->absSpeed = abs(RxMess.Speed);
                pData->AngleSetting = RxMess.Angle;
            }
            else
            {
              //S'il y a une erreur.
              BSP_LEDToggle(BSP_LED_6); // Toggle Led6     
            }
        }
    }
    else
    {      
        //Attendre 10 cycles avant de switcher en mode local
        
        //Si 10 cycles passées 
        if (nbrCycles > NEUF) 
        {
            //Remet a 0 le nbr de cycle
            nbrCycles = ZERO; 
            
            //Indicateur de reception à 0
            commStatus = ZERO;
         }
        
        //Incrémentation du nbr de cycles  
        nbrCycles++;  
    }
    
    // Gestion controle de flux de la réception
    if(GetWriteSpace ( &descrFifoRX) >= (2*MESS_SIZE)) {
        // autorise émission par l'autre
        RS232_RTS = ZERO;
    }
    return commStatus;
}
/*----------------------------------------------------------------------------*/
/*           Construction et mise en FIFO d?un message à envoyer (TX)         */
/*----------------------------------------------------------------------------*/

/**
 * @brief Construit un message complet (STX, Speed, Angle, CRC) à partir de pData,
 *        et l?ajoute dans le FIFO d?émission (TX).
 *
 * @param[in] pData : Pointeur vers la structure S_pwmSettings
 *                    contenant les valeurs de vitesse et d?angle.
 */
void SendMessage(S_pwmSettings *pData)
{
   
    int8_t freeSize;                //Réceptionne la place disponible en écriture
    uint16_t ValCRC16Tx = 0xFFFF;     //Calcul CRC

    //Test si place pour ecrire 1 message
    freeSize = GetWriteSpace(&descrFifoTX);       
    if (freeSize >= MESS_SIZE)
    {  
        //Compose le message
        //Calcul CRC
        ValCRC16Tx = updateCRC16(ValCRC16Tx, STX_code);
        ValCRC16Tx = updateCRC16(ValCRC16Tx, pData->SpeedSetting);
        ValCRC16Tx = updateCRC16(ValCRC16Tx, pData->AngleSetting);

        // Place les éléments du message dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, STX_code); // Place le byte de départ dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, pData->SpeedSetting); // Place la vitesse dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, pData->AngleSetting); // Place l'angle dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, (ValCRC16Tx & 0xFF00)>>8); // Place le CRC16 (partie haute) dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, ValCRC16Tx & 0x00FF); // Place le CRC16 (partie basse) dans le buffer FIFO
    }

    // Gestion du controle de flux
    // si on a un caractère à envoyer et que CTS = 0
    freeSize = GetReadSize(&descrFifoTX);
    if ((RS232_CTS == ZERO) && (freeSize > ZERO))
    {
        // Autorise int émission    
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);                
    }

}

/*----------------------------------------------------------------------------*/
/*                     Routine d'interruption USART1                          */
/*----------------------------------------------------------------------------*/

/**
 * @brief Gère les interruptions de l'UART1 : erreurs, réception et émission.
 *
 * Vecteur      : _UART_1_VECTOR
 * Priorité IPL : ipl5AUTO
 */
void __ISR(_UART_1_VECTOR, ipl5AUTO) _IntHandlerDrvUsartInstance0(void) {
    USART_ERROR currentError;
    bool hwBufferFull;
    uint8_t rxAvailable;
    int8_t oneByte;
    uint8_t neededToSend;

    /* Indicateur de début d'interruption : LED3 OFF */
    LED3_W = UN;

    /* --- Gestion des erreurs (USART_1_ERROR) --- */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_ERROR) &&
            PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_ERROR)) {
        /* Efface le flag d'interruption d'erreur */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);

        /* Récupère l'erreur courante (overrun, framing, parité) */
        currentError = PLIB_USART_ErrorsGet(USART_ID_1);

        /* Si overrun => on le clear */
        if ((currentError & USART_ERROR_RECEIVER_OVERRUN) == USART_ERROR_RECEIVER_OVERRUN) {
            PLIB_USART_ReceiverOverrunErrorClear(USART_ID_1);
        }

        /* Vider le buffer RX matériel en cas de données résiduelles */
        while (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)) {
            (void) PLIB_USART_ReceiverByteReceive(USART_ID_1);
        }
    }

    /* --- Gestion de la réception (USART_1_RECEIVE) --- */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE) &&
            PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_RECEIVE)) {
        currentError = PLIB_USART_ErrorsGet(USART_ID_1);

        /* Pas d'erreurs de framing/parité/overrun ? */
        if ((currentError & (USART_ERROR_PARITY
                | USART_ERROR_FRAMING
                | USART_ERROR_RECEIVER_OVERRUN)) == ZERO) {
            /* Transfert des données du buffer RX matériel vers le FIFO RX */
            rxAvailable = (uint8_t) PLIB_USART_ReceiverDataIsAvailable(USART_ID_1);
            while (rxAvailable) {
                oneByte = (int8_t) PLIB_USART_ReceiverByteReceive(USART_ID_1);
                PutCharInFifo(&descrFifoRX, oneByte);
                rxAvailable = (uint8_t) PLIB_USART_ReceiverDataIsAvailable(USART_ID_1);
            }

            /* Toggle LED4 pour indiquer une réception */
            LED4_W = !LED4_R;

            /* Nettoyage du flag d'interruption RX */
            PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        } else {
            /* Si overrun => clear */
            if ((currentError & USART_ERROR_RECEIVER_OVERRUN) == USART_ERROR_RECEIVER_OVERRUN) {
                PLIB_USART_ReceiverOverrunErrorClear(USART_ID_1);
            }
        }

        /* Contrôle de flux : si le FIFO RX est presque plein => RTS = 1 */
        if (GetWriteSpace(&descrFifoRX) <= RX_FIFO_STOP_THRESHOLD) {
            RS232_RTS = UN;
        }
    }

    /* --- Gestion de l'émission (USART_1_TRANSMIT) --- */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT) &&
            PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT)) {
        neededToSend = (uint8_t) GetReadSize(&descrFifoTX);
        hwBufferFull = PLIB_USART_TransmitterBufferIsFull(USART_ID_1);

        /* Tant qu'il y a des données et que CTS=0 et que le buffer matériel TX n'est pas plein */
        if ((RS232_CTS == ZERO) && (neededToSend > ZERO) && (!hwBufferFull)) {
            do {
                /* Envoi d'un octet depuis le FIFO TX vers le matériel */
                GetCharFromFifo(&descrFifoTX, &oneByte);
                PLIB_USART_TransmitterByteSend(USART_ID_1, (uint8_t) oneByte);

                /* Mise à jour des conditions */
                neededToSend = (uint8_t) GetReadSize(&descrFifoTX);
                hwBufferFull = PLIB_USART_TransmitterBufferIsFull(USART_ID_1);

            } while ((RS232_CTS == ZERO) && (neededToSend > ZERO) && (!hwBufferFull));

            /* Plus rien à émettre => on désactive l'interruption TX pour économiser des ressources */
            if (GetReadSize(&descrFifoTX) == ZERO) {
                PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
            }
        }// else {
        /* Sinon, on coupe l'interruption d'émission */
        //     PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
        //}

        /* Nettoyage du flag d'interruption TX */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);

        /* Toggle LED5 pour indiquer l'activité TX */
        LED5_W = !LED5_R;
    }

    /* Fin d'interruption : LED3 ON */
    LED3_W = ZERO;
}
