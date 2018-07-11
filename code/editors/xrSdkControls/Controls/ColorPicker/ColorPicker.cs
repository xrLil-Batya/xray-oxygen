using System;
using System.Drawing;
using System.Windows.Forms;

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
                if (pbColor.ColorSample == value)
                    return;

                if (_alphaEnabled)
                {
                    nslAlpha.Value = value.A;
                }

                nslRed.Value = value.R;
                nslGreen.Value = value.G;
                nslBlue.Value = value.B;

                pbColor.ColorSample = value;
                UpdateColor();
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
                nslAlpha.Value = value ? byte.MinValue : byte.MaxValue;
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

            nslRed.ValueChanged += (obj, args) => UpdateColor();
            nslGreen.ValueChanged += (obj, args) => UpdateColor();
            nslBlue.ValueChanged += (obj, args) => UpdateColor();
            nslAlpha.ValueChanged += (obj, args) => UpdateColor();

            UpdateColor();
        }

        private void UpdateColor()
        {
            var newColor = Color.FromArgb(
                Convert.ToInt32(nslAlpha.Value),
                Convert.ToInt32(nslRed.Value),
                Convert.ToInt32(nslGreen.Value),
                Convert.ToInt32(nslBlue.Value));

            if (pbColor.ColorSample == newColor)
                return;

            pbColor.ColorSample = newColor;

            ColorChanged?.Invoke(this, newColor);
        }
    }
}
