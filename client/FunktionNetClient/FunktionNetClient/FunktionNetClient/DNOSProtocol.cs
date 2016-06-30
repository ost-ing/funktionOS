using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace FunktionNetClient
{
    public class DNOSMessage
    {
        /// <summary>
        /// These bytes are specific to the protocol and should not exist in the data-frame of the message
        /// If these bytes do appear in the data portion of the message, they are to be delimited.
        /// </summary>
        private enum ProtocolSymbols : byte
        {
            /// <summary>
            /// Synchronisation byte. Used in conjunction with the STX byte to identify the start of frame
            /// </summary>
            SYN = 0xFA,     
            /// <summary>
            /// Start of frame byte
            /// </summary>
            STX = 0xFD,
            /// <summary>
            /// End of frame byte
            /// </summary>
            ETX = 0xFE,
            /// <summary>
            /// Delimiter byte
            /// </summary>
            DLM = 0xFC,
            /// <summary>
            /// Acknowledgement - if the device gets a valid message, it responds with this byte
            /// </summary>
            ACK = 0xEF,
            /// <summary>
            /// Not Acknowledgement - if the device gets invalid or bad data it will spit this byte out
            /// </summary>
            NAK = 0xEE
        }

        public enum DNOSProtocolCommand : byte
        {
            Reserved    = 0x00,
            Diagnostics = 0x01,
            AD9833      = 0x20,
            PulseGen    = 0x21,
        }

        public enum AD9833Command : byte
        {
            Reserved       = 0x00,
            Stop           = 0x01,
            Waveform       = 0x02,
            Frequency      = 0x03,
            Phase          = 0x04,
            FreqSweepBegin = 0x05,
            FreqSweepStop  = 0x06,
        }

        public enum AD9833Waveform : byte
        {
            Sine     = 0x01,
            Square   = 0x02,
            Sawtooth = 0x03
        }

        public enum AD9833FreqSweep : byte
        {
	        Reserved = 0x00,
	        RiseFall = 0x01,
	        Rise     = 0x02,
	        Fall     = 0x03,
        }

        [Flags]
        public enum AD9833FreqFlags : byte
        {
            Reserved = 0x00,
            Loop = 0x01,
            Playing = 0x02,
        }

        public enum PulseGenCommand : byte
        {
            Reserved  = 0x00,
            Start     = 0x01,
            Stop      = 0x02,
            Configure = 0x03,
        }

        public DNOSProtocolCommand Command { get; private set; }
        public byte SubCommand { get; private set; }
        public byte[] DataFrame { get; private set; }
        public ushort Checksum { get; private set; }

        public DNOSMessage(DNOSProtocolCommand command, byte subcommand, byte data) :
            this(command, subcommand, new byte[] { data })
        {
        }
 

        public DNOSMessage(DNOSProtocolCommand command, byte subcommand, byte[] data)
        {
            this.Command = command;
            this.SubCommand = subcommand;

            // Scan through the dataframe and delimit the data
            // this.DataFrame = DelimitData(data);
            this.DataFrame = data;

            // Calculate the checksum for the message
            Checksum = CalculateChecksum();
        }

        private ushort CalculateChecksum()
        {
            ushort crc = 0;
            crc ^= (byte)Command;
            crc ^= SubCommand;
            crc ^= (ushort)DataFrame.Length;
            DataFrame.ToList().ForEach(x => crc ^= x);
            return crc;
        }

        public byte[] Payload()
        {
            using (var writer = new BinaryWriter(new MemoryStream()))
            {
                writer.Write((byte)ProtocolSymbols.SYN);
                writer.Write((byte)ProtocolSymbols.STX);
                writer.Write((byte)Command);
                writer.Write(SubCommand);
                writer.Write((ushort)DataFrame.Length);
                writer.Write(DataFrame);
                writer.Write(Checksum);
                writer.Write((byte)ProtocolSymbols.ETX);
                return ((MemoryStream)writer.BaseStream).ToArray();
            }
        }

        /// <summary>
        /// This function will scan through all the bytes passed to it
        /// and will insert a Delimiter or Escape byte for any bytes that are deemed 'protocol' only bytes.
        /// For example, the STX and ETX. If these bytes appear in the data, they will be delimited.
        /// [ 0x01, 0x03, 0x05, STX, 0x66, DLM, ETX, STX, 0x55 ] becomes:
        /// [ 0x01, 0x03, 0x05, DLM, STX, 0x66, DLM, DLM, DLM, ETX, DLM, STX, 0x55 ]
        /// </summary>
        private byte[] DelimitData(byte[] data)
        {
            var dataList = new List<byte>();

            foreach (byte b in data)
            {
                if (Enum.IsDefined(typeof(ProtocolSymbols), b))
                {
                    // This byte is a ProtocolSymbol, delimit it
                    dataList.Add((byte)ProtocolSymbols.DLM);
                }
                dataList.Add(b);
            }
            return dataList.ToArray();
        }
    }
}
