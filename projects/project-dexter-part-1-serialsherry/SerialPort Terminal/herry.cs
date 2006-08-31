using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;

namespace SerialPortTerminal
{
    #region Public Enumerations
    public enum DataMode { Text, Hex }
    public enum LogMsgType { Incoming, Outgoing, Normal, Warning, Error };
    #endregion

    /// <summary>
    /// SerialSherry
    /// </summary>
    public class SerialSherry
    {
        // NOTE: Setting buffer size at 64 with Atmel STK500 loaded with an AtMega8
        // causes data loss (not sure why exactly though) so 16 is the max at this time
        private const int bufferSize = 16;
        // This must be at least 1 millisecond
        private const int sleepTime = 1;

        #region Constructor, opening and closing

        /// <summary>
        /// Constructor
        /// </summary>
        public SerialSherry()
        {
            comPort = new SerialPort();
            comPort.PinChanged += new SerialPinChangedEventHandler(comPort_PinChanged);
            comPort.ErrorReceived += new SerialErrorReceivedEventHandler(comPort_ErrorReceived);
            comPort.DataReceived += new SerialDataReceivedEventHandler(comPort_DataReceived);
            comPort.DiscardNull = true;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="BaudRate"></param>
        /// <param name="DataBits"></param>
        /// <param name="StopBits"></param>
        /// <param name="Parity"></param>
        /// <param name="PortName"></param>
        /// <returns></returns>
        #region Open Port
        public bool OpenPort(int BaudRate, int DataBits, StopBits StopBits, Parity Parity, string PortName)
        {
            // If the port is open, close it.
            if (comPort.IsOpen) comPort.Close();
            else
            {
                // Set the port's settings
                comPort.BaudRate = BaudRate;
                comPort.DataBits = DataBits;
                comPort.StopBits = StopBits;
                comPort.Parity = Parity;
                comPort.PortName = PortName;
                // Open the port
                comPort.Open();
            }
            // If the port is open, send focus to the send data box
            return (comPort.IsOpen);
        }


        #endregion


        /// <summary>
        /// Close the port
        /// </summary>
        /// <returns></returns>
        #region ClosePort
        public bool ClosePort()
        {
            comPort.Close();
            return true;
        }        
        #endregion


        #endregion
        
        #region Receiving of data
        /// <summary>
        /// ReceiveData
        /// This method will be called when there is data waiting in the port's buffer
        /// </summary>
        public string ReceiveData()
        {
            string data = null;
            lock (syncObject)
            {
                // Determain which mode (string or binary) the user is in
                if (CurrentDataMode == DataMode.Text)
                {
                    data = comPort.ReadExisting();                    
                }
                else
                {
                    // Obtain the number of bytes waiting in the port's buffer
                    int bytes = comPort.BytesToRead;
                    // Create a byte array buffer to hold the incoming data
                    byte[] buffer = new byte[bytes];
                    // Read the data from the port and store it in our buffer
                    comPort.Read(buffer, 0, bytes);
                    data = ByteArrayToHexString(buffer);
                }
            }
            receivedData+=data;
            return data;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="e"></param>
        protected void OnDataReceived(SerialDataReceivedEventArgs2 e)
        {
            if (DataReceived != null) DataReceived(this.comPort, e);
        }        
        #endregion

        #region Sending of data

        /// <summary>
        /// 
        /// </summary>
        /// <param name="data"></param>
        public void SendData(string data)
        {
            if (!comPort.IsOpen) { comPort.Open(); }
            if (CurrentDataMode == DataMode.Text)
            {
                #region Buffered Send (for small devices)

                char[] writeBuffer = (char[])Array.CreateInstance(typeof(char), bufferSize);
                int writeBufferSize = bufferSize;
                int lastPosition = 0;
                int currentPosition = 0;

                using (System.IO.StringReader reader = new System.IO.StringReader(data))
                {
                    do
                    {
                        lock (syncObject)
                        {
                            Array.Clear(writeBuffer, 0, writeBuffer.Length);
                            currentPosition = lastPosition;
                            lastPosition += reader.ReadBlock(writeBuffer, 0, writeBufferSize);
                            this.comPort.Write(writeBuffer, 0, lastPosition - currentPosition);
                        }
                        System.Threading.Thread.Sleep(sleepTime);

                    } while (currentPosition != lastPosition);

                    reader.Close();
                }

                #endregion

                #region Single Line send (commented out)
                // Send the user's text straight out the port
                // comPort.Write(data);
                #endregion
            }
            else
            {
                try
                {
                    // Convert the user's string of hex digits (ex: B4 CA E2) to a byte array
                    byte[] dataBytes = HexStringToByteArray(data);

                    // Send the binary data out the port
                    comPort.Write(dataBytes, 0, dataBytes.Length);
                }
                catch (FormatException)
                {
                    throw;
                }
            }

        }

        #endregion

        #region Event Handlers
        
        /// <summary>
        /// Data Received
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void comPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            //lock (syncObject)
            //{
                string data = ReceiveData();
                OnDataReceived(new SerialDataReceivedEventArgs2(data, e.EventType));
            //}
        }

        /// <summary>
        /// Pin Changed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void comPort_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            //lock (syncObject)
            //{
                System.Diagnostics.Debug.WriteLine(e.EventType);
            //}
        }

