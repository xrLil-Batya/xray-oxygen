using System;
using XRay.ManagedApi.Core;

namespace xrPostprocessEditor
{
    public struct NoiseParams
    {
        public float Intensity;
        public float Grain;
        public float FPS;
    }

    public struct ColorMappingParams
    {
        public float Influence;
        public string Texture;
    }

    public class EditorEngine : IDisposable
    {
        private BasicPostProcessAnimator _animator;

        public EditorEngine()
        {
            _animator = new BasicPostProcessAnimator(0, false);
        }

        public void Dispose()
        {
            if (_animator == null)
                return;
            _animator.Dispose();
            _animator = null;
        }

        public PostProcessParamBase GetParam(PostProcessParamType paramType)
        {
            return _animator.GetParam(paramType);
        }
        
        public void CreateKey(PostProcessParamType paramType, float time)
        {
            PostProcessParamBase param = _animator.GetParam(paramType);
            try
            {
                switch (paramType)
                {
                    case PostProcessParamType.AddColor:
                    case PostProcessParamType.BaseColor:
                        // 3 components
                        param.AddValue(time, 0.0f, 0);
                        param.AddValue(time, 0.0f, 1);
                        param.AddValue(time, 0.0f, 2);
                        break;
                    case PostProcessParamType.GrayColor:
                        // 3+1 components
                        param.AddValue(time, 0.0f, 0);
                        param.AddValue(time, 0.0f, 1);
                        param.AddValue(time, 0.0f, 2);
                        param.Dispose();
                        param = _animator.GetParam(PostProcessParamType.GrayValue);
                        param.AddValue(time, 0.0f, 0);
                        break;
                    case PostProcessParamType.DualityH:
                        param.AddValue(time, 0.44f, 0);
                        param.Dispose();
                        param = _animator.GetParam(PostProcessParamType.DualityV);
                        param.AddValue(time, 0.44f, 0);
                        // 2 components
                        break;
                    case PostProcessParamType.NoiseIntensity:
                        param.AddValue(time, 0.33f, 0);
                        param.Dispose();
                        param = _animator.GetParam(PostProcessParamType.NoiseGrain);
                        param.AddValue(time, 0.11f, 0);
                        param.Dispose();
                        param = _animator.GetParam(PostProcessParamType.NoiseFps);
                        param.AddValue(time, 1.0f, 0);
                        break;
                    case PostProcessParamType.Blur:
                    case PostProcessParamType.ColorMappingInfluence:
                        // 1 component
                        param.AddValue(time, 0.33f, 0);
                        break;
                    default:
                        throw new ArgumentException("Invalid param type.");
                }
            }
            finally
            {
                param.Dispose();
            }
        }

        public ColorF GetAddColor(int keyIndex)
        {
            ColorF result;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.AddColor))
            {
                float time = param.GetKeyTime(keyIndex);
                result.a = 0;
                param.GetValue(time, out result.r, 0);
                param.GetValue(time, out result.g, 1);
                param.GetValue(time, out result.b, 2);
            }
            return result;
        }

        public ColorF GetBaseColor(int keyIndex)
        {
            ColorF result;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.BaseColor))
            {
                float time = param.GetKeyTime(keyIndex);
                result.a = 0;
                param.GetValue(time, out result.r, 0);
                param.GetValue(time, out result.g, 1);
                param.GetValue(time, out result.b, 2);
            }
            return result;
        }

        public ColorF GetGrayColor(int keyIndex)
        {
            ColorF result;
            float time;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.GrayColor))
            {
                time = param.GetKeyTime(keyIndex);                
                param.GetValue(time, out result.r, 0);
                param.GetValue(time, out result.g, 1);
                param.GetValue(time, out result.b, 2);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.GrayValue))
            {
                param.GetValue(time, out result.a, 0);
            }
            return result;
        }

        public Vector2F GetDuality(int keyIndex)
        {
            Vector2F result;
            float time;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.DualityH))
            {
                time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result.x, 0);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.DualityV))
            {
                param.GetValue(time, out result.y, 0);
            }
            return result;
        }

        public NoiseParams GetNoise(int keyIndex)
        {
            NoiseParams result;
            float time;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.NoiseIntensity))
            {
                time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result.Intensity, 0);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.NoiseGrain))
            {
                param.GetValue(time, out result.Grain, 0);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.NoiseFps))
            {
                param.GetValue(time, out result.FPS, 0);
            }
            return result;
        }

        public float GetBlur(int keyIndex)
        {
            float result;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.Blur))
            {
                float time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result, 0);
            }
            return result;
        }

        public ColorMappingParams GetColorMapping(int keyIndex)
        {
            ColorMappingParams result;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.ColorMappingInfluence))
            {
                float time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result.Influence, 0);
            }
            result.Texture = _animator.PPInfo.ColorMappingGradient1;
            return result;
        }

        public void Reset() { _animator.Create(); }

        public void LoadEffect(string fileName) { _animator.Load(fileName, false); }

        public void SaveEffect(string fileName) { _animator.Save(fileName); }

        public float EffectDuration => _animator.Length;
    }
}
