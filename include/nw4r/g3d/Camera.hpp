#ifndef NW4R_G3D_CAMERA_HPP_
#define NW4R_G3D_CAMERA_HPP_

#include "nw4r/math/types.h"

namespace nw4r {
    namespace g3d {

        // (written for this project) A 4x4 matrix, as projMtx below is.
        struct CameraMtx44 {
            float m[4][4];
        };
        // (written for this project) The vectors of CameraData. Like
        // NW4R's VEC3 and VEC2 they are a plain base plus a constructor: the
        // original copies CameraData member by member (a class with
        // constructors), each vector as words (its plain base).
        struct CameraVec3Base {
            float x, y, z;
        };
        struct CameraVec3 : CameraVec3Base {
            CameraVec3() {}
        };
        struct CameraVec2Base {
            float x, y;
        };
        struct CameraVec2 : CameraVec2Base {
            CameraVec2() {}
        };

        // The data a Camera works on, 0x10C bytes. The members follow the
        // layout NW4R decompilations give g3d's CameraData (consulted for
        // the structure only; names theirs); the types are confirmed by the
        // member-by-member copy msw::CEventMdlObj::ObjProc (DOL) makes:
        // the matrices and vectors copied as words, the lone floats with
        // lfs/stfs.
        struct CameraData {
            math::MTX34 cameraMtx;            // 0x00
            CameraMtx44 projMtx;              // 0x30
            unsigned long flags;              // 0x70
            CameraVec3 cameraPos;             // 0x74
            CameraVec3 cameraUp;              // 0x80
            CameraVec3 cameraTarget;          // 0x8C
            CameraVec3 cameraRotate;          // 0x98
            float cameraTwist;                // 0xA4
            long projType;                    // 0xA8 (GXProjectionType)
            float projFovy;                   // 0xAC
            float projAspect;                 // 0xB0
            float projNear;                   // 0xB4
            float projFar;                    // 0xB8
            float projTop;                    // 0xBC
            float projBottom;                 // 0xC0
            float projLeft;                   // 0xC4
            float projRight;                  // 0xC8
            float lightScaleS;                // 0xCC
            float lightScaleT;                // 0xD0
            float lightTransS;                // 0xD4
            float lightTransT;                // 0xD8
            CameraVec2 viewportOrigin;        // 0xDC
            CameraVec2 viewportSize;          // 0xE4
            float viewportNear;               // 0xEC
            float viewportFar;                // 0xF0
            unsigned long scissorX;           // 0xF4
            unsigned long scissorY;           // 0xF8
            unsigned long scissorWidth;       // 0xFC
            unsigned long scissorHeight;      // 0x100
            long scissorOffsetX;              // 0x104
            long scissorOffsetY;              // 0x108
        }; // size 0x10C

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
