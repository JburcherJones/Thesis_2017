using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectE448_Final
{
    public class MainTable_SQLite
    {
        [PrimaryKey, AutoIncrement]
        public int ID { get; set; }
        public string ID_BBB { get; set; }
        public string Device { get; set; }
        public string Data_Time { get; set; }
        public string Data_State { get; set; }
        public int Has_Sent { get; set; }
    }
}
