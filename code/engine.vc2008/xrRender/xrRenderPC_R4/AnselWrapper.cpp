#include "stdafx.h"

#ifdef USE_ANSEL
#include <AnselSDK/AnselSDK.h>
#include <directxmath.h>

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	DirectX::XMFLOAT4 vLightDir;
	DirectX::XMFLOAT4 vLightColor;
	DirectX::XMFLOAT4 vOutputColor;
	float vTime;
};

ansel::Configuration config;
bool g_AnselSessionIsActive = false;
float g_LightIntensity = 0.666;

void InitAnsel()
{
	if (!ansel::isAnselAvailable()) return;

	// Configure the Ansel SDK
	config.rotationalSpeedInDegreesPerSecond = 220.0f;
	config.translationalSpeedInWorldUnitsPerSecond = 50.0f;
	config.right = { 1.0f, 0.0f, 0.0f };
	config.up = { 0.0f, 1.0f, 0.0f };
	config.forward = { 0.0f, 0.0f, 1.0f };
	config.captureLatency = 0;
	config.captureSettleLatency = 0;
	config.metersInWorldUnit = 2.0f;
	config.fovType = ansel::kVerticalFov;
	config.startSessionCallback = [](ansel::SessionConfiguration& conf, void* userPointer)
	{
		Device.Pause(true, 0, 0, "");
		g_AnselSessionIsActive = true;
		Device.mView_saved = Device.mView;
		return ansel::kAllowed;
	};
	config.stopSessionCallback = [](void* userPointer)
	{
		Device.Pause(false, 0, 0, "");
		g_AnselSessionIsActive = false;
		Device.mView = Device.mView_saved;
	};
	config.startCaptureCallback = [](const ansel::CaptureConfiguration&, void*)
	{
		// turn non-uniform full screen effects like vignette off here
	};
	config.stopCaptureCallback = [](void*)
	{
		// turn disabled effects back on here
	};
	config.isCameraFovSupported = true;
	config.isCameraOffcenteredProjectionSupported = true;
	config.isCameraRotationSupported = true;
	config.isCameraTranslationSupported = true;
	config.userPointer = nullptr;
	config.gameWindowHandle = Device.m_hWnd;
	config.titleNameUtf8 = "AnselSDKIntegration";
	ansel::setConfiguration(config);

	// Expose light intensity setting as user control in the Ansel UI
	{
		ansel::UserControlDesc ui_position_slider;
		ui_position_slider.labelUtf8 = "Light intensity";
		ui_position_slider.info.userControlId = 1;
		ui_position_slider.info.userControlType = ansel::kUserControlSlider;
		ui_position_slider.info.value = &g_LightIntensity;
		ui_position_slider.callback = [](const ansel::UserControlInfo& info) {
			g_LightIntensity = *reinterpret_cast<const float*>(info.value);
		};
		ansel::addUserControl(ui_position_slider);
	}
}

void AnselRender()
{
	// After Ansel SDK integration:
	if (g_AnselSessionIsActive)
	{
		ansel::Camera cam;
		// set up ansel::Camera object with the current camera parameters
		cam.fov = (Device.fFOV/ DirectX::XM_PI) * 180.0f;
		cam.projectionOffsetX = 0;
		cam.projectionOffsetY = 0;

		// convert view matrix (XMMATRIX) into a pair of a position and a quaternion
		DirectX::XMMATRIX invView = XMMatrixInverse(nullptr, Device.mView);
		const nv::Vec3 right = { invView.r[0].m128_f32[0], invView.r[0].m128_f32[1], invView.r[0].m128_f32[2] };
		const nv::Vec3 up = { invView.r[1].m128_f32[0], invView.r[1].m128_f32[1], invView.r[1].m128_f32[2] };
		const nv::Vec3 forward = { invView.r[2].m128_f32[0], invView.r[2].m128_f32[1], invView.r[2].m128_f32[2] };

		ansel::rotationMatrixVectorsToQuaternion(right, up, forward, cam.rotation);
		cam.position = { invView.r[3].m128_f32[0], invView.r[3].m128_f32[1], invView.r[3].m128_f32[2] };
		ansel::updateCamera(cam);

		// convert position and quaternion returned in ansel::updateCamera call and update camera parameters
		invView = DirectX::XMMatrixAffineTransformation(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&cam.rotation)),
			DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&cam.position)));

		Device.mView = XMMatrixInverse(nullptr, invView);
		Device.fFOV = (cam.fov / 180.0f) * DirectX::XM_PI;
		//g_ProjectionOffsetX = cam.projectionOffsetX;
		//g_ProjectionOffsetY = cam.projectionOffsetY;
		//updateProjectionMatrix();
	}
}

#endif