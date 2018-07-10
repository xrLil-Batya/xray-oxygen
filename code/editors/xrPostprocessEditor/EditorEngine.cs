using System;
using System.Runtime.ExceptionServices;
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

        public delegate void ErrorHandler(string message);

        public event ErrorHandler ErrorOccuredEvent;

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

        public void RemoveKey(PostProcessParamType paramType, float time)
        {
            using (PostProcessParamBase param = _animator.GetParam(paramType))
            {
                param.DeleteValue(time);
            }
        }

        public ColorF GetAddColor(int keyIndex)
        {
            ColorF result = default(ColorF);

            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.AddColor))
            {
                float time = param.GetKeyTime(keyIndex);

                SafetyGetValue(param, ref result, time);
            }
            return result;
        }

        public ColorF GetBaseColor(int keyIndex)
        {
            ColorF result = default(ColorF);

            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.BaseColor))
            {
                float time = param.GetKeyTime(keyIndex);
                
                SafetyGetValue(param, ref result, time);
            }
            return result;
        }

        public ColorF GetGrayColor(int keyIndex)
        {
            ColorF result = default(ColorF);

            float time;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.GrayColor))
            {
                time = param.GetKeyTime(keyIndex);   
                
                SafetyGetValue(param, ref result, time);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.GrayValue))
            {
                SafetyGetValue(param, time, ref result.a, keyIndex);
            }
            return result;
        }

        public Vector2F GetDuality(int keyIndex)
        {
            Vector2F result = default(Vector2F);

            float time;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.DualityH))
            {
                time = param.GetKeyTime(keyIndex);
                SafetyGetValue(param, time, ref result.x, 0);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.DualityV))
            {
                SafetyGetValue(param, time, ref result.y, 0);
            }
            return result;
        }

        public NoiseParams GetNoise(int keyIndex)
        {
            NoiseParams result = default(NoiseParams);
            float time;

            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.NoiseIntensity))
            {
                time = param.GetKeyTime(keyIndex);
                SafetyGetValue(param, time, ref result.Intensity, 0);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.NoiseGrain))
            {
                SafetyGetValue(param, time, ref result.Grain, 0);
            }
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.NoiseFps))
            {
                SafetyGetValue(param, time, ref result.FPS, 0);
            }
            return result;
        }

        public float GetBlur(int keyIndex)
        {
            float result = 0;
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.Blur))
            {
                float time = param.GetKeyTime(keyIndex);

                SafetyGetValue(param, time, ref result, 0);
            }
            return result;
        }

        public ColorMappingParams GetColorMapping(int keyIndex)
        {
            ColorMappingParams result = default(ColorMappingParams);
            using (PostProcessParamBase param = _animator.GetParam(PostProcessParamType.ColorMappingInfluence))
            {
                float time = param.GetKeyTime(keyIndex);

                SafetyGetValue(param, time, ref result.Influence, 0);
            }
            result.Texture = _animator.PPInfo.ColorMappingGradient1;
            return result;
        }

        public void Reset() { _animator.Create(); }

        public void LoadEffect(string fileName) { _animator.Load(fileName, false); }

        public void SaveEffect(string fileName) { _animator.Save(fileName); }

        public float EffectDuration => _animator.Length;

        //#Collector: Really bad idea, just temporary mock.

        [HandleProcessCorruptedStateExceptions]
        private void SafetyGetValue(PostProcessParamBase param, float time, ref float value, int index)
        {
            try
            {
                param.GetValue(time, out value, index);
            }
            catch (AccessViolationException e)
            {
                ErrorOccuredEvent?.Invoke(e.Message);
            }
        }

        private void SafetyGetValue(PostProcessParamBase param, ref ColorF color, float time)
        {
            color.a = 0;

            SafetyGetValue(param, time, ref color.r, 0);
            SafetyGetValue(param, time, ref color.g, 1);
            SafetyGetValue(param, time, ref color.b, 2);
        }
    }
}
