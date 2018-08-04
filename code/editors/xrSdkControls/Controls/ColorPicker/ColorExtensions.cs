using System.Drawing;

namespace XRay.SdkControls.Controls.ColorPicker
{
    public static class ColorExtensions
    {
        public static Color SetColorChannel(this Color clr, string channel, byte value)
        {
            Color newColor;

            switch (channel)
            {
                case "red":
                    newColor = Color.FromArgb(clr.A, value, clr.G, clr.B);
                    break;
                case "blue":
                    newColor = Color.FromArgb(clr.A, clr.R, clr.G, value);
                    break;
                case "green":
                    newColor = Color.FromArgb(clr.A, clr.R, value, clr.B);
                    break;
                case "alpha":
                    newColor = Color.FromArgb(value, clr.R, clr.G, clr.B);
                    break;
                default:
                    return new Color();
            }

            return newColor;
        }
    }
}
