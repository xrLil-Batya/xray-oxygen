using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using XRay;

namespace xrPostprocessEditor
{
    public partial class MainDialog
    {
        // XXX collectioner: Temporary mock, should be refactoring
        private static readonly Dictionary<PostProcessParamType, List<PostProcessParamType>> TypesRelationships =
            new Dictionary<PostProcessParamType, List<PostProcessParamType>>
            {
                {
                    PostProcessParamType.DualityH, new List<PostProcessParamType>
                    {
                        PostProcessParamType.DualityH,
                        PostProcessParamType.DualityV
                    }
                }
            };

        private class ChannelDesc
        {
            public int SelectedKey = DefaultUnselectedIndex;
            public delegate void UpdateHandler(int keyIndex);
            public readonly TabPage Page;
            public readonly KeyFrameBox List;
            public readonly PostProcessParamType Type;
            public readonly UpdateHandler Update;

            public ChannelDesc(TabPage page, KeyFrameBox kfb, PostProcessParamType type, UpdateHandler updater)
            {
                Page = page;
                List = kfb;
                Type = type;
                Update = updater;
            }
        }

        public static EditorEngine Engine;
        private const int DefaultUnselectedIndex = -1;
        private const string DefaultEffectName = "untitled";
        private string _effectName;
        private readonly ChannelDesc[] _chInfo;

