using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Xamarin.Forms;

namespace ProjectE448_Final
{
    public partial class App : Application
    {
        static MainTable_SQLiteDatabase database;
        public static MainPage myMainPage = new MainPage();
        public App()
        {
            InitializeComponent();

            MainPage = myMainPage;

        }

        public static MainTable_SQLiteDatabase Database
        {
            get
            {
                if (database == null)
                {
                    database = new MainTable_SQLiteDatabase(DependencyService.Get<IFileHelper>().GetLocalFilePath("ProjectE448SQLite.db3"));
                }
                return database;
            }
        }


        protected override void OnStart()
        {
            // Handle when your app starts
        }

        protected override void OnSleep()
        {
            // Handle when your app sleeps
        }

        protected override void OnResume()
        {
            // Handle when your app resumes
        }
    }
}
