using System;
using System.Windows.Forms;

namespace XRay.SdkControls
{
    public sealed partial class NumericSlider : UserControl
    {
        //private bool _ignoreOnChanged;

        public NumericSlider() => InitializeComponent();

        public event EventHandler ValueChanged;
        
        private void OnValueChanged()
        {
            ValueChanged?.Invoke(this, null);
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            numSpinner.ValueChanged += (obj, args) =>
            {
                //if (_ignoreOnChanged)
                //    return;
                OnValueChanged();
            };
        }
    }
}
