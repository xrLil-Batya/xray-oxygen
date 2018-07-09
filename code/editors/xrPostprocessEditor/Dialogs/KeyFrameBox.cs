using System;
using System.Globalization;
using System.Windows.Forms;

namespace xrPostprocessEditor
{
    public partial class KeyFrameBox
    {
        public delegate void KeyFrameEventHandler(object sender, decimal keyTime);

        public event KeyFrameEventHandler AddTimeKeyEvent;
        public event EventHandler SelectedIndexChanged;
        public event EventHandler RemoveButtonClick;
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
            // Работаем по мотивам ПЫС, нужен коэф для названий в ListBox

            RemoveButtonClick += OnRemoveButtonClick;
            ClearButtonClick += OnClearButtonClick;
        }

        public void OnRemoveButtonClick(object sender, EventArgs e)
        {
            try
            {
                int size = lbKeyFrames.Items.Count - 1;
                lbKeyFrames.Items.RemoveAt(size);
            }
            catch(Exception)
            {
                MessageBox.Show("KeyFrames is empty!");
            }
        }

        public void OnClearButtonClick(object sender, EventArgs e)
        {
            lbKeyFrames.Items.Clear();
        }

        public void BtnAdd_Click(object sender, EventArgs e)
        {
            decimal keyTime = numKeyFrameTime.Value;
            lbKeyFrames.Items.Add(keyTime.ToString(CultureInfo.InvariantCulture));

            AddTimeKeyEvent?.Invoke(this, keyTime);
        }

        private void BtnRemove_Click(object sender, EventArgs e) => RemoveButtonClick?.Invoke(this, e);

        private void BtnClear_Click(object sender, EventArgs e) => ClearButtonClick?.Invoke(this, e);

        private void NumKeyFrameTime_ValueChanged(object sender, EventArgs e) { }

        private void LbKeyFrames_SelectedIndexChanged(object sender, EventArgs e) =>
            SelectedIndexChanged?.Invoke(sender, e);
    }
}
