using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace xrEditor
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		RenderTickThread tickThread;
		public MainWindow()
		{
			tickThread = new RenderTickThread(this);
			InitializeComponent();
		}

		public XRay.XRayRenderHost GetXRayHostInterop()
		{
			return AWDA;
		}

		public override void EndInit()
		{
			base.EndInit();
			GetXRayHostInterop().EditorMode = !EngineMode.IsChecked.Value;
			tickThread.StartLoopThread();
		}
		private void checkBox_Checked(object sender, RoutedEventArgs e)
		{
			if(AWDA != null)
				AWDA.EditorMode = false;
		}
		private void checkBox_Unchecked(object sender, RoutedEventArgs e)
		{
			if(AWDA != null)
				AWDA.EditorMode = true;
		}
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            XRay.File eObject = XRay.File.OpenExternalRead("E:\\X-Ray CoP SDK\\editors\rawdata\\objects\\dynamics\\fence\\debris_01.object");
            XRay.Editor.EObject realObject = XRay.Editor.EObject.CreateEObject(eObject);

            // Hide console, and load to viewport a EObject

        }
    }
}
