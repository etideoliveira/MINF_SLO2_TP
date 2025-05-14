using System;
using System.IO.Ports;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppCsTp2Pwm
{
    // Classe de base abstraite d’un signal (définition commune aux signaux envoyés et reçus)
    public class Signal
    {
        // Attributs protégés accessibles dans les classes héritées
        protected ushort m_amplitude;
        protected ushort m_frequence;
        protected short m_offset;
        protected string[] m_tb_signal;

        // Accesseurs publics (encapsulation)
        public ushort Amplitude
        {
            get => m_amplitude;
            set => m_amplitude = value;
        }

        public ushort Frequence
        {
            get => m_frequence;
            set => m_frequence = value;
        }

        public short Offset
        {
            get => m_offset;
            set => m_offset = value;
        }

        public string[] TbSignal
        {
            get => m_tb_signal;
            set => m_tb_signal = value;
        }

        // Méthodes virtuelles à redéfinir dans les sous-classes
        public virtual int EnvoyerSignal() => 0;
        public virtual int ReceptionSignal() => 0;
    }

    // Classe pour gérer l’envoi d’un signal via une trame UART
    public class SendSignal : Signal
    {
        private bool m_sauvegarde;               // indique si on sauvegarde le signal ou non
        private char[] m_tb_trameSend;           // trame convertie en tableau de caractères

        public SerialPort PortSerie { get; set; } // liaison série utilisée pour l’envoi

        // Propriétés publiques
        public bool Sauvegarde
        {
            get => m_sauvegarde;
            set => m_sauvegarde = value;
        }

        public char[] TrameSend
        {
            get => m_tb_trameSend;
            set => m_tb_trameSend = value;
        }

        public int FormeIndex { get; set; } // index de la forme sélectionnée (0 à 3)

        private readonly char[] formes = { 'C', 'D', 'S', 'T' }; // codes de formes

        // Construit une trame texte à partir des valeurs de signal
        public string CoderTrame()
        {
            string offsetStr = Offset < 0 ? Offset.ToString("D2") : "+" + Offset.ToString("D2");

            string trame = string.Format("!S={0}F={1:D2}A={2:D2}O={3}W={4}#",
                                         formes[FormeIndex],
                                         Frequence,
                                         Amplitude,
                                         offsetStr,
                                         Sauvegarde ? 1 : 0);

            m_tb_trameSend = trame.ToCharArray(); // stocke aussi sous forme de tableau de caractères
            return trame;
        }

        // Envoie la trame générée via le port série
        public override int EnvoyerSignal()
        {
            if (PortSerie != null && PortSerie.IsOpen)
            {
                try
                {
                    string trame = CoderTrame();
                    PortSerie.Write(trame);
                    return 1; // succès
                }
                catch
                {
                    return -1; // erreur d'envoi
                }
            }
            return 0; // port non ouvert ou null
        }
    }

    // Classe pour gérer la réception d’un signal
    public class ReceiveSignal : Signal
    {
        private int[] m_tb_trameReceived; // trame reçue convertie en tableau d'entiers (caractères ASCII)

        public int[] TrameReceived
        {
            get => m_tb_trameReceived;
            set => m_tb_trameReceived = value;
        }

        // Marqueur de réception de signal (implémentation à compléter selon besoins)
        public override int ReceptionSignal()
        {
            return 1;
        }

        // Convertit une trame ASCII en tableau d'entiers (codes caractères)
        public virtual int DecoderTrame(string trame)
        {
            m_tb_trameReceived = new int[trame.Length];
            for (int i = 0; i < trame.Length; i++)
                m_tb_trameReceived[i] = trame[i];

            return 1;
        }
    }
}
