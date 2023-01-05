using System;
using System.Collections.Generic;
using PCSC;
using PCSC.Iso7816;

namespace WindowsFormsApp1
{
    internal class CardTool
    {
        private static ISCardContext context;
        private static string readerName;
        private static ICardReader cardReader;
        private static string errorMessage = "No card Selected";
        public static void establishConnection()
        {
            context = ContextFactory.Instance.Establish(SCardScope.System);
        }
        public static string[] listOfreaders()
        {
            return context.GetReaders();
        }

        public static void setReader(string rdr)
        {
            try
            {
                if (readerName != rdr)
                {
                    if (readerName != null)
                    {
                        cardReader.Disconnect(SCardReaderDisposition.Unpower);
                    }

                    readerName = rdr;
                    cardReader = context.ConnectReader(readerName, SCardShareMode.Shared, SCardProtocol.Any);

                }
            }
            catch(Exception ex) 
            {
                readerName = null;
                errorMessage = ex.Message;
            }
        }
        public static string ResetCard()
        {
            try
            {
                if (cardReader != null)
                {
                    /*Reset the card*/
                    cardReader.Reconnect(SCardShareMode.Shared,SCardProtocol.Any, SCardReaderDisposition.Reset);

                    ReaderStatus rs = cardReader.GetStatus();
                    var atrbytes = rs.GetAtr();
                    return BitConverter.ToString(atrbytes).Replace("-", " ");
                }
            }catch(Exception ex)
            {
                cardReader = null;
                readerName = null;
                errorMessage = ex.Message;
            }

            return errorMessage;
        }
        public static byte[] sendCommand(byte[] command)
        {
            using (cardReader.Transaction(SCardReaderDisposition.Leave))
            {
                var sendPci = SCardPCI.GetPci(cardReader.Protocol);
                var receivePci = new SCardPCI(); // IO returned protocol control information.

                var receiveBuffer = new byte[256];

                var bytesReceived = cardReader.Transmit(
                    sendPci, // Protocol Control Information (T0, T1 or Raw)
                    command, // command APDU
                    command.Length,
                    receivePci, // returning Protocol Control Information
                    receiveBuffer,
                    receiveBuffer.Length); // data buffer

                var responseApdu =
                    new ResponseApdu(receiveBuffer, bytesReceived, IsoCase.Case2Short, cardReader.Protocol);
                Console.WriteLine("SW1: {0:X2}, SW2: {1:X2}\nUid: {2}",
                    responseApdu.SW1,
                    responseApdu.SW2,
                    responseApdu.HasData ? BitConverter.ToString(responseApdu.GetData()) : "No uid received");

                return responseApdu.ToArray();
            }
        }

    }
}
