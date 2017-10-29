using System;
using Microsoft.WindowsAzure.MobileServices;
using Newtonsoft.Json;

namespace ProjectE448_Final
{
	public class LocationTable
    {
		string id;
        string country;
        string mac_Adrs;
        string province_state;
        string town;
        string suburb;
        string street; 
        string house_Num;

        [JsonProperty(PropertyName = "id")]
		public string Id
		{
			get { return id; }
			set { id = value;}
		}

		[JsonProperty(PropertyName = "Country")]
		public string Country
        {
			get { return country; }
			set { country = value;}
		}

        [JsonProperty(PropertyName = "Province_State")]
        public string Province_State
        {
            get { return province_state; }
            set { province_state = value; }
        }

        [JsonProperty(PropertyName = "Town")]
        public string Town
        {
            get { return town; }
            set { town = value; }
        }

        [JsonProperty(PropertyName = "Suburb")]
        public string Suburb
        {
            get { return suburb; }
            set { suburb = value; }
        }

        [JsonProperty(PropertyName = "Street")]
        public string Street
        {
            get { return street; }
            set { street = value; }
        }

        [JsonProperty(PropertyName = "House_Num")]
        public string House_Num
        {
            get { return house_Num; }
            set { house_Num = value; }
        }



        [Version]
        public string Version { get; set; }
	}
}

