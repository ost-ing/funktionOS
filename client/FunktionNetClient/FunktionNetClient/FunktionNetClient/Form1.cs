using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;

namespace FunktionNetClient
{
    public partial class Form1 : Form
    {
        private SerialPort port;

        public Form1()
        {
            InitializeComponent();

            PopulateComports();
            PopulateFields();
        }

        private void PopulateComports()
        {
            var portList = SerialPort.GetPortNames().ToList();
            this.comportComboBox.DataSource = new BindingSource(portList, null);           
        }

        private void PopulateFields()
        {
            waveformComboBox.Items.Add(DNOSMessage.AD9833Waveform.Sine);
            waveformComboBox.Items.Add(DNOSMessage.AD9833Waveform.Square);
            waveformComboBox.Items.Add(DNOSMessage.AD9833Waveform.Sawtooth);
            freqSweepModeComboBox.Items.Add(DNOSMessage.AD9833FreqSweep.Fall);
            freqSweepModeComboBox.Items.Add(DNOSMessage.AD9833FreqSweep.Rise);
            freqSweepModeComboBox.Items.Add(DNOSMessage.AD9833FreqSweep.RiseFall);
        }

        /// <summary>
        /// Change the waveform 
        /// </summary>
        private void OnWaveformButton(object sender, EventArgs e)
        {
            if (this.port == null || !this.port.IsOpen)
            {
                MessageBox.Show("Cannot send data - COM port is not open.");
                return;
            }

            if (waveformComboBox.SelectedIndex < 0)
                return;

            var message = new DNOSMessage(DNOSMessage.DNOSProtocolCommand.AD9833, (byte)DNOSMessage.AD9833Command.Waveform, (byte)waveformComboBox.SelectedItem);
            WriteSerial(message.Payload());
        }
        
        private void OnFrequencyButton(object sender, EventArgs args)
        {
            try
            {
                if (this.port == null || !this.port.IsOpen)
                    throw new Exception("Cannot send data - COM port is not open.");

                int value = int.Parse(frequencyTextBox.Text);

                var freq = BitConverter.GetBytes(value);

                var message = new DNOSMessage(DNOSMessage.DNOSProtocolCommand.AD9833, (byte)DNOSMessage.AD9833Command.Frequency, freq);
                WriteSerial(message.Payload());
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return;
            }
        }

        private void OnPhaseButton(object sender, EventArgs args)
        {
            try
            {
                if (this.port == null || !this.port.IsOpen)
                    throw new Exception("Cannot send data - COM port is not open.");             
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return;
            }
        }


        private void WriteSerial(byte[] data)
        {
            System.Diagnostics.Debug.WriteLine("Writing data: " + BitConverter.ToString(data));
            this.port.Write(data, 0, data.Length);
        }

        private void ReadSerial(object obj, SerialDataReceivedEventArgs args)
        {
            try
            {
                var data = new byte[this.port.BytesToRead];
                var bytesRead = this.port.Read(data, 0, this.port.BytesToRead);
                DisplayToConsole(data);
            }
            catch { }
        }

        private void DisplayToConsole(byte[] data)
        {
            if (this.InvokeRequired)
            {
                BeginInvoke(new Action(() => DisplayToConsole(data)));
                return;
            }

            this.consoleTextBox.Text += (Encoding.ASCII.GetString(data));
        }

        private void comportOpenButton_Click(object sender, EventArgs args)
        {
            if (this.comportComboBox.SelectedIndex == -1)
                return;

            port = new SerialPort((string)this.comportComboBox.SelectedItem, 19200, Parity.None, 8, StopBits.One);
            port.DataReceived += ReadSerial;

            try
            {
                port.Open();
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return;
            }
        }

        private void freqSweepSend_Click(object sender, EventArgs args)
        {
            try
            {
                if (this.port == null || !this.port.IsOpen)
                    throw new Exception("Cannot send data - COM port is not open.");

                if (this.freqSweepModeComboBox.SelectedIndex < 0)
                    return;

                var freqStart = uint.Parse(this.freqStartBox.Text);
                var freqEnd = uint.Parse(this.freqEndBox.Text);
                var timeStep = uint.Parse(this.timeStepTextBox.Text);
                var freqGrad = uint.Parse(this.freqGradientBox.Text);

                // build data
                using (var writer = new BinaryWriter(new MemoryStream()))
                {
                    writer.Write((byte)freqSweepModeComboBox.SelectedItem);
                    writer.Write(freqStart);
                    writer.Write(freqEnd);
                    writer.Write(timeStep);
                    writer.Write(freqGrad);

                    byte[] data = ((MemoryStream)writer.BaseStream).ToArray();

                    var message = new DNOSMessage(DNOSMessage.DNOSProtocolCommand.AD9833, (byte)DNOSMessage.AD9833Command.FreqSweepBegin, data);
                    WriteSerial(message.Payload());
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return;
            }
        }

        private void freqSweepStopButton_Click(object sender, EventArgs args)
        {
            try
            {
                if (this.port == null || !this.port.IsOpen)
                    throw new Exception("Cannot send data - COM port is not open.");

                var message = new DNOSMessage(DNOSMessage.DNOSProtocolCommand.AD9833, (byte)DNOSMessage.AD9833Command.FreqSweepStop, new byte[] { });
                
                WriteSerial(message.Payload());
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return;
            }
        }

        private void pulseGenStartButton_Click(object sender, EventArgs args)
        {
            try
            {
                if (this.port == null || !this.port.IsOpen)
                    throw new Exception("Cannot send data - COM port is not open.");

                var message = new DNOSMessage(DNOSMessage.DNOSProtocolCommand.PulseGen, (byte)DNOSMessage.PulseGenCommand.Start, new byte[] { });
                WriteSerial(message.Payload());
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        private void pulseGenStopButton_Click(object sender, EventArgs args)
        {
            try
            {
                if (this.port == null || !this.port.IsOpen)
                    throw new Exception("Cannot send data - COM port is not open.");

                var message = new DNOSMessage(DNOSMessage.DNOSProtocolCommand.PulseGen,
                                              (byte)DNOSMessage.PulseGenCommand.Stop,
                                              new byte[] { });
                WriteSerial(message.Payload());
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        private void pulseGenConfigureButton_Click(object sender, EventArgs args)
        {
            byte[] states = { 0, 1, 0, 1, 0 };
            uint[] micros = { 30000, 350, 400, 450, 500 };
           
            try
            {
                if (this.port == null || !this.port.IsOpen)
                    throw new Exception("Cannot send data - COM port is not open.");

                 // build data
                using (var writer = new BinaryWriter(new MemoryStream()))
                {
                    writer.Write(states.Length);
                    writer.Write(states);
            
                    foreach (uint i in micros)
                        writer.Write(i);
            
                    byte[] data = ((MemoryStream)writer.BaseStream).ToArray();

                    var message = new DNOSMessage(DNOSMessage.DNOSProtocolCommand.PulseGen,
                                                  (byte)DNOSMessage.PulseGenCommand.Configure,
                                                  data);

                    byte[] payload = message.Payload();
                    WriteSerial(payload);
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        
    }
}
