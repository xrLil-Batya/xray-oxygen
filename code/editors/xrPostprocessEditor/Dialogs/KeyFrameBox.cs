using System;
using System.Globalization;
using System.Linq;
using System.Windows.Forms;

namespace xrPostprocessEditor
{
    public partial class KeyFrameBox
    {
        public delegate void KeyFrameEventHandler(object sender, decimal keyTime);
        public delegate void KeyFrameErrorHandler(string message);

        public event KeyFrameEventHandler AddTimeKeyEvent;
        public event KeyFrameEventHandler RemoveTimeKeyEvent;
        public event KeyFrameErrorHandler ErrorOccuredEvent;

        public event EventHandler SelectedIndexChanged;
        public event EventHandler ClearButtonClick;

        public ContextMenu CopyMenu => btnCopyFrom.Menu;

        public ListBox.ObjectCollection Items => lbKeyFrames.Items;

        public int SelectedIndex
        {
            get => lbKeyFrames.SelectedIndex;
            set => lbKeyFrames.SelectedIndex = value;
        }

        public KeyFrameBox()
        {
            InitializeComponent();

            ClearButtonClick += OnClearButtonClick;
        }

        public void OnClearButtonClick(object sender, EventArgs e)
        {
            lbKeyFrames.Items.Clear();
        }

        public void BtnAdd_Click(object sender, EventArgs e)
        {
            decimal keyTime = numKeyFrameTime.Value;
            var keyTimeString = keyTime.ToString(CultureInfo.InvariantCulture);

            if (!VerifyKeyTime(keyTime))
            {
                lbKeyFrames.Items.Add(keyTimeString);
                AddTimeKeyEvent?.Invoke(this, keyTime);

                return;
            }

            ErrorOccuredEvent?.Invoke("This time is already exists in the frame list.");
        }

        private void BtnRemove_Click(object sender, EventArgs e)
        {
            if (lbKeyFrames.Items.Count != 0)
            {
                int index = lbKeyFrames.SelectedIndex;
                decimal keyTime = decimal.Parse(lbKeyFrames.SelectedItem.ToString());

                RemoveTimeKeyEvent?.Invoke(this, keyTime);

                lbKeyFrames.Items.RemoveAt(index);

                return;
            }

            ErrorOccuredEvent?.Invoke("KeyFrame is empty.");
        }

        private void BtnClear_Click(object sender, EventArgs e) => ClearButtonClick?.Invoke(this, e);

        private void NumKeyFrameTime_ValueChanged(object sender, EventArgs e) { }

        private void LbKeyFrames_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lbKeyFrames.SelectedItem != null)
            {
                SelectedIndexChanged?.Invoke(sender, e);
            }
        }

        private bool VerifyKeyTime(decimal newKeyTime)
        {
            var items = lbKeyFrames.Items.Cast<string>();

            return items.Contains(newKeyTime.ToString(CultureInfo.InvariantCulture));
        }
    }
}
