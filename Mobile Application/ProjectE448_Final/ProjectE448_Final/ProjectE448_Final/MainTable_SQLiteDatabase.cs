using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectE448_Final
{
    public class MainTable_SQLiteDatabase
    {
        readonly SQLiteAsyncConnection database;

        public MainTable_SQLiteDatabase(string dbPath)
        {
            database = new SQLiteAsyncConnection(dbPath);
            database.CreateTableAsync<MainTable_SQLite>().Wait();
        }

        public Task<List<MainTable_SQLite>> GetItemsAsync()
        {
            return database.Table<MainTable_SQLite>().ToListAsync();
        }

        public Task<List<MainTable_SQLite>> GetUnsentItemsAsync()
        {
            return database.QueryAsync<MainTable_SQLite>("SELECT * FROM MainTable_SQLite WHERE Has_Sent = 0");
        }


        /*   public Task<List<MainTable>> GetItemsNotDoneAsync()
           {
               return database.QueryAsync<MainTable>("SELECT * FROM [MainTable] WHERE [Done] = 0");
           }*/

        public Task<MainTable_SQLite> GetItemAsync(int id)
        {
            return database.Table<MainTable_SQLite>().Where(i => i.ID == id).FirstOrDefaultAsync();
        }

        public Task<int> SaveItemAsync(MainTable_SQLite item)
        {
            if (item.ID != 0)
            {
                return database.UpdateAsync(item);
            }
            else
            {
                return database.InsertAsync(item);
            }
        }

        public Task<int> DeleteItemAsync(MainTable_SQLite item)
        {
            return database.DeleteAsync(item);
        }
    }
}
