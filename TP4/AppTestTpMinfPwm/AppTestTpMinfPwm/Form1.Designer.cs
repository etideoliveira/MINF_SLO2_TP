namespace AppCsTp2Pwm
{
    partial class Form1
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.gbCom = new System.Windows.Forms.GroupBox();
            this.btOpenClose = new System.Windows.Forms.Button();
            this.cboPortNames = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.btSend = new System.Windows.Forms.Button();
            this.lstDataOut = new System.Windows.Forms.ListBox();
            this.gbTx = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.btSave = new System.Windows.Forms.Button();
            this.ChoiceFrequency = new System.Windows.Forms.NumericUpDown();
            this.ChoiceOffset = new System.Windows.Forms.NumericUpDown();
            this.ChoiceShape = new System.Windows.Forms.ComboBox();
            this.ChoiceAmplitude = new System.Windows.Forms.NumericUpDown();
            this.lstDataIn = new System.Windows.Forms.ListBox();
            this.serialPort2 = new System.IO.Ports.SerialPort(this.components);
            this.gbCom.SuspendLayout();
            this.gbTx.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChoiceFrequency)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChoiceOffset)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChoiceAmplitude)).BeginInit();
            this.SuspendLayout();
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 19200;
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // gbCom
            // 
            this.gbCom.Controls.Add(this.btOpenClose);
            this.gbCom.Controls.Add(this.cboPortNames);
            this.gbCom.Location = new System.Drawing.Point(12, 12);
            this.gbCom.Name = "gbCom";
            this.gbCom.Size = new System.Drawing.Size(625, 70);
            this.gbCom.TabIndex = 20;
            this.gbCom.TabStop = false;
            this.gbCom.Text = "Réglages communication";
            // 
            // btOpenClose
            // 
            this.btOpenClose.Location = new System.Drawing.Point(16, 30);
            this.btOpenClose.Name = "btOpenClose";
            this.btOpenClose.Size = new System.Drawing.Size(75, 23);
            this.btOpenClose.TabIndex = 19;
            this.btOpenClose.Text = "Open";
            this.btOpenClose.UseVisualStyleBackColor = true;
            this.btOpenClose.Click += new System.EventHandler(this.btOpenClose_Click);
            // 
            // cboPortNames
            // 
            this.cboPortNames.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboPortNames.FormattingEnabled = true;
            this.cboPortNames.Location = new System.Drawing.Point(110, 30);
            this.cboPortNames.Margin = new System.Windows.Forms.Padding(2);
            this.cboPortNames.Name = "cboPortNames";
            this.cboPortNames.Size = new System.Drawing.Size(98, 21);
            this.cboPortNames.Sorted = true;
            this.cboPortNames.TabIndex = 17;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(25, 21);
            this.label6.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(35, 13);
            this.label6.TabIndex = 55;
            this.label6.Text = "Offset";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 56);
            this.label5.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(53, 13);
            this.label5.TabIndex = 54;
            this.label5.Text = "Amplitude";
            // 
            // btSend
            // 
            this.btSend.Location = new System.Drawing.Point(381, 16);
            this.btSend.Name = "btSend";
            this.btSend.Size = new System.Drawing.Size(119, 47);
            this.btSend.TabIndex = 52;
            this.btSend.Text = "Envoi";
            this.btSend.UseVisualStyleBackColor = true;
            this.btSend.Click += new System.EventHandler(this.btSend_Click);
            // 
            // lstDataOut
            // 
            this.lstDataOut.FormattingEnabled = true;
            this.lstDataOut.Location = new System.Drawing.Point(26, 128);
            this.lstDataOut.Margin = new System.Windows.Forms.Padding(2);
            this.lstDataOut.Name = "lstDataOut";
            this.lstDataOut.Size = new System.Drawing.Size(262, 121);
            this.lstDataOut.TabIndex = 56;
            this.lstDataOut.SelectedIndexChanged += new System.EventHandler(this.lstDataOut_SelectedIndexChanged);
            // 
            // gbTx
            // 
            this.gbTx.Controls.Add(this.label2);
            this.gbTx.Controls.Add(this.button1);
            this.gbTx.Controls.Add(this.pictureBox1);
            this.gbTx.Controls.Add(this.label4);
            this.gbTx.Controls.Add(this.label1);
            this.gbTx.Controls.Add(this.btSave);
            this.gbTx.Controls.Add(this.ChoiceFrequency);
            this.gbTx.Controls.Add(this.ChoiceOffset);
            this.gbTx.Controls.Add(this.ChoiceShape);
            this.gbTx.Controls.Add(this.ChoiceAmplitude);
            this.gbTx.Controls.Add(this.lstDataOut);
            this.gbTx.Controls.Add(this.btSend);
            this.gbTx.Controls.Add(this.label6);
            this.gbTx.Controls.Add(this.lstDataIn);
            this.gbTx.Controls.Add(this.label5);
            this.gbTx.Enabled = false;
            this.gbTx.Location = new System.Drawing.Point(12, 88);
            this.gbTx.Name = "gbTx";
            this.gbTx.Size = new System.Drawing.Size(638, 406);
            this.gbTx.TabIndex = 21;
            this.gbTx.TabStop = false;
            this.gbTx.Text = "TX";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(0, 390);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(135, 13);
            this.label2.TabIndex = 22;
            this.label2.Text = "EDA/VCO, ETML-ES 2025";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(506, 16);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(119, 47);
            this.button1.TabIndex = 53;
            this.button1.Text = "Envoi cyclique";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(309, 133);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(304, 250);
            this.pictureBox1.TabIndex = 23;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(172, 56);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(58, 13);
            this.label4.TabIndex = 64;
            this.label4.Text = "Fréquence";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(194, 21);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 13);
            this.label1.TabIndex = 63;
            this.label1.Text = "Forme";
            // 
            // btSave
            // 
            this.btSave.Location = new System.Drawing.Point(442, 69);
            this.btSave.Name = "btSave";
            this.btSave.Size = new System.Drawing.Size(118, 33);
            this.btSave.TabIndex = 60;
            this.btSave.Text = "Sauvegarde";
            this.btSave.UseVisualStyleBackColor = true;
            this.btSave.Click += new System.EventHandler(this.button1_Click);
            // 
            // ChoiceFrequency
            // 
            this.ChoiceFrequency.Increment = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.ChoiceFrequency.Location = new System.Drawing.Point(234, 54);
            this.ChoiceFrequency.Maximum = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.ChoiceFrequency.Minimum = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.ChoiceFrequency.Name = "ChoiceFrequency";
            this.ChoiceFrequency.Size = new System.Drawing.Size(92, 20);
            this.ChoiceFrequency.TabIndex = 59;
            this.ChoiceFrequency.Value = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.ChoiceFrequency.ValueChanged += new System.EventHandler(this.ChoiceFrequency_ValueChanged);
            // 
            // ChoiceOffset
            // 
            this.ChoiceOffset.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.ChoiceOffset.Location = new System.Drawing.Point(65, 19);
            this.ChoiceOffset.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.ChoiceOffset.Minimum = new decimal(new int[] {
            5000,
            0,
            0,
            -2147483648});
            this.ChoiceOffset.Name = "ChoiceOffset";
            this.ChoiceOffset.Size = new System.Drawing.Size(92, 20);
            this.ChoiceOffset.TabIndex = 58;
            this.ChoiceOffset.ValueChanged += new System.EventHandler(this.ChoiceOffset_ValueChanged);
            // 
            // ChoiceShape
            // 
            this.ChoiceShape.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ChoiceShape.FormattingEnabled = true;
            this.ChoiceShape.Items.AddRange(new object[] {
            "Carre",
            "Dent de scie",
            "Sinus",
            "Triangle"});
            this.ChoiceShape.Location = new System.Drawing.Point(234, 16);
            this.ChoiceShape.Margin = new System.Windows.Forms.Padding(2);
            this.ChoiceShape.Name = "ChoiceShape";
            this.ChoiceShape.Size = new System.Drawing.Size(92, 21);
            this.ChoiceShape.Sorted = true;
            this.ChoiceShape.TabIndex = 20;
            this.ChoiceShape.SelectedIndexChanged += new System.EventHandler(this.ChoiceShape_SelectedIndexChanged);
            // 
            // ChoiceAmplitude
            // 
            this.ChoiceAmplitude.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.ChoiceAmplitude.Location = new System.Drawing.Point(65, 54);
            this.ChoiceAmplitude.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.ChoiceAmplitude.Name = "ChoiceAmplitude";
            this.ChoiceAmplitude.Size = new System.Drawing.Size(92, 20);
            this.ChoiceAmplitude.TabIndex = 57;
            this.ChoiceAmplitude.ValueChanged += new System.EventHandler(this.ChoiceAmplitude_ValueChanged);
            // 
            // lstDataIn
            // 
            this.lstDataIn.FormattingEnabled = true;
            this.lstDataIn.Location = new System.Drawing.Point(26, 262);
            this.lstDataIn.Margin = new System.Windows.Forms.Padding(2);
            this.lstDataIn.Name = "lstDataIn";
            this.lstDataIn.Size = new System.Drawing.Size(262, 121);
            this.lstDataIn.TabIndex = 52;
            this.lstDataIn.SelectedIndexChanged += new System.EventHandler(this.lstDataIn_SelectedIndexChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(649, 495);
            this.Controls.Add(this.gbTx);
            this.Controls.Add(this.gbCom);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "App de test TP MINF USB";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.gbCom.ResumeLayout(false);
            this.gbTx.ResumeLayout(false);
            this.gbTx.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChoiceFrequency)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChoiceOffset)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChoiceAmplitude)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.GroupBox gbCom;
        private System.Windows.Forms.ComboBox cboPortNames;
        private System.Windows.Forms.Button btOpenClose;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button btSend;
        private System.Windows.Forms.ListBox lstDataOut;
        private System.Windows.Forms.GroupBox gbTx;
        private System.Windows.Forms.ListBox lstDataIn;
        private System.IO.Ports.SerialPort serialPort2;
        private System.Windows.Forms.Button btSave;
        private System.Windows.Forms.NumericUpDown ChoiceFrequency;
        private System.Windows.Forms.NumericUpDown ChoiceOffset;
        private System.Windows.Forms.ComboBox ChoiceShape;
        private System.Windows.Forms.NumericUpDown ChoiceAmplitude;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button button1;
    }
}

