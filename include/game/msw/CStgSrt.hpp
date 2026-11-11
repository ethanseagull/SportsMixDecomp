#ifndef MSW_CSTGSRT_HPP_
#define MSW_CSTGSRT_HPP_

#include "bcs/BCS_Vec.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/math/types.h"

namespace msw {

    // Position, rotation and scale of a stage object; msw::CStgAct embeds
    // one at 0x24 (0x144 bytes, up to its member at 0x168). A DOL class.
    // msw::CStgGmk_WaterPillar::GetPos reads the position at 0x00; the
    // inline msw::CStgObj::LookAt writes the direction at 0x0C and sets bit
    // 1 of the flags at 0xC0. The vptr is at 0xC4 (the data members come
    // before the first virtual function): the DOL constructor (no exported
    // name, called by CStgAct's) stores it there, and the vtable's third
    // slot sets the position (flag bit 0) and calls it from
    // msw::CStgObj::SetPos. Names of the slots are guesses.
    class CStgSrt {
        public:
            // (name guessed)
            const bcs::FVEC3 &GetPos() const { return m_Pos; }
            // (name guessed) The velocity; msw::CStgGmk_Trolley::GetVel
            // reads it.
            const bcs::FVEC3 &GetVel() const { return m_Vel; }

            void SetFront(const bcs::FVEC3 &rFront);
            // (added for game/msw/CStgGmk_Trolley.cpp) The same from the
            // three components: the DOL's fn_801747B8, next to SetFront.
            void SetFront(float x, float y, float z);

            // (name guessed) Point the direction from the position towards
            // rTarget; inline in msw::CStgObj::LookAt.
            void SetLookAtPos(const bcs::FVEC3 &rTarget)
            {
                bcs::FVEC3Sub(&m_Dir, &rTarget, &m_Pos);
                m_nFlag |= 2;
            }

            // (name guessed) The model matrix: scale, then rotation, then
            // translation, rebuilt from the parts flagged as changed (bit 2
            // the scale, bit 1 the direction, bit 0 the position).
            // Inline in msw::CStgGmk_WaterPillar_Act::UpdateMdlMtx (DOL).
            const bcs::FMTX34 &GetMtx()
            {
                if (m_nFlag & 7) {
                    if (m_nFlag & 4) {
                        PSMTXIdentity(m_ScaleMtx.m);
                        nw4r::math::MTX34Scale(reinterpret_cast<nw4r::math::MTX34 *>(&m_ScaleMtx),
                                               reinterpret_cast<const nw4r::math::MTX34 *>(&m_ScaleMtx),
                                               reinterpret_cast<const nw4r::math::VEC3 *>(&m_Scale));
                        CalcRotMtx(m_RotMtx, m_Dir, m_Up);
                        PSMTXConcat(m_RotMtx.m, m_ScaleMtx.m, m_Mtx.m);
                        PSMTXTransApply(m_Mtx.m, m_Mtx.m, m_Pos.x, m_Pos.y, m_Pos.z);
                        m_nFlag &= ~4;
                    }
                    if (m_nFlag & 2) {
                        CalcRotMtx(m_RotMtx, m_Dir, m_Up);
                        m_nFlag &= ~2;
                    }
                    PSMTXConcat(m_RotMtx.m, m_ScaleMtx.m, m_Mtx.m);
                    PSMTXTransApply(m_Mtx.m, m_Mtx.m, m_Pos.x, m_Pos.y, m_Pos.z);
                    m_nFlag &= ~7;
                }
                return m_Mtx;
            }

            // (name and meaning guessed) Clears both vectors at 0x100 and
            // 0x10C and flags them (bit 4 of the word at 0xC8). Inline in
            // msw::CStgGmk_WaterPillar_Act's constructor (DOL).
            void ClearVelocity()
            {
                bcs::FVEC3 zero(0.0f, 0.0f, 0.0f);
                m_Unk10C = zero;
                m_Unk100 = zero;
                m_nFlagC8 |= 0x10;
            }

