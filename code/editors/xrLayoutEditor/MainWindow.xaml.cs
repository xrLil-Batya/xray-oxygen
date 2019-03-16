using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace xrLayoutEditor
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
		public MainWindow()
		{
			InitializeComponent();

            XRay.Xml xml = new XRay.Xml("ui", "ui_mm_loading_screen.xml");
            XRay.Log.Info(String.Format("result {0}", xml.ReadAttributeInt("View", 0, "x", 0)));

            XRay.Test test = new XRay.Test();
		}
    }
}
