using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.IO;

namespace CrashStack
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public static int ExitCode;
		
        [STAThread]
        public static int Main(string[] InputText)
        {
            if (InputText.Length == 0)
            {
                MessageBox.Show("REPORT FILE MISSING!");
                return 1;
            }

            App pApp = new App();

            bool bReadedAllText = true;
            string CrashContent = "";
            try
            {
                CrashContent = File.ReadAllText(InputText[0]);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                bReadedAllText = false;
            }

            if (bReadedAllText)
            {
                MainWindow pWindows = new MainWindow(CrashContent);
                pApp.Run(pWindows);
            }
            else
            {
                MessageBox.Show(InputText[0]);
                return 1;
            }

            return App.ExitCode;
        }
    }
}
