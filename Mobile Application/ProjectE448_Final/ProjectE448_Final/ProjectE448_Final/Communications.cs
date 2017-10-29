using Sockets.Plugin;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace ProjectE448_Final
{

    public class Communications
    {
        string address;
        int port;
        TcpSocketClient client;
        SQLiteDB_Manager sqliteManager = new SQLiteDB_Manager();

        byte[] myID = new byte[4] { 0x02, 0x13, 0x31, 0x04 };
        byte[] bbbAK = new byte[3] { 0x02, 0x06, 0x04 };

        async public void getConnectionAsync()
        {
            address = "192.168.4.1";
            //address = "192.168.121.137";
            port = 8888;
            client = new TcpSocketClient();
            await client.ConnectAsync(address, port);
        }

        public async void receiveTCP()
        {
            try
            {
                bool validMsg = false;
                bool msgComplete = false;
                byte[] finalMsg = new byte[1000];


                int index_finalMsg = 0;

                await Task.Run(async () =>
                {
                    try
                    {
                        byte[] byteTCP = new byte[1];
                        while (client.ReadStream.Read(byteTCP, 0, 1) != 0)
                        {
                            /*determine if the message is valid before doing anything else*/
                            if (byteTCP[0] == 0x02)
                            {
                                validMsg = true;
                            }
                            else if (byteTCP[0] == 0x04)
                            {
                                msgComplete = true;
                            }
                            /*determine if the message is valid before doing anything else*/

                            /*save the message if it is valid*/
                            if (validMsg == true)
                            {
                                finalMsg[index_finalMsg] = byteTCP[0];
                                index_finalMsg++;
                            }
                            if (msgComplete == true)
                            {
                                validMsg = false;
                                /*Handle the complete message*/
                                await handleTCPAsync(finalMsg);
                                /*Handle the complete message*/
                                msgComplete = false;
                                index_finalMsg = 0;
                                finalMsg = new byte[1000];
                            }
                            /*save the message if it is valid*/
                        }
                        await client.DisconnectAsync();
                        Device.BeginInvokeOnMainThread(() =>
                        {
                            App.myMainPage.tcpDisconnectedView();
                        });
                    }
                    catch (Exception e)
                    {
                        await client.DisconnectAsync();
                        Device.BeginInvokeOnMainThread(() =>
                        {
                            App.myMainPage.tcpDisconnectedView();
                        });
                    }
                });
            }
            catch (Exception e)
            {
                await client.DisconnectAsync();
                Device.BeginInvokeOnMainThread(() =>
                {
                    App.myMainPage.tcpDisconnectedView();
                });
            }
        }

        public async Task handleTCPAsync(byte[] tcp)
        {
            if (tcp[1] == 0x05)                                //server wants to know what we are
            {
                TCPIPSend(myID);
            }
            else if (tcp[1] == 0x06)                            //must get this message when server is in wating state
            {
                //connected, ready for a request or data
                Device.BeginInvokeOnMainThread(() =>
                {
                    App.myMainPage.reqSQLView();                //allows button to be pressed that sends req
                });
            }
            else if (tcp[1] == 0x44)                            //message contains data
            {
                //handle data
            }
            else if ((tcp[1] == 0x52) && (tcp[1] == 0x15))        //NAK on sql req
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    App.myMainPage.reqSQLView();                //allows button to be pressed that sends req
                });
            }
            else if ((tcp[1] == 0x53) && (tcp[2] == 0x17))
            {
                Device.BeginInvokeOnMainThread(() =>
                {
                    App.myMainPage.reqSQLView();                //allows button to be pressed that sends req
                });
                TCPIPSend(bbbAK);
            }
            else if (tcp[1] == 0x53)
            {
                await sqliteManager.handleDataForSQLiteAsync(tcp);

                TCPIPSend(bbbAK);
            }
        }

        public async void TCPIPSend(byte[] myByteArr)
        {
            try
            {
                if (client != null)
                {
                    await client.WriteStream.WriteAsync(myByteArr, 0, myByteArr.Length);
                    await client.WriteStream.FlushAsync();
                }
            }
            catch (Exception)
            {
                await client.DisconnectAsync();
                App.myMainPage.tcpDisconnectedView();
            }
        }
    }
}
