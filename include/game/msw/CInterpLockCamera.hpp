#ifndef CINTERPLOCKCAMERA_HPP_
#define CINTERPLOCKCAMERA_HPP_

#include "bcs/BCS_Vec.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CWorldEnv.hpp"
#include "nw4r/math/triangular.h"

namespace msw {

    // Size 0x184. A camera that looks at a point from a direction and a
    // distance, and moves from one such setting to the next at a rate that
    // falls off geometrically. Only TourF has it; its two named functions,
    // its vtable and its destructor are in game/tour/CTournamentCamera.cpp.
    // _SetRate and CalcRate are the only functions with a name; everything
    // else is inline, and the names of those and of the members are guessed.
    // The split into small inline setters is not: msw::CTournamentCamera::
    // SetValueCamera and msw::CTournamentMan::SetCameraRate only come out
    // right with it.
    class CInterpLockCamera : public bcs::CCamera {
        public:
            // Size 0x24.
            struct _CAMERA_INFO_LOOK {
                bcs::FVEC3 at;  // 0x00
                bcs::FVEC3 dir; // 0x0C: unit vector from the camera to `at`
                float dist;     // 0x18
                float roll;     // 0x1C, radians
                float fovy;     // 0x20

                // Sets `dir` from a rotation about x, then y, in radians.
                void SetRot(float rotX, float rotY) { CalcDir(dir, rotX, rotY); }
            };

            CInterpLockCamera(long view, short priority)
                : CCamera(view, priority, 0), m_fStep(0.5f), m_fTotal(0.0f), m_fRateAdd(0.0f), m_fRateMul(1.0f),
                  m_fRateMin(0.00001f), m_Unk181(true)
            {
                _CAMERA_INFO_LOOK info;

                info.at.Set(0.0f, 0.0f, 0.0f);
                info.dist = 15.0f;
                info.roll = 0.0f;
                info.SetRot(160.0f * (3.14159265f / 180.0f), 0.0f);
                info.fovy = bcs::CWorldEnv::GetViewFovy(view);
                SetValueDirect(info);
            }

            virtual ~CInterpLockCamera() {}

            // Starts a move with remaining = (remaining + add) * mul per
            // step, which is over when that falls to `min`. Returns true when
            // there is nothing to move.
            bool _SetRate(float add, float mul, float min);
            // What is left of the move `time` after its start: 1 to 0.
            float CalcRate(float time);

            // The viewing direction for a rotation about x, then y. The four
            // locals come out in this order on the stack when this is one or
            // three inline calls deep and reversed when it is two; it is
            // three deep in the constructor (through _CAMERA_INFO_LOOK::
            // SetRot) and in CTournamentCamera::SetValueCamera.
            static void CalcDir(bcs::FVEC3 &rDir, float rotX, float rotY)
            {
                float sinX;
                float cosX;
                float sinY;
                float cosY;

                nw4r::math::SinCosRad(&sinX, &cosX, rotX);
                nw4r::math::SinCosRad(&sinY, &cosY, rotY);
                rDir.x = cosX * sinY;
                rDir.y = -sinX;
                rDir.z = cosX * cosY;
            }

            // Jumps to `rInfo`.
            void SetValueDirect(const _CAMERA_INFO_LOOK &rInfo)
            {
                m_Start = rInfo;
                m_End = rInfo;
                m_Cur = rInfo;
                m_bDirect = true;
                m_bEnd = true;
            }

            // Each of these starts one part of the setting moving from
            // where it is now to the value given. The move itself begins
            // with _SetRate.
            void SetAt(const bcs::FVEC3 &rAt)
            {
                m_Start.at = m_Cur.at;
                m_End.at = rAt;
            }

            void SetDir(const bcs::FVEC3 &rDir)
            {
                m_Start.dir = m_Cur.dir;
                m_End.dir = rDir;
            }

            void SetDirRot(float rotX, float rotY)
            {
                m_Start.dir = m_Cur.dir;
                m_End.SetRot(rotX, rotY);
            }

            void SetRoll(float roll)
            {
                m_Start.roll = m_Cur.roll;
                m_End.roll = roll;
            }

