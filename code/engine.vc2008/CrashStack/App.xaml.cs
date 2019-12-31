using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

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
            App pApp = new App();
            MainWindow pWindows = new MainWindow(InputText[0]);
            pApp.Run(pWindows);

            return App.ExitCode;
        }
    }
}
