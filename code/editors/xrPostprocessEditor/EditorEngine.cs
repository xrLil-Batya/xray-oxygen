using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using XRay.xmAPI;

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
        private BasicPostProcessAnimator animator;

        public EditorEngine()
        {
            animator = new BasicPostProcessAnimator(0, false);
        }

        public void Dispose()
        {
            if (animator == null)
                return;
            animator.Dispose();
            animator = null;
        }

        public xmAPI::xmPostprocessParam GetParam(xmAPI::xmPostprocessParamType paramType)
        {
            return animator.GetParam(paramType);
        }
        
        public void CreateKey(xmAPI::xmPostprocessParamType paramType, float time)
        {
            xmAPI::xmPostprocessParam param = animator.GetParam(paramType);
            try
            {
                switch (paramType)
                {
                    case xmAPI::xmPostprocessParamType.AddColor:
                    case xmAPI::xmPostprocessParamType.BaseColor:
                        // 3 components
                        param.AddValue(time, 0.0f, 0);
                        param.AddValue(time, 0.0f, 1);
                        param.AddValue(time, 0.0f, 2);
                        break;
                    case xmAPI::xmPostprocessParamType.GrayColor:
                        // 3+1 components
                        param.AddValue(time, 0.0f, 0);
                        param.AddValue(time, 0.0f, 1);
                        param.AddValue(time, 0.0f, 2);
                        param.Dispose();
                        param = animator.GetParam(xmAPI::xmPostprocessParamType.GrayValue);
                        param.AddValue(time, 0.0f, 0);
                        break;
                    case xmAPI::xmPostprocessParamType.DualityH:
                        param.AddValue(time, 0.44f, 0);
                        param.Dispose();
                        param = animator.GetParam(xmAPI::xmPostprocessParamType.DualityV);
                        param.AddValue(time, 0.44f, 0);
                        // 2 components
                        break;
                    case xmAPI::xmPostprocessParamType.NoiseIntensity:
                        param.AddValue(time, 0.33f, 0);
                        param.Dispose();
                        param = animator.GetParam(xmAPI::xmPostprocessParamType.NoiseGrain);
                        param.AddValue(time, 0.11f, 0);
                        param.Dispose();
                        param = animator.GetParam(xmAPI::xmPostprocessParamType.NoiseFps);
                        param.AddValue(time, 1.0f, 0);
                        break;
                    case xmAPI::xmPostprocessParamType.Blur:
                    case xmAPI::xmPostprocessParamType.ColorMappingInfluence:
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
            float time;
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.AddColor))
            {
                time = param.GetKeyTime(keyIndex);
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
            float time;
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.BaseColor))
            {
                time = param.GetKeyTime(keyIndex);
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
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.GrayColor))
            {
                time = param.GetKeyTime(keyIndex);                
                param.GetValue(time, out result.r, 0);
                param.GetValue(time, out result.g, 1);
                param.GetValue(time, out result.b, 2);
            }
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.GrayValue))
            {
                param.GetValue(time, out result.a, 0);
            }
            return result;
        }

        public Vector2F GetDuality(int keyIndex)
        {
            Vector2F result;
            float time;
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.DualityH))
            {
                time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result.x, 0);
            }
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.DualityV))
            {
                param.GetValue(time, out result.y, 0);
            }
            return result;
        }

        public NoiseParams GetNoise(int keyIndex)
        {
            NoiseParams result;
            float time;
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.NoiseIntensity))
            {
                time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result.Intensity, 0);
            }
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.NoiseGrain))
            {
                param.GetValue(time, out result.Grain, 0);
            }
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.NoiseFps))
            {
                param.GetValue(time, out result.FPS, 0);
            }
            return result;
        }

        public float GetBlur(int keyIndex)
        {
            float result;
            float time;
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.Blur))
            {
                time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result, 0);
            }
            return result;
        }

        public ColorMappingParams GetColorMapping(int keyIndex)
        {
            ColorMappingParams result;
            float time;
            using (xmAPI::xmPostprocessParam param = animator.GetParam(xmAPI::xmPostprocessParamType.ColorMappingInfluence))
            {
                time = param.GetKeyTime(keyIndex);
                param.GetValue(time, out result.Influence, 0);
            }
            result.Texture = animator.PPInfo.ColorMappingGradient1;
            return result;
        }

        public void Reset() { animator.Create(); }

        public void LoadEffect(string fileName) { animator.Load(fileName, false); }

        public void SaveEffect(string fileName) { animator.Save(fileName); }

        public float EffectDuration { get { return animator.Length; } }
    }
}
