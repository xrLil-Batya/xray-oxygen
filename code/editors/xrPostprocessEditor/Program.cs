using System;
using System.Windows.Forms;

namespace xrPostprocessEditor
{
    internal static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            XRay.System.Initialize("xrPostrprocessEditor");
            using (var engine = new EditorEngine())
            {
                var mainDialog = new MainDialog();
                mainDialog.Initialize(engine);
                Application.Run(mainDialog);
            }
            //Core.Destroy();
        }
    }
}
