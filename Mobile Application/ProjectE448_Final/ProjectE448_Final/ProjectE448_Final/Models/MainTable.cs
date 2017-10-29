using System;
using Microsoft.WindowsAzure.MobileServices;
using Newtonsoft.Json;

namespace ProjectE448_Final
{
	public class MainTable
    {
		string id;
        int id_BBB;
        int id_CP;
        string device_ID;
        string data_Time;
        string data_State;


        [JsonProperty(PropertyName = "id")]
		public string Id
		{
			get { return id; }
			set { id = value;}
		}

		[JsonProperty(PropertyName = "ID_BBB")]
		public int ID_BBB
        {
			get { return id_BBB; }
			set { id_BBB = value;}
		}

        [JsonProperty(PropertyName = "ID_CP")]
        public int ID_CP
        {
            get { return id_CP; }
            set { id_CP = value; }
        }

        [JsonProperty(PropertyName = "Device_ID")]
        public string Device_ID
        {
            get { return device_ID; }
            set { device_ID = value; }
        }

        [JsonProperty(PropertyName = "Data_Time")]
        public string Data_Time
        {
            get { return data_Time; }
            set { data_Time = value; }
        }

        [JsonProperty(PropertyName = "Data_State")]
        public string Data_State
        {
            get { return data_State; }
            set { data_State = value; }
        }



        [Version]
        public string Version { get; set; }
	}
}

