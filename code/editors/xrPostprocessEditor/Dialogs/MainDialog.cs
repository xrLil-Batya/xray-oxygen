using System;
using System.Drawing;
using System.Windows.Forms;
using XRay.ManagedApi.Core;

namespace xrPostprocessEditor
{
    public partial class MainDialog
    {
        private class ChannelDesc
        {
            public delegate void UpdateHandler(int keyIndex);
            public readonly TabPage Page;
            public readonly KeyFrameBox List;
            public readonly PostProcessParamType Type;
            private string _name;
            public readonly UpdateHandler Update;

            public ChannelDesc(TabPage page, KeyFrameBox kfb, PostProcessParamType type, string name,
                UpdateHandler updater)
            {
                Page = page;
                List = kfb;
                Type = type;
                _name = name;
                Update = updater;
            }
        }

        public static EditorEngine Engine;
        private const string DefaultEffectName = "untitled";
        private string _effectName;
        private readonly ChannelDesc[] _chInfo;

        public MainDialog()
        {
            InitializeComponent();
            _chInfo = new[]
            {
                new ChannelDesc(tpAC, kfbAC, PostProcessParamType.AddColor, "Add color", UpdateAC),
                new ChannelDesc(tpBC, kfbBC, PostProcessParamType.BaseColor, "Base color", UpdateBC),
                new ChannelDesc(tpGC, kfbGC, PostProcessParamType.GrayColor, "Gray color", UpdateGC),
                new ChannelDesc(tpDuality, kfbDuality, PostProcessParamType.DualityH, "Duality", UpdateDuality),
                new ChannelDesc(tpNoise, kfbNoise, PostProcessParamType.NoiseIntensity, "Noise", UpdateNoise),
                new ChannelDesc(tpBlur, kfbBlur, PostProcessParamType.Blur, "Blur", UpdateBlur),
                new ChannelDesc(tpColorMapping, kfbColorMapping, PostProcessParamType.ColorMappingInfluence,
                    "Color mapping", UpdateColorMapping)
            };

            for (int kfbIndex = 0; kfbIndex < _chInfo.Length; kfbIndex++)
            {
                var dstChannel = _chInfo[kfbIndex];
                for (int tabIndex = 0; tabIndex < _chInfo.Length; tabIndex++)
                {
                    var srcChannel = _chInfo[tabIndex];
                    var item = new MenuItem(srcChannel.Page.Text);
                    if (kfbIndex != tabIndex)
                        item.Click += (s, e) => CopyKeyFrames(dstChannel, srcChannel);
                    else
                        item.Enabled = false;
                    dstChannel.List.CopyMenu.MenuItems.Add(item);
                }
            }

            SetCurrentEffectName(DefaultEffectName);

            SetUpHandlers();
        }

        Color ConvertColor(ColorF value)
        {
            Color result = Color.FromArgb(
                (byte)(255*value.a),
                (byte)(255*value.r),
                (byte)(255*value.g),
                (byte)(255*value.b));
            return result;
        }

        private void UpdateAC(int keyIndex)
        {
            ColorF value = Engine.GetAddColor(keyIndex);
            cpAC.Value = ConvertColor(value);
        }

        private void UpdateBC(int keyIndex)
        {
            ColorF value = Engine.GetBaseColor(keyIndex);
            cpBC.Value = ConvertColor(value);
        }

        private void UpdateGC(int keyIndex)
        {
            ColorF value = Engine.GetGrayColor(keyIndex);
            cpGC.Value = ConvertColor(value);
        }

        private void UpdateDuality(int keyIndex)
        {
            Vector2F value = Engine.GetDuality(keyIndex);
            nslDualityX.Value = (decimal)value.x;
            nslDualityY.Value = (decimal)value.y;
        }

        private void UpdateNoise(int keyIndex)
        {
            NoiseParams value = Engine.GetNoise(keyIndex);
            nslNoiseIntensity.Value = (decimal)value.Intensity;
            nslNoiseGrain.Value = (decimal)value.Grain;
            nslNoiseFPS.Value = (decimal)value.FPS;
        }

