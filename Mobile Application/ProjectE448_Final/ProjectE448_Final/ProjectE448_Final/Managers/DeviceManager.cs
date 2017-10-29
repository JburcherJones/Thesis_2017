/*
 * To add Offline Sync Support:
 *  1) Add the NuGet package Microsoft.Azure.Mobile.Client.SQLiteStore (and dependencies) to all client projects
 *  2) Uncomment the #define OFFLINE_SYNC_ENABLED
 *
 * For more information, see: http://go.microsoft.com/fwlink/?LinkId=620342
 */
//#define OFFLINE_SYNC_ENABLED

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Threading.Tasks;
using Microsoft.WindowsAzure.MobileServices;


namespace ProjectE448_Final
{
    public partial class DeviceManager
    {
        static DeviceManager defaultInstance = new DeviceManager();
        MobileServiceClient client;

        IMobileServiceTable<DeviceTable> deviceTable;

        private DeviceManager()
        {
            this.client = new MobileServiceClient(Constants.ApplicationURL);
            this.deviceTable = client.GetTable<DeviceTable>();
        }

        public static DeviceManager DefaultManager
        {
            get
            {
                return defaultInstance;
            }
            private set
            {
                defaultInstance = value;
            }
        }

        public MobileServiceClient CurrentClient
        {
            get { return client; }
        }

        public async Task<ObservableCollection<DeviceTable>> GetDevicesAsync(bool syncItems = false)
        {
            try
            {
                IEnumerable<DeviceTable> items = await deviceTable
                    .ToEnumerableAsync();

                return new ObservableCollection<DeviceTable>(items);
            }
            catch (MobileServiceInvalidOperationException msioe)
            {
                Debug.WriteLine(@"Invalid sync operation: {0}", msioe.Message);
            }
            catch (Exception e)
            {
                Debug.WriteLine(@"Sync error: {0}", e.Message);
            }
            return null;
        }

        public async Task SaveTaskAsync(DeviceTable item)
        {
            if (item.Id == null)
            {
                await deviceTable.InsertAsync(item);
            }
            else
            {
                await deviceTable.UpdateAsync(item);
            }
        }
    }
}
