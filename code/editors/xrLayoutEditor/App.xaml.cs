using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using XRay;

namespace xrLayoutEditor
{
    /// <summary>
    /// Логика взаимодействия для App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(System.Windows.StartupEventArgs e)
        {
            SpectreEngine.xrCoreInit();
            SpectreEngine.xrEngineInit();
            base.OnStartup(e);

            Log.Info(Filesystem.IsFileExist("fsgame.ltx") ? "fs.ltx are exist" : "fs.ltx are not exist");

            Ini ini = new Ini("system.ltx");

            if (ini.IsSectionExist("zone_pick_dof"))
            {
                Log.Info("section is exist");
            }
            else
            {
                Log.Info("section is not exist");
            }
        }
    }
}
