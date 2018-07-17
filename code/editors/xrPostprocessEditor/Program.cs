using System;
using System.Windows.Forms;
using XRay.ManagedApi.Core;

namespace xrPostprocessEditor
{
    internal static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Core.Initialize("xrPostrprocessEditor");
            using (var engine = new EditorEngine())
            {
                var mainDialog = new MainDialog();
                mainDialog.Initialize(engine);
                Application.Run(mainDialog);
            }
            Core.Destroy();
        }
    }
}
