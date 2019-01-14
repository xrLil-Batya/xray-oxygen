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
		private XRay.xrManagedUILib.UIXMLParser UIParser;

		public MainWindow()
		{
			InitializeComponent();

			UIParser = new XRay.xrManagedUILib.UIXMLParser();

            XRay.xrManagedUILib.Test test = new XRay.xrManagedUILib.Test();
		}
    }
}
