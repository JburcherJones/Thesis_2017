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
    public partial class MainTableManager
    {
        static MainTableManager defaultInstance = new MainTableManager();
        MobileServiceClient client;

        IMobileServiceTable<MainTable> mainTable;

        private MainTableManager()
        {
            this.client = new MobileServiceClient(Constants.ApplicationURL);
            this.mainTable = client.GetTable<MainTable>();
        }

        public static MainTableManager DefaultManager
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

        public async Task<ObservableCollection<MainTable>> GetDevicesAsync(bool syncItems = false)
        {
            try
            {
                IEnumerable<MainTable> items = await mainTable
                    .ToEnumerableAsync();

                return new ObservableCollection<MainTable>(items);
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

        public async Task SaveTaskAsync(MainTable item)
        {
            if (item.Id == null)
            {
                await mainTable.InsertAsync(item);
            }
            else
            {
                await mainTable.UpdateAsync(item);
            }
        }
    }
}
