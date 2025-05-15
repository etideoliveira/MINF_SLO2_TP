/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/common/sys_common.h"
#include "app.h"
#include "app_gen.h"
#include "system_definitions.h"
#include "bsp.h"
#include "GesPec12.h"
#include "Generateur.h"
#include "MenuGen.h"
#include <stdint.h>


#define COUNTER_3SEC 2999
#define COUNTER_10MS 9

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
 
void __ISR(_CHANGE_NOTICE_VECTOR, ipl1AUTO) _IntHandlerChangeNotification(void)
{
    /* TODO: Add code to process interrupt here */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE);
}

 

void __ISR(_TIMER_1_VECTOR, ipl3AUTO) IntHandlerDrvTmrInstance0(void)
{
    static uint16_t initCounter = 0;    //Compteur du temps d'initialisation
    static uint8_t stateCounter = 10;   //Compteur du changement d'�tat de la machine d'�tat du syst�me
    //Est-ce que le compteur du temps d'initialisation a d�pass� 3 secondes?
    if(initCounter <= COUNTER_3SEC)
    {
        //Incr�mentation du temps d'initialisation
        initCounter++;
    }
    else
    {
        //Appel de la fonction lisant les actions sur le bouton PEC12
        ScanPec12(PEC12_A, PEC12_B, PEC12_PB);
        //Est-ce que le compteur de la machine d'�tat a d�pass� 10ms?
        if(stateCounter > COUNTER_10MS)
        {
           //Remise � zero du compteur de la machine d'�tat
           stateCounter = 0;
           //Changement d'�tat de la machine d'�tat pour APP_STATE_SERVICE_TASKS
           APP_Gen_UpdateState(APP_GEN_STATE_SERVICE_TASKS);
        }      
        //Incr�mentation du compteur de la machine d'�tat
        stateCounter++;
    }
    //Remise � z�ro du flag de l'interruption
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_1);
}
void __ISR(_TIMER_3_VECTOR, ipl7AUTO) IntHandlerDrvTmrInstance1(void)
{
    //Allumage de la lED0 pour la dur�e d'ex�cution de GENSIG_Execute
    LED0_W = true;
    //Appel de la fonction �crivant les valeurs des �chantillons sur le DAC
    GENSIG_Execute();
    //Extinction de la lED0 pour la dur�e d'ex�cution de GENSIG_Execute
    LED0_W = false;
    //Remise � z�ro du flag de l'interruption
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
 void __ISR(_USB_1_VECTOR, ipl4AUTO) _IntHandlerUSBInstance0(void)
{
    DRV_USBFS_Tasks_ISR(sysObj.drvUSBObject);
}

/*******************************************************************************
 End of File
*/
