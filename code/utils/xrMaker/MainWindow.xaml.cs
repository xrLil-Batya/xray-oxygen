using System.Windows;
using System.IO;

namespace xrMaker
{
	/// <summary>
	/// Логика взаимодействия для MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		public MainWindow()
		{
			InitializeComponent();
			this.WorkDir.Text = Directory.GetCurrentDirectory();
		}

		private void MakeSLNFile(object sender, RoutedEventArgs e)
		{
			CMakeSolution Maker = new CMakeSolution(this.WorkDir.Text);

			Maker.Make(this.BinDir.Text);
		}
	}
}
