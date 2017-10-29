using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectE448_Final
{
    public class SQLiteDB_Manager
    {
        MainTable_SQLite mainTable_SQLite = new MainTable_SQLite() { };

        public async Task handleDataForSQLiteAsync(byte[] dataStr)
        {
            //need to break the reply up andthen sqlite it
            byte[] choppedMsg = new byte[1000];
            int choppedLen = 0;
            for (int i = 0; i < dataStr.Length; i++)
            {
                if (dataStr[i + 2] == 0x04)
                {
                    choppedLen = i;
                    break;
                }
                choppedMsg[i] = dataStr[i + 2];
            }

            string str = System.Text.Encoding.UTF8.GetString(choppedMsg, 0, (choppedLen - 1));

            int commas = str.Split(',').Length - 1;

            string[] strDBLINES = str.Split(',');

            string[] strENTRYMODEL = new string[] { };

            for (int i = 0; i < (commas + 1); i++)
            {
                strENTRYMODEL = new string[] { };
                strENTRYMODEL = strDBLINES[i].Split(':');
                mainTable_SQLite = new MainTable_SQLite() { };

                mainTable_SQLite.ID_BBB = strENTRYMODEL[0];
                mainTable_SQLite.Device = strENTRYMODEL[1];
                mainTable_SQLite.Data_Time = strENTRYMODEL[2];
                mainTable_SQLite.Data_State = strENTRYMODEL[3];
                mainTable_SQLite.Has_Sent = 0;

                await App.Database.SaveItemAsync(mainTable_SQLite);
            }
        }
    }
}
