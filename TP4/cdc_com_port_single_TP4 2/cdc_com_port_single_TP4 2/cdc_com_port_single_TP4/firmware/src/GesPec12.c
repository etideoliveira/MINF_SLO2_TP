// GesPec12.c  Canevas pour r�alisation  
// C. HUBER    09/02/2015

// Fonctions pour la gestion du Pec12
//
//
// Principe : Il est n�cessaire d'appeler cycliquement la fonction ScanPec12
//            avec un cycle de 1 ms
//
//  Pour la gestion du Pec12, il y a 9 fonctions � disposition :
//       Pec12IsPlus       true indique un nouveau incr�ment
//       Pec12IsMinus      true indique un nouveau d�cr�ment
//       Pec12IsOK         true indique action OK
//       Pec12IsESC        true indique action ESC
//       Pec12NoActivity   true indique abscence d'activit� sur PEC12
//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incr�ment
//       Pec12ClearMinus   annule indication de d�cr�ment
//       Pec12ClearOK      annule indication action OK
//       Pec12ClearESC     annule indication action ESC
//
//
//---------------------------------------------------------------------------


// d�finitions des types qui seront utilis�s dans cette application

#include "GesPec12.h"
#include "Mc32Debounce.h"
#include "Mc32DriverLcd.h"
#include "MenuGen.h"

// Descripteur des sinaux
S_SwitchDescriptor DescrA;
S_SwitchDescriptor DescrB;
S_SwitchDescriptor DescrPB;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Principe utilisation des fonctions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
// ScanPec12 (bool ValA, bool ValB, bool ValPB)
//              Routine effectuant la gestion du Pec12
//              recoit la valeur des signaux et du boutons
//
// s'appuie sur le descripteur global.
// Apr�s l'appel le descripteur est mis � jour

// Comportement du PEC12
// =====================

// Attention 1 cran g�n�re une pulse compl�te (les 4 combinaisons)
// D'ou traitement uniquement au flanc descendant de B

// Dans le sens horaire CW:
//     __________                      ________________
// B:            |____________________|
//     ___________________                       _________
// A:                     |_____________________|                    

// Dans le sens anti-horaire CCW:
//     ____________________                      _________
// B:                      |____________________|
//     __________                       __________________
// A:            |_____________________|        

/**
 * Function name :ScanPec12
 * @author 
 * @date 
 *
 * @brief Analyse l'�tat des signaux provenant d'un encodeur rotatif (PEC12) et d'un bouton poussoir, g�re l'antirebond et d�tecte les actions de l'utilisateur.
 * 
 * Cette fonction analyse l'�tat des signaux provenant d'un encodeur rotatif (PEC12) et d'un bouton poussoir, effectue le traitement de l'antirebond et d�tecte 
 * les actions de l'utilisateur telles que les rotations dans le sens horaire ou anti-horaire, les pressions et les rel�chements sur le bouton poussoir. 
 * Elle g�re �galement l'activation et la d�sactivation de la r�tro�clairage de l'�cran LCD en fonction de l'activit� de l'utilisateur.
 * 
 * @param ValA �tat du signal A de l'encodeur rotatif.
 * @param ValB �tat du signal B de l'encodeur rotatif.
 * @param ValPB �tat du bouton poussoir de l'encodeur rotatif.
 * 
 * @return  Aucune valeur de retour.
 */

