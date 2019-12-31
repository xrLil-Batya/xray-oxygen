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

namespace CrashStack
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow(string InputReport)
        {
            InitializeComponent();
            this.ReportBox.Text = InputReport;

            var Localization = System.Globalization.CultureInfo.CurrentCulture;
            if (Localization.ToString() == "ru-RU")
            {
                this.BreakBtn.Content = "Прервать";
                this.ContinueBtn.Content = "Продолжить";
                this.ReportBtn.Content = "Сообщить";
            }
        }

        private void MakeBreak(object sender, RoutedEventArgs e)
        {
            App.ExitCode = 1;
            this.Close();
        }

        private void MakeContinue(object sender, RoutedEventArgs e)
        {
            App.ExitCode = 0;
            this.Close();
        }

        private void MakeReport(object sender, RoutedEventArgs e)
        {
            Clipboard.SetText(this.ReportBox.Text);
            System.Diagnostics.Process.Start("https://discord.gg/Gt5GT5x");
        }
    }
}
