using Sockets.Plugin;
using Syncfusion.SfChart.XForms;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace ProjectE448_Final
{
    public partial class MainPage : ContentPage
    {
        Communications Comms = new Communications();
        DeviceManager deviceManager;
        LocationManager locationManager;
        MainTableManager mainTableManager;
        ColumnSeries seriesY = new ColumnSeries();
        List<DataTable> tvdList = new List<DataTable>();


        public MainPage()
        {
            InitializeComponent();
            deviceManager = DeviceManager.DefaultManager;           //static funcy thing
            locationManager = LocationManager.DefaultManager;
            mainTableManager = MainTableManager.DefaultManager;
            DateTime dt = DateTime.Now;
            for (int i = 0; i < 10; i++)
            {
                tvdList.Add(new DataTable()
                {
                    dataY = i,
                    dateTime = dt
                });
                dt = dt.AddSeconds(1);
            }
            //ChartY.ColorModel = new ChartColorModel()
            //{
            //    Palette = ChartColorPalette.Custom,
            //    CustomBrushes = new ChartColorCollection()
            //     {
            //         Color.Red

            //     }
            //};
            seriesY.ItemsSource = tvdList;
            seriesY.YBindingPath = "dataY";
            seriesY.XBindingPath = "dateTime";
            ChartY.Series.Clear();
            ChartY.Series.Add(seriesY);
        }

        public void setMessageLable(string s)
        {
            Message.Text = s;
        }

        private void connctToSever_Clicked(object sender, EventArgs e)
        {
            Comms.getConnectionAsync();
            connectionStatus.TextColor = Color.Green;
            connectionStatus.Text = "CONNECTED";
            connctToSever.IsEnabled = false;
            connctToSever.IsVisible = false;
            Comms.receiveTCP();
        }

        private void reqSQL_Clicked(object sender, EventArgs e)
        {
            byte[] reqSQLMsg = new byte[3] { 0x02, 0x52, 0x04 };
            Comms.TCPIPSend(reqSQLMsg);

            Device.BeginInvokeOnMainThread(() =>
            {
                reqSQL.IsEnabled = false;                           //hides the message till we can req again
                reqSQL.IsVisible = false;
            });

        }

        private async void pushSQL_Clicked(object sender, EventArgs e)
        {
            pushSQL.IsEnabled = false;
            pushSQL.IsVisible = false;
            List<MainTable_SQLite> thing2 = new List<MainTable_SQLite>() { };
            thing2 = await App.Database.GetUnsentItemsAsync();

            if (thing2 != null)
            {
                foreach (var item in thing2)
                {

                    MainTable mainTable = new MainTable
                    {
                        ID_BBB = int.Parse(item.ID_BBB),
                        ID_CP = item.ID,
                        Device_ID = item.Device,
                        Data_Time = item.Data_Time,
                        Data_State = item.Data_State
                    };
                    await mainTableManager.SaveTaskAsync(mainTable);

                    item.Has_Sent = 1;
                    await App.Database.SaveItemAsync(item);
                }
            }
            pushSQL.IsEnabled = true;
            pushSQL.IsVisible = true;
        }

        public void tcpDisconnectedView()
        {
            Device.BeginInvokeOnMainThread(() =>
            {
                connectionStatus.TextColor = Color.Red;
                connectionStatus.Text = "NOT CONNECTED";
                connctToSever.IsEnabled = true;
                connctToSever.IsVisible = true;
            });

        }

        public void reqSQLView()
        {
            Device.BeginInvokeOnMainThread(() =>
            {
                reqSQL.IsEnabled = true;
                reqSQL.IsVisible = true;
            });

        }
    }
}
