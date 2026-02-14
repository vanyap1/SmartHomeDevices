using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Runtime.InteropServices;

using Peak.Can.Basic;
using TPCANHandle = System.UInt16;
using TPCANBitrateFD = System.String;
using TPCANTimestampFD = System.UInt64;

namespace CanBusSystemMonitor
{   
    internal class Program
    {
        #region Structures
        /// <summary>
        /// Message Status structure used to show CAN Messages
        /// in a ListView
        /// </summary>
        
        private class MessageStatus
        {
            private TPCANMsgFD m_Msg;
            private TPCANTimestampFD m_TimeStamp;
            private TPCANTimestampFD m_oldTimeStamp;
            private int m_iIndex;
            private int m_Count;
            private bool m_bShowPeriod;
            private bool m_bWasChanged;

            public MessageStatus(TPCANMsgFD canMsg, TPCANTimestampFD canTimestamp, int listIndex)
            {
                m_Msg = canMsg;
                m_TimeStamp = canTimestamp;
                m_oldTimeStamp = canTimestamp;
                m_iIndex = listIndex;
                m_Count = 1;
                m_bShowPeriod = true;
                m_bWasChanged = false;
            }

            public void Update(TPCANMsgFD canMsg, TPCANTimestampFD canTimestamp)
            {
                m_Msg = canMsg;
                m_oldTimeStamp = m_TimeStamp;
                m_TimeStamp = canTimestamp;
                m_bWasChanged = true;
                m_Count += 1;
            }

            public TPCANMsgFD CANMsg
            {
                get { return m_Msg; }
            }

            public TPCANTimestampFD Timestamp
            {
                get { return m_TimeStamp; }
            }

            public int Position
            {
                get { return m_iIndex; }
            }

            public string TypeString
            {
                get { return GetMsgTypeString(); }
            }

            public string IdString
            {
                get { return GetIdString(); }
            }


            public int Count
            {
                get { return m_Count; }
            }

            public bool ShowingPeriod
            {
                get { return m_bShowPeriod; }
                set
                {
                    if (m_bShowPeriod ^ value)
                    {
                        m_bShowPeriod = value;
                        m_bWasChanged = true;
                    }
                }
            }

            public bool MarkedAsUpdated
            {
                get { return m_bWasChanged; }
                set { m_bWasChanged = value; }
            }

            public string TimeString
            {
                get { return GetTimeString(); }
            }

            private string GetTimeString()
            {
                double fTime;

                fTime = (m_TimeStamp / 1000.0);
                if (m_bShowPeriod)
                    fTime -= (m_oldTimeStamp / 1000.0);
                return fTime.ToString("F1");
            }



            private string GetIdString()
            {
                // We format the ID of the message and show it
                //
                if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_EXTENDED) == TPCANMessageType.PCAN_MESSAGE_EXTENDED)
                    return string.Format("{0:X8}h", m_Msg.ID);
                else
                    return string.Format("{0:X3}h", m_Msg.ID);
            }

