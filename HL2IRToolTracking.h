#pragma once
#include "HL2IRTracking.g.h"
#include "ResearchModeApi.h"
#include "TimeConverter.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <wchar.h>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <future>
#include <cmath>
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <MemoryBuffer.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Perception.Spatial.Preview.h>
#include <winrt/Windows.Media.Devices.Core.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Foundation.h>

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <Eigen/Eigen>

#include "IRToolTrack.h"

namespace winrt::HL2IRToolTracking::implementation
{
    struct HL2IRTracking : HL2IRTrackingT<HL2IRTracking>
    {

        HL2IRTracking();
        static HRESULT CheckCamConsent();
        static HRESULT CheckImuConsent();

        UINT16 GetCenterDepth();
        int GetDepthBufferSize();
        int GetLongDepthBufferSize();
        hstring PrintDepthResolution();
        hstring PrintDepthExtrinsics();

        void InitializeDepthSensor();


        bool DepthMapUpdated();
        bool DepthMapTextureUpdated();
        bool PointCloudUpdated();
        bool LongDepthMapTextureUpdated();
        bool LongDepthMapUpdated();
        bool LFImageUpdated();
        bool RFImageUpdated();
        bool LLImageUpdated();
        bool RRImageUpdated();
        bool PVImageUpdated();
        bool LongThrowLUTUpdated();
        bool ShortThrowLUTUpdated();
        bool PVInterrupted();

        void SetReferenceCoordinateSystem(Windows::Perception::Spatial::SpatialCoordinateSystem refCoord);


        INT64 GetPVTimestamp();
        INT64 GetLongDepthTimestamp();
        INT64 GetShortDepthTimestamp();
        INT64 GetIMUTimestamp();
        float GetSpatialLfFps();
        float GetSpatialRfFps();


        
        void StartDepthSensorLoop();

        //On device IR Tool Tracking
        bool IsTrackingTools();
        bool AddToolDefinition(int sphere_count, array_view<const float> sphere_positions, float sphere_radius, hstring identifier);
        bool AddToolDefinition(int sphere_count, array_view<const float> sphere_positions, float sphere_radius, hstring identifier, int min_visible_spheres);
        bool AddToolDefinition(int sphere_count, array_view<const float> sphere_positions, float sphere_radius, hstring identifier, int min_visible_spheres, float lowpass_rotation, float lowpass_position);
        bool RemoveToolDefinition(hstring identifier);
        bool RemoveAllToolDefinitions();
        bool StartToolTracking();
        void StopToolTracking();

        com_array<float> GetToolTransform(hstring identifier);
        com_array<float> GetDepthToWorldTransform();
        com_array<uint8_t> GetShortAbImageTextureBuffer();
        com_array<uint8_t> GetDepthMapTextureBuffer();
        INT64 GetTrackingTimestamp();
        bool ShortAbImageTextureUpdated();
        bool DepthMapImagePointToCameraUnitPlane(float (&uv)[2], float (&xy)[2]);

    private:
        float* m_lut_short = nullptr;
        int m_lutLength_short = 0;

        float* m_depthToWorldPose = nullptr;
        UINT8* m_shortAbImageTexture = nullptr;
        UINT8 m_test1[262144];
        UINT8* m_depthMapTexture = nullptr;
        std::atomic_bool m_shortAbImageTextureUpdated = false;

        IResearchModeSensor* m_depthSensor = nullptr;
        IResearchModeCameraSensor* m_pDepthCameraSensor = nullptr;
        ResearchModeSensorResolution m_depthResolution;

        IResearchModeSensorDevice* m_pSensorDevice = nullptr;
        std::vector<ResearchModeSensorDescriptor> m_sensorDescriptors;
        IResearchModeSensorDeviceConsent* m_pSensorDeviceConsent = nullptr;
        Windows::Perception::Spatial::SpatialLocator m_locator = 0;
        Windows::Perception::Spatial::SpatialCoordinateSystem m_refFrame = nullptr;
        std::atomic_int m_depthBufferSize, m_shortAbImageBufferSize = 0;
        std::atomic_int m_spatialBufferSize = 0;

        std::atomic_bool m_depthSensorLoopStarted = false;

        std::atomic_bool m_LFImageUpdated = false;
        std::atomic_bool m_RFImageUpdated = false;
        

        std::atomic_bool m_LUTGenerated_short = false;

        float m_roiBound[3]{ 0,0,0 };
        float m_roiCenter[3]{ 0,0,0 };


        static void DepthSensorLoop(HL2IRTracking* pHL2IRTracking);
        static void CamAccessOnComplete(ResearchModeSensorConsent consent);
        static void ImuAccessOnComplete(ResearchModeSensorConsent consent);
        std::string MatrixToString(DirectX::XMFLOAT4X4 mat);
        static DirectX::XMMATRIX HL2IRTracking::SpatialLocationToDxMatrix(Windows::Perception::Spatial::SpatialLocation location);
        
        float* EncodeXMFloat4x4(DirectX::XMFLOAT4X4 mat);

        DirectX::XMFLOAT4X4 m_depthCameraPose;
        DirectX::XMMATRIX m_depthCameraPoseInvMatrix;
        DirectX::XMFLOAT4X4 m_LFCameraPose;
        DirectX::XMMATRIX m_LFCameraPoseInvMatrix;
        DirectX::XMFLOAT4X4 m_RFCameraPose;
        DirectX::XMMATRIX m_RFCameraPoseInvMatrix;

        
        std::thread* m_pDepthUpdateThread;
        
        struct DepthCamRoi {
            float kRowLower = 0.2;
            float kRowUpper = 0.5;
            float kColLower = 0.3;
            float kColUpper = 0.7;
            UINT16 depthNearClip = 200; // Unit: mm
            UINT16 depthFarClip = 800;
        } depthCamRoi;
        UINT16 m_depthOffset = 0;

        std::shared_mutex mu;
        
        long long m_latestShortDepthTimestamp = 0;

        IRToolTracker* m_IRToolTracker = nullptr;
        long long m_latestTrackedFrame = 0;
    };
}
namespace winrt::HL2IRToolTracking::factory_implementation
{
    struct HL2IRTracking : HL2IRTrackingT<HL2IRTracking, implementation::HL2IRTracking>
    {
    };
}
