// Luigi's vacuum (CC_CMN::LUIGI::CcOvacuumForce): an obstacle task that,
// while enabled, pulls the characters of the other team towards a point in
// front of him and knocks back those held in it too long. The modes'
// CChrCore<Mode>Luigi classes create it and feed it his position, facing
// and the tuning of each game.
//
// The original file is wider than this unit: the float constants are shared
// with the other CC_CMN classes from 0x801C39D4 on, and the weak inlines the
// vtable names are emitted at the end of that file (0x801D1108 on).

#include "game/msw/CcOvacuumForce.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"
#include <math.h>

namespace msw {
    namespace CC_CMN {
        namespace LUIGI {

            namespace {
                // (name guessed) An angle brought into [0, 2 pi).
                inline float RoundRad(float fRad)
                {
                    while (fRad > 6.2831855f) {
                        fRad -= 6.2831855f;
                    }
                    while (fRad < 0.0f) {
                        fRad += 6.2831855f;
                    }
                    return fRad;
                }
            }

            CcOvacuumForce::CcOvacuumForce(bcs::CTask *pParent, long nPlayer, long nTeam)
                : CChrObstacle(pParent, 1, 0x17), m_bEnable(0), m_fKeepFrame(0.0f), m_nPlayer(nPlayer),
                  m_nTeam(nTeam)
            {
                m_fLimit = 100.0f;
                m_fSpeedMin = 4.0f / 60.0f;
                m_fSpeedMax = 10.0f / 60.0f;
                m_fDistNear = 0.0f;
                m_fDistFar = 30.0f;
                m_fAngleNear = 30.0f;
                m_fAngleFar = 31.0f;
                m_nDmgFlag = 0x204;
                m_fDmgEffectFrm = 0.0f;
                m_nDmgArgF4 = -1;
                m_fDmgArgF8 = 0.0f;
                m_nDmgArgFC = 0;
                m_nDmgArg100 = 0;
                for (long i = 0; i < CHR_NUM; i++) {
                    m_afPullFrame[i] = 0.0f;
                }

                m_BodyColl.Create(1, 5, 1, 0);
                m_BodyColl.SetTargetFlagAll(2);
                m_BodyColl.SetPushRankAll(5);
                m_BodyColl.SetPushPowerAll(6);
                m_BodyColl.SetPosAll(m_Pos);
                bcs::FVEC3 size;
                size.x = 0.2f;
                size.y = 1.5f;
                size.z = 0.2f;
                m_BodyColl.SetSizeAll(size);
                m_BodyColl.SetPushEnableAll();
                m_BodyColl.SetDisableAll();
                unsigned long nFlag = 0x55;
                if (nTeam >= 0) {
                    if (nTeam > 0) {
                        nFlag = 0xAA;
                    }
                    m_BodyColl.SetNoHitFlagAll(nFlag);
                }
            }

            CcOvacuumForce::~CcOvacuumForce()
            {
            }

            void CcOvacuumForce::Main()
            {
                int bActive = 0;
                if (m_bEnable) {
                    bActive = 1;
                } else {
                    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
                        m_fKeepFrame = 0.0f;
                    }
                    if (m_fKeepFrame > 0.0f) {
                        bActive = 1;
                        m_fKeepFrame -= GetFrameRateF();
                        if (m_fKeepFrame < 0.0f) {
                            m_fKeepFrame = 0.0f;
                        }
                    }
                }

                if (bActive) {
                    Force_(GetFrameRateF());
                } else {
                    for (long i = 0; i < CHR_NUM; i++) {
                        m_afPullFrame[i] = 0.0f;
                    }
                }
            }

            void CcOvacuumForce::SetForceEnable(int bEnable, int bCollision, float fKeepFrame)
            {
                if (!m_bEnable && bEnable) {
                    CGame::StartCameraQuake(0x4B, -1, -1);
                }
                if (!bEnable && m_bEnable) {
                    m_fKeepFrame = fKeepFrame;
                }
                m_bEnable = bEnable;
                if (bEnable && bCollision) {
                    m_BodyColl.SetEnableAll();
                } else {
                    m_BodyColl.SetDisableAll();
                }
            }

            void CcOvacuumForce::SetForceBasePos(const bcs::FVEC3 &pos)
            {
                m_Pos.Set(pos.x, 0.0f, pos.z);
                m_BodyColl.SetPosAll(m_Pos);
            }

            void CcOvacuumForce::SetForceYRot(float fRotY)
            {
                m_fRotY = RoundRad(fRotY);
                m_BodyColl.SetYRotAll(m_fRotY);
            }

            void CcOvacuumForce::SetForceLimit(float fLimit)
            {
                m_fLimit = fLimit;
            }

