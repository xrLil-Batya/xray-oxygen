using System;
using System.Windows.Forms;

namespace XRay.SdkControls
{
    public sealed partial class NumericSlider : UserControl
    {
        private bool _isClicked;

        public delegate void SliderValueChanged(object sender, decimal value);
        public event SliderValueChanged TemporaryValueChanged;
        public event SliderValueChanged ValueChanged;

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
                ValueChanged?.Invoke(this, numSpinner.Value);
            };

            trackBar.MouseDown += (s, args) => { _isClicked = true; };

            trackBar.MouseUp += (s, args) =>
            {
                if (!_isClicked)
                {
                    return;
                }

                _isClicked = false;

                ValueChanged?.Invoke(this, trackBar.Value);
            };

            trackBar.Scroll += (sender, args) =>
            {
                numSpinner.Value = trackBar.Value;

                TemporaryValueChanged?.Invoke(this, numSpinner.Value);
            };
        }
    }
}