        private void UpdateBlur(int keyIndex)
        {
            float value = Engine.GetBlur(keyIndex);
            nslBlur.Value = (decimal)value;
        }

        private void UpdateColorMapping(int keyIndex)
        {
            ColorMappingParams value = Engine.GetColorMapping(keyIndex);
            nslColorMappingInfluence.Value = (decimal)value.Influence;
            tbColorMappingTexture.Text = value.Texture;
        }

        public void Initialize(EditorEngine nEngine)
        {
            Engine = nEngine;

            Engine.ErrorOccuredEvent += Engine_ErrorOccuredEvent;
        }

        private void Engine_ErrorOccuredEvent(string message) => MessageBox.Show(message);

        private void CopyKeyFrames(ChannelDesc dst, ChannelDesc src)
        {
            using (var dstParam = Engine.GetParam(dst.Type))
            using (var srcParam = Engine.GetParam(src.Type))
            {
                // 1. engine: remove old keyframes
                dst.List.Items.Clear();
                dstParam.Reset();
                // 2. engine: create new ones
                for (int i = 0; i < srcParam.KeyCount; i++)
                    Engine.CreateKey(dst.Type, srcParam.GetKeyTime(i));
            }
            LoadChannel(dst);
        }

        private void SetCurrentEffectName(string name)
        {
            _effectName = name;
            Text = $"{_effectName} - {Application.ProductName}";
        }

        private void LoadChannel(ChannelDesc ch)
        {
            using (var param = Engine.GetParam(ch.Type))
            {
                ch.List.Items.Clear();
                for (int i = 0; i < param.KeyCount; i++)
                    ch.List.Items.Add(param.GetKeyTime(i));
            }
        }
        
        private void LoadAllChannels()
        {
            foreach (var ch in _chInfo)
                LoadChannel(ch);
        }

        private void CreateEffect(object sender, EventArgs e)
        {
            // XXX: show confirmation dialog if there are unsaved changes
            Engine.Reset();
            SetCurrentEffectName(DefaultEffectName);
            LoadAllChannels();
        }

        private void LoadEffect(object sender, EventArgs e)
        {
            string fileName;
            using (var dlg = new OpenFileDialog())
            {
                dlg.RestoreDirectory = true;
                dlg.Multiselect = false;
                dlg.Filter = "Post-process effects (.ppe)|*.ppe|All Files (*.*)|*.*";
                if (dlg.ShowDialog() != DialogResult.OK)
                    return;
                fileName = dlg.FileName;
            }
            Engine.LoadEffect(fileName);
            SetCurrentEffectName(fileName);
            LoadAllChannels();
        }

        private void SaveEffect(object sender, EventArgs e)
        {
            if (Engine.EffectDuration == 0.0f)
            {
                MessageBox.Show("Can't save zero length effect.", Application.ProductName);
                return;
            }
            using (var dlg = new SaveFileDialog())
            {
                dlg.RestoreDirectory = true;
                dlg.Filter = "Post-process effects (.ppe)|*.ppe|All Files (*.*)|*.*";
                if (dlg.ShowDialog() == DialogResult.OK)
                    Engine.SaveEffect(dlg.FileName);
            }
        }
        
        private void SetUpHandlers()
        {
            foreach (var ch in _chInfo)
            {
                ch.List.SelectedIndexChanged += (s, e) => ch.Update(((ListBox) s).SelectedIndex);
                ch.List.AddTimeKeyEvent += (sender, keyTime) => Engine.CreateKey(ch.Type, (float) keyTime);
                ch.List.RemoveTimeKeyEvent += (sender, keyTime) => Engine.RemoveKey(ch.Type, (float) keyTime);
                ch.List.ErrorOccuredEvent += message => MessageBox.Show(message);
            }
        }
    }
}
