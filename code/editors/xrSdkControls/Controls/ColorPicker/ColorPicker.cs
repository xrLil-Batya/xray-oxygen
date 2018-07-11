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

        private bool _hexadecimal;
        private bool _ignoreOnChanged;
        private bool _alphaEnabled = true;
        private HorizontalAlignment _textAlignment = HorizontalAlignment.Left;
        
        public ColorPicker()
        {
            InitializeComponent();
        }

        public Color Value
        {
            get => pbColor.ColorSample;
            set
            {
                if (pbColor.ColorSample == value)
                    return;
                _ignoreOnChanged = true;
                if (_alphaEnabled)
                    nslAlpha.Value = value.A;
                nslRed.Value = value.R;
                nslGreen.Value = value.G;
                nslBlue.Value = value.B;
                pbColor.ColorSample = value;
                _ignoreOnChanged = false;
                UpdateColor();
            }
        }

        public byte Red { get; private set; }
        public byte Green { get; private set; }
        public byte Blue { get; private set; }
        public byte Alpha { get; private set; }

        public bool AlphaEnabled
        {
            get => _alphaEnabled;
            set
            {
                if (_alphaEnabled == value)
                    return;
                _alphaEnabled = value;
                _ignoreOnChanged = true;
                nslAlpha.Value = nslAlpha.Maximum;
                _ignoreOnChanged = false;
                UpdateColor();
                lAlpha.Visible = _alphaEnabled;
                nslAlpha.Visible = _alphaEnabled;
                int delta = (_alphaEnabled ? 1 : -1)*27;
                Point loc = chkHexadecimal.Location;
                loc.Y += delta;
                chkHexadecimal.Location = loc;
            }
        }

        public bool Hexadecimal
        {
            get => _hexadecimal;
            set
            {
                if (_hexadecimal == value)
                    return;
                _hexadecimal = value;
                chkHexadecimal.Checked = value;
                nslRed.Hexadecimal = value;
                nslGreen.Hexadecimal = value;
                nslBlue.Hexadecimal = value;
                nslAlpha.Hexadecimal = value;
            }
        }

        public HorizontalAlignment TextAlign
        {
            get => _textAlignment;
            set
            {
                if (_textAlignment == value)
                    return;
                _textAlignment = value;
                nslAlpha.TextAlign = value;
                nslRed.TextAlign = value;
                nslGreen.TextAlign = value;
                nslBlue.TextAlign = value;
            }
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            nslRed.ValueChanged += (obj, args) => UpdateColor();
            nslGreen.ValueChanged += (obj, args) => UpdateColor();
            nslBlue.ValueChanged += (obj, args) => UpdateColor();
            nslAlpha.ValueChanged += (obj, args) => UpdateColor();
            chkHexadecimal.CheckedChanged += (obj, args) => Hexadecimal = chkHexadecimal.Checked;
            UpdateColor();
        }

        private void OnColorChanged()
        {
            if (!_ignoreOnChanged)
                ColorChanged?.Invoke(this, Value);
        }

        private void UpdateColor()
        {
            if (_ignoreOnChanged)
                return;
            var newColor = Color.FromArgb(
                Convert.ToInt32(nslAlpha.Value),
                Convert.ToInt32(nslRed.Value),
                Convert.ToInt32(nslGreen.Value),
                Convert.ToInt32(nslBlue.Value));
            if (pbColor.ColorSample == newColor)
                return;
            pbColor.ColorSample = newColor;
            OnColorChanged();
        }
    }
}