void ScanPec12 (bool ValA, bool ValB, bool ValPB)
{    
   // Traitement de l'antirebond sur A, B et PB
   DoDebounce (&DescrA, ValA);
   DoDebounce (&DescrB, ValB);
   DoDebounce (&DescrPB, ValPB);
   //Est-ce qu'il y a un flanc descendant sur le signal B?
   if(DebounceIsPressed(&DescrB))
   {
       //Est-ce qu'il y a une activit� de l'utilisateur sur le bouton PEC12?
       if(Pec12.NoActivity == true)
       {
            //Allumage de la backlight du LCD
            lcd_bl_on(); 
            //Changement de l'�tat de la variable d'activit� sur le PEC12
            Pec12.NoActivity = false;
       }
       //Test de l'�tat du signal A
       if(DebounceGetInput(&DescrA))
       {
           //Incr�ment sens horaire d�tect� 
           Pec12.Dec = false;
           Pec12.Inc = true;
       }
       
       else 
       {
           //Incr�ment sens anti-horaire d�tect� 
           Pec12.Dec = true;
           Pec12.Inc = false;
       }
       //Remise � z�ro de la variable d'inactivit� sur le PEC12
       Pec12.InactivityDuration = 0;
   }
   //Est-ce qu'il y a un flanc descendant sur le signal PB?
   if(DebounceIsPressed(&DescrPB))
   {
       //Est-ce qu'il y a une activit� de l'utilisateur sur le bouton PEC12?
       if(Pec12.NoActivity == true)
       {
           //Allumage de la backlight du LCD
            lcd_bl_on(); 
            //Changement de l'�tat de la variable d'activit� sur le PEC12
            Pec12.NoActivity = false;
       }
       //Incr�mentation de la dur�e de pression sur le PEC12
       Pec12.PressDuration++;
       //Test de l'�tat du signal PB
       if(DebounceGetInput(&DescrPB))
       {//Remise � z�ro du flag du bouton S9
           
           DebounceClearPressed(&DescrPB);
           //Est ce que le PEC12 a �t� press� plus de 500ms?
           if(Pec12.PressDuration >= PRESSDURATIONESC)
           {
               //Action escape detect�e
               Pec12.ESC = true;
               Pec12.OK = false;
               //Remise � z�ro de la dur�e de pression sur le PEC12
               Pec12.PressDuration = 0;
           }
           else
           {
               //Action escape detect�e
               Pec12.OK = true;
               Pec12.ESC = false;
               //Remise � z�ro de la dur�e de pression sur le PEC12
               Pec12.PressDuration = 0;
           }            
       }
       //Remise � z�ro de la variable d'inactivit� sur le PEC12
       Pec12.InactivityDuration = 0;
   }
   //Est-ce qu'il y a une inactivit� de l'utilisateur durant plus de 5sec?
   if(Pec12.InactivityDuration < INACTIVITYDURATIONMAX)
   {
       //Incr�mentation � z�ro de la variable d'inactivit� sur le PEC12
       Pec12.InactivityDuration++;
   }
   else
   {
       // Inactivit� de l'utilisateur detect�e
       Pec12.NoActivity = true;
       //Remise � z�ro de la variable d'inactivit� sur le PEC12
       Pec12.InactivityDuration = 0;
       //Extinction de la backlight du LCD
       lcd_bl_off();
   }
   
   //Clear des flag de la fonction
   DebounceClearPressed(&DescrA);
   DebounceClearPressed(&DescrB);
   DebounceClearReleased(&DescrPB);

 } // ScanPec12

/**
 * Function name :Pec12Init
 * @author 
 * @date 
 *
 * @brief Initialise les descripteurs de touches et la structure de gestion de l'encodeur rotatif PEC12.
 * 
 * Cette fonction effectue l'initialisation des descripteurs de touches pour les signaux A, B et PB de 
 * l'encodeur rotatif PEC12, ainsi que l'initialisation de la structure de gestion associ�e � cet encodeur. 
 * Elle met tous les �v�nements de l'encodeur et les indicateurs de l'activit� � z�ro, pr�ts � �tre mis � jour 
 * par les fonctions de gestion des �v�nements.
 *  
 * @return  Aucune valeur de retour.
 */

void Pec12Init (void)
{
   // Initialisation des descripteurs de touches Pec12
   DebounceInit(&DescrA);
   DebounceInit(&DescrB);
   DebounceInit(&DescrPB);
   
   // Init de la structure PEc12
    Pec12.Inc = false;             // �v�nement incr�ment  
    Pec12.Dec = false;             // �v�nement d�cr�ment 
    Pec12.OK = false;              // �v�nement action OK
    Pec12.ESC = false;             // �v�nement action ESC
    Pec12.NoActivity = false;      // Indication d'activit�
    Pec12.PressDuration = false;   // Pour dur�e pression du P.B.
    Pec12.InactivityDuration = false; // Dur�e inactivit�
  
 } // Pec12Init





//       Pec12IsPlus       true indique un nouveau incr�ment
bool Pec12IsPlus    (void) {
   return (Pec12.Inc);
}

//       Pec12IsMinus      true indique un nouveau d�cr�ment
bool Pec12IsMinus    (void) {
   return (Pec12.Dec);
}

//       Pec12IsOK         true indique action OK
bool Pec12IsOK    (void) {
   return (Pec12.OK);
}

//       Pec12IsESC        true indique action ESC
bool Pec12IsESC    (void) {
   return (Pec12.ESC);
}

//       Pec12NoActivity   true indique abscence d'activit� sur PEC12
bool Pec12NoActivity    (void) {
   return (Pec12.NoActivity);
}

//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incr�ment
void Pec12ClearPlus   (void) {
   Pec12.Inc = 0;
}

//       Pec12ClearMinus   annule indication de d�cr�ment
void Pec12ClearMinus   (void) {
   Pec12.Dec = 0;
}

//       Pec12ClearOK      annule indication action OK
void Pec12ClearOK   (void) {
   Pec12.OK = 0;
}

//       Pec12ClearESC     annule indication action ESC
void Pec12ClearESC   (void) {
   Pec12.ESC = 0;
}

void Pec12ClearInactivity   (void) {
  Pec12.NoActivity = 0;
  Pec12.InactivityDuration = 0;
}


