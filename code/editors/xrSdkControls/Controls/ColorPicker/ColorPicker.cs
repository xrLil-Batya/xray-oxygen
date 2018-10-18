using System;
using System.Drawing;
using System.Windows.Forms;
using XRay.SdkControls.Controls.ColorPicker;

namespace XRay.SdkControls
{
    // XXX nitrocaster: implement hex color TextBox (NumericBox)
    public sealed partial class ColorPicker : UserControl
    {
        public delegate void ColorChangedEventHandler(object sender, Color color);
        public event ColorChangedEventHandler ColorChanged;

        private bool _alphaEnabled = true;
        private HorizontalAlignment _textAlignment = HorizontalAlignment.Left;

        public ColorPicker() => InitializeComponent();

        public Color Value
        {
            get => pbColor.ColorSample;
            set
            {
                //if (pbColor.ColorSample == value)
                //    return;

                byte alphaValue = _alphaEnabled ? value.A : byte.MaxValue;

                nslAlpha.Value = alphaValue;
                nslRed.Value = value.R;
                nslGreen.Value = value.G;
                nslBlue.Value = value.B;

                // XXX collectioner: dirty hardcode. Should be changed
                pbColor.ColorSample = Color.FromArgb(alphaValue, value.R, value.G, value.B);

				isAReversed.Visible = true;
				isRReversed.Visible = true;
				isGReversed.Visible = true;
				isBReversed.Visible = true;
            }
        }

        public bool AlphaEnabled
        {
            get => _alphaEnabled;
            set
            {
                _alphaEnabled = value;
                nslAlpha.Visible = value;
                lAlpha.Visible = value;
                nslAlpha.Value = byte.MaxValue;
            }
        }

        public HorizontalAlignment TextAlign
        {
            get => _textAlignment;
            set
            {
                if (Equals(_textAlignment, value))
                    return;
                _textAlignment = value;
                //nslAlpha.TextAlign = value;
                //nslRed.TextAlign = value;
                //nslGreen.TextAlign = value;
                //nslBlue.TextAlign = value;
            }
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            SubscribeOnSliderEvents();
        }

        private void SubscribeOnSliderEvents()
        {
            foreach (var control in Controls)
            {
                if (!(control is NumericSlider slider)) continue;

                slider.TemporaryValueChanged += UpdateColorSample;
                slider.ValueChanged += UpdateColor;
            }
        }

        private void UpdateColor(object sender, decimal value)
        {
            UpdateColorSample(sender, value);

            ColorChanged?.Invoke(this, pbColor.ColorSample);
        }

        private void UpdateColorSample(object sender, decimal value)
        {
            if (!(sender is NumericSlider slider)) return;
            var currentColor = pbColor.ColorSample;

            pbColor.ColorSample = currentColor.SetColorChannel(slider.Tag.ToString(), Convert.ToByte(value));
        }
    }
}