        public MainDialog()
        {
            InitializeComponent();
            _chInfo = new[]
            {
                new ChannelDesc(tpAC, kfbAC, PostProcessParamType.AddColor, UpdateAddColor),
                new ChannelDesc(tpBC, kfbBC, PostProcessParamType.BaseColor, UpdateBaseColor),
                new ChannelDesc(tpGC, kfbGC, PostProcessParamType.GrayColor, UpdateGrayColor),
                new ChannelDesc(tpDuality, kfbDuality, PostProcessParamType.DualityH, UpdateDuality),
                new ChannelDesc(tpNoise, kfbNoise, PostProcessParamType.NoiseIntensity, UpdateNoise),
                new ChannelDesc(tpBlur, kfbBlur, PostProcessParamType.Blur, UpdateBlur),
                new ChannelDesc(tpColorMapping, kfbColorMapping, PostProcessParamType.ColorMappingInfluence,
                    UpdateColorMapping)
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

			cpBC.isAReversed.Visible = false;
			cpBC.isRReversed.Location = new Point(cpBC.isRReversed.Location.X - 15, cpBC.isRReversed.Location.Y);
			cpBC.isGReversed.Location = new Point(cpBC.isGReversed.Location.X - 15, cpBC.isGReversed.Location.Y);
			cpBC.isBReversed.Location = new Point(cpBC.isBReversed.Location.X - 15, cpBC.isBReversed.Location.Y);

			cpAC.isAReversed.Visible = false;
			cpAC.isRReversed.Location = new Point(cpAC.isRReversed.Location.X - 15, cpAC.isRReversed.Location.Y);
			cpAC.isGReversed.Location = new Point(cpAC.isGReversed.Location.X - 15, cpAC.isGReversed.Location.Y);
			cpAC.isBReversed.Location = new Point(cpAC.isBReversed.Location.X - 15, cpAC.isBReversed.Location.Y);
			
			nslNoiseGrain.Value = 1;
			nslNoiseFPS.Value = 1;

			SetCurrentEffectName(DefaultEffectName);

            SetUpHandlers();
        }

		Color ConvertColor(ColorF value, ref XRay.SdkControls.ColorPicker Picer)
		{
			int NewA = (int)(value.a * 127.5);
			int NewR = (int)(value.r * 127.5);
			int NewG = (int)(value.g * 127.5);
			int NewB = (int)(value.b * 127.5);

			if (value.a < 0)
			{
				Picer.isAReversed.Checked = true;
			}
			NewA += 127;

			if (value.r < 0)
			{
				Picer.isRReversed.Checked = true;
			}
			NewR += 127;

			if (value.g < 0)
			{
				Picer.isGReversed.Checked = true;
			}
			NewG += 127;

			if (value.b < 0)
			{
				Picer.isBReversed.Checked = true;
			}
			NewB += 127;

			Color result = Color.FromArgb(NewA, NewR, NewB, NewG);
			return result;
		}

        private void UpdateAddColor(int keyIndex)
        {
            ColorF value = Engine.GetAddColor(keyIndex);
            cpAC.Value = ConvertColor(value, ref cpAC);
        }

        private void UpdateBaseColor(int keyIndex)
        {
            ColorF value = Engine.GetBaseColor(keyIndex);
            cpBC.Value = ConvertColor(value, ref cpBC);
        }

        private void UpdateGrayColor(int keyIndex)
        {
            ColorF value = Engine.GetGrayColor(keyIndex);
            cpGC.Value = ConvertColor(value, ref cpGC);
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

			nslNoiseIntensity.Value = (decimal)(value.Intensity * 255);
			nslNoiseGrain.Value = (decimal)(value.Grain * 255);
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

        private void ResetAllChannels()
        {
            foreach (var ch in _chInfo)
            {
                ch.List.Items.Clear();
                ch.SelectedKey = DefaultUnselectedIndex;
            }
        }

        private void CreateEffect(object sender, EventArgs e)
        {
            // XXX: show confirmation dialog if there are unsaved changes
            Engine.Reset();
            SetCurrentEffectName(DefaultEffectName);
            ResetAllChannels();
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
				{
					Engine.SaveEffect(dlg.FileName);
				}
            }
        }

        private void SetUpHandlers()
        {
            foreach (var ch in _chInfo)
            {
                ch.List.SelectedIndexChanged += (s, e) =>
                {
                    int keyIndex = ((ListBox) s).SelectedIndex;
                    ch.SelectedKey = keyIndex;
                    ch.Update(keyIndex);
                };
                ch.List.AddTimeKeyEvent += (sender, keyTime) => Engine.CreateKey(ch.Type, (float) keyTime);
                ch.List.RemoveTimeKeyEvent += (sender, keyTime) => Engine.RemoveKey(ch.Type, (float) keyTime);
                ch.List.ErrorOccuredEvent += message => MessageBox.Show(message);
            }

            cpAC.ColorChanged += (s, color) => UpdateEngineValue(PostProcessParamType.AddColor, color);
            cpBC.ColorChanged += (s, color) => UpdateEngineValue(PostProcessParamType.BaseColor, color);
            cpGC.ColorChanged += (s, color) => UpdateEngineValue(PostProcessParamType.GrayColor, color);

            nslDualityX.ValueChanged += (s, value) => UpdateEngineValue(PostProcessParamType.DualityH, value);
            nslDualityY.ValueChanged += (s, value) => UpdateEngineValue(PostProcessParamType.DualityV, value);

			nslNoiseIntensity.ValueChanged += (s, nlsVal) => UpdateEngineValue(PostProcessParamType.NoiseIntensity, nlsVal);
			nslNoiseFPS.ValueChanged += (s, nlsVal) => UpdateEngineValue(PostProcessParamType.NoiseFps, nlsVal);
			nslNoiseGrain.ValueChanged += (s, nlsVal) => UpdateEngineValue(PostProcessParamType.NoiseGrain, nlsVal);
		}

        private void UpdateEngineValue(PostProcessParamType paramType, Color color)
        {
            var channel = _chInfo.First(ch => ch.Type == paramType);
            if (channel.SelectedKey == -1) return;

            Engine.UpdateColorValue(channel.SelectedKey, paramType, color);
        }

        // XXX collectioner: Refactor this fucking shit!
        private void UpdateEngineValue(PostProcessParamType paramType, decimal value)
        {
            KeyValuePair<PostProcessParamType, List<PostProcessParamType>> relationPair =
                TypesRelationships.FirstOrDefault(item => item.Value.Contains(paramType));

            PostProcessParamType searchedType =
                relationPair.Equals(default(KeyValuePair<PostProcessParamType, List<PostProcessParamType>>))
                    ? paramType
                    : relationPair.Key;

            ChannelDesc channel = _chInfo.FirstOrDefault(ch => ch.Type == searchedType);
            if (channel == null || channel.SelectedKey == -1) return;

            Engine.UpdateValue(channel.SelectedKey, paramType, value);
        }
    }
}
