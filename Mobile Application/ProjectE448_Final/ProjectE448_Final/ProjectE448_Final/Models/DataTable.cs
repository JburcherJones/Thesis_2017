using SQLite;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectE448_Final
{
    public class DataTable
    {
        [PrimaryKey, AutoIncrement]
        public int ID { get; set; }
        public DateTime dateTime { get; set; }

        public double dataX { get; set; }

        public double dataY { get; set; }

        public double dataZ { get; set; }

    }
}
