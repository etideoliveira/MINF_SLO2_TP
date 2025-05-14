
using System;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.Text;
using System.Drawing;

namespace AppCsTp2Pwm
{
    public partial class Form1 : Form
    {
        // Délégué utilisé pour exécuter la méthode de réception depuis un thread UI
        public delegate void ReceiverD();
        public ReceiverD myDelegate;

        private int m_SendCount = 0; // compteur pour les envois
        private const byte stx = 0x21;  // caractère de début de trame ('!')

        // Instance de SendSignal issue de la classe Signal (héritage)
        private SendSignal signal = new SendSignal();

        private bool btSaveValue = false;             // état du bouton Sauvegarde
        private bool isSaveMode_On_Or_Off = false;    // toggle pour le bouton Sauvegarde

        public Form1()
        {
            InitializeComponent(); // Initialise les composants graphiques
            string[] ports = SerialPort.GetPortNames(); // Récupère les ports COM disponibles
            cboPortNames.Items.AddRange(ports); // Affiche dans la liste déroulante
            if (ports.Length > 0) cboPortNames.SelectedIndex = 0;
            lstDataIn.Items.Clear();
            myDelegate = new ReceiverD(DispInListRxData); // Associe méthode de réception
        }

        // Affiche un message TX dans la ListBox et conserve 10 derniers
        void DispTxMess(string message)
        {
            lstDataOut.Items.Add(message);
            if (lstDataOut.Items.Count > 10)
                lstDataOut.Items.RemoveAt(0);
        }

        // Compose la trame à envoyer en remplissant les propriétés de SendSignal
        void ComposeMessage()
        {
            signal.Frequence = Convert.ToUInt16(ChoiceFrequency.Value);
            signal.Amplitude = Convert.ToUInt16(ChoiceAmplitude.Value);
            signal.Offset = Convert.ToInt16(ChoiceOffset.Value);
            signal.FormeIndex = ChoiceShape.SelectedIndex;
            signal.Sauvegarde = btSaveValue;

            // Utilisation de la méthode CoderTrame() de la classe SendSignal
            string message = signal.CoderTrame();
            DispTxMess(message); // affichage dans la zone TX
            textMessageTX = message;
            DessinerSignal();
        }

        string textMessageTX; // stocke la trame finale à envoyer

        // Envoie la trame sur le port série
        void SendMessage(int count)
        {
            if (serialPort1.IsOpen)
            {
                ComposeMessage(); // crée la trame
                try
                {
                    signal.PortSerie = serialPort1;
                    int resultat = signal.EnvoyerSignal(); // envoie par UART
                }
                catch (Exception ex)
                {
                    // Gestion d'erreur
                    MessageBox.Show(ex.ToString(), "Erreur à l'envoi !", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    timer1.Stop();
                    serialPort1.DiscardInBuffer();
                    serialPort1.DiscardOutBuffer();
                }
            }
            else
            {
                MessageBox.Show("Port non ouvert", "Erreur", MessageBoxButtons.OK, MessageBoxIcon.Error);
                timer1.Stop();
            }
        }

        // Ouvre ou ferme le port série
        private void btOpenClose_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
            {
                // Paramètres de communication
                serialPort1.PortName = (string)cboPortNames.SelectedItem;
                serialPort1.BaudRate = 57600;
                serialPort1.Parity = Parity.None;
                serialPort1.DataBits = 8;
                serialPort1.StopBits = StopBits.One;
                serialPort1.Handshake = Handshake.RequestToSend;
                serialPort1.ReadTimeout = 100;
                serialPort1.WriteTimeout = 100;

                try
                {
                    serialPort1.Open(); // ouverture du port
                    btOpenClose.Text = "Close";
                    gbTx.Enabled = true;
                    cboPortNames.Enabled = false;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString(), "Erreur à l'ouverture du port !", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                serialPort1.Close(); // fermeture
                btOpenClose.Text = "Open";
                gbTx.Enabled = false;
                cboPortNames.Enabled = true;
                timer1.Stop();
            }
        }

        // Déclenchée automatiquement quand des données arrivent sur le port série
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            lstDataIn.BeginInvoke(myDelegate); // appel en thread UI
        }

