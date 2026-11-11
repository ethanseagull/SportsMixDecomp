#ifndef MSW_CHR_CORE_BOSS_CSTICKOBJ_HPP_
#define MSW_CHR_CORE_BOSS_CSTICKOBJ_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CGameTask.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"

namespace msw {
    namespace CHR_CORE_BOSS {

        // The stick a boss character holds in its shoot stance (BossF
        // game/boss/CStickObj.cpp; the model is "bstick" of the character's
        // resource file). A task with a four-state machine: it fades in
        // (Appear), swings up to a larger scale and waits (Shoot), then
        // fades out (Disappear) and deletes itself. 0xB8 bytes (the shoot
        // stance allocates that much). bcs::CFsm at 0x34, its vptr at 0x78.
        // Member names are guessed.
        class CStickObj : public CGameTask, public bcs::CFsm<CStickObj> {
            public:
                enum {
                    STATE_SLEEP,     // hidden
                    STATE_APPEAR,    // fades in
                    STATE_SHOOT,     // grows to the shot scale, then waits
                    STATE_DISAPPEAR, // fades out, then deletes the task
                    STATE_NUM
                };

                // pAttach and nNode: the model and node the stick follows;
                // nChrId: the owner's character number (character 0x11 has
                // other effects).
                CStickObj(bcs::CTask *pParent, nw4r::g3d::ResMdl mdl, bcs::CMdlObj *pAttach, long nNode, long nChrId);
                virtual ~CStickObj();
                virtual void Main();

                long GetState() const { return m_nState; }

                // Fades in over fFrame frames, unless it is already
                // appearing or shooting.
                void Appear(float fFrame)
                {
                    long state = GetState();
                    if (state < STATE_APPEAR || state > STATE_SHOOT) {
                        m_fAppearSpeed = 1.0f;
                        if (fFrame > 0.0f) {
                            m_fAppearSpeed = 1.0f / fFrame;
                        }
                        ChangeState(STATE_APPEAR);
                    }
                }

                // Grows to fScale in fFrame frames, waits fWait frames and
                // then fades out over fFadeFrame frames.
                void Shoot(float fScale, float fFrame, float fWait, float fFadeFrame)
                {
                    if (GetState() != STATE_DISAPPEAR) {
                        m_fShootScale = fScale;
                        m_fShootSpeed = 1.0f;
                        if (fFrame > 0.0f) {
                            m_fShootSpeed = 1.0f / fFrame;
                        }
                        m_fShootWait = fWait;
                        m_fDisappearSpeed = 1.0f / fFadeFrame;
                        ChangeState(STATE_SHOOT);
                    }
                }

                // Fades out over fFrame frames, unless it is hidden or
                // already fading out.
                void Disappear(float fFrame)
                {
                    long state = GetState();
                    if (state != STATE_SLEEP && state != STATE_DISAPPEAR) {
                        m_fDisappearSpeed = 1.0f;
                        if (fFrame > 0.0f) {
                            m_fDisappearSpeed = 1.0f / fFrame;
                        }
                        ChangeState(STATE_DISAPPEAR);
                    }
                }

            private:
                void CtorStateSleep();
                void LoopStateSleep();
                void DtorStateSleep();
                void CtorStateAppear();
                void LoopStateAppear();
                void DtorStateAppear();
                void CtorStateShoot();
                void LoopStateShoot();
                void DtorStateShoot();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                // Scales the stick's scale node by m_Scale.
                void UpdateScale()
                {
                    if (m_pMdl != 0 && m_nScaleNode >= 0) {
                        bcs::FMTX34 mtx;
                        PSMTXIdentity(mtx.m);
                        nw4r::math::MTX34Scale(reinterpret_cast<nw4r::math::MTX34 *>(&mtx),
                                               reinterpret_cast<const nw4r::math::MTX34 *>(&mtx),
                                               reinterpret_cast<const nw4r::math::VEC3 *>(&m_Scale));
                        m_pMdl->SetNodeLocalMtx(m_nScaleNode, mtx);
                    }
                }

                void SetScale(float fScale)
                {
                    m_Scale.x = fScale;
                    m_Scale.y = fScale;
                    m_Scale.z = fScale;
                    UpdateScale();
                }

                bcs::CMdlObj *m_pMdl;    // 0x7C
                long m_nChrId;           // 0x80
                long m_nScaleNode;       // 0x84, node "stick_scal"; negative for none
                bcs::FVEC3 m_Scale;      // 0x88
                float m_fAlpha;          // 0x94
                float m_fAppearSpeed;    // 0x98, alpha per frame
                float m_fDisappearSpeed; // 0x9C, alpha per frame
                long m_nShootStep;       // 0xA0: 0 growing, then waiting
                float m_fShootRate;      // 0xA4, 0 to 1 over the growth
                float m_fShootSpeed;     // 0xA8, rate per frame
                float m_fShootScale;     // 0xAC, the scale at the end of the growth
                float m_fShootWait;      // 0xB0, frames left before the fade
                unsigned long m_hEffect; // 0xB4, the stick's attached effect
        };
    }
}

#endif /* !MSW_CHR_CORE_BOSS_CSTICKOBJ_HPP_ */