            // (names and meanings guessed; inline in
            // msw::CStgGmk_Trolley_Act, DOL) The acceleration at 0x10C:
            // straight down by the gravity.
            void SetGravity(float gravity) { m_Unk10C.Set(0.0f, gravity, 0.0f); }
            // Like ClearVelocity, with the gravity as the vector at 0x10C.
            void ResetVelocity(float gravity)
            {
                bcs::FVEC3 zero(0.0f, 0.0f, 0.0f);
                m_Unk10C.Set(0.0f, gravity, 0.0f);
                m_Unk100 = zero;
                m_nFlagC8 |= 0x10;
            }
            void ClearUnkE4() { m_UnkE4.Set(0.0f, 0.0f, 0.0f); }
            // The run of the trolley: StopAccel clears flag 4 and the
            // vectors at 0xF0 and 0xFC; SetLocalVel sets a local velocity
            // and turns it into the world velocity at 0xD8 by the
            // rotation; StopRun clears both velocities as well.
            void StopAccel()
            {
                m_nFlagC8 &= ~0x10;
                m_UnkF0.Set(0.0f, 0.0f, 0.0f);
                m_fUnkFC = 0.0f;
            }
            void ClearFlagC8(unsigned long flag) { m_nFlagC8 &= ~flag; }
            void SetLocalVel(const bcs::FVEC3 &rVel)
            {
                m_Vel = rVel;
                PSMTXMultVec(GetRotMtx().m, (const Vec *)&rVel, (Vec *)&m_WorldVel);
            }
            void StopRun()
            {
                StopAccel();
                m_Vel.Set(0.0f, 0.0f, 0.0f);
                m_WorldVel.Set(0.0f, 0.0f, 0.0f);
            }
            // The rotation matrix, rebuilt when the direction changed.
            const bcs::FMTX34 &GetRotMtx()
            {
                if (m_nFlag & 2) {
                    CalcRotMtx(m_RotMtx, m_Dir, m_Up);
                    m_nFlag &= ~2;
                }
                return m_RotMtx;
            }

            // In the DOL: the rotation matrix for direction rDir and up
            // vector rUp.
            static void CalcRotMtx(bcs::FMTX34 &rMtx, const bcs::FVEC3 &rDir, const bcs::FVEC3 &rUp);

        private:
            bcs::FVEC3 m_Pos;             // 0x00
            bcs::FVEC3 m_Dir;             // 0x0C
            bcs::FVEC3 m_Up;              // 0x18 (name guessed)
            bcs::FVEC3 m_Scale;           // 0x24
            bcs::FMTX34 m_Mtx;            // 0x30, see GetMtx
            bcs::FMTX34 m_RotMtx;         // 0x60
            bcs::FMTX34 m_ScaleMtx;       // 0x90
            unsigned long m_nFlag;        // 0xC0, 7 after construction

        public:
            virtual ~CStgSrt();                           // 0x08
            virtual void Reset();                         // 0x0C
            virtual void SetPos(const bcs::FVEC3 &rPos);  // 0x10
            // (added for game/msw/CStgGmk_Trolley.cpp; names guessed) Weak
            // or out-of-line DOL functions, declared without bodies so no
            // copy is emitted: SetPos from the components (it passes them
            // to SetPos), an update without arguments, the per-frame
            // update that moves the object by its velocity, and an empty
            // hook.
            virtual void SetPos(float x, float y, float z); // 0x14
            virtual void Update();                          // 0x18
            virtual void Update(float rate);                // 0x1C
            virtual void OnUpdate();                        // 0x20

        private:
            unsigned long m_nFlagC8;     // 0xC8
            bcs::FVEC3 m_Vel;            // 0xCC (name guessed)
            bcs::FVEC3 m_WorldVel;       // 0xD8, m_Vel turned by the rotation (StartRun)
            bcs::FVEC3 m_UnkE4;          // 0xE4
            bcs::FVEC3 m_UnkF0;          // 0xF0
            float m_fUnkFC;              // 0xFC
            bcs::FVEC3 m_Unk100;         // 0x100
            bcs::FVEC3 m_Unk10C;         // 0x10C
            unsigned char m_Unk118[0x2C]; // 0x118
    };
}

#endif /* !MSW_CSTGSRT_HPP_ */