        // Affiche la trame reçue dans la liste RX
        public void DispInListRxData()
        {
            byte[] buffer = new byte[1024];
            int bytesRead = 0;
            int indexTrame = 0;
            byte[] RxMess = new byte[30];
            string RxMessage = "";

            try
            {
                bytesRead = serialPort1.Read(buffer, 0, buffer.Length);

                if (bytesRead > 0)
                {
                    int startIndex = Array.IndexOf(buffer, stx, 0, bytesRead); // cherche '!'
                    if (startIndex >= 0)
                    {
                        for (int i = startIndex; i < bytesRead && buffer[i] != '#'; i++)
                        {
                            RxMess[indexTrame++] = buffer[i];
                        }
                        if (indexTrame < RxMess.Length)
                        {
                            RxMess[indexTrame++] = (byte)'#';
                        }

                        RxMessage = Encoding.ASCII.GetString(RxMess, 0, indexTrame); // conversion
                        lstDataIn.Items.Add(RxMessage);
                        if (lstDataIn.Items.Count > 10)
                        {
                            lstDataIn.Items.RemoveAt(0);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Erreur lors de la réception", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        // Bouton unique pour envoyer une trame
        private void btSend_Click(object sender, EventArgs e)
        {
            m_SendCount = 0;
            SendMessage(m_SendCount);

            if (timer1.Enabled)
            {
                timer1.Stop();
            }
        }

        // Envoi automatique toutes les X ms si activé
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                m_SendCount++;
                SendMessage(m_SendCount);
            }
            else
            {
                btOpenClose.Text = "Open";
                gbTx.Enabled = false;
            }
        }

        // Nettoyage lors de la fermeture du formulaire
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            timer1.Stop();
            try
            {
                serialPort1.Handshake = Handshake.None;
                serialPort1.DtrEnable = false;
                serialPort1.RtsEnable = false;
                serialPort1.DataReceived -= serialPort1_DataReceived;
                Thread.Sleep(200);
                if (serialPort1.IsOpen)
                {
                    serialPort1.DiscardInBuffer();
                    serialPort1.DiscardOutBuffer();
                    serialPort1.Close();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Erreur à la fermeture du port !", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        // Gère l’état du bouton Sauvegarde
        private void button1_Click(object sender, EventArgs e)
        {
            if (!isSaveMode_On_Or_Off)
            {
                btSaveValue = true;
                isSaveMode_On_Or_Off = true;
                btSave.Text = "Ne pas sauvegarder"; // 🟢 État actif
            }
            else
            {
                btSaveValue = false;
                isSaveMode_On_Or_Off = false;
                btSave.Text = "Sauvegarder"; // 🔴 État inactif
            }
        }

        private void ChoiceAmplitude_ValueChanged(object sender, EventArgs e)
        {
            ChoiceAmplitude.Value = Math.Round(ChoiceAmplitude.Value / 100) * 100;
            DessinerSignal();
        }

        private void ChoiceOffset_ValueChanged(object sender, EventArgs e)
        {
            ChoiceOffset.Value = Math.Round(ChoiceOffset.Value / 100) * 100;
            DessinerSignal();
        }

        private void ChoiceFrequency_ValueChanged(object sender, EventArgs e)
        {
            ChoiceFrequency.Value = Math.Round(ChoiceFrequency.Value / 20) * 20;
            DessinerSignal();
        }

        private void ChoiceShape_SelectedIndexChanged(object sender, EventArgs e)
        {
            DessinerSignal();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            DessinerSignal();
        }


        private void DessinerSignal()
        {
            int width = pictureBox1.Width;
            int height = pictureBox1.Height;

            Bitmap bmp = new Bitmap(width, height);
            Graphics g = Graphics.FromImage(bmp);
            g.Clear(Color.White);

            // Quadrillage
            Pen gridPen = new Pen(Color.LightGray, 1);
            for (int x = 0; x < width; x += 20)
                g.DrawLine(gridPen, x, 0, x, height);
            for (int y = 0; y < height; y += 20)
                g.DrawLine(gridPen, 0, y, width, y);

            // Axes X et Y
            Pen axisPen = new Pen(Color.Black, 1);
            g.DrawLine(axisPen, 0, height / 2, width, height / 2);     // Axe X
            g.DrawLine(axisPen, width / 2, 0, width / 2, height);      // Axe Y

            // Paramètres utilisateur
            int amplitude_mV = (int)ChoiceAmplitude.Value;   // crête à crête
            int offset_mV = (int)ChoiceOffset.Value;
            int freq = (int)ChoiceFrequency.Value;
            int shape = ChoiceShape.SelectedIndex;

            // Échelle verticale : 20V → pleine hauteur
            double pixelsPerVolt = height / 20.0;
            int yOffsetPixels = (int)(offset_mV / 1000.0 * pixelsPerVolt);
            int yCenter = height / 2 - yOffsetPixels;

            // ⚠ Inversé : À 20Hz → 1.5 périodes, à 2000Hz → 20 périodes
            double periodsToShow = Math.Max(0.5, Math.Min(20.0, freq / 100.0));
            double timeToShow = periodsToShow / freq; // secondes
            double sampleRate = width / timeToShow;   // pixels par seconde

            // Calcul du signal
            Point[] points = new Point[width];
            for (int x = 0; x < width; x++)
            {
                double t = x / sampleRate; // secondes
                double phase = 2 * Math.PI * freq * t;
                double yNorm = 0;

                switch (shape)
                {
                    case 0: yNorm = Math.Sign(Math.Sin(phase)); break;
                    case 1: yNorm = 2 * (t * freq - Math.Floor(t * freq + 0.5)); break;
                    case 2: yNorm = Math.Sin(phase); break;
                    case 3: yNorm = 2 * Math.Abs(2 * (t * freq - Math.Floor(t * freq + 0.5))) - 1; break;
                }

                double ySignal = yNorm * (amplitude_mV / 2000.0) * pixelsPerVolt;
                int yPixel = yCenter - (int)ySignal;

                points[x] = new Point(x, yPixel);
            }

            // Dessiner le signal
            Pen signalPen = new Pen(Color.Blue, 1.5f);
            g.DrawLines(signalPen, points);

            pictureBox1.Image = bmp;
        }

        private int Clamp(int value, int min, int max)
        {
            return (value < min) ? min : (value > max) ? max : value;
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            if (!timer1.Enabled)
            {
                timer1.Interval = 25; // ⏱ Envoi toutes les 25 ms
                m_SendCount = 0;
                timer1.Start();
                button1.Text = "Stop cyclique";
            }
            else
            {
                timer1.Stop();
                button1.Text = "Envoi cyclique";
            }
        }

        private void lstDataOut_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void lstDataIn_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }

}
