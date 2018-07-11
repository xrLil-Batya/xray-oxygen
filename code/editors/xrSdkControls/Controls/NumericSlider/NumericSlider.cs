using System;
using System.Windows.Forms;

namespace XRay.SdkControls
{
    public sealed partial class NumericSlider : UserControl
    {
        public NumericSlider() => InitializeComponent();
        
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            numSpinner.ValueChanged += (obj, args) => { trackBar.Value = (int) numSpinner.Value; };

            trackBar.Scroll += (sender, args) => { numSpinner.Value = trackBar.Value; };
        }
    }
}
