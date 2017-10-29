using System;
using Microsoft.WindowsAzure.MobileServices;
using Newtonsoft.Json;

namespace ProjectE448_Final
{
	public class DeviceTable
	{
		string id;
        string device_ID;
        string mac_Adrs;
        string location;
        string type;


        [JsonProperty(PropertyName = "id")]
		public string Id
		{
			get { return id; }
			set { id = value;}
		}

		[JsonProperty(PropertyName = "Device_ID")]
		public string Device_ID
        {
			get { return device_ID; }
			set { device_ID = value;}
		}

        [JsonProperty(PropertyName = "Mac_Adrs")]
        public string Mac_Adrs
        {
            get { return mac_Adrs; }
            set { mac_Adrs = value; }
        }

        [JsonProperty(PropertyName = "Location")]
        public string Location
        {
            get { return location; }
            set { location = value; }
        }

        [JsonProperty(PropertyName = "Type")]
        public string Type
        {
            get { return type; }
            set { type = value; }
        }



        [Version]
        public string Version { get; set; }
	}
}

