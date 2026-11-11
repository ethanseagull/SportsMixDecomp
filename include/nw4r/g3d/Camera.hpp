#ifndef NW4R_G3D_CAMERA_HPP_
#define NW4R_G3D_CAMERA_HPP_

#include "nw4r/math/types.h"

namespace nw4r {
    namespace g3d {

        // The data a Camera works on. Only its size is known so far: it is
        // the last local of a 0x170-byte frame in VolleyF
        // msw::CStageVolley42::OnPostInitialize (0x5C..0x16C).
        struct CameraData {
            float m_Unk000[0x110 / 4]; // word-aligned (its offset on that stack)
        };

        // Handle to a CameraData (one pointer, as on the stack of the caller
        // above). Declarations follow the mangled names of the calls.
        class Camera {
            public:
                // How SetPosture aims the camera. (names guessed)
                enum PostureType {
                    POSTURE_LOOKAT,
                    POSTURE_ROTATE,
                    POSTURE_AIM
                };

                // 0x2C bytes: the caller fills only the type, the up vector
                // and the target. (field names guessed)
                struct PostureInfo {
                    PostureType tp;        // 0x00
                    math::VEC3 cameraUp;     // 0x04
                    math::VEC3 cameraTarget; // 0x10
                    math::VEC3 cameraRotate; // 0x1C
                    float cameraTwist;       // 0x28
                };

                Camera(CameraData *pData);
                void Init();
                void SetPosition(const math::VEC3 &rPos);
                void SetPosture(const PostureInfo &rInfo);
                void SetOrtho(float top, float bottom, float left, float right, float near, float far);
                // The view matrix (world to camera).
                void GetCameraMtx(math::MTX34 *pMtx) const;
                // (added for the DOL's pinball ball's debug drawing) Load
                // the camera's viewport, projection and scissor into GX.
                void GXSetViewport() const;
                void GXSetProjection() const;
                void GXSetScissor() const;
                void GXSetScissorBoxOffset() const;

            private:
                CameraData *mCamData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_CAMERA_HPP_ */