            private string GetMsgTypeString()
            {
                string strTemp;
                bool isEcho = (m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_ECHO) == TPCANMessageType.PCAN_MESSAGE_ECHO;

                if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_STATUS) == TPCANMessageType.PCAN_MESSAGE_STATUS)
                    return "STATUS";

                if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_ERRFRAME) == TPCANMessageType.PCAN_MESSAGE_ERRFRAME)
                    return "ERROR";

                if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_EXTENDED) == TPCANMessageType.PCAN_MESSAGE_EXTENDED)
                    strTemp = "EXT";
                else
                    strTemp = "STD";

                if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_RTR) == TPCANMessageType.PCAN_MESSAGE_RTR)
                    strTemp += isEcho ? "/RTR [ ECHO ]" : "/RTR";
                else
                {
                    if ((int)m_Msg.MSGTYPE > (int)TPCANMessageType.PCAN_MESSAGE_EXTENDED)
                    {
                        if (isEcho)
                            strTemp += " [ ECHO";
                        else
                            strTemp += " [ ";
                        if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_FD) == TPCANMessageType.PCAN_MESSAGE_FD)
                            strTemp += " FD";
                        if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_BRS) == TPCANMessageType.PCAN_MESSAGE_BRS)
                            strTemp += " BRS";
                        if ((m_Msg.MSGTYPE & TPCANMessageType.PCAN_MESSAGE_ESI) == TPCANMessageType.PCAN_MESSAGE_ESI)
                            strTemp += " ESI";
                        strTemp += " ]";
                    }
                }

                return strTemp;
            }

        }
        #endregion

        #region Delegates
        /// <summary>
        /// Read-Delegate Handler
        /// </summary>
        private delegate void ReadDelegateHandler();
        #endregion

        #region Members
        /// <summary>
        /// Saves the desired connection mode
        /// </summary>
        private static bool m_IsFD;
        /// <summary>
        /// Saves the handle of a PCAN hardware
        /// </summary>
        private static TPCANHandle m_PcanHandle;
        /// <summary>
        /// Saves the baudrate register for a conenction
        /// </summary>
        private static TPCANBaudrate m_Baudrate;
        /// <summary>
        /// Saves the type of a non-plug-and-play hardware
        /// </summary>
        private static TPCANType m_HwType;
        /// <summary>
        /// Stores the status of received messages for its display
        /// </summary>
        private System.Collections.ArrayList m_LastMsgsList;
        /// <summary>
        /// Read Delegate for calling the function "ReadMessages"
        /// </summary>
        private ReadDelegateHandler m_ReadDelegate;
        /// <summary>
        /// Receive-Event
        /// </summary>
        private System.Threading.AutoResetEvent m_ReceiveEvent;
        /// <summary>
        /// Thread for message reading (using events)
        /// </summary>
        private System.Threading.Thread m_ReadThread;
        /// <summary>
        /// Handles of non plug and play PCAN-Hardware
        /// </summary>
        private TPCANHandle[] m_NonPnPHandles;
        #endregion


        public static int pcgCounter = 0;
        public static Dictionary<uint, (TPCANMsg CANMsg, uint Counter)> packetStore = new Dictionary<uint, (TPCANMsg, uint)>();





        static void Main(string[] args)
        {
    
            TPCANStatus stsResult;
            uint iChannelsCount;
            bool bIsFD;

            stsResult = PCANBasic.GetValue(PCANBasic.PCAN_NONEBUS, TPCANParameter.PCAN_ATTACHED_CHANNELS_COUNT, out iChannelsCount, sizeof(uint));
            if (stsResult == TPCANStatus.PCAN_ERROR_OK)
            {
                TPCANChannelInformation[] info = new TPCANChannelInformation[iChannelsCount];

                stsResult = PCANBasic.GetValue(PCANBasic.PCAN_NONEBUS, TPCANParameter.PCAN_ATTACHED_CHANNELS, info);
                if (stsResult == TPCANStatus.PCAN_ERROR_OK)
                {
                    foreach (TPCANChannelInformation channel in info)
                        if ((channel.channel_condition & PCANBasic.PCAN_CHANNEL_AVAILABLE) == PCANBasic.PCAN_CHANNEL_AVAILABLE)
                        {
                            bIsFD = (channel.device_features & PCANBasic.FEATURE_FD_CAPABLE) == PCANBasic.FEATURE_FD_CAPABLE;
                            Console.WriteLine(FormatChannelName(channel.channel_handle, bIsFD));
                        }
                }

            }
            m_PcanHandle = 81;
            m_Baudrate = TPCANBaudrate.PCAN_BAUD_500K;
            m_HwType = TPCANType.PCAN_TYPE_ISA;

            stsResult = PCANBasic.Initialize(
                    m_PcanHandle,
                    m_Baudrate,
                    m_HwType,
                    Convert.ToUInt32("0100", 16),
                    Convert.ToUInt16("3"));


            TPCANMsg CANMsg;
            TPCANTimestamp CANTimeStamp;

            while (true)
            {
                stsResult = PCANBasic.Read(m_PcanHandle, out CANMsg, out CANTimeStamp);
                if (stsResult != TPCANStatus.PCAN_ERROR_QRCVEMPTY)
                {
                    if (packetStore.ContainsKey(CANMsg.ID))
                    {
                        var packet = packetStore[CANMsg.ID];
                        packetStore[CANMsg.ID] = (packet.CANMsg, packet.Counter + 1);
                    }
                    else
                    {
                        packetStore[CANMsg.ID] = (CANMsg, 1);
                    }
                   
                    if(CANMsg.ID == 0x301)
                    {
                        string dataStr = $"Inputs: {CANMsg.DATA[0]:X2}; Outputs: {CANMsg.DATA[1]:X2}; Pressure: {(CANMsg.DATA[2] << 8) + CANMsg.DATA[3]};  Plim: {(CANMsg.DATA[4] << 8) + CANMsg.DATA[5]}; Pdelta: {CANMsg.DATA[6]}; level: {CANMsg.DATA[7]}";
                        Console.WriteLine(dataStr);
                    }
                    else if(CANMsg.ID == 0x080)
                    {
                        Int16[] temp = new Int16[4];
                        for(int i = 0; i < 4; i++)
                        {
                            temp[i] = (Int16)((CANMsg.DATA[2*i + 1] << 8) + CANMsg.DATA[2*i]);
                        }
                        Console.WriteLine($"T1: {temp[0]}; T2: {temp[1]}; T3: {temp[2]}; T4: {temp[3]}; ");
                    }
                    else
                    {
                        Console.WriteLine(string.Format("{0:X2}:{1:X2} {2:X2} {3:X2} {4:X2} {5:X2} {6:X2} {7:X2} {8:X2} N {9}", CANMsg.ID, CANMsg.DATA[0], CANMsg.DATA[1], CANMsg.DATA[2], CANMsg.DATA[3], CANMsg.DATA[4], CANMsg.DATA[5], CANMsg.DATA[6], CANMsg.DATA[7], pcgCounter));
                    }
                    
                    pcgCounter++;
                }
            }
        }

        public static TPCANStatus ReadMessage()
        {
            TPCANMsg CANMsg;
            TPCANTimestamp CANTimeStamp;
            TPCANStatus stsResult;
            stsResult = PCANBasic.Read(m_PcanHandle, out CANMsg, out CANTimeStamp);
            if (stsResult != TPCANStatus.PCAN_ERROR_QRCVEMPTY)
            {
                Console.WriteLine(string.Format("{0:X2}:{1:X2} {2:X2} {3:X2} {4:X2} {5:X2} {6:X2} {7:X2} {8:X2} N {9}", CANMsg.ID, CANMsg.DATA[0], CANMsg.DATA[1], CANMsg.DATA[2], CANMsg.DATA[3], CANMsg.DATA[4], CANMsg.DATA[5], CANMsg.DATA[6], CANMsg.DATA[7], pcgCounter));
                pcgCounter++;
            }
            return stsResult;
        }

        public static string FormatChannelName(TPCANHandle handle, bool isFD)
        {
            TPCANDevice devDevice;
            byte byChannel;

            // Gets the owner device and channel for a 
            // PCAN-Basic handle
            //
            if (handle < 0x100)
            {
                devDevice = (TPCANDevice)(handle >> 4);
                byChannel = (byte)(handle & 0xF);
            }
            else
            {
                devDevice = (TPCANDevice)(handle >> 8);
                byChannel = (byte)(handle & 0xFF);
            }

            // Constructs the PCAN-Basic Channel name and return it
            //
            if (isFD)
                return string.Format("{0}:FD {1} ({2:X2}h)", devDevice, byChannel, handle);
            else
                return string.Format("{0} {1} ({2:X2}h)", devDevice, byChannel, handle);
        }
    }
}
