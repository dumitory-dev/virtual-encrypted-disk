using MahApps.Metro.SimpleChildWindow;
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
using MahApps.Metro.Controls;
using MahApps.Metro.Controls.Dialogs;

namespace GUI
{
    /// <summary>
    /// Interaction logic for MountDisk.xaml
    /// </summary>
    public partial class MountDisk : ChildWindow
    {
        public string Password { get; set; }
        public char Letter { get; set; }
        public MountDisk() 
        {
            InitializeComponent();
            this.AvailableLetters.ItemsSource = Utils.Utils.GetAvailableDiskLetters();
         
        }

        private void OkButtonOnClick(object sender, RoutedEventArgs e)
        {
           
        }

        private void CloseSec_OnClick(object sender, RoutedEventArgs e)
        {
            this.Close(CloseReason.Cancel, false);
        }

        private  void MessageButtonOnClick(object sender, RoutedEventArgs e)
        {
            if (this.MountPassword.Password.Length == 0 || this.AvailableLetters.Text.Length == 0)
                return;

            this.Password = this.MountPassword.Password;
            this.Letter = (char)AvailableLetters.SelectionBoxItem;

            this.Close(CloseReason.Ok, true);
        }
    }
}