            void CcOvacuumForce::SetForceParam(float fSpeedMin, float fSpeedMax, float fDistNear, float fDistFar,
                                               float fAngleNear, float fAngleFar)
            {
                m_fSpeedMin = fSpeedMin * (1.0f / 60.0f);
                m_fSpeedMax = fSpeedMax * (1.0f / 60.0f);
                m_fDistNear = fDistNear;
                m_fDistFar = fDistFar;
                m_fAngleNear = bcs::DEG_TO_RAD(fAngleNear);
                m_fAngleFar = bcs::DEG_TO_RAD(fAngleFar);
                if (m_fSpeedMax < m_fSpeedMin) {
                    float f = m_fSpeedMin;
                    m_fSpeedMin = m_fSpeedMax;
                    m_fSpeedMax = f;
                }
                if (m_fDistFar < m_fDistNear) {
                    float f = m_fDistNear;
                    m_fDistNear = m_fDistFar;
                    m_fDistFar = f;
                }
                if (m_fAngleFar < m_fAngleNear) {
                    float f = m_fAngleNear;
                    m_fAngleNear = m_fAngleFar;
                    m_fAngleFar = f;
                }
            }

            void CcOvacuumForce::SetForceDmgEffectFrm(float fFrame)
            {
                m_fDmgEffectFrm = fFrame;
            }

            void CcOvacuumForce::Force_(float fFrame)
            {
                float fSpeedMin = m_fSpeedMin;
                float fSpeedMax = m_fSpeedMax;
                float fDistNear = m_fDistNear;
                float fDistFar = m_fDistFar;
                float fAngleNear = m_fAngleNear;
                float fAngleFar = m_fAngleFar;
                bcs::FVEC3 basePos = m_Pos;
                float fRotY = m_fRotY;

                long nChr = CChrMan::GetChrCoreNum();
                for (long i = 0; i < nChr; i++) {
                    if (i == m_nPlayer) {
                        continue;
                    }
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr == 0 || pChr->IsDisable() || m_nTeam == pChr->GetTeamNo() || pChr->IsKeeper()
                        || !pChr->IsDodgeInfield() || (pChr->GetStatusFlag() & 0x20000)) {
                        continue;
                    }

                    // the pull is towards the base position
                    bcs::FVEC3 dir;
                    bcs::BCS_VecOneZ_RotY(dir, pChr->GetRotY_ToPosXZ(basePos));

                    float fDistRate = 0.0f;
                    float fAngleRate = fDistRate;

                    bcs::FVEC3 pos;
                    pChr->GetTransNodePos(pos);
                    pos.x -= basePos.x;
                    pos.z -= basePos.z;
                    bcs::FVEC3 posXZ;
                    posXZ.x = pos.x;
                    posXZ.y = 0.0f;
                    posXZ.z = pos.z;

                    // 1 up to fDistNear, 0 from fDistFar on
                    float fDist = bcs::FVEC3Len(&posXZ);
                    if (fDist <= fDistNear) {
                        fDistRate = 1.0f;
                    } else if (fDist >= fDistFar) {
                        fDistRate = 0.0f;
                    } else if (fDistNear < fDistFar) {
                        float f = (fDist - fDistNear) / (fDistFar - fDistNear);
                        if (f > 1.0f) {
                            f = 1.0f;
                        } else if (f < 0.0f) {
                            f = 0.0f;
                        }
                        fDistRate = 1.0f - f;
                    }

                    // the same for the angle between Luigi's facing and the character
                    float fAngle = RoundRad(nw4r::math::Atan2Rad(pos.x, pos.z));
                    fAngle = RoundRad(fAngle - fRotY);
                    if (fAngle > 3.1415927f) {
                        fAngle -= 6.2831855f;
                    }
                    fAngle = fabsf(fAngle);
                    if (fAngle <= fAngleNear) {
                        fAngleRate = 1.0f;
                    } else if (fAngle >= fAngleFar) {
                        fAngleRate = 0.0f;
                    } else if (fAngleNear < fAngleFar) {
                        float f = (fAngle - fAngleNear) / (fAngleFar - fAngleNear);
                        if (f > 1.0f) {
                            f = 1.0f;
                        } else if (f < 0.0f) {
                            f = 0.0f;
                        }
                        fAngleRate = 1.0f - f;
                    }

                    float fRate = fDistRate * fAngleRate;
                    if (fRate > 1.0f) {
                        fRate = 1.0f;
                    } else if (fRate < 0.0f) {
                        fRate = 0.0f;
                    }
                    pChr->SetForceVel(dir, fSpeedMin + fRate * (fSpeedMax - fSpeedMin));

                    if (fRate > 0.0f) {
                        if (!pChr->IsInvincible()) {
                            m_afPullFrame[i] += fFrame;
                        } else {
                            m_afPullFrame[i] = 0.0f;
                        }
                    }

                    if (m_fLimit > 0.0f && m_afPullFrame[i] > m_fLimit && !pChr->IsInvincible()) {
                        pChr->SetHookDamage(m_nDmgFlag, pChr->GetRotY(), 0.0f, m_fDmgEffectFrm, m_nDmgArgF4,
                                            m_fDmgArgF8, m_nDmgArgFC, m_nDmgArg100, -1, -1);
                    }
                }
            }
        }
    }
}