            void SetDist(float dist)
            {
                m_Start.dist = m_Cur.dist;
                m_End.dist = dist;
            }

            void SetFovy(float fovy)
            {
                m_Start.fovy = m_Cur.fovy;
                m_End.fovy = fovy;
            }

            // (name guessed) from + (to - from) * t. The arguments are
            // values, not expressions written out in Interp: that is what
            // gives the original's registers.
            static float Lerp(float from, float to, float t) { return t * (to - from) + from; }

            // m_Cur = m_End + (m_Start - m_End) * rate.
            void Interp(float rate)
            {
                bcs::BCS_Vec_Lerp(m_Cur.at, m_End.at, m_Start.at, rate);
                bcs::BCS_Vec_Slerp(m_Cur.dir, m_End.dir, m_Start.dir, rate);
                m_Cur.dist = Lerp(m_End.dist, m_Start.dist, rate);
                m_Cur.fovy = Lerp(m_End.fovy, m_Start.fovy, rate);
                m_Cur.roll = Lerp(m_End.roll, m_Start.roll, rate);
            }

            // Advances the move by `step`.
            void Update(float step)
            {
                if (!m_bEnd || !m_Unk181) {
                    m_fTime += step;
                    Interp(CalcRate(m_fTime));
                    m_bEnd = m_fTime >= m_fTotal;
                }
            }

            // Where the camera is now: its position, the point it looks at,
            // its roll and its field of view.
            void GetLook(bcs::FVEC3 &rPos, bcs::FVEC3 &rAt, float &rRoll, float &rFovy) const
            {
                rAt = m_Cur.at;
                bcs::FVEC3Scale(&rPos, &m_Cur.dir, -m_Cur.dist);
                rFovy = m_Cur.fovy;
                rRoll = m_Cur.roll;
                bcs::FVEC3Add(&rPos, &rPos, &rAt);
            }

            // Starts the move. What msw::CTournamentMan::SetCameraRate shows
            // after inlining.
            void SetRate(float add, float mul, float min)
            {
                if (_SetRate(add, mul, min)) {
                    Interp(0.0f);
                } else {
                    Update(0.0f);
                }
            }

            // (name guessed) The same, stretched to take `frame` in all
            // (msw::CTournamentMan::SetCameraRateFrame).
            void SetRateFrame(float frame, float add, float mul, float min)
            {
                if (_SetRate(add, mul, min)) {
                    Interp(0.0f);
                } else {
                    float scale = frame / m_fTotal;
                    m_fStep *= scale;
                    m_fTotal *= scale;
                    Update(0.0f);
                }
            }

            // (names guessed) Read and written by msw::CTournamentMan.
            void SetStep(float step) { m_fStep = step; }
            float GetTotal() const { return m_fTotal; }
            int IsEnd() const { return m_bEnd; }
            const bcs::FVEC3 &GetAt() const { return m_Cur.at; }

            // (name guessed) Jumps to `rInfo` as SetValueDirect does, with
            // m_bDirect given (msw::CTournamentMan::SetCameraActive).
            void SetValueDirect(const _CAMERA_INFO_LOOK &rInfo, int bDirect)
            {
                m_Start = rInfo;
                m_End = rInfo;
                m_Cur = rInfo;
                m_bDirect = bDirect;
                m_bEnd = true;
            }

        protected:
            _CAMERA_INFO_LOOK m_Start; // 0xFC
            _CAMERA_INFO_LOOK m_End;   // 0x120
            _CAMERA_INFO_LOOK m_Cur;   // 0x144
            float m_fTime;             // 0x168: since the move began
            float m_fStep;             // 0x16C: the time one step of the rate takes
            float m_fTotal;            // 0x170: how long the move takes
            float m_fRateAdd;          // 0x174
            float m_fRateMul;          // 0x178
            float m_fRateMin;          // 0x17C
            signed char m_bEnd;        // 0x180: the move is over
            signed char m_Unk181;      // 0x181: 1 from the constructor
            signed char m_bDirect;     // 0x182: set by SetValueDirect, cleared by _SetRate
    };
}

#endif /* !CINTERPLOCKCAMERA_HPP_ */