        /// <summary>
        /// Error Received
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void comPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            //lock (syncObject)
            //{
                System.Diagnostics.Debug.WriteLine(e.EventType);
            //}
        }


        #endregion

        #region Utility Methods
        /// <summary> Convert a string of hex digits (ex: E4 CA B2) to a byte array. </summary>
        /// <param name="s"> The string containing the hex digits (with or without spaces). </param>
        /// <returns> Returns an array of bytes. </returns>
        private byte[] HexStringToByteArray(string s)
        {
            s = s.Replace(" ", "");
            byte[] buffer = new byte[s.Length / 2];
            for (int i = 0; i < s.Length; i += 2)
                buffer[i / 2] = (byte)Convert.ToByte(s.Substring(i, 2), 16);
            return buffer;
        }

        /// <summary> Converts an array of bytes into a formatted string of hex digits (ex: E4 CA B2)</summary>
        /// <param name="data"> The array of bytes to be translated into a string of hex digits. </param>
        /// <returns> Returns a well formatted string of hex digits with spacing. </returns>
        private string ByteArrayToHexString(byte[] data)
        {
            StringBuilder sb = new StringBuilder(data.Length * 3);
            foreach (byte b in data)
                sb.Append(Convert.ToString(b, 16).PadLeft(2, '0').PadRight(3, ' '));
            return sb.ToString().ToUpper();
        }
        #endregion

        #region Events, Fields and Properties

        public object syncObject = new object();

        /// <summary>
        /// Data Receiver Event Handler
        /// </summary>
        public EventHandler<SerialDataReceivedEventArgs2> DataReceived = null;
        /// <summary>
        /// ComPort
        /// </summary>
        public SerialPort ComPort { get { return comPort = new SerialPort(); } }
        /// <summary>
        /// CurrentDataMode
        /// </summary>
        public DataMode CurrentDataMode
        {
            get { return currentDataMode; }
            set { currentDataMode = value; }
        }
        public string ReceivedData
        {
            get { return receivedData; }
            set { receivedData = value; }
        }

        private string receivedData = string.Empty;
        private SerialPort comPort = null;
        private DataMode currentDataMode = DataMode.Text;
                
        #endregion

    }

    /// <summary>
    /// SerialDataReceivedEventArgs2
    /// </summary>
    #region SerialDataReceivedEventArgs2
    public class SerialDataReceivedEventArgs2 : EventArgs
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="Data"></param>
        /// <param name="EventType"></param>
         
        public SerialDataReceivedEventArgs2(string Data, SerialData EventType)
        {
            this.data = Data;
            this.eventType = EventType;
        }

        private string data;
        private SerialData eventType = SerialData.Chars;
        
        /// <summary>
        /// 
        /// </summary>
        public SerialData EventType
        {
            get { return eventType; }
        }
        /// <summary>
        /// 
        /// </summary>
        public string Data
        {
            get { return data; }
            set { data = value; }
        }

    }

    #endregion

    public delegate void EventHandler<SerialDataReceivedEventArgs2>(object sender, SerialDataReceivedEventArgs2 e);
}
