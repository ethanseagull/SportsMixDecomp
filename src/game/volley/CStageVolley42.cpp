// msw::CStageVolley42: a volley stage that adds only an orthographic
// projection camera (the same code as CStageVolley10's OnPostInitialize).
#include "game/msw/CStageVolley42.hpp"

#include "bcs/CWorldEnv.hpp"
#include "nw4r/g3d/Camera.hpp"

msw::CStageVolley42::CStageVolley42(bcs::CTask *pParent) : CStageManVolley(pParent)
{
}

msw::CStageVolley42::~CStageVolley42()
{
}

void msw::CStageVolley42::OnPostInitialize()
{
    CStageManVolley::OnPostInitialize();

    // Projection camera of view 0: orthographic, 4 units high, looking down
    // from straight above with +X as up.
    nw4r::g3d::CameraData data;
    nw4r::g3d::Camera camera(&data);
    camera.Init();
    float aspect = 1.0f;
    float height = 4.0f;
    float width = height * aspect;
    camera.SetOrtho(0.5f * height, 0.5f * -height, 0.5f * -width, 0.5f * width, 0.5f, 10000.0f);
    camera.SetPosition(nw4r::math::VEC3(0.0f, 1.0f, 0.0f));

    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_LOOKAT;
    posture.cameraUp = nw4r::math::VEC3(1.0f, 0.0f, 0.0f);
    posture.cameraTarget = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    camera.SetPosture(posture);

    bcs::CWorldEnv::SetProjCameraData(0, data);
}
