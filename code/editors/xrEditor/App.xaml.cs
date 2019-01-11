using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace xrEditor
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(System.Windows.StartupEventArgs e)
        {
            SpectreEngine.xrCoreInit("Editor", "fs.ltx");
            SpectreEngine.xrEngineInit();
            SpectreEngine.xrRenderInit();
            base.OnStartup(e);
        }

    }
}
