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
        XRay.Editor.EditorOcclusion ourOcclusion;
        public MainWindow()
		{
			tickThread = new RenderTickThread(this);
            ourOcclusion = new XRay.Editor.EditorOcclusion();
            InitializeComponent();
		}

		public XRay.XRayRenderHost GetXRayHostInterop()
		{
			return AWDA;
		}

		public override void EndInit()
		{
			base.EndInit();
            XRay.XRayRenderHost renderHost = GetXRayHostInterop();
            
            renderHost.EditorMode = !EngineMode.IsChecked.Value;
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
            string testFile = "D:\\Games\\S.T.A.L.K.E.R\\SDK\\editors\\rawdata\\objects\\dynamics\\fence\\debris_01.object";
            XRay.File eObject = XRay.File.OpenExternalRead(testFile);
            string[] fileTokens = testFile.Split('\\');

            XRay.Editor.EObject realObject = XRay.Editor.EObject.CreateEObject(eObject, fileTokens[fileTokens.Length - 1]);
            XRay.Model objectModel = realObject.renderable;
            //ourOcclusion.AddObjectToRenderableList()
            // Hide console, and load to viewport a EObject
            ourOcclusion.AddObjectToRenderableList(objectModel);
        }
    }
}
