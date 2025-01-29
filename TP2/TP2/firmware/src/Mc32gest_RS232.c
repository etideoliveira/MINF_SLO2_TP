/******************************************************************************
 * Fichier : Mc32Gest_RS232.c
 * Objet   : Gestion de la communication s�rie (RS232) avec CRC et FIFO
 *           - Initialisation des FIFOs de r�ception et d'�mission
 *           - Lecture/�criture de messages structur�s (vitesse, angle, CRC)
 *           - Gestion d'interruptions (erreurs, RX, TX)
 *
 * Auteur  : CHR / SCA 
 * Ann�e   : 2017-2018 (Mise � jour des commentaires : 2023)
 * Eleve   : EDA;TCT / Ann�e 2025
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
/*                  Descripteurs de FIFO (R�ception et �mission)              */
/*----------------------------------------------------------------------------*/
S_fifo descrFifoRX; /**< Descripteur du FIFO de r�ception (RX).            */
S_fifo descrFifoTX; /**< Descripteur du FIFO d'�mission (TX).             */

/*----------------------------------------------------------------------------*/
/*                          Initialisation FIFO et RTS                         */
/*----------------------------------------------------------------------------*/

/**
 * @brief Initialise les deux FIFOs (RX et TX) et positionne la ligne RTS.
 *        - FIFO RX et FIFO TX sont allou�s localement.
 *        - RTS = 1 pour interdire l'�mission distante (flow control).
 */
void InitFifoComm(void) {
    /* Allocation m�moire pour les deux FIFOs */
    static int8_t fifoRX[FIFO_RX_SIZE];
    static int8_t fifoTX[FIFO_TX_SIZE];

    /* Initialisation des FIFOs (RX et TX) */
    InitFifo(&descrFifoRX, FIFO_RX_SIZE, fifoRX, ZERO);
    InitFifo(&descrFifoTX, FIFO_TX_SIZE, fifoTX, ZERO);

    /* RTS = 1 => on bloque l'�mission du p�riph�rique distant */
    RS232_RTS = UN;
}

/*----------------------------------------------------------------------------*/
/*            Lecture d'un message complet depuis le FIFO de r�ception        */
/*----------------------------------------------------------------------------*/

/**
 * @brief R�cup�re un message complet (STX, Speed, Angle, CRC) dans le FIFO RX.
 *        Met � jour la structure PWM si le message est valide (CRC correct).
 *
 * @param[in,out] pData : Pointeur vers la structure S_pwmSettings
 *                        (vitesse, angle, etc.)
 * @return
 *         - 0 : Aucun message valide re�u => mode local
 *         - 1 : Message valide re�u => mode remote
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
    
    // Traitement de r�ception � introduire ICI
    // Lecture et d�codage fifo r�ception
    // ...
    
    //Lecture du nbr de caractere
    NbCharToRead = GetReadSize(&descrFifoRX);
    
    // Si NbCharToRead >= taille message alors traite (5 octets)
    if (NbCharToRead >= MESS_SIZE)
    {
        //D�but du mesage
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
            
            //Si le CRC ne vaut pas 0, aucun champs serra mis � jour
            if (ValCRC16 == ZERO) 
            {
                //Indicateur de reception � 1
                commStatus = UN; 
                
                //Remet a 0 le nbr de cycle
                nbrCycles = ZERO; 
                
                //Mettre � jour les champs dans la structure
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
        
        //Si 10 cycles pass�es 
        if (nbrCycles > NEUF) 
        {
            //Remet a 0 le nbr de cycle
            nbrCycles = ZERO; 
            
            //Indicateur de reception � 0
            commStatus = ZERO;
         }
        
        //Incr�mentation du nbr de cycles  
        nbrCycles++;  
    }
    
    // Gestion controle de flux de la r�ception
    if(GetWriteSpace ( &descrFifoRX) >= (2*MESS_SIZE)) {
        // autorise �mission par l'autre
        RS232_RTS = ZERO;
    }
    return commStatus;
}
/*----------------------------------------------------------------------------*/
/*           Construction et mise en FIFO d?un message � envoyer (TX)         */
/*----------------------------------------------------------------------------*/

/**
 * @brief Construit un message complet (STX, Speed, Angle, CRC) � partir de pData,
 *        et l?ajoute dans le FIFO d?�mission (TX).
 *
 * @param[in] pData : Pointeur vers la structure S_pwmSettings
 *                    contenant les valeurs de vitesse et d?angle.
 */
void SendMessage(S_pwmSettings *pData)
{
   
    int8_t freeSize;                //R�ceptionne la place disponible en �criture
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

        // Place les �l�ments du message dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, STX_code); // Place le byte de d�part dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, pData->SpeedSetting); // Place la vitesse dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, pData->AngleSetting); // Place l'angle dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, (ValCRC16Tx & 0xFF00)>>8); // Place le CRC16 (partie haute) dans le buffer FIFO
        PutCharInFifo(&descrFifoTX, ValCRC16Tx & 0x00FF); // Place le CRC16 (partie basse) dans le buffer FIFO
    }

    // Gestion du controle de flux
    // si on a un caract�re � envoyer et que CTS = 0
    freeSize = GetReadSize(&descrFifoTX);
    if ((RS232_CTS == ZERO) && (freeSize > ZERO))
    {
        // Autorise int �mission    
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);                
    }

}

