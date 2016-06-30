namespace FunktionNetClient
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.waveformButton = new System.Windows.Forms.Button();
            this.comportComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.comportOpenButton = new System.Windows.Forms.Button();
            this.waveformComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.frequencyButton = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.phaseButton = new System.Windows.Forms.Button();
            this.frequencyTextBox = new System.Windows.Forms.TextBox();
            this.phaseTextBox = new System.Windows.Forms.TextBox();
            this.consoleTextBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.freqStartBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.freqSweepStopButton = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.freqSweepModeComboBox = new System.Windows.Forms.ComboBox();
            this.freqSweepSend = new System.Windows.Forms.Button();
            this.timeStepTextBox = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.freqGradientBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.freqEndBox = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.pulseGenConfigureButton = new System.Windows.Forms.Button();
            this.pulseGenLoopCheckbox = new System.Windows.Forms.CheckBox();
            this.pulseGenStartButton = new System.Windows.Forms.Button();
            this.pulseGenStopButton = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.SuspendLayout();
            // 
            // waveformButton
            // 
            this.waveformButton.Location = new System.Drawing.Point(161, 68);
            this.waveformButton.Name = "waveformButton";
            this.waveformButton.Size = new System.Drawing.Size(75, 21);
            this.waveformButton.TabIndex = 0;
            this.waveformButton.Text = "Set";
            this.waveformButton.UseVisualStyleBackColor = true;
            this.waveformButton.Click += new System.EventHandler(this.OnWaveformButton);
            // 
            // comportComboBox
            // 
            this.comportComboBox.FormattingEnabled = true;
            this.comportComboBox.Location = new System.Drawing.Point(10, 16);
            this.comportComboBox.Name = "comportComboBox";
            this.comportComboBox.Size = new System.Drawing.Size(148, 21);
            this.comportComboBox.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 19);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(0, 13);
            this.label1.TabIndex = 2;
            // 
            // comportOpenButton
            // 
            this.comportOpenButton.Location = new System.Drawing.Point(163, 16);
            this.comportOpenButton.Name = "comportOpenButton";
            this.comportOpenButton.Size = new System.Drawing.Size(73, 21);
            this.comportOpenButton.TabIndex = 3;
            this.comportOpenButton.Text = "Open";
            this.comportOpenButton.UseVisualStyleBackColor = true;
            this.comportOpenButton.Click += new System.EventHandler(this.comportOpenButton_Click);
            // 
            // waveformComboBox
            // 
            this.waveformComboBox.FormattingEnabled = true;
            this.waveformComboBox.Location = new System.Drawing.Point(10, 68);
            this.waveformComboBox.Name = "waveformComboBox";
            this.waveformComboBox.Size = new System.Drawing.Size(148, 21);
            this.waveformComboBox.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 54);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Waveform";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 19);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(57, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Frequency";
            // 
            // frequencyButton
            // 
            this.frequencyButton.Location = new System.Drawing.Point(161, 33);
            this.frequencyButton.Name = "frequencyButton";
            this.frequencyButton.Size = new System.Drawing.Size(75, 21);
            this.frequencyButton.TabIndex = 6;
            this.frequencyButton.Text = "Set";
            this.frequencyButton.UseVisualStyleBackColor = true;
            this.frequencyButton.Click += new System.EventHandler(this.OnFrequencyButton);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 89);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(37, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "Phase";
            // 
            // phaseButton
            // 
            this.phaseButton.Location = new System.Drawing.Point(161, 101);
            this.phaseButton.Name = "phaseButton";
            this.phaseButton.Size = new System.Drawing.Size(75, 21);
            this.phaseButton.TabIndex = 9;
            this.phaseButton.Text = "Send";
            this.phaseButton.UseVisualStyleBackColor = true;
            this.phaseButton.Click += new System.EventHandler(this.OnPhaseButton);
            // 
            // frequencyTextBox
            // 
            this.frequencyTextBox.Location = new System.Drawing.Point(10, 34);
            this.frequencyTextBox.Name = "frequencyTextBox";
            this.frequencyTextBox.Size = new System.Drawing.Size(148, 20);
            this.frequencyTextBox.TabIndex = 12;
            // 
            // phaseTextBox
            // 
            this.phaseTextBox.Location = new System.Drawing.Point(10, 102);
            this.phaseTextBox.Name = "phaseTextBox";
            this.phaseTextBox.Size = new System.Drawing.Size(148, 20);
            this.phaseTextBox.TabIndex = 13;
            // 
            // consoleTextBox
            // 
            this.consoleTextBox.Location = new System.Drawing.Point(9, 15);
            this.consoleTextBox.Multiline = true;
            this.consoleTextBox.Name = "consoleTextBox";
            this.consoleTextBox.Size = new System.Drawing.Size(537, 178);
            this.consoleTextBox.TabIndex = 14;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(1, 359);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(0, 13);
            this.label5.TabIndex = 15;
            // 
            // freqStartBox
            // 
            this.freqStartBox.Location = new System.Drawing.Point(9, 35);
            this.freqStartBox.Name = "freqStartBox";
            this.freqStartBox.Size = new System.Drawing.Size(114, 20);
            this.freqStartBox.TabIndex = 16;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 20);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(75, 13);
            this.label6.TabIndex = 17;
            this.label6.Text = "Freq Start (Hz)";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.freqSweepStopButton);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.freqSweepModeComboBox);
            this.groupBox1.Controls.Add(this.freqSweepSend);
            this.groupBox1.Controls.Add(this.timeStepTextBox);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.freqGradientBox);
            this.groupBox1.Controls.Add(this.label8);
            this.groupBox1.Controls.Add(this.freqEndBox);
            this.groupBox1.Controls.Add(this.freqStartBox);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Location = new System.Drawing.Point(3, 190);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(247, 135);
            this.groupBox1.TabIndex = 18;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Frequency Sweep";
            // 
            // freqSweepStopButton
            // 
            this.freqSweepStopButton.Location = new System.Drawing.Point(126, 106);
            this.freqSweepStopButton.Name = "freqSweepStopButton";
            this.freqSweepStopButton.Size = new System.Drawing.Size(55, 21);
            this.freqSweepStopButton.TabIndex = 27;
            this.freqSweepStopButton.Text = "Stop";
            this.freqSweepStopButton.UseVisualStyleBackColor = true;
            this.freqSweepStopButton.Click += new System.EventHandler(this.freqSweepStopButton_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(8, 93);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(70, 13);
            this.label10.TabIndex = 26;
            this.label10.Text = "Sweep Mode";
            // 
            // freqSweepModeComboBox
            // 
            this.freqSweepModeComboBox.FormattingEnabled = true;
            this.freqSweepModeComboBox.Location = new System.Drawing.Point(9, 107);
            this.freqSweepModeComboBox.Name = "freqSweepModeComboBox";
            this.freqSweepModeComboBox.Size = new System.Drawing.Size(114, 21);
            this.freqSweepModeComboBox.TabIndex = 25;
            // 
            // freqSweepSend
            // 
            this.freqSweepSend.Location = new System.Drawing.Point(186, 106);
            this.freqSweepSend.Name = "freqSweepSend";
            this.freqSweepSend.Size = new System.Drawing.Size(51, 21);
            this.freqSweepSend.TabIndex = 19;
            this.freqSweepSend.Text = "Begin";
            this.freqSweepSend.UseVisualStyleBackColor = true;
            this.freqSweepSend.Click += new System.EventHandler(this.freqSweepSend_Click);
            // 
            // timeStepTextBox
            // 
            this.timeStepTextBox.Location = new System.Drawing.Point(125, 72);
            this.timeStepTextBox.Name = "timeStepTextBox";
            this.timeStepTextBox.Size = new System.Drawing.Size(112, 20);
            this.timeStepTextBox.TabIndex = 22;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(122, 19);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(72, 13);
            this.label7.TabIndex = 19;
            this.label7.Text = "Freq End (Hz)";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(122, 58);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(74, 13);
            this.label9.TabIndex = 23;
            this.label9.Text = "Timestep (mS)";
            // 
            // freqGradientBox
            // 
            this.freqGradientBox.Location = new System.Drawing.Point(9, 72);
            this.freqGradientBox.Name = "freqGradientBox";
            this.freqGradientBox.Size = new System.Drawing.Size(114, 20);
            this.freqGradientBox.TabIndex = 20;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 57);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(91, 13);
            this.label8.TabIndex = 21;
            this.label8.Text = "Freq gradient (Hz)";
            // 
            // freqEndBox
            // 
            this.freqEndBox.Location = new System.Drawing.Point(125, 35);
            this.freqEndBox.Name = "freqEndBox";
            this.freqEndBox.Size = new System.Drawing.Size(112, 20);
            this.freqEndBox.TabIndex = 18;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.pulseGenConfigureButton);
            this.groupBox2.Controls.Add(this.pulseGenLoopCheckbox);
            this.groupBox2.Controls.Add(this.pulseGenStartButton);
            this.groupBox2.Controls.Add(this.pulseGenStopButton);
            this.groupBox2.Location = new System.Drawing.Point(256, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(303, 322);
            this.groupBox2.TabIndex = 19;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Pulse Generator";
            // 
            // pulseGenConfigureButton
            // 
            this.pulseGenConfigureButton.Location = new System.Drawing.Point(65, 287);
            this.pulseGenConfigureButton.Name = "pulseGenConfigureButton";
            this.pulseGenConfigureButton.Size = new System.Drawing.Size(72, 21);
            this.pulseGenConfigureButton.TabIndex = 29;
            this.pulseGenConfigureButton.Text = "Configure";
            this.pulseGenConfigureButton.UseVisualStyleBackColor = true;
            this.pulseGenConfigureButton.Click += new System.EventHandler(this.pulseGenConfigureButton_Click);
            // 
            // pulseGenLoopCheckbox
            // 
            this.pulseGenLoopCheckbox.AutoSize = true;
            this.pulseGenLoopCheckbox.Location = new System.Drawing.Point(233, 265);
            this.pulseGenLoopCheckbox.Name = "pulseGenLoopCheckbox";
            this.pulseGenLoopCheckbox.Size = new System.Drawing.Size(64, 17);
            this.pulseGenLoopCheckbox.TabIndex = 27;
            this.pulseGenLoopCheckbox.Text = "Looping";
            this.pulseGenLoopCheckbox.UseVisualStyleBackColor = true;
            // 
            // pulseGenStartButton
            // 
            this.pulseGenStartButton.Location = new System.Drawing.Point(221, 287);
            this.pulseGenStartButton.Name = "pulseGenStartButton";
            this.pulseGenStartButton.Size = new System.Drawing.Size(72, 21);
            this.pulseGenStartButton.TabIndex = 28;
            this.pulseGenStartButton.Text = "Start";
            this.pulseGenStartButton.UseVisualStyleBackColor = true;
            this.pulseGenStartButton.Click += new System.EventHandler(this.pulseGenStartButton_Click);
            // 
            // pulseGenStopButton
            // 
            this.pulseGenStopButton.Location = new System.Drawing.Point(143, 287);
            this.pulseGenStopButton.Name = "pulseGenStopButton";
            this.pulseGenStopButton.Size = new System.Drawing.Size(72, 21);
            this.pulseGenStopButton.TabIndex = 27;
            this.pulseGenStopButton.Text = "Stop";
            this.pulseGenStopButton.UseVisualStyleBackColor = true;
            this.pulseGenStopButton.Click += new System.EventHandler(this.pulseGenStopButton_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.comportOpenButton);
            this.groupBox3.Controls.Add(this.comportComboBox);
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Location = new System.Drawing.Point(3, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(247, 46);
            this.groupBox3.TabIndex = 20;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Connectivity";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.frequencyTextBox);
            this.groupBox4.Controls.Add(this.frequencyButton);
            this.groupBox4.Controls.Add(this.label3);
            this.groupBox4.Controls.Add(this.waveformComboBox);
            this.groupBox4.Controls.Add(this.waveformButton);
            this.groupBox4.Controls.Add(this.label2);
            this.groupBox4.Controls.Add(this.phaseTextBox);
            this.groupBox4.Controls.Add(this.label4);
            this.groupBox4.Controls.Add(this.phaseButton);
            this.groupBox4.Location = new System.Drawing.Point(3, 53);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(247, 131);
            this.groupBox4.TabIndex = 21;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Signal Output";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.consoleTextBox);
            this.groupBox5.Location = new System.Drawing.Point(3, 329);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(556, 199);
            this.groupBox5.TabIndex = 22;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Console In";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(561, 529);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label5);
            this.Name = "Form1";
            this.Text = "Funktion Tester";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button waveformButton;
        private System.Windows.Forms.ComboBox comportComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button comportOpenButton;
        private System.Windows.Forms.ComboBox waveformComboBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button frequencyButton;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button phaseButton;
        private System.Windows.Forms.TextBox frequencyTextBox;
        private System.Windows.Forms.TextBox phaseTextBox;
        private System.Windows.Forms.TextBox consoleTextBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox freqStartBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button freqSweepSend;
        private System.Windows.Forms.TextBox timeStepTextBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox freqGradientBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox freqEndBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox freqSweepModeComboBox;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button pulseGenStartButton;
        private System.Windows.Forms.Button pulseGenStopButton;
        private System.Windows.Forms.CheckBox pulseGenLoopCheckbox;
        private System.Windows.Forms.Button pulseGenConfigureButton;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Button freqSweepStopButton;
    }
}

