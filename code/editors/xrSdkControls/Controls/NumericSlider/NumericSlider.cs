using System;
using System.Windows.Forms;

namespace XRay.SdkControls
{
    public sealed partial class NumericSlider : UserControl
    {
        public delegate void SliderValueChanged(decimal value);

        public event EventHandler ValueChanged;

        public decimal Value
        {
            get => trackBar.Value;
            set
            {
                trackBar.Value = (int)value;
                numSpinner.Value = value;
            }
        }

        public NumericSlider() => InitializeComponent();
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            numSpinner.ValueChanged += (obj, args) =>
            {
                trackBar.Value = (int) numSpinner.Value; 
                ValueChanged?.Invoke(this, null);
            };

            trackBar.Scroll += (sender, args) =>
            {
                numSpinner.Value = trackBar.Value;
                ValueChanged?.Invoke(this, null);
            };
        }
    }
}