/*----------------------------------------------------------------------------*/
/*                     Routine d'interruption USART1                          */
/*----------------------------------------------------------------------------*/

/**
 * @brief G�re les interruptions de l'UART1 : erreurs, r�ception et �mission.
 *
 * Vecteur      : _UART_1_VECTOR
 * Priorit� IPL : ipl5AUTO
 */
void __ISR(_UART_1_VECTOR, ipl5AUTO) _IntHandlerDrvUsartInstance0(void) {
    USART_ERROR currentError;
    bool hwBufferFull;
    uint8_t rxAvailable;
    int8_t oneByte;
    uint8_t neededToSend;

    /* Indicateur de d�but d'interruption : LED3 OFF */
    LED3_W = UN;

    /* --- Gestion des erreurs (USART_1_ERROR) --- */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_ERROR) &&
            PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_ERROR)) {
        /* Efface le flag d'interruption d'erreur */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);

        /* R�cup�re l'erreur courante (overrun, framing, parit�) */
        currentError = PLIB_USART_ErrorsGet(USART_ID_1);

        /* Si overrun => on le clear */
        if ((currentError & USART_ERROR_RECEIVER_OVERRUN) == USART_ERROR_RECEIVER_OVERRUN) {
            PLIB_USART_ReceiverOverrunErrorClear(USART_ID_1);
        }

        /* Vider le buffer RX mat�riel en cas de donn�es r�siduelles */
        while (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)) {
            (void) PLIB_USART_ReceiverByteReceive(USART_ID_1);
        }
    }

    /* --- Gestion de la r�ception (USART_1_RECEIVE) --- */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE) &&
            PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_RECEIVE)) {
        currentError = PLIB_USART_ErrorsGet(USART_ID_1);

        /* Pas d'erreurs de framing/parit�/overrun ? */
        if ((currentError & (USART_ERROR_PARITY
                | USART_ERROR_FRAMING
                | USART_ERROR_RECEIVER_OVERRUN)) == ZERO) {
            /* Transfert des donn�es du buffer RX mat�riel vers le FIFO RX */
            rxAvailable = (uint8_t) PLIB_USART_ReceiverDataIsAvailable(USART_ID_1);
            while (rxAvailable) {
                oneByte = (int8_t) PLIB_USART_ReceiverByteReceive(USART_ID_1);
                PutCharInFifo(&descrFifoRX, oneByte);
                rxAvailable = (uint8_t) PLIB_USART_ReceiverDataIsAvailable(USART_ID_1);
            }

            /* Toggle LED4 pour indiquer une r�ception */
            LED4_W = !LED4_R;

            /* Nettoyage du flag d'interruption RX */
            PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        } else {
            /* Si overrun => clear */
            if ((currentError & USART_ERROR_RECEIVER_OVERRUN) == USART_ERROR_RECEIVER_OVERRUN) {
                PLIB_USART_ReceiverOverrunErrorClear(USART_ID_1);
            }
        }

        /* Contr�le de flux : si le FIFO RX est presque plein => RTS = 1 */
        if (GetWriteSpace(&descrFifoRX) <= RX_FIFO_STOP_THRESHOLD) {
            RS232_RTS = UN;
        }
    }

    /* --- Gestion de l'�mission (USART_1_TRANSMIT) --- */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT) &&
            PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT)) {
        neededToSend = (uint8_t) GetReadSize(&descrFifoTX);
        hwBufferFull = PLIB_USART_TransmitterBufferIsFull(USART_ID_1);

        /* Tant qu'il y a des donn�es et que CTS=0 et que le buffer mat�riel TX n'est pas plein */
        if ((RS232_CTS == ZERO) && (neededToSend > ZERO) && (!hwBufferFull)) {
            do {
                /* Envoi d'un octet depuis le FIFO TX vers le mat�riel */
                GetCharFromFifo(&descrFifoTX, &oneByte);
                PLIB_USART_TransmitterByteSend(USART_ID_1, (uint8_t) oneByte);

                /* Mise � jour des conditions */
                neededToSend = (uint8_t) GetReadSize(&descrFifoTX);
                hwBufferFull = PLIB_USART_TransmitterBufferIsFull(USART_ID_1);

            } while ((RS232_CTS == ZERO) && (neededToSend > ZERO) && (!hwBufferFull));

            /* Plus rien � �mettre => on d�sactive l'interruption TX pour �conomiser des ressources */
            if (GetReadSize(&descrFifoTX) == ZERO) {
                PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
            }
        }// else {
        /* Sinon, on coupe l'interruption d'�mission */
        //     PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
        //}

        /* Nettoyage du flag d'interruption TX */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);

        /* Toggle LED5 pour indiquer l'activit� TX */
        LED5_W = !LED5_R;
    }

    /* Fin d'interruption : LED3 ON */
    LED3_W = ZERO;
}
