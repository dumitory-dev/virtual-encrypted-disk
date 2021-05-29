using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace GUI.View
{
    class VirtualDiskView : INotifyPropertyChanged
    {

        private readonly ObservableCollection<VirtualDisk> _virtualDisks;
        private VirtualDisk _selectedDisk;

        public VirtualDiskView()
        {
            this._virtualDisks = new ObservableCollection<VirtualDisk>();
        }


        public ObservableCollection<VirtualDisk> DiskCollection => this._virtualDisks;

        public VirtualDisk SelectedDisk
        {
            get => this._selectedDisk;

            set
            {
                if (value == this._selectedDisk) return;
                this._selectedDisk = value;
                this.OnPropertyChanged("SelectedDisk");
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            var handler = this.PropertyChanged;
            if (handler == null) return;
            var e = new PropertyChangedEventArgs(propertyName);
            handler(this, e);
        }
        public ICommand DeleteDisk => new DelegateCommand(this.OnDelete);

        private void OnDelete()
        {
            if (this._selectedDisk != null)
                this._virtualDisks.Remove(this._selectedDisk);
        }


    }

    public class DelegateCommand : ICommand
    {
        public delegate void SimpleEventHandler();
        private readonly SimpleEventHandler _handler;
        private bool _isEnabled = true;

        public event EventHandler CanExecuteChanged;

        public DelegateCommand(SimpleEventHandler handler)
        {
            this._handler = handler;
        }

        private void OnCanExecuteChanged()
        {
            if (this.CanExecuteChanged != null)
            {
                this.CanExecuteChanged(this, EventArgs.Empty);
            }
        }

        bool ICommand.CanExecute(object arg)
        {
            return this.IsEnabled;
        }

        void ICommand.Execute(object arg)
        {
            this._handler();
        }

        public bool IsEnabled
        {
            get => this._isEnabled;

            set
            {
                this._isEnabled = value;
                this.OnCanExecuteChanged();
            }
        }
    }
}
