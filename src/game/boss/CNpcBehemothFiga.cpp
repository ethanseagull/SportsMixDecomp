// msw::CNpcBehemothBoss, the Behemoth of the boss game, and the attacks it
// throws: the Figa fire balls that home in on the characters, the
// Thunderga lightning, the meteors with the marks where they will land, and
// a debug read-out of the damage it takes. The file holds the boss's
// constructor, destructor, state machine set-up and its callbacks;
// game/boss/CNpcBehemothBoss.cpp has most of its states.
#include "game/msw/CNpcBehemothBoss.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBlurEffect.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"

#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"
#include "revolution/mtx.h"

#include <math.h>

namespace bcs {

    // (DOL class; name from the symbol) Debug text on the screen. Print is
    // empty in the release build: its weak copy follows its first caller.
    class CDbgPrint {
        public:
            static void Print(long x, long y, long, long nSize, BCS_Color color, const char *pFormat, ...) {}
    };
}

namespace msw {

    namespace {
        // The angle brought into [0, 2 pi).
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

        // A random float between the two bounds, in either order.
        inline float RandRangeSort(float fMin, float fMax)
        {
            if (fMin > fMax) {
                float fTmp = fMin;
                fMin = fMax;
                fMax = fTmp;
            }
            return bcs::BCS_RandRange(fMin, fMax);
        }

        // The length of rVec in the XZ plane.
        inline float LenXZ(const bcs::FVEC3 &rVec)
        {
            return nw4r::math::FSqrt(rVec.x * rVec.x + rVec.z * rVec.z);
        }

        // The effect of handle hEffect; null when there is none.
        inline CEffectCluster *FindEffect(unsigned long hEffect)
        {
            CEffectCluster *pEff;
            if (hEffect != 0) {
                pEff = CEffectCtrl::GetInstance()->FindList(hEffect);
            } else {
                pEff = 0;
            }
            return pEff;
        }
    }

    // The model nodes of CNpcBehemothBoss::VNODE_NUM (GetVNodePos).
    static const char *s_apVNodeName[CNpcBehemothBoss::VNODE_NUM] = {
        "kosi_jnt",        "mune2_jnt",       "kubi_jnt",     "atama_jnt",    "L_te_jnt",     "R_te_jnt",
        "L_tuma_jnt",      "R_tuma_jnt",      "eff_horn",     "eff_magic_point", "eff_FR_foot", "eff_FL_foot",
        "trans",           "sippo_00_jnt",    "sippo_01_jnt", "sippo_02_jnt", "sippo_03_jnt", "sippo_04_jnt",
    };

    // The colour animations (SetColorAnim_), of the normal and of the hard
    // Behemoth.
    static const char *s_apAnmClrName[2][CNpcBehemothBoss::ANMCLR_NUM] = {
        {
            "n00_MAGIC_01_R",
            "n00_MAGIC_02_R",
            "n00_MAGIC_03_R",
            "n00_MAGIC_01_T_R",
            "n00_MAGIC_02_T_R",
            "n00_MAGIC_03_T_R",
            "n00_METEOR_01_R",
            "n00_METEOR_02_R",
            "n00_METEOR_03_R",
            "n00_METEOR_04_R",
            "n00_METEOR_05_R",
        },
        {
            "n15_MAGIC_01_R",
            "n15_MAGIC_02_R",
            "n15_MAGIC_03_R",
            "n15_MAGIC_01_T_R",
            "n15_MAGIC_02_T_R",
            "n15_MAGIC_03_T_R",
            "n15_METEOR_01_R",
            "n15_METEOR_02_R",
            "n15_METEOR_03_R",
            "n15_METEOR_04_R",
            "n15_METEOR_05_R",
        },
    };

    namespace BEHEMOTH {

        // A Figa fire ball (an obstacle of kind 0x11): it flies from the
        // boss's mouth and homes in on one character. Size 0xAC.
        class CNpcBehemothFiga : public CChrObstacle {
            public:
                CNpcBehemothFiga(bcs::CTask *pParent);
                virtual ~CNpcBehemothFiga();
                virtual void Main();

                void Create(unsigned long nMdl, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir, CChrCore *pTarget,
                            long nLv);

            private:
                int DoHoming_(float fFrame);

                // Turns the fire effect along the direction of flight.
                void SetEffectRot_(CEffectCluster *pEff)
                {
                    bcs::FVEC3 vDir = m_Velocity;
                    bcs::FVEC3 vRot;
                    vRot.x = RoundRad(nw4r::math::Atan2Rad(bcs::BCS_Vec_GetLenXZ(vDir), vDir.y));
                    vRot.x = RoundRad(vRot.x + 3.1415927f);
                    vRot.y = RoundRad(nw4r::math::Atan2Rad(vDir.x, vDir.z));
                    vRot.z = 0.0f;
                    pEff->SetRotateRad(&vRot);
                }

                void StopEffect_()
                {
                    CEffectCtrl::StopAttachEffect(m_hEffect);
                    m_hEffect = 0;
                }

                void StopSe_()
                {
                    if (m_hSe != 0) {
                        bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hSe));
                        m_hSe = 0;
                        if (pSe != 0 && !pSe->IsDelete()) {
                            pSe->Stop(4.0f);
                        }
                    }
                }

                unsigned long m_hEffect; // 0x94, the fire
                unsigned long m_hSe;     // 0x98, the sound of the flight
                long m_nStep;            // 0x9C, 0 flying, 1 hit the ground
                float m_fWait;           // 0xA0, frames to the deletion after the hit
                CChrCore *m_pTarget;     // 0xA4
                float m_fHomingDist;     // 0xA8, no more homing nearer than this
        };

        // The Thunderga lightning: a few strikes around one spot. Size 0x64.
        class CNpcBehemothThunderga : public bcs::CUnivTaskEx {
            public:
                CNpcBehemothThunderga(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir, long nLv);
                virtual ~CNpcBehemothThunderga();
                virtual void Main();

            private:
                CAtkCollision m_AtkColl; // 0x34
                bcs::FVEC3 m_vPos;       // 0x40
                bcs::FVEC3 m_vDir;       // 0x4C
                long m_nCount;           // 0x58, strikes left
                long m_nCountMax;        // 0x5C
                float m_fWait;           // 0x60, frames to the next strike
        };

        // A meteor (an obstacle of kind 0x12): falls from the sky onto a
        // mark on the court. Size 0xE4.
        class CNpcBehemothMeteor : public CChrObstacle {
            public:
                CNpcBehemothMeteor(bcs::CTask *pParent, unsigned long hOwner, const bcs::FVEC3 &rPos,
                                   const bcs::FVEC3 &rDir, float fWait, long nLv);
                virtual ~CNpcBehemothMeteor();
                virtual void Main();

            private:
                enum {
                    EFFECT_NUM = 8
                };

                // Moves the effects of the meteor to rPos.
                void SetEffectPos_(const bcs::FVEC3 &rPos)
                {
                    for (long i = 0; i < EFFECT_NUM; i++) {
                        unsigned long hEffect = m_ahEffect[i];
                        if (hEffect != 0) {
                            CEffectCluster *pEff = CEffectCtrl::GetInstance()->FindList(hEffect);
                            if (pEff != 0) {
                                pEff->SetTranslate(&rPos);
                                pEff->SetRotateRad((const bcs::FVEC3 *)&m_fRotX);
                            }
                        }
                    }
                }

                long m_nStep;                        // 0x94, 0 waiting, 1 falling, 2 blast, 3 after
                float m_fWait;                       // 0x98, frames before it starts to fall
                float m_fTime;                       // 0x9C
                CAtkCollision m_HitColl;             // 0xA0, the blast on landing
                unsigned long m_ahEffect[EFFECT_NUM]; // 0xAC
                unsigned long m_hSe;                 // 0xCC, the sound of the fall
                float m_fRadius;                     // 0xD0, of the blast at the start
                float m_fPower;                      // 0xD4
                float m_fGrowTime;                   // 0xD8, frames the blast grows
                float m_fRadiusMax;                  // 0xDC
                float m_fLinger;                     // 0xE0, frames to the deletion after
        };

        // The mark on the court where a meteor will land. Size 0x64.
        class CMeteorTouchDownMark : public bcs::CTask {
            public:
                // The mark at rPos on the ground, turned against the
                // direction rVel of the fall.
                CMeteorTouchDownMark(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel)
                    : CTask(pParent, -1)
                {
                    m_nStep = 0;
                    m_fTime = 0.0f;
                    m_vPos.x = rPos.x;
                    m_vPos.y = 0.0f;
                    m_vPos.z = rPos.z;
                    m_vRot.x = 0.0f;
                    m_vRot.y = RoundRad(nw4r::math::Atan2Rad(-rVel.x, -rVel.z));
                    m_vRot.z = 0.0f;
                    for (long i = 0; i < EFFECT_NUM; i++) {
                        m_ahEffect[i] = 0;
                    }
                }
                virtual ~CMeteorTouchDownMark();
                virtual void Main();

            private:
                enum {
                    EFFECT_NUM = 4
                };

                void CreateEffect_();
                void SetEffectAlphaRate_(float fRate);

                long m_nStep;                         // 0x34
                float m_fTime;                        // 0x38
                bcs::FVEC3 m_vPos;                    // 0x3C
                bcs::FVEC3 m_vRot;                    // 0x48
                unsigned long m_ahEffect[EFFECT_NUM]; // 0x54
        };

        // A debug read-out of the damage of a combined attack. Size 0x44.
        class CNpcBehemothDamageInfo : public bcs::CTask {
            public:
                CNpcBehemothDamageInfo(bcs::CTask *pParent)
                    : CTask(pParent, -1)
                {
                    m_bShow = 0;
                    m_fTime = 0.0f;
                    m_nRate = 0;
                    m_nDamage = 0;
                }
                virtual ~CNpcBehemothDamageInfo();
                virtual void Main();
                virtual void Disp();

            private:
                long m_bShow;    // 0x34
                float m_fTime;   // 0x38, frames left to show
                long m_nRate;    // 0x3C
                long m_nDamage;  // 0x40
        };
    }

    using namespace BEHEMOTH;

    // CNpcBehemothFiga

    // (auto-inlined into ShotFiga_; the linker drops this copy)
    CNpcBehemothFiga::CNpcBehemothFiga(bcs::CTask *pParent)
        : CChrObstacle(pParent, 0x11, 0x17)
    {
        m_hSe = 0;
        m_nStep = 0;
        m_fWait = 0.0f;
        m_pTarget = 0;
        m_hEffect = 0;
    }

    CNpcBehemothFiga::~CNpcBehemothFiga()
    {
        StopSe_();
        StopEffect_();
    }

    // Starts the fire ball at rPos towards rTarget (the point in front of
    // the boss); hOwner is the handle of the boss model.
    void CNpcBehemothFiga::Create(unsigned long hOwner, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget,
                                  CChrCore *pTarget, long nLv)
    {
        bcs::FVEC3Sub(&m_Velocity, &rTarget, &rPos);
        m_Pos = rPos;
        m_pTarget = pTarget;
        bcs::BCS_Vec_Normalize(m_Velocity, m_Velocity);
        bcs::FVEC3Scale(&m_Velocity, &m_Velocity, 0.4166667f);
        m_hEffect = CEffectCtrl::CreateAttachEffect(0x544, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
        CEffectCluster *pEff = FindEffect(m_hEffect);
        if (pEff != 0) {
            pEff->SetTranslate(&m_Pos);
            SetEffectRot_(pEff);
        }
        m_hSe = CGameSnd::Play(0xDFB, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
        m_AtkColl.Create(1, 5, 0, hOwner);
        m_AtkColl.SetAtkTypeAll(0x1803);
        m_AtkColl.SetTargetFlagAll(2);
        m_AtkColl.SetRadiusAll(CComMan::GetDatF(0x1B, 0L));
        m_AtkColl.SetEnableAll();
        m_AtkColl.SetHitEffectFrameAll(60.0f * CComMan::GetDatF(0x1A, 0L));
        m_AtkColl.SetAtkPowerAll(CComMan::GetDatF(0x1D, nLv));
        m_fHomingDist = CComMan::GetDatF(0x1C, 0L);
    }

    void CNpcBehemothFiga::Main()
    {
        if (m_nStep == 0) {
            int bLanded = DoHoming_(GetFrameRateF());
            CEffectCluster *pEff = FindEffect(m_hEffect);
            if (pEff != 0) {
                pEff->SetTranslate(&m_Pos);
                SetEffectRot_(pEff);
            }
            m_AtkColl.SetPosAll(m_Pos);
            if (bLanded) {
                StopEffect_();
                StopSe_();
                CGameSnd::Play(0xE07, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                bcs::FVEC3 vPos = m_Pos;
                vPos.y = 0.0f;
                CEffectCtrl::SimpleEffect(0x545, vPos, 3, 0, 0);
                m_AtkColl.SetPosAll(m_Pos, m_Pos);
                m_fWait = 1.0f;
                m_nStep++;
            }
        } else {
            m_fWait -= GetFrameRateF();
            if (m_fWait <= 0.0f) {
                Delete();
                m_nStep++;
            }
        }
    }

    // Turns the flight towards the target by at most 8 degrees and moves the
    // ball on by fFrame frames; not zero once it reaches the ground.
    int CNpcBehemothFiga::DoHoming_(float fFrame)
    {
        float fSpeed = bcs::BCS_Vec_GetLen(m_Velocity);
        if (m_pTarget != 0) {
            float fMaxRot = 0.13962634f;
            bcs::FVEC3 vTarget;
            m_pTarget->GetTransNodePos(vTarget);
            vTarget.y += 1.0f;
            bcs::FVEC3 vTo;
            bcs::FVEC3Sub(&vTo, &vTarget, &m_Pos);
            if (bcs::BCS_Vec_GetLen(vTo) > m_fHomingDist) {
                bcs::FVEC3 vDir;
                bcs::BCS_Vec_Normalize(vDir, m_Velocity);
                bcs::FVEC3 vToDir;
                bcs::BCS_Vec_Normalize(vToDir, vTo);
                float fCos = bcs::FVEC3Dot(&vDir, &vToDir);
                if (fCos >= nw4r::math::CosFIdx(NW4R_MATH_RAD_TO_FIDX(fMaxRot))) {
                    vDir = vToDir;
                } else {
                    // Rotate the direction by fMaxRot about vDir x vToDir
                    // (vTo stays the axis when the two are almost opposite).
                    if (fCos > nw4r::math::CosFIdx(96.0f)) {
                        PSVECCrossProduct((const Vec *)&vDir, (const Vec *)&vToDir, (Vec *)&vTo);
                    }
                    float fSin;
                    float fCosH;
                    nw4r::math::SinCosFIdx(&fSin, &fCosH, NW4R_MATH_RAD_TO_FIDX(0.5f * fMaxRot));
                    bcs::QUATERNION qRot = {vTo.x * fSin, vTo.y * fSin, vTo.z * fSin, fCosH};
                    bcs::QUATERNION qDir = {vDir.x, vDir.y, vDir.z, 0.0f};
                    bcs::QUATERNION qInv = {-vTo.x * fSin, -vTo.y * fSin, -vTo.z * fSin, fCosH};
                    PSQUATMultiply((const Quaternion *)&qRot, (const Quaternion *)&qDir, (Quaternion *)&qDir);
                    PSQUATMultiply((const Quaternion *)&qDir, (const Quaternion *)&qInv, (Quaternion *)&qDir);
                    vDir.y = qDir.y;
                    vDir.z = qDir.z;
                    vDir.x = qDir.x;
                    bcs::BCS_Vec_Normalize(vDir, vDir);
                }
                bcs::FVEC3Scale(&m_Velocity, &vDir, 0.4166667f);
            }
        }

        float fGravity = 0.0f;
        float fAcc = fGravity * fFrame;
        bcs::FVEC3 vMove;
        bcs::FVEC3Scale(&vMove, &m_Velocity, fFrame);
        vMove.y += 0.5f * (fAcc * fFrame);
        m_Velocity.y += fAcc;
        bcs::FVEC3Add(&m_Pos, &m_Pos, &vMove);
        if (m_Pos.y <= 0.0f) {
            m_Pos.y = 0.0f;
            return 1;
        }
        return 0;
    }

    // CNpcBehemothThunderga

    CNpcBehemothThunderga::CNpcBehemothThunderga(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir,
                                                 long nLv)
        : CUnivTaskEx(pParent, -1)
    {
        m_vPos = rPos;
        m_vDir = rDir;
        m_nCount = 3;
        m_fWait = ceil(RandRangeSort(CComMan::GetDatF(0x21, 0L), CComMan::GetDatF(0x22, 0L)));
        m_AtkColl.Create(1, 5, 3, 0);
        m_AtkColl.SetAtkTypeAll(0x1901);
        m_AtkColl.SetTargetFlagAll(2);
        m_AtkColl.SetRadiusAll(CComMan::GetDatF(0x20, 0L));
        m_AtkColl.SetAtkPowerAll(CComMan::GetDatF(0x23, nLv));
        bcs::FVEC3 vPos = m_vPos;
        vPos.y = 20.0f;
        m_AtkColl.SetPos(0, vPos, vPos);
        m_AtkColl.SetEnableAll();
        m_nCountMax = m_nCount;
    }

    CNpcBehemothThunderga::~CNpcBehemothThunderga()
    {
    }

    void CNpcBehemothThunderga::Main()
    {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            bcs::FVEC3 vPos = m_vPos;
            if (m_nCount > 1) {
                float fRot = RoundRad(bcs::BCS_RandMax(6.2831855f));
                bcs::BCS_VecZ_RotY(vPos, 5.0f * bcs::BCS_RandRange(0.2f, 0.4f), fRot);
                vPos.x += m_vPos.x;
                vPos.z += m_vPos.z;
            }
            if (m_nCount == m_nCountMax) {
                CGameSnd::Play(0xDFC, m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            CEffectCluster *pEff = FindEffect(CEffectCtrl::CreateEffect(0x549, 3, 0, 0, 0));
            if (pEff != 0) {
                pEff->SetTranslate(&vPos);
            }
            m_AtkColl.SetPos2(0, m_vPos);
            m_nCount--;
            m_fWait = ceil(bcs::BCS_RandRange(2.0f, 4.0f));
        }
        if (m_nCount <= 0) {
            Delete();
        }
    }

    // CNpcBehemothMeteor

    // Falls from rPos along the direction rDir after fWait frames.
    CNpcBehemothMeteor::CNpcBehemothMeteor(bcs::CTask *pParent, unsigned long hOwner, const bcs::FVEC3 &rPos,
                                           const bcs::FVEC3 &rDir, float fWait, long nLv)
        : CChrObstacle(pParent, 0x12, 0x17)
        , m_nStep(0)
        , m_fWait(fWait)
        , m_fTime(0.0f)
    {
        m_hSe = 0;
        for (long i = 0; i < EFFECT_NUM; i++) {
            m_ahEffect[i] = 0;
        }
        m_Pos = rPos;
        m_Velocity = rDir;
        m_fRotX = nw4r::math::Atan2Rad(LenXZ(m_Velocity), m_Velocity.y);
        m_fRotY = nw4r::math::Atan2Rad(m_Velocity.x, m_Velocity.z);
        m_fRotZ = 0.0f;
        m_fRotX = RoundRad(m_fRotX + 3.1415927f);
        m_fRotY = RoundRad(m_fRotY);
        m_fRadius = CComMan::GetDatF(0x10, 0L);
        m_fGrowTime = CComMan::GetDatF(0x12, 0L);
        m_fRadiusMax = CComMan::GetDatF(0x11, 0L);
        m_fLinger = CComMan::GetDatF(0x13, 0L);
        m_fPower = CComMan::GetDatF(0x14, nLv);
        bcs::FVEC3Scale(&m_Velocity, &m_Velocity, 0.016666668f * CComMan::GetDatF(0xF, 0L));
        m_HitColl.Create(1, 5, 0, hOwner);
        m_HitColl.SetAtkTypeAll(0x1A01);
        m_HitColl.SetTargetFlagAll(2);
        m_HitColl.SetRadiusAll(m_fRadius);
        m_HitColl.SetAtkPowerAll(m_fPower);
        m_HitColl.SetDisableAll();
    }

    CNpcBehemothMeteor::~CNpcBehemothMeteor()
    {
        for (long i = 0; i < EFFECT_NUM; i++) {
            CEffectCtrl::StopAttachEffect(m_ahEffect[i]);
            m_ahEffect[i] = 0;
        }
        bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hSe));
        if (pSe != 0 && !pSe->IsDelete()) {
            pSe->Stop(0.0f);
        }
    }

    void CNpcBehemothMeteor::Main()
    {
        switch (m_nStep) {
        case 0:
            m_fWait -= GetFrameRateF();
            if (m_fWait <= 0.0f) {
                m_ahEffect[0] = CEffectCtrl::CreateAttachEffect(0x539, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                m_ahEffect[2] = CEffectCtrl::CreateAttachEffect(0x537, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                SetEffectPos_(m_Pos);
                m_HitColl.SetPosAll(m_Pos);
                m_HitColl.SetEnableAll();
                m_hSe = CGameSnd::Play(0xDF6, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                m_nStep++;
            }
            break;
        case 1: {
            int bLanded = 0;
            bcs::FVEC3 vOld = m_Pos;
            float fFrame = GetFrameRateF();
            float fGravity = 0.0f;
            float fAcc = fGravity * fFrame;
            bcs::FVEC3 vMove;
            bcs::FVEC3Scale(&vMove, &m_Velocity, fFrame);
            vMove.y += 0.5f * (fAcc * fFrame);
            m_Velocity.y += fAcc;
            bcs::FVEC3Add(&m_Pos, &m_Pos, &vMove);
            if (m_Pos.y <= 1.0f) {
                m_Pos.y = 1.0f;
                bLanded = 1;
            }
            SetEffectPos_(m_Pos);
            m_HitColl.SetPosAll(m_Pos);
            if (bLanded) {
                m_fRotZ = 0.0f;
                m_fRotX = 0.0f;
                for (long i = 0; i < EFFECT_NUM; i++) {
                    CEffectCtrl::StopAttachEffect(m_ahEffect[i]);
                    m_ahEffect[i] = 0;
                }
                bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hSe));
                if (pSe != 0 && !pSe->IsDelete()) {
                    pSe->Stop(0.0f);
                }
                new (0x100) CMeteorTouchDownMark(CGame::GetTempParent(), m_Pos, m_Velocity);

                // The blast on the ground.
                bcs::FVEC3 vPos = m_Pos;
                vPos.y = 0.0f;
                m_ahEffect[0] = CEffectCtrl::CreateAttachEffect(0x538, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                m_ahEffect[1] = CEffectCtrl::CreateAttachEffect(0x53A, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                m_ahEffect[3] = CEffectCtrl::CreateAttachEffect(0x53B, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                SetEffectPos_(vPos);
                for (long i = 0; i < EFFECT_NUM; i++) {
                    unsigned long hEffect = m_ahEffect[i];
                    if (hEffect == 0) {
                        continue;
                    }
                    CEffectCluster *pEff = CEffectCtrl::GetInstance()->FindList(hEffect);
                    if (pEff != 0) {
                        long nNum = pEff->GetMdlObjNum();
                        for (long j = 0; j < nNum; j++) {
                            bcs::CMdlObj *pMdl = pEff->GetMdlObj(j);
                            if (pMdl != 0) {
                                pMdl->SetDrawPriority(0x7F, 0x7F);
                                pMdl->SetBBoxZSortCalcType(1);
                            }
                        }
                    }
                }
                CGameSnd::Play(0xDF7, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                CGame::StartCameraQuake(0x1C, -1, -1);
                m_fTime = 0.0f;
                m_HitColl.SetPosAll(vPos);
                m_HitColl.SetRadiusAll(m_fRadius);
                m_nStep++;
            }
            break;
        }
        case 2:
            // The blast grows from m_fRadius to m_fRadiusMax.
            m_fTime += GetFrameRateF();
            if (m_fGrowTime > 0.0f) {
                float fRate = m_fTime / m_fGrowTime;
                fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
                m_HitColl.SetRadiusAll(m_fRadius + fRate * (m_fRadiusMax - m_fRadius));
            }
            if (m_fTime >= m_fGrowTime) {
                m_fTime = m_fLinger;
                m_nStep++;
            }
            break;
        default:
            if (m_fTime <= 0.0f) {
                Delete();
            } else {
                m_fTime -= GetFrameRateF();
            }
            break;
        }
    }

    // CMeteorTouchDownMark

    CMeteorTouchDownMark::~CMeteorTouchDownMark()
    {
        for (long i = 0; i < EFFECT_NUM; i++) {
            CEffectCtrl::EraseAttachEffect(m_ahEffect[i]);
            m_ahEffect[i] = 0;
        }
    }

    // Waits, shows the mark for 180 frames, then fades it out over 60.
    void CMeteorTouchDownMark::Main()
    {
        if (!(CGame::g_uPauseBit & 4)) {
            float fFrame = GetFrameRateF();
            switch (m_nStep) {
            case 0:
                m_fTime -= fFrame;
                if (m_fTime <= 0.0f) {
                    CreateEffect_();
                    m_fTime = 0.0f;
                    m_nStep++;
                }
                break;
            case 1:
                m_fTime = 180.0f;
                m_nStep++;
                break;
            case 2:
                m_fTime -= fFrame;
                if (m_fTime <= 0.0f) {
                    m_fTime = 1.0f;
                    m_nStep++;
                }
                break;
            case 3:
                m_fTime -= 0.016666668f;
                if (m_fTime <= 0.0f) {
                    for (long i = 0; i < EFFECT_NUM; i++) {
                        CEffectCtrl::EraseAttachEffect(m_ahEffect[i]);
                        m_ahEffect[i] = 0;
                    }
                    m_nStep++;
                } else {
                    SetEffectAlphaRate_(m_fTime);
                }
                break;
            default:
                Delete();
                break;
            }
        }
    }

    // The mark effect, drawn on top of the court.
    void CMeteorTouchDownMark::CreateEffect_()
    {
        m_ahEffect[1] = CEffectCtrl::CreateAttachEffect(0x53C, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0);
        for (long i = 0; i < EFFECT_NUM; i++) {
            CEffectCluster *pEff = CEffectCtrl::GetInstance()->FindList(m_ahEffect[i]);
            if (pEff != 0) {
                pEff->SetTranslate(&m_vPos);
                pEff->SetRotateRad(&m_vRot);
            }
        }
        for (long i = 0; i < 2; i++) {
            CEffectCluster *pEff = FindEffect(m_ahEffect[i]);
            if (pEff != 0) {
                long nNum = pEff->GetMdlObjNum();
                for (long j = 0; j < nNum; j++) {
                    bcs::CMdlObj *pMdl = pEff->GetMdlObj(j);
                    if (pMdl != 0) {
                        pMdl->SetExBlendMode(0);
                        pMdl->MatSetZModeAll(1, GX_ALWAYS, 0);
                        pMdl->SetBBoxZSortCalcType(1);
                        pMdl->SetDrawPriority(2, 2);
                        pMdl->DetachFromParent();
                        bcs::CRenderMan::PushBack(pMdl, 5);
                    }
                }
            }
        }
    }

    // The alpha of every model of the mark effects, clamped to [0, 1].
    void CMeteorTouchDownMark::SetEffectAlphaRate_(float fRate)
    {
        float fAlpha = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
        for (long i = 0; i < EFFECT_NUM; i++) {
            CEffectCluster *pEff = CEffectCtrl::GetInstance()->FindList(m_ahEffect[i]);
            if (pEff != 0) {
                long nNum = pEff->GetMdlObjNum();
                for (long j = 0; j < nNum; j++) {
                    bcs::CMdlObj *pMdl = pEff->GetMdlObj(j);
                    if (pMdl != 0) {
                        pMdl->SetAlphaRateF(fAlpha);
                    }
                }
            }
        }
    }

    // CNpcBehemothDamageInfo

    CNpcBehemothDamageInfo::~CNpcBehemothDamageInfo()
    {
    }

    void CNpcBehemothDamageInfo::Main()
    {
        if (!(CGame::g_uPauseBit & 4) && m_bShow != 0 && m_fTime > 0.0f) {
            m_fTime -= GetFrameRateF();
            if (m_fTime <= 0.0f) {
                m_fTime = 0.0f;
                m_bShow = 0;
            }
        }
    }

    void CNpcBehemothDamageInfo::Disp()
    {
        if (m_bShow > 0) {
            // Each line twice: a black shadow, then the text.
            bcs::CDbgPrint::Print(0x41, 0xDF, 0, 0xE, bcs::BCS_Color(0x000000FF), "トス連携");
            bcs::CDbgPrint::Print(0x40, 0xDE, 0, 0xE, bcs::BCS_Color(0xF0F064FF), "トス連携");
            bcs::CDbgPrint::Print(0x41, 0xF1, 0, 0x14, bcs::BCS_Color(0x000000FF), "%d％", m_nRate);
            bcs::CDbgPrint::Print(0x40, 0xF0, 0, 0x14, bcs::BCS_Color(0xFFFF64FF), "%d％", m_nRate);
            bcs::CDbgPrint::Print(0x41, 0x10B, 0, 0x20, bcs::BCS_Color(0x000000FF), "%d", m_nDamage);
            bcs::CDbgPrint::Print(0x40, 0x10A, 0, 0x20, bcs::BCS_Color(0xFFFFFFFF), "%d", m_nDamage);
            bcs::CDbgPrint::Print(0x41, 0x129, 0, 0xE, bcs::BCS_Color(0x000000FF), "ダメージ");
            bcs::CDbgPrint::Print(0x40, 0x128, 0, 0xE, bcs::BCS_Color(0xF0DCDCFF), "ダメージ");
        }
    }

    namespace CNpcBehemothFiga_cpp {
        // Uncalled stand-in: stripped code of the original (likely what
        // starts the damage read-out) left this constant here.
        float UnusedDamageInfoTime() { return 300.0f; }
    }

    // CNpcBehemothBoss

    CNpcBehemothBoss::CNpcBehemothBoss(CChrMan *pMan, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nLevel)
        : CNpcBase(pMan, 0x13)
        , m_bHard(0)
        , m_fHP(0.0f)
        , m_fMaxHP(0.0f)
        , m_Unk1F8(0)
        , m_pAnmClr(0)
    {
        m_nAnmClr = -1;
        m_pBall = 0;
        m_Unk258 = -1;
        m_Unk260 = 0;
        m_Unk26C = -1;
        m_vUnk270.x = 0.0f;
        m_vUnk270.y = 0.0f;
        m_vUnk270.z = 0.0f;
        m_nLookNode = 0;
        m_Unk368 = 0;
        CStageMan *pStage = CStageMan::GetInstance();
        if (pStage == 0) {
            m_fCourtRadius = 23.0f;
        } else {
            m_fCourtRadius = pStage->GetCourtRadius();
        }
        if (nLevel >= 2) {
            m_bHard = 1;
        }
        m_vPos = rPos;
        m_vRot = rRot;
        m_vScale.x = 1.0f;
        m_vScale.y = 1.0f;
        m_vScale.z = 1.0f;
        m_nLevel = nLevel;
        GameResetingInitialize();
    }

    void CNpcBehemothBoss::GameResetingInitialize()
    {
        m_fMaxHP = GetDatF(0xC, GetLvI());
        m_fHP = m_fMaxHP;
        m_nSubStep = 1;
        m_Unk33C = 0;
        m_vLookRot.x = 0.0f;
        m_vLookRot.y = 0.0f;
        m_vLookRot.z = 0.0f;
        m_Unk260 = 0;
        m_Unk264 = 0;
        m_Unk268 = 0;
        ResetNeglect();
        m_bLookAt = 0;
        m_Unk2AC = 0;
        m_fUnk2B0 = 0.0f;
        m_fUnk2B4 = 0.0f;
        m_Unk2B8 = 0;
        m_Unk2BC = 0;
        SetStickDamageCount();
        m_fUnk2C4 = 0.0f;
        m_Unk2C8 = -1;
        m_Unk2CC = -1;
        m_Unk25C = 0;
        m_Unk258 = -1;
        m_vUnk270.x = 0.0f;
        m_vUnk270.y = 0.0f;
        m_vUnk270.z = 0.0f;
        m_fUnk29C = 0.0f;
        m_fUnk360 = 0.0f;
        m_Unk2D4 = 0x14;
        m_Unk2D8 = 0x14;
        for (long i = 0; i < 8; i++) {
            m_Unk340[i] = 0;
        }
    }

    void CNpcBehemothBoss::Create()
    {
        InitFsm_();
        CreateMdlObj_();
        if (m_pMdl != 0) {
            // Attack units 0-2 hit the characters, 3 knocks them down, 4-7
            // are the body's.
            CollAtkCreate(8);
            m_pAtkColl->SetTargetFlagAll(6);
            CollAtkSetAtkType(0, 0x11F01);
            CollAtkSetHitSe(0, 0xDEF);
            CollAtkSetAtkType(1, 0x11F01);
            CollAtkSetHitSe(1, 0xDEF);
            CollAtkSetAtkType(2, 0x11F01);
            CollAtkSetHitSe(2, 0xDEF);
            m_pAtkColl->ResetTargetFlag(2, 4);
            CollAtkSetAtkType(3, 0x11F01);
            CollAtkSetCollType(3, 1);
            CollAtkSetHitSe(3, 0xDEF);
            for (long i = 0; i < 4; i++) {
                CollAtkSetCollType(i + 4, 3);
                CollAtkSetAtkType(i + 4, 0x11F01);
                CollAtkSetHitSe(i + 4, 0xDEF);
            }

            CollBodyCreate(5);
            float x = GetDatF(0, 0);
            float y = GetDatF(1, 0);
            float z = GetDatF(2, 0);
            m_pBodyColl->SetPushEnable(0);
            m_pBodyColl->SetSize(0, x, y, z);
            m_pBodyColl->SetTargetFlag(0, 2);
            m_pBodyColl->SetPushRankAll(5);
            m_pBodyColl->SetPushPowerAll(6);

            x = GetDatF(3, 0);
            y = GetDatF(4, 0);
            z = GetDatF(5, 0);
            m_pBodyColl->SetCollType(4, 3);
            m_pBodyColl->SetSize(4, x, y, z);
            m_pBodyColl->SetTargetFlag(4, 0x16);
            m_pBodyColl->SetBodyEnable(4);

            m_pBodyColl->SetPushEnable(1);
            m_pBodyColl->SetSize(1, 2.5f, 7.0f, 6.5f);
            m_pBodyColl->SetTargetFlag(1, 0x14);

            x = GetDatF(6, 0);
            y = GetDatF(7, 0);
            z = GetDatF(8, 0);
            m_pBodyColl->SetCollType(2, 3);
            m_pBodyColl->SetSize(2, x, y, z);
            m_pBodyColl->SetCatchEnable(2);

            x = GetDatF(9, 0);
            y = GetDatF(10, 0);
            z = GetDatF(11, 0);
            m_pBodyColl->SetCollType(3, 3);
            m_pBodyColl->SetSize(3, x, y, z);
            m_pBodyColl->SetBallCatchCallback(3, CallbackBallReflect, this);
            m_pBodyColl->SetCatchEnable(3);
        }
    }

    CNpcBehemothBoss::~CNpcBehemothBoss()
    {
    }

    void CNpcBehemothBoss::InitFsm_()
    {
        FsmqInitAlign(this, STATE_NUM, 8, GetQueueWorkSize());
        FsmqSetState(STATE_DEFAULT, &CNpcBehemothBoss::CtorStateDefaultCall, &CNpcBehemothBoss::LoopStateDefaultCall,
                     &CNpcBehemothBoss::DtorStateDefaultCall);
        FsmqSetState(STATE_SLEEP, &CNpcBehemothBoss::CtorStateSleepCall, &CNpcBehemothBoss::LoopStateSleepCall,
                     &CNpcBehemothBoss::DtorStateSleepCall);
        FsmqSetState(STATE_NEUTRAL, &CNpcBehemothBoss::CtorStateNeutralCall, &CNpcBehemothBoss::LoopStateNeutralCall,
                     &CNpcBehemothBoss::DtorStateNeutralCall);
        FsmqSetState(STATE_MOVE, &CNpcBehemothBoss::CtorStateMoveCall, &CNpcBehemothBoss::LoopStateMoveCall,
                     &CNpcBehemothBoss::DtorStateMoveCall);
        FsmqSetState(STATE_START_WAR_CRY, &CNpcBehemothBoss::CtorStateStartWarCryCall,
                     &CNpcBehemothBoss::LoopStateStartWarCryCall, &CNpcBehemothBoss::DtorStateStartWarCryCall);
        FsmqSetState(STATE_ATTACK_FANG, &CNpcBehemothBoss::CtorStateAttackFangCall,
                     &CNpcBehemothBoss::LoopStateAttackFangCall, &CNpcBehemothBoss::DtorStateAttackFangCall);
        FsmqSetState(STATE_ATTACK_CLAW, &CNpcBehemothBoss::CtorStateAttackClawCall,
                     &CNpcBehemothBoss::LoopStateAttackClawCall, &CNpcBehemothBoss::DtorStateAttackClawCall);
        FsmqSetState(STATE_ATTACK_HORN, &CNpcBehemothBoss::CtorStateAttackHornCall,
                     &CNpcBehemothBoss::LoopStateAttackHornCall, &CNpcBehemothBoss::DtorStateAttackHornCall);
        FsmqSetState(STATE_ATTACK_TAIL, &CNpcBehemothBoss::CtorStateAttackTailCall,
                     &CNpcBehemothBoss::LoopStateAttackTailCall, &CNpcBehemothBoss::DtorStateAttackTailCall);
        FsmqSetState(STATE_ATTACK_METEOR, &CNpcBehemothBoss::CtorStateAttackMeteorCall,
                     &CNpcBehemothBoss::LoopStateAttackMeteorCall, &CNpcBehemothBoss::DtorStateAttackMeteorCall);
        FsmqSetState(STATE_ATTACK_FIGA, &CNpcBehemothBoss::CtorStateAttackFigaCall,
                     &CNpcBehemothBoss::LoopStateAttackFigaCall, &CNpcBehemothBoss::DtorStateAttackFigaCall);
        FsmqSetState(STATE_ATTACK_THUNDERGA, &CNpcBehemothBoss::CtorStateAttackThundergaCall,
                     &CNpcBehemothBoss::LoopStateAttackThundergaCall, &CNpcBehemothBoss::DtorStateAttackThundergaCall);
        FsmqSetState(STATE_ATTACK_JUMP, &CNpcBehemothBoss::CtorStateAttackJumpCall,
                     &CNpcBehemothBoss::LoopStateAttackJumpCall, &CNpcBehemothBoss::DtorStateAttackJumpCall);
        FsmqSetState(STATE_DAMAGE, &CNpcBehemothBoss::CtorStateDamageCall, &CNpcBehemothBoss::LoopStateDamageCall,
                     &CNpcBehemothBoss::DtorStateDamageCall);
        FsmqSetState(STATE_KO, &CNpcBehemothBoss::CtorStateKOCall, &CNpcBehemothBoss::LoopStateKOCall,
                     &CNpcBehemothBoss::DtorStateKOCall);
        FsmqSetState(STATE_DOLL, &CNpcBehemothBoss::CtorStateDollCall, &CNpcBehemothBoss::LoopStateDollCall,
                     &CNpcBehemothBoss::DtorStateDollCall);
        FsmqSetState(STATE_ESCAPE_JUMP, &CNpcBehemothBoss::CtorStateEscapeJumpCall,
                     &CNpcBehemothBoss::LoopStateEscapeJumpCall, &CNpcBehemothBoss::DtorStateEscapeJumpCall);
        FsmqSetState(STATE_RUSH, &CNpcBehemothBoss::CtorStateRushCall, &CNpcBehemothBoss::LoopStateRushCall,
                     &CNpcBehemothBoss::DtorStateRushCall);
        FsmqSetState(STATE_ATTACK_SMASH, &CNpcBehemothBoss::CtorStateAttackSmashCall,
                     &CNpcBehemothBoss::LoopStateAttackSmashCall, &CNpcBehemothBoss::DtorStateAttackSmashCall);
        FsmqSetState(STATE_ATTACK_TURN, &CNpcBehemothBoss::CtorStateAttackTurnCall,
                     &CNpcBehemothBoss::LoopStateAttackTurnCall, &CNpcBehemothBoss::DtorStateAttackTurnCall);
        FsmqSetState(STATE_THINK, &CNpcBehemothBoss::CtorStateThinkCall, &CNpcBehemothBoss::LoopStateThinkCall,
                     &CNpcBehemothBoss::DtorStateThinkCall);
        FsmqSetState(STATE_DO_NOTHING, &CNpcBehemothBoss::CtorStateDoNothingCall,
                     &CNpcBehemothBoss::LoopStateDoNothingCall, &CNpcBehemothBoss::DtorStateDoNothingCall);
        FsmqSetState(STATE_ATTACK_HOWLING, &CNpcBehemothBoss::CtorStateAttackHowlingCall,
                     &CNpcBehemothBoss::LoopStateAttackHowlingCall, &CNpcBehemothBoss::DtorStateAttackHowlingCall);
        FsmqSetState(STATE_WAIT_DUNK, &CNpcBehemothBoss::CtorStateWaitDunkCall,
                     &CNpcBehemothBoss::LoopStateWaitDunkCall, &CNpcBehemothBoss::DtorStateWaitDunkCall);
        FsmqSetState(STATE_WANDERING, &CNpcBehemothBoss::CtorStateWanderingCall,
                     &CNpcBehemothBoss::LoopStateWanderingCall, &CNpcBehemothBoss::DtorStateWanderingCall);
        FsmqSetState(STATE_ANGER, &CNpcBehemothBoss::CtorStateAngerCall, &CNpcBehemothBoss::LoopStateAngerCall,
                     &CNpcBehemothBoss::DtorStateAngerCall);
        FsmqSetState(STATE_ARTS_EVENT, &CNpcBehemothBoss::CtorStateArtsEventCall,
                     &CNpcBehemothBoss::LoopStateArtsEventCall, &CNpcBehemothBoss::DtorStateArtsEventCall);
        FsmqChangeState(STATE_DO_NOTHING);
    }

    // The model of the NPC slot that holds the Behemoth (NPC type 0 or
    // 0xF), its motion, colour animations, shadow and reflection.
    void CNpcBehemothBoss::CreateMdlObj_()
    {
        long nSlot = -1;
        for (long i = 0; i < 8; i++) {
            if (CGame::GetNpcType(i) == 0) {
                nSlot = i;
                break;
            }
            if (CGame::GetNpcType(i) == 0xF) {
                nSlot = i;
                break;
            }
        }
        nw4r::g3d::ResFile mdlFile = CLoadData::GetRes(nSlot + 0xB);
        nw4r::g3d::ResFile motFile = CLoadData::GetRes(nSlot + 0x1B);
        nw4r::g3d::ResMdl mdl = mdlFile.GetResMdl(0UL);

        m_pMdl = new bcs::CMdlObj;
        unsigned long anmFlag = 0;
        mdlFile.HasResAnmChr(); // (tested, but adds no flag)
        if (mdlFile.HasResAnmClr()) {
            anmFlag |= 0x108;
        }
        if (mdlFile.HasResAnmTexSrt()) {
            anmFlag |= 0x204;
        }
        if (mdlFile.HasResAnmTexPat()) {
            anmFlag |= 0x3;
        }
        if (mdlFile.HasResAnmShp()) {
            anmFlag |= 0x7000;
        }
        m_pMdl->Create(mdl, anmFlag, 0);
        m_pMdl->OnFlag(0x100);
        m_pMdl->OnFlag(0x2);
        m_pMdl->SetExBlendMode(1);
        m_pMdl->OnFlag(0x2000);

        m_pAnmMot = new (0) bcs::CAnmMotion;
        m_pAnmMot->Create(m_pMdl, motFile, CC_DATA::GetNpcMdl_NodeExInfo(0), 3, 0);
        m_pAnmMot->SetTrgCallback(CallbackMotTrg, this);
        m_pAnmMot->OnFlag(0x8);
        m_pMdl->AttachAnmObj(m_pAnmMot, bcs::CMdlObj::ANMOBJTYPE_CHR);

        if (mdlFile.HasResAnmClr()) {
            long nFirst = -1;
            for (unsigned long i = 0; i < ANMCLR_NUM; i++) {
                m_aAnmClr[i] = mdlFile.GetResAnmClr(s_apAnmClrName[m_bHard != 0][i]);
                if (m_aAnmClr[i].IsValid() && nFirst < 0) {
                    nFirst = i;
                }
            }
            if (nFirst >= 0) {
                m_pAnmClr = new (0x100) bcs::CAnmMatClr;
                if (m_pAnmClr != 0) {
                    m_pAnmClr->Create(m_pMdl, m_aAnmClr[nFirst], 0, 0);
                    m_pMdl->AttachAnmObj(m_pAnmClr, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
                    m_nAnmClr = 0;
                    SetColorAnim_(-1);
                }
            }
        }

        if (m_pMdl != 0) {
            m_pMdl->m_vTrans = m_vPos;
            m_pMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
        }

        for (long i = 0; i < VNODE_NUM; i++) {
            nw4r::g3d::ResNode node = mdl.GetResNode(s_apVNodeName[i]);
            if (node.IsValid()) {
                // (the original keeps only the low byte of the index, signed)
                m_nVNode[i] = (signed char)node.GetID();
            } else {
                m_nVNode[i] = 0;
            }
        }
        bcs::CRenderMan::PushBack(m_pMdl, 2);

        nw4r::g3d::ResMdl shadowMdl = mdlFile.GetResMdl(0UL);
        m_pShadow = new bcs::CMdlShadow(m_pMdl, shadowMdl, 0x100, 0, 0);
        bcs::CRenderMan::PushBack(m_pShadow, 2);
        nw4r::g3d::ResMdl refMdl = mdlFile.GetResMdl(0UL);
        m_pRef = new bcs::CMdlRef(m_pMdl, refMdl, 0, 0, 0);
        bcs::CRenderMan::PushBack(m_pRef, 4);

        SetupLookAtParam_();
    }

    void CNpcBehemothBoss::SetColorAnim_(long nAnm)
    {
        if (nAnm != m_nAnmClr) {
            if (m_pMdl != 0 && m_pAnmClr != 0 && m_aAnmClr[nAnm].IsValid()) {
                m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), m_aAnmClr[nAnm]);
                if (nAnm < 0) {
                    m_pAnmClr->SetFrame(m_pAnmClr->VFunc28());
                }
            }
            m_nAnmClr = nAnm;
        }
    }

    long CNpcBehemothBoss::SendMessage(long nMsg, long nArg0, long nArg1, long, long)
    {
        switch (nMsg) {
        case 0xF:
            Reset();
            GameResetingInitialize();
            FsmqChangeState(STATE_NEUTRAL);
            break;
        case 0x12:
            Reset();
            break;
        case 0xB:
            // The arts event: to its place 5 in front of the goal.
            Eff_EraseAll();
            SE_StopAll(0.0f);
            m_vPos.x = 0.0f;
            m_vPos.y = 0.0f;
            m_vPos.z = 5.0f;
            m_vRot.x = 0.0f;
            m_vRot.y = 3.1415927f;
            m_vRot.z = 0.0f;
            if (CGame::GetTargetGoalIdx(1)) {
                m_vRot.y = 0.0f;
                m_vPos.z *= -1.0f;
            }
            m_nSubStep = 0;
            m_Unk2AC = 1;
            FsmqClear();
            FsmqChangeState(STATE_ARTS_EVENT);
            break;
        case 0x3:
            Reset();
            FsmqChangeState(STATE_NEUTRAL);
            break;
        case 0x24:
            switch (nArg0) {
            case 0:
                m_Unk2AC = 1;
                if (m_nSubStep == 1) {
                    FsmqChangeState(STATE_WAIT_DUNK);
                }
                break;
            case 1:
                m_Unk2AC = 0;
                break;
            case 2:
                if (FsmqGetState() == STATE_ARTS_EVENT) {
                    FsmqGetWork<ARTS_EVENT_WORK>()->m_nCount = nArg1;
                }
                break;
            }
            break;
        }
        return 0;
    }

    // Back to the start: 12 in front of the goal it defends, then the war
    // cry (straight to neutral with flag 0x200 of the game).
    void CNpcBehemothBoss::Reset()
    {
        Eff_EraseAll();
        SE_StopAll(0.0f);
        m_vPos.x = 0.0f;
        m_vPos.y = 0.0f;
        m_vPos.z = 12.0f;
        m_vRot.x = 0.0f;
        m_vRot.y = 3.1415927f;
        m_vRot.z = 0.0f;
        if (CGame::GetTargetGoalIdx(1)) {
            m_vRot.y = 0.0f;
            m_vPos.z *= -1.0f;
        }
        FsmqClear();
        if (CGame::g_GameWork.m_nFlag18 & 0x200) {
            FsmqChangeState(STATE_NEUTRAL);
        } else {
            FsmqChangeState(STATE_START_WAR_CRY);
        }
    }

    void CNpcBehemothBoss::GetVNodePos(long nNode, bcs::FVEC3 &rPos) const
    {
        bcs::FMTX34 mtx;
        m_pMdl->GetScnMtxPos(&mtx, m_nVNode[nNode], bcs::CScnObj::SCNOBJMTXTYPE_1);
        rPos.x = mtx.m[0][3];
        rPos.y = mtx.m[1][3];
        rPos.z = mtx.m[2][3];
    }

    // The body capsule: its two ends, and its radius as the result.
    float CNpcBehemothBoss::GetBodyCollInfo(bcs::FVEC3 &rTop, bcs::FVEC3 &rBottom)
    {
        if (m_pBodyColl == 0) {
            return 0.0f;
        }
        CCollUnit *pColl = m_pBodyColl->GetColl(0);
        if (pColl == 0) {
            return 0.0f;
        }
        bcs::FVEC3 vPos = pColl->GetPos();
        bcs::FVEC3 vSize = pColl->GetSize();
        if (vSize.x < vSize.z) {
            bcs::FVEC3 vOfs;
            bcs::BCS_VecZ_RotY(vOfs, vSize.z - vSize.x, pColl->m_fRotY);
            bcs::FVEC3Add(&rTop, &vPos, &vOfs);
            bcs::FVEC3Sub(&rBottom, &vPos, &vOfs);
        } else {
            bcs::FVEC3 vOfs;
            bcs::BCS_VecX_RotY(vOfs, vSize.x - vSize.z, pColl->m_fRotY);
            bcs::FVEC3Add(&rTop, &vPos, &vOfs);
            bcs::FVEC3Sub(&rBottom, &vPos, &vOfs);
        }
        return vSize.x;
    }

    void CNpcBehemothBoss::AnimPlay(long nMot, int bLoop, float fBlend, float fFrame)
    {
        CNpcBase::AnimPlay(nMot, bLoop, fBlend, fFrame);
    }

    void CNpcBehemothBoss::AnimStop()
    {
        CNpcBase::AnimStop();
    }

    void CNpcBehemothBoss::AnimSetSpeed(float fSpeed)
    {
        CNpcBase::AnimSetSpeed(fSpeed);
    }

    void CNpcBehemothBoss::AnimSetFrame(float fFrame)
    {
        CNpcBase::AnimSetFrame(fFrame);
    }

    // The feet: a footstep sound (0xDEA for parameter 1) and a camera quake
    // at foot nFoot (0-3).
    void CNpcBehemothBoss::CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg)
    {
        if ((long)pTrg->m_nType == 0x10) {
            long nSe = 0xDE9;
            long nFoot = pTrg->m_nParam1;
            if (pTrg->m_nParam0 == 1) {
                nSe = 0xDEA;
            }
            bcs::FMTX34 mtx;
            m_pMdl->GetScnMtxPos(&mtx, m_nVNode[VNODE_FOOT + (nFoot > 3 ? 3 : (nFoot < 0 ? 0 : nFoot))],
                                 bcs::CScnObj::SCNOBJMTXTYPE_1);
            bcs::FVEC3 vPos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
            PlaySePos(nSe, &vPos, 0, -1, 0.0f, 1.0f, 0.0f);
            StartCameraQuake(0x1A);
        }
    }

    // Pushed by another body: moves by the push, stopped by the walls of
    // the map.
    void CNpcBehemothBoss::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush != 0) {
            bcs::FVEC3 vFrom = m_vPos;
            bcs::FVEC3 vTo(vFrom.x + pPush->x, vFrom.y, vFrom.z + pPush->z);
            bcs::FVEC3 vPos;
            bcs::FVEC3 vNormal;
            float fTime;
            if (CCollMan::DetectMapHit(vPos, vNormal, fTime, vFrom, vTo, -m_fUnk2B0, 3, 0x1000000)) {
                // What the wall took off the push goes back to the other.
                vNormal.x = vPos.x - vTo.x;
                vNormal.z = vPos.z - vTo.z;
                pOther->DoBodyPushCallback(pBody, &vNormal);
            } else {
                pOther->DoBodyPushCallback(pBody, 0);
            }
            m_vPos = vPos;
            if (m_pBodyColl != 0) {
                vTo.x = vPos.x - vFrom.x;
                vTo.z = vPos.z - vFrom.z;
                m_pBodyColl->AddPosXZAll(vTo);
                m_pBodyColl->CalcAABBAll();
            }
        }
    }

    // A shot of the players' team hits the body: the damage, then the
    // ball goes up off the body.
    void CNpcBehemothBoss::CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (pBall != 0) {
            if (FsmqGetState() != STATE_ARTS_EVENT && pBall->IsShootMode() && pBall->GetShotTeamIdx() == 0) {
                PlaySePos(0xDF8, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                m_Unk2D8 = STATE_THINK;
                float fPoint = 0.0f;
                long nStrong = 0;
                CalcDamagePoint_(pBall, fPoint, nStrong);
                m_fHP -= fPoint;
                m_fHP = m_fHP > m_fMaxHP ? m_fMaxHP : (m_fHP < 0.0f ? 0.0f : m_fHP);
                if (m_fHP <= 0.0f) {
                    FsmqChangeState(STATE_KO);
                    CGame::SendMessage(99, 3, 0, 0);
                } else {
                    CGame::SendMessage(99, 2, (long)m_fHP, (long)m_fMaxHP);
                    if (FsmqGetState() != STATE_ATTACK_JUMP && FsmqGetState() != STATE_ESCAPE_JUMP &&
                        FsmqGetState() != STATE_ATTACK_METEOR && FsmqGetState() != STATE_ATTACK_TURN) {
                        DAMAGE_WORK *pWork = static_cast<DAMAGE_WORK *>(FsmqSetState(STATE_DAMAGE));
                        pWork->m_vPos = pBall->GetPos();
                        pWork->m_nKind = nStrong;
                    }
                }
                Eff_CreateSimple(0x57E, pBall->GetPos(), 3, 0);
                m_Unk2BC = 0;

                long nPower = pBall->GetVsBossCoinPower();
                nPower = nPower > 5 ? 5 : (nPower < 0 ? 0 : nPower);
                long nQuake = -1;
                switch (pBall->GetShootType()) {
                case 1:
                    if (nPower >= 5) {
                        nQuake = 0x47;
                    }
                    break;
                case 6:
                    if (nPower >= 5) {
                        nQuake = 0x47;
                    }
                    break;
                case 3:
                    nQuake = 0x45;
                    break;
                case 9:
                    nQuake = 0x45;
                    break;
                }
                if (nQuake >= 0) {
                    StartCameraQuake(nQuake);
                }

                long nChr = pBall->GetShotChrIdx();
                if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
                    CChrCore *pChr = CChrMan::GetChrCore(nChr);
                    if (pChr != 0 && pChr->IsEnable()) {
                        m_Unk340[nChr]++;
                        m_Unk26C = nChr;
                    }
                }
                bcs::FVEC3 vPos = pBall->GetPos();
                pBall->SetVsBossTossUpMode(vPos, 1, 1);
                CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x17, 0, 0, 0, 0);
            }
        }
    }

    void CNpcBehemothBoss::CallbackBallReflect(CCollBodyUnit *pBody, CBall *pBall, void *pArg)
    {
        if (pArg != 0) {
            static_cast<CNpcBehemothBoss *>(pArg)->CallbackBallReflect_(pBody, pBall);
        }
    }

    // A shot of the players' team on the hard part of the body (unit 3):
    // it may bounce off.
    void CNpcBehemothBoss::CallbackBallReflect_(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (pBall == 0) {
            return;
        }
        long nState = FsmqGetState();
        if ((nState >= STATE_DAMAGE && nState <= STATE_KO) || nState == STATE_ARTS_EVENT) {
            return;
        }
        if (!pBall->CanbeVsBossReflect() || pBall->GetShotTeamIdx() != 0) {
            return;
        }
        SetStopAttackTime(0.0f);
        ResetNeglect();
        pBall->SetVsBossReflectCheckEnd();
        if (IsBallReflect_(pBall)) {
            // The spark turned from the boss towards the ball.
            bcs::FVEC3 vDir;
            bcs::FVEC3Sub(&vDir, &pBall->GetPos(), &m_vPos);
            vDir.y = 0.0f;
            float fRot = 0.0f;
            if (vDir.x * vDir.x + vDir.z * vDir.z > 0.0f) {
                bcs::FVEC3 vXZ(vDir.x, 0.0f, vDir.z);
                bcs::BCS_Vec_Normalize(vDir, vXZ);
                fRot = nw4r::math::Atan2Rad(vDir.x, vDir.z);
            }
            Eff_CreateSimple(0x53D, pBall->GetPos(), fRot, 3, 0);
            long nChr = pBall->GetShotChrIdx();
            if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum() && CChrMan::GetChrCore(nChr) != 0) {
                m_Unk26C = nChr;
            }
            bcs::FVEC3 vPos = pBall->GetPos();
            pBall->SetVsBossTossUpMode(vPos, 1, 1);
            CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x17, 0, 0, 0, 0);
        }
    }

    unsigned long CNpcBehemothBoss::CallbackBodyDamage_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                        const bcs::FVEC3 &rPos)
    {
        DoDamage_(pAtk->m_nAtkType, pAtk->m_nOwnerChrIdx, rPos);
        if (pAtk->GetAtkKind() == 0x1F) {
            const_cast<CCollAtkUnit *>(pAtk)->m_nFlag &= ~0x28;
        }
        return 1;
    }

    namespace CNpcBehemothFiga_cpp {
        // Uncalled stand-in: stripped code of the original left this
        // constant here.
        float UnusedRate() { return 0.1f; }
    }

    // Hit by attack nAtkType of character nChr at rPos: kinds 0x1B, 0x1C
    // and 0x1F are the players' special attacks, sub-kind 0xC a stick hit
    // (enough of them make it turn round).
    void CNpcBehemothBoss::DoDamage_(unsigned long nAtkType, long nChr, const bcs::FVEC3 &rPos)
    {
        unsigned long nKind = (nAtkType >> 8) & 0xFF;
        unsigned long nSub = nAtkType & 0xFF;
        int bStick = 0;
        if ((nKind >= 0x1B && nKind <= 0x1C) || nKind == 0x1F) {
            m_Unk2BC = 0;
            m_Unk2D8 = STATE_ANGER;
        }
        if (nSub == 0xC) {
            m_Unk2D8 = STATE_THINK;
            m_Unk2BC++;
            bStick = 1;
        }
        if (nChr >= 0) {
            m_Unk340[nChr]++;
            m_Unk26C = nChr;
        }

        float fPoint = 10.0f;
        switch (nKind) {
        case 0x1C:
            fPoint = 30.0f;
            break;
        case 0x1F:
            fPoint = 50.0f;
            break;
        case 0x1B:
            fPoint = 555.0f;
            break;
        }
        if (nSub == 0xC) {
            fPoint = 20.0f;
        }
        m_fHP -= fPoint;
        m_fHP = m_fHP > m_fMaxHP ? m_fMaxHP : (m_fHP < 0.0f ? 0.0f : m_fHP);
        if (m_fHP <= 0.0f) {
            FsmqChangeState(STATE_KO);
            CGame::SendMessage(99, 3, 0, 0);
        } else {
            CGame::SendMessage(99, 2, (long)m_fHP, (long)m_fMaxHP);
            if (m_Unk2BC < GetStickDamageCount()) {
                long nState = FsmqGetState();
                if ((nState < STATE_ATTACK_JUMP || nState > STATE_DAMAGE) && nState != STATE_ARTS_EVENT &&
                    nState != STATE_ATTACK_METEOR && nState != STATE_ESCAPE_JUMP) {
                    if (nState == STATE_ATTACK_TURN) {
                        if (bStick == 1 && m_nSubStep == 1) {
                            DAMAGE_WORK *pWork = static_cast<DAMAGE_WORK *>(FsmqSetState(STATE_DAMAGE));
                            pWork->m_vPos = rPos;
                            pWork->m_nKind = 2;
                        }
                    } else if (bStick) {
                        if (m_nSubStep == 1) {
                            DAMAGE_WORK *pWork = static_cast<DAMAGE_WORK *>(FsmqSetState(STATE_DAMAGE));
                            pWork->m_vPos = rPos;
                            pWork->m_nKind = 2;
                        }
                    } else {
                        DAMAGE_WORK *pWork = static_cast<DAMAGE_WORK *>(FsmqSetState(STATE_DAMAGE));
                        pWork->m_vPos = rPos;
                        pWork->m_nKind = 1;
                    }
                }
            } else {
                // Enough stick hits: it turns round on the one who hit last.
                m_Unk2BC = 0;
                SetStickDamageCount();
                m_Unk258 = SearchingForTargetPlayer();
                ResetNeglect();
                m_vUnk270.z = 0.0f;
                FsmqChangeState(STATE_ATTACK_TURN);
            }
        }
    }

    // Whether pBall bounces off (by the chance of the state it is in);
    // shots of types 3 and 9 never do.
    int CNpcBehemothBoss::IsBallReflect_(CBall *pBall)
    {
        int bCan;
        if (pBall == 0) {
            bCan = 0;
        } else if (pBall->GetShootType() == 3 || pBall->GetShootType() == 9) {
            bCan = 0;
        } else {
            bCan = 1;
        }
        if (!bCan) {
            return 0;
        }
        long nRate = 0;
        switch (FsmqGetState()) {
        case STATE_DO_NOTHING:
            nRate = GetDatI(0x14E, GetLvI());
            break;
        case STATE_SLEEP:
            nRate = GetDatI(0x152, GetLvI());
            break;
        case STATE_MOVE:
            nRate = GetDatI(0x167, GetLvI());
            break;
        case STATE_RUSH:
            nRate = GetDatI(0x17D, GetLvI());
            break;
        case STATE_ATTACK_FANG:
            nRate = GetDatI(0x194, GetLvI());
            break;
        case STATE_ATTACK_SMASH:
            nRate = GetDatI(0x1A8, GetLvI());
            break;
        case STATE_ATTACK_CLAW:
            nRate = GetDatI(0x1BC, GetLvI());
            break;
        case STATE_ATTACK_HORN:
            nRate = GetDatI(0x1D0, GetLvI());
            break;
        case STATE_ATTACK_TAIL:
            nRate = GetDatI(0x1E4, GetLvI());
            break;
        case STATE_ATTACK_JUMP:
            nRate = GetDatI(0x1FA, GetLvI());
            break;
        case STATE_ATTACK_TURN:
            nRate = GetDatI(0x211, GetLvI());
            break;
        case STATE_ATTACK_METEOR:
            nRate = GetDatI(0x224, GetLvI());
            break;
        case STATE_ATTACK_FIGA:
            nRate = GetDatI(0x240, GetLvI());
            break;
        case STATE_ATTACK_THUNDERGA:
            nRate = GetDatI(0x250, GetLvI());
            break;
        case STATE_ESCAPE_JUMP:
            nRate = GetDatI(0x174, GetLvI());
            break;
        }
        return LotRate() < nRate;
    }

    // The damage pBall does (rPoint), from the coins the team holds, and
    // whether the shot is a strong one (rStrong: a special shot with the
    // full coin power, or shot type 9, which always does 555).
    void CNpcBehemothBoss::CalcDamagePoint_(CBall *pBall, float &rPoint, long &rStrong)
    {
        if (pBall != 0) {
            float fPoint = 5.0f;
            long nCoin = CGame::GetTeamCoin(0);
            switch (nCoin) {
            case 0:
                break;
            case 1:
                fPoint = 10.0f;
                break;
            case 2:
                fPoint = 25.0f;
                break;
            case 3:
                fPoint = 50.0f;
                break;
            case 4:
                fPoint = 75.0f;
                break;
            case 5:
                fPoint = 100.0f;
                break;
            default:
                fPoint = 10.0f * (nCoin - 5) + 100.0f;
                if (fPoint >= 999.0f) {
                    fPoint = 999.0f;
                }
                break;
            }
            long nPower = pBall->GetVsBossCoinPower();
            nPower = nPower > 5 ? 5 : (nPower < 0 ? 0 : nPower);
            rStrong = 0;
            switch (pBall->GetShootType()) {
            case 1:
                if (nPower >= 5) {
                    rStrong = 1;
                }
                break;
            case 6:
                if (nPower >= 5) {
                    rStrong = 1;
                }
                break;
            case 3:
                if (nPower >= 5) {
                    rStrong = 1;
                }
                break;
            case 9:
                rStrong = 1;
                fPoint = 555.0f;
                break;
            }
            rPoint = fPoint;
        }
    }

    static inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    // Counts a timer down to zero.
    static inline void CountDown(float &rTime, float fFrame)
    {
        if (rTime > 0.0f) {
            rTime -= fFrame;
            rTime = Max(0.0f, rTime);
        }
    }

    // Shows a model in the colour of a stunned boss (flags 0xC0) or not.
    static inline void SetStunColor(bcs::CScnObj *pObj, int bOff)
    {
        if (pObj != 0) {
            if (bOff == 0) {
                pObj->OnFlag(0xC0);
            } else {
                pObj->OffFlag(0xC0);
            }
        }
    }

    void CNpcBehemothBoss::MainFunc()
    {
        float fFrame = GetFrameRateF();

        // The ball in play.
        CBall *pBall;
        if (!CBallMan::IsActive()) {
            pBall = 0;
        } else if (CBallMan::GetActiveBallNum() <= 0) {
            pBall = 0;
        } else {
            pBall = CBallMan::GetBallPtr(0);
            if (pBall == 0) {
                pBall = 0;
            } else {
                pBall = pBall->IsActive() ? pBall : 0;
            }
        }
        m_pBall = pBall;

        // How far it keeps off the walls: 8 while it rushes.
        long nState = FsmqGetState();
        if (nState == STATE_RUSH) {
            if (m_Unk2B8 == 0) {
                m_fUnk2B4 = 8.0f;
            } else {
                m_fUnk2B4 = 0.0f;
            }
        } else {
            m_fUnk2B4 = 0.0f;
        }
        m_fUnk2B0 += 0.015625f * (m_fUnk2B4 - m_fUnk2B0);

        nState = FsmqGetState();
        if (nState == STATE_SLEEP || nState == STATE_MOVE) {
            CountDown(m_vUnk270.x, fFrame);
        }
        nState = FsmqGetState();
        if (nState == STATE_SLEEP || nState == STATE_MOVE) {
            CountDown(m_fUnk360, fFrame);
        }
        nState = FsmqGetState();
        if (nState == STATE_SLEEP || nState == STATE_MOVE) {
            CountDown(m_fNeglect, fFrame);
        }
        CountDown(m_vUnk270.z, fFrame);

        // The time one character holds the ball.
        m_Unk2CC = m_Unk2C8;
        CChrCore *pHolder = CChrMan::Util_SearchHaveBallChr(0);
        if (pHolder != 0) {
            if (!pHolder->IsVsBossDead()) {
                m_Unk2C8 = pHolder->GetPlayerNo();
            } else {
                m_Unk2C8 = -1;
            }
        } else {
            m_Unk2C8 = -1;
        }
        if (m_Unk2C8 >= 0 && m_Unk2C8 == m_Unk2CC) {
            m_fUnk2C4 -= fFrame;
            m_fUnk2C4 = Max(0.0f, m_fUnk2C4);
        } else {
            SetTimeMonitoring();
        }

        FsmqExec();

        bcs::FVEC3 vPos = m_vPos;
        bcs::FVEC3 vNormal;
        float fTime;
        if (CCollMan::DetectMapHit(vPos, vNormal, fTime, m_vPos, m_vPos, -m_fUnk2B0, 3, 0x1000000)) {
            m_vPos.x = vPos.x;
            m_vPos.z = vPos.z;
        }

        if (m_pMdl != 0) {
            bcs::FVEC3 vScale = m_vScale;
            bcs::FVEC3Scale(&vScale, &vScale, GetParamScale());
            m_pMdl->m_vScale = vScale;
            m_pMdl->UpdateFrame(fFrame);
            if (m_pMdl != 0) {
                m_pMdl->m_vTrans = m_vPos;
                m_pMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
                m_pMdl->CalcRootMtx();
                m_pMdl->CalcWorldMatrixAll(0);
            }
            if (m_Unk33C != 0) {
                UpdateLook_();
            }
            m_pMdl->CalcWorldMatrixAll(0);
        }
        UpdateCollAtk_();
        UpdateCollBody_();

        int bOff = 1;
        if (FsmqGetState() == STATE_NEUTRAL) {
            bOff = 0;
        }
        SetStunColor(m_pMdl, bOff);
        SetStunColor(m_pRef, bOff);
        SetStunColor(m_pShadow, bOff);
    }

    float CNpcBehemothBoss::GetParamScale() const
    {
        return 1.0f;
    }

    void CNpcBehemothBoss::GetOffsetPos_(bcs::FVEC3 vOfs, bcs::FVEC3 &rPos)
    {
        bcs::FVEC3 v = vOfs;
        bcs::BCS_Vec_RotY(v, v, m_vRot.y);
        bcs::FVEC3Add(&rPos, &m_vPos, &v);
    }

    // The point 4 in front of the boss, 2 up.
    void CNpcBehemothBoss::GetBasePos_(bcs::FVEC3 &rPos)
    {
        bcs::FVEC3 v(0.0f, 2.0f, 4.0f);
        bcs::BCS_Vec_RotY(v, v, m_vRot.y);
        bcs::FVEC3Add(&rPos, &m_vPos, &v);
    }

    // The distance in the XZ plane from GetBasePos_ to rPos.
    float CNpcBehemothBoss::GetDistFromBasePos_(const bcs::FVEC3 &rPos)
    {
        bcs::FVEC3 vBase;
        GetBasePos_(vBase);
        bcs::FVEC3 vFrom(vBase.x, 0.0f, vBase.z);
        bcs::FVEC3 vTo(rPos.x, 0.0f, rPos.z);
        return bcs::FVEC3Dist(&vFrom, &vTo);
    }

    // The distance in the XZ plane from the boss to rPos.
    float CNpcBehemothBoss::GetDistFromCenterPos_(const bcs::FVEC3 &rPos)
    {
        bcs::FVEC3 vFrom(m_vPos.x, 0.0f, m_vPos.z);
        bcs::FVEC3 vTo(rPos.x, 0.0f, rPos.z);
        return bcs::FVEC3Dist(&vFrom, &vTo);
    }

    // Finds the look-at node and its front (z) in its own space, from the
    // model in its bind pose.
    void CNpcBehemothBoss::SetupLookAtParam_()
    {
        if (m_pMdl != 0) {
            AnimPlay(1, 0, 0.0f, 0.0f);
            m_pMdl->m_vTrans.x = 0.0f;
            m_pMdl->m_vTrans.y = 0.0f;
            m_pMdl->m_vTrans.z = 0.0f;
            m_pMdl->m_fRotX = 0.0f;
            m_pMdl->m_fRotY = 0.0f;
            m_pMdl->m_fRotZ = 0.0f;
            m_pMdl->UpdateFrame(0.0f);
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
            AnimStop();
            m_nLookNode = m_nVNode[2];
            if (m_nLookNode > 0) {
                m_vLookDir.x = 0.0f;
                m_vLookDir.y = 0.0f;
                m_vLookDir.z = 1.0f;
                bcs::FMTX34 mtx;
                m_pMdl->GetNodeWorldMtx(m_nLookNode, mtx);
                PSMTXInverse(mtx.m, mtx.m);
                nw4r::math::VEC3TransformNormal((nw4r::math::VEC3 *)&m_vLookDir, (const nw4r::math::MTX34 *)&mtx,
                                                (const nw4r::math::VEC3 *)&m_vLookDir);
                bcs::BCS_Vec_Normalize(m_vLookDir, m_vLookDir);
            }
        }
    }

    // Turns the head to the target where the state's parameter says so.
    void CNpcBehemothBoss::UpdateLook_()
    {
        bcs::FVEC3 vTarget;
        GetTargetPos(vTarget, 0);
        switch (FsmqGetState()) {
        case STATE_DO_NOTHING:
            SetLookAtEnable_(0x14D);
            break;
        case STATE_SLEEP:
            SetLookAtEnable_(0x151);
            break;
        case STATE_MOVE:
            SetLookAtEnable_(0x164);
            break;
        case STATE_RUSH:
            SetLookAtEnable_(0x17A);
            break;
        case STATE_ATTACK_FANG:
            SetLookAtEnable_(0x192);
            break;
        case STATE_ATTACK_SMASH:
            SetLookAtEnable_(0x1A6);
            break;
        case STATE_ATTACK_CLAW:
            SetLookAtEnable_(0x1BA);
            break;
        case STATE_ATTACK_HORN:
            SetLookAtEnable_(0x1CE);
            break;
        case STATE_ATTACK_TAIL:
            SetLookAtEnable_(0x1E2);
            break;
        case STATE_ATTACK_JUMP:
            SetLookAtEnable_(0x1F6);
            break;
        case STATE_ATTACK_TURN:
            SetLookAtEnable_(0x20C);
            break;
        case STATE_ATTACK_METEOR:
            SetLookAtEnable_(0x223);
            break;
        case STATE_ATTACK_FIGA:
            SetLookAtEnable_(0x23F);
            break;
        case STATE_ATTACK_THUNDERGA:
            SetLookAtEnable_(0x24F);
            break;
        case STATE_ESCAPE_JUMP:
            SetLookAtEnable_(0x170);
            break;
        case STATE_THINK:
            break;
        default:
            m_bLookAt = 0;
            break;
        }
        UpdateLookAt_(m_bLookAt, vTarget, 0.05f);
    }

    // The pitch (x) and the yaw from the body's front (y, in [-pi, pi]) of
    // the direction rDir.
    static inline void GetLookAngle(bcs::FVEC3 &rAngle, const bcs::FVEC3 &rDir, float fRotY)
    {
        rAngle.x = nw4r::math::Atan2Rad(-1.0f * rDir.y, LenXZ(rDir));
        rAngle.y = nw4r::math::Atan2Rad(rDir.x, rDir.z);
        rAngle.z = 0.0f;
        rAngle.y = RoundRad(rAngle.y - fRotY);
        if (rAngle.y > 3.1415927f) {
            rAngle.y -= 6.2831855f;
        }
    }

    // Turns the look-at node towards rPos (bEnable), or back to the front,
    // within 30 degrees of yaw and no pitch.
    void CNpcBehemothBoss::UpdateLookAt_(int bEnable, const bcs::FVEC3 &rPos, float fRate)
    {
        long nNode = m_nLookNode;
        if (nNode > 0) {
            int bWiden = 0;
            // (the rate is worked out and clamped, but not used)
            float fTurn = fRate * m_fUnk40;
            if (!(CGame::g_uPauseBit & 4)) {
                fTurn = fTurn > 1.0f ? 1.0f : (fTurn < 0.0f ? 0.0f : fTurn);
            }

            bcs::FMTX34 world;
            bcs::FMTX34 local;
            m_pMdl->GetNodeWorldMtx(nNode, world);
            m_pMdl->GetNodeLocalMtx(nNode, local);

            // Where the head looks now.
            bcs::FVEC3 vDir;
            nw4r::math::VEC3TransformNormal((nw4r::math::VEC3 *)&vDir, (const nw4r::math::MTX34 *)&world,
                                            (const nw4r::math::VEC3 *)&m_vLookDir);
            bcs::BCS_Vec_Normalize(vDir, vDir);
            bcs::FVEC3 vNow;
            GetLookAngle(vNow, vDir, m_vRot.y);
            if (m_vLookRot.z <= 0.0f) {
                m_vLookRot.x = vNow.x;
                m_vLookRot.y = vNow.y;
                m_vLookRot.z = 3.1415927f;
            }

            // Where it should look.
            bcs::FVEC3 vTo(world.m[0][3], world.m[1][3], world.m[2][3]);
            bcs::FVEC3Sub(&vTo, &rPos, &vTo);
            bcs::BCS_Vec_Normalize(vTo, vTo);
            bcs::FVEC3 vAim;
            GetLookAngle(vAim, vTo, m_vRot.y);

            float fMinX = 0.0f;
            float fMaxX = 0.0f;
            float fMinY = -0.5235988f;
            float fMaxY = 0.5235988f;
            if (bWiden) {
                fMinX = vNow.x > fMinX ? fMinX : vNow.x;
                fMaxX = vNow.x < fMaxX ? fMaxX : vNow.x;
                fMinY = vNow.y > fMinY ? fMinY : vNow.y;
                fMaxY = vNow.y < fMaxY ? fMaxY : vNow.y;
            } else {
                vNow.x = vNow.x > fMaxX ? fMaxX : (vNow.x < fMinX ? fMinX : vNow.x);
                vNow.y = vNow.y > fMaxY ? fMaxY : (vNow.y < fMinY ? fMinY : vNow.y);
            }
            vAim.x = vAim.x > fMaxX ? fMaxX : (vAim.x < fMinX ? fMinX : vAim.x);
            vAim.y = vAim.y > fMaxY ? fMaxY : (vAim.y < fMinY ? fMinY : vAim.y);

            if (bEnable) {
                m_vLookRot.x = 0.0f;
                m_vLookRot.y += (vAim.y - m_vLookRot.y) * 0.03125f;
            } else {
                m_vLookRot.x += -m_vLookRot.x * 0.0625f;
                m_vLookRot.y += -m_vLookRot.y * 0.0625f;
            }
            vAim.x = m_vLookRot.x - vNow.x;
            vAim.y = m_vLookRot.y - vNow.y;

            // The node's local matrix turned by the difference.
            bcs::FMTX34 mtx;
            PSMTXIdentity(mtx.m);
            bcs::FMTX34 rot;
            nw4r::math::MTX34RotXYZFIdx((nw4r::math::MTX34 *)&rot, 0.0f, NW4R_MATH_RAD_TO_FIDX(RoundRad(vAim.y)),
                                        0.0f);
            PSMTXConcat(mtx.m, rot.m, mtx.m);
            PSMTXConcat(mtx.m, local.m, mtx.m);
            mtx.m[0][3] = local.m[0][3];
            mtx.m[1][3] = local.m[1][3];
            mtx.m[2][3] = local.m[2][3];
            m_pMdl->SetNodeLocalMtx(nNode, mtx);
        }
    }

    // The attack units of the attack the state is in, while its motion
    // has them on (m_Unk260; m_Unk264 and m_Unk268 for the two hands of the
    // smash).
    void CNpcBehemothBoss::UpdateCollAtk_()
    {
        if (m_pAtkColl == 0) {
            return;
        }
        m_pAtkColl->SetDisableAll();
        switch (FsmqGetState()) {
        case STATE_ATTACK_TAIL:
            if (m_Unk260 != 0) {
                // The four pieces of the tail.
                float fPower = GetDatF(0x1F3, GetLvI());
                float fRadius = 3.0f;
                for (long i = 0; i < 4; i++) {
                    m_pAtkColl->SetEnable(i + 4);
                    bcs::FVEC3 vFrom;
                    bcs::FVEC3 vTo;
                    GetVNodePos(VNODE_TAIL + i, vFrom);
                    GetVNodePos(VNODE_TAIL + i + 1, vTo);
                    m_pAtkColl->SetPos(i + 4, vFrom, vTo);
                    m_pAtkColl->SetRadius(i + 4, fRadius * GetParamScale());
                }
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        case STATE_ATTACK_JUMP: {
            float x = GetDatF(0x1F7, 0);
            float y = GetDatF(0x1F8, 0);
            float z = GetDatF(0x1F9, 0);
            if (m_Unk260 != 0) {
                float fPower = GetDatF(0x209, GetLvI());
                m_pAtkColl->SetEnable(3);
                m_pAtkColl->SetPos(3, m_vPos);
                m_pAtkColl->SetSize(3, bcs::FVEC3(x * GetParamScale(), y * GetParamScale(), z * GetParamScale()));
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        }
        case STATE_ESCAPE_JUMP: {
            float x = GetDatF(0x171, 0);
            float y = GetDatF(0x172, 0);
            float z = GetDatF(0x173, 0);
            if (m_Unk260 != 0) {
                float fPower = GetDatF(0x177, GetLvI());
                m_pAtkColl->SetEnable(3);
                m_pAtkColl->SetPos(3, m_vPos);
                m_pAtkColl->SetSize(3, bcs::FVEC3(x * GetParamScale(), y * GetParamScale(), z * GetParamScale()));
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        }
        case STATE_RUSH:
            if (m_Unk260 != 0) {
                if (m_Unk25C == 0) {
                    // The head, as a column down to the ground.
                    float fRadius = GetDatF(0x17C, 0);
                    float fPower = GetDatF(0x18C, GetLvI());
                    m_pAtkColl->SetCollType(2, 1);
                    m_pAtkColl->SetEnable(2);
                    bcs::FVEC3 vPos;
                    GetVNodePos(VNODE_HEAD, vPos);
                    vPos.y = 0.0f;
                    m_pAtkColl->SetPos(2, vPos);
                    m_pAtkColl->SetSize(2, bcs::FVEC3(fRadius * GetParamScale(), 4.0f * GetParamScale(),
                                                      fRadius * GetParamScale()));
                    m_pAtkColl->SetAtkPowerAll(fPower);
                } else {
                    float fPower = GetDatF(0x1DF, GetLvI());
                    float fRadius = GetDatF(0x1CF, 0);
                    m_pAtkColl->SetCollType(2, 0);
                    m_pAtkColl->SetEnable(2);
                    bcs::FVEC3 vPos;
                    GetVNodePos(VNODE_HORN, vPos);
                    m_pAtkColl->SetPos(2, vPos);
                    m_pAtkColl->SetRadius(2, fRadius);
                    m_pAtkColl->SetAtkPowerAll(fPower);
                }
            }
            break;
        case STATE_ATTACK_FANG:
            if (m_Unk260 != 0) {
                float fPower = GetDatF(0x1A3, GetLvI());
                float fRadius = GetDatF(0x193, 0);
                m_pAtkColl->SetCollType(2, 0);
                m_pAtkColl->SetEnable(2);
                bcs::FVEC3 vPos;
                GetVNodePos(VNODE_HEAD, vPos);
                m_pAtkColl->SetPos(2, vPos);
                m_pAtkColl->SetRadius(2, fRadius);
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        case STATE_ATTACK_CLAW:
            if (m_Unk260 != 0) {
                float fPower = GetDatF(0x1CB, GetLvI());
                float fRadius = GetDatF(0x1BB, 0);
                m_pAtkColl->SetEnable(0);
                bcs::FVEC3 vPos;
                GetVNodePos(VNODE_HAND_L, vPos);
                m_pAtkColl->SetPos(0, vPos);
                m_pAtkColl->SetRadius(0, fRadius);
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        case STATE_ATTACK_SMASH: {
            float fPower = GetDatF(0x1B7, GetLvI());
            float fRadius = GetDatF(0x1A7, 0);
            bcs::FVEC3 vPos;
            if (m_Unk264 != 0) {
                m_pAtkColl->SetEnable(0);
                GetVNodePos(VNODE_HAND_L, vPos);
                m_pAtkColl->SetPos(0, vPos);
                m_pAtkColl->SetRadius(0, fRadius);
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            if (m_Unk268 != 0) {
                m_pAtkColl->SetEnable(1);
                GetVNodePos(VNODE_HAND_R, vPos);
                m_pAtkColl->SetPos(1, vPos);
                m_pAtkColl->SetRadius(1, fRadius);
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        }
        case STATE_ATTACK_HORN:
            if (m_Unk260 != 0) {
                float fPower = GetDatF(0x1DF, GetLvI());
                float fRadius = GetDatF(0x1CF, 0);
                m_pAtkColl->SetCollType(2, 0);
                m_pAtkColl->SetEnable(2);
                bcs::FVEC3 vPos;
                GetVNodePos(VNODE_HORN, vPos);
                m_pAtkColl->SetPos(2, vPos);
                m_pAtkColl->SetRadius(2, fRadius);
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        case STATE_ATTACK_TURN:
            if (m_Unk260 != 0) {
                float x = GetDatF(0x20D, 0);
                float y = GetDatF(0x20E, 0);
                float z = GetDatF(0x20F, 0);
                float fPower = GetDatF(0x220, GetLvI());
                m_pAtkColl->SetEnable(3);
                m_pAtkColl->SetPos(3, m_vPos);
                m_pAtkColl->SetSize(3, bcs::FVEC3(x, y, z));
                m_pAtkColl->SetAtkPowerAll(fPower);
            }
            break;
        }
    }

    // The body units follow the hips: 0 and 1 the body, 2 to 4 the
    // capsule along it. All off while it stands still or lies as a doll.
    void CNpcBehemothBoss::UpdateCollBody_()
    {
        CBodyCollision *pBody = m_pBodyColl;
        if (pBody != 0) {
            int bEnable = 1;
            long nState = FsmqGetState();
            if (nState == STATE_NEUTRAL || nState == STATE_DOLL) {
                bEnable = 0;
            }
            if (bEnable) {
                pBody->SetEnableAll();
                float fFront = 2.0f;

                bcs::FVEC3 vPos;
                GetVNodePos(VNODE_HIP, vPos);
                bcs::FVEC3 vOfs;
                bcs::BCS_VecZ_RotY(vOfs, fFront, m_vRot.y);
                bcs::FVEC3 vCenter;
                bcs::FVEC3Add(&vCenter, &vPos, &vOfs);
                vPos.x = vCenter.x;
                vPos.z = vCenter.z;
                vPos.y = vCenter.y - 6.0f;
                bcs::FVEC3 vUnused = vCenter;
                m_pBodyColl->SetPos(0, vPos);
                m_pBodyColl->SetYRot(0, m_vRot.y);
                m_pBodyColl->SetPos(1, vPos);
                m_pBodyColl->SetYRot(1, m_vRot.y);

                float fScale = GetParamScale();
                bcs::FVEC3 vFrom(0.0f, fFront * fScale, -4.0f * fScale);
                bcs::FVEC3 vTo(0.0f, fFront * fScale, 4.0f * fScale);
                bcs::BCS_Vec_RotY(vFrom, vFrom, m_vRot.y);
                bcs::BCS_Vec_RotY(vTo, vTo, m_vRot.y);
                GetVNodePos(VNODE_HIP, vPos);
                bcs::BCS_VecZ_RotY(vOfs, fFront, m_vRot.y);
                bcs::FVEC3Add(&vCenter, &vPos, &vOfs);
                vPos.x = vCenter.x;
                vPos.y = vCenter.y - 4.0f;
                vPos.z = vCenter.z;
                bcs::FVEC3Add(&vFrom, &vFrom, &vPos);
                bcs::FVEC3Add(&vTo, &vTo, &vPos);
                vUnused = vCenter;
                m_pBodyColl->SetPos(2, vFrom, vTo);
                m_pBodyColl->SetPos(3, vFrom, vTo);
                m_pBodyColl->SetPos(4, vFrom, vTo);
            } else {
                pBody->SetDisableAll();
            }
        }
    }

    float CNpcBehemothBoss::AnimSearchTrgFrame(long nMot, unsigned long nTrg, float fFrame) const
    {
        if (m_pAnmMot != 0) {
            return m_pAnmMot->SearchTrgFrame(nMot, nTrg, fFrame);
        }
        return -1.0f;
    }

    // The frame of trigger 2 (the ball leaves the hand) of motion nMot;
    // fDefault when the motion has none.
    float CNpcBehemothBoss::GetBallShotTrgFrame_(long nMot, float fDefault) const
    {
        float fFrame = AnimSearchTrgFrame(nMot, 2, 0.0f);
        int bFound = fFrame >= 0.0f;
        if (!bFound) {
            fFrame = fDefault;
        }
        return fFrame;
    }

    // Goes straight on to thinking.
    void CNpcBehemothBoss::CtorStateDefault()
    {
        m_Unk33C = 1;
        FsmqChangeState(STATE_THINK);
        FsmqRetry();
    }

    void CNpcBehemothBoss::CtorStateSleep()
    {
        m_nSubStep = 0;
        AnimStop();
    }

    void CNpcBehemothBoss::LoopStateSleep()
    {
    }

    void CNpcBehemothBoss::DtorStateSleep()
    {
    }

    void CNpcBehemothBoss::CtorStateStartWarCry()
    {
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        m_nSubStep = 0;
        pWork->m_nStep = 0;
        pWork->m_fWait = 150.0f;
    }

    // The war cry at the start of the game: a roar with a blur of the
    // screen for the frames of the state work.
    void CNpcBehemothBoss::LoopStateStartWarCry()
    {
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        switch (pWork->m_nStep) {
        case 0:
            AnimPlay(0x17, 0, 8.0f, 0.0f);
            pWork->m_nStep++;
            break;
        case 1:
            if (IsAnimEnd() || AnimGetFrame() >= AnimGetFrameNum() - 40.0f) {
                PlaySe(0xDF9, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (IsAnimEnd()) {
                StartPadVibration(7, 8);
                AnimPlay(0x18, 1, 0.0f, 0.0f);
                Eff_CreateAttachAuto(8, 0x62, 3);
                StartCameraQuake(0x19);
                new CGameBlurEffect(CGame::GetTempParent(), 0.0f, pWork->m_fWait - 32.0f, 32.0f, 2.0f,
                                    bcs::BCS_Color(0xFF, 0xFF, 0xFF, 0xA0));
                pWork->m_nStep++;
            }
            break;
        case 3:
            pWork->m_fWait -= GetFrameRateF();
            if (pWork->m_fWait <= 0.0f) {
                Eff_Stop(8);
                AnimPlay(0x19, 0, 0.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        default:
            if (IsAnimEnd()) {
                FsmqDoChain();
            }
            break;
        }
    }

    void CNpcBehemothBoss::DtorStateStartWarCry()
    {
        Eff_Stop(8);
    }

    void CNpcBehemothBoss::CtorStateAttackMeteor()
    {
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        m_nSubStep = 0;
        pWork->m_nStep = 0;
    }

    // Meteors: the magic builds up over four motions, then they fall.
    void CNpcBehemothBoss::LoopStateAttackMeteor()
    {
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        switch (pWork->m_nStep) {
        case 0:
            AnimPlay(0xE, 0, 16.0f, 0.0f);
            SetColorAnim_(6);
            StartCameraQuake(0x1B);
            SE_PlayPos(0, 0xDF5, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_nStep++;
            break;
        case 1:
            if (IsAnimEnd()) {
                AnimPlay(0xF, 0, 0.0f, 0.0f);
                SetColorAnim_(7);
                Eff_CreateAttachAuto(8, 0x6C, 3);
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (IsAnimEnd()) {
                AnimPlay(0x10, 0, 0.0f, 0.0f);
                SetColorAnim_(8);
                pWork->m_nStep++;
            }
            break;
        case 3:
            if (IsAnimEnd()) {
                AnimPlay(0x11, 0, 0.0f, 0.0f);
                SetColorAnim_(9);
                Eff_Stop(8);
                Eff_CreateAttachAuto(9, 0x6D, 3);
                pWork->m_nStep++;
            }
            break;
        case 4:
            if (IsAnimEnd()) {
                AnimPlay(0x12, 0, 0.0f, 0.0f);
                SetColorAnim_(10);
                ShotMeteor_();
                pWork->m_nStep++;
            }
            break;
        default:
            if (IsAnimEnd()) {
                CGame::StopCameraQuake(0x1B);
                SE_Stop(0, 0.0f);
                SetStopAttackTime(GetRandValueF(GetDatF(0x22D, GetLvI()), GetDatF(0x230, GetLvI())));
                ExitNowStatus(GetRandValueF(GetDatF(0x227, GetLvI()), GetDatF(0x22A, GetLvI())));
            }
            break;
        }
    }

    void CNpcBehemothBoss::DtorStateAttackMeteor()
    {
        SetColorAnim_(10);
        Eff_Stop(8);
        SE_Stop(0, 0.0f);
    }

    void CNpcBehemothBoss::CtorStateDamage()
    {
        DAMAGE_WORK *pWork = FsmqGetWork<DAMAGE_WORK>();
        m_nSubStep = 0;
        m_Unk260 = 0;
        m_Unk264 = 0;
        m_Unk268 = 0;
        pWork->m_nStep = 0;
        switch (pWork->m_nKind) {
        case 1:
            AnimPlay(0x14, 0, 8.0f, 0.0f);
            PlaySePos(0xDF0, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            break;
        case 2:
            AnimPlay(0x13, 0, 8.0f, 0.0f);
            AnimSetSpeed(1.2f);
            PlaySePos(0xDF0, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            break;
        default:
            AnimPlay(0x13, 0, 8.0f, 0.0f);
            PlaySePos(0xDF0, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            break;
        }
    }

    void CNpcBehemothBoss::LoopStateDamage()
    {
        if (IsAnimEnd()) {
            FsmqChangeState(m_Unk2D8);
        }
    }

    void CNpcBehemothBoss::DtorStateDamage()
    {
        SetStopAttackTime(0.0f);
        ResetNeglect();
    }

    void CNpcBehemothBoss::CtorStateKO()
    {
        KO_WORK *pWork = FsmqGetWork<KO_WORK>();
        m_nSubStep = 0;
        pWork->m_nStep = 0;
        pWork->m_vVel.x = 0.0f;
        pWork->m_vVel.z = 0.0f;
        pWork->m_vVel.y = -1.6666667f;
    }

    // Knocked out: it drops to the ground, falls over and lies there.
    void CNpcBehemothBoss::LoopStateKO()
    {
        KO_WORK *pWork = FsmqGetWork<KO_WORK>();
        switch (pWork->m_nStep) {
        case 0:
            CGame::SendMessage(99, 3, 1, 0);
            pWork->m_nStep++;
        case 1: {
            bcs::FVEC3 vPos = m_vPos;
            if (vPos.y > 0.0f) {
                bcs::FVEC3 vNew(0.0f, 0.0f, 0.0f);
                float fFrame = GetFrameRateF();
                float fGravity = 0.0f;
                float fAcc = fGravity * fFrame;
                bcs::FVEC3 vMove;
                bcs::FVEC3Scale(&vMove, &pWork->m_vVel, fFrame);
                pWork->m_vVel.y += fAcc;
                vMove.y += 0.5f * (fAcc * fFrame);
                bcs::FVEC3Add(&vNew, &vNew, &vMove);
                bcs::FVEC3Add(&vNew, &vNew, &m_vPos);
                if (vNew.y <= 0.0f) {
                    vNew.y = 0.0f;
                    AnimPlay(0x21, 0, 8.0f, 0.0f);
                    StartCameraQuake(0x3F);
                    pWork->m_nStep++;
                }
                m_vPos = vNew;
            } else {
                pWork->m_nStep = 3;
            }
            break;
        }
        case 2:
            if (IsAnimEnd()) {
                pWork->m_nStep++;
            }
            break;
        case 3:
            AnimPlay(0x15, 0, 8.0f, 0.0f);
            PlaySePos(0xDF4, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_nStep++;
            break;
        case 4:
            if (IsAnimEnd() || AnimGetFrame() >= 130.0f) {
                PlaySePos(0xDF3, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                StartCameraQuake(0x23);
                pWork->m_nStep++;
            }
        case 5:
            if (IsAnimEnd()) {
                AnimPlay(0x16, 1, 0.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        }
    }

    void CNpcBehemothBoss::DtorStateKO()
    {
    }

    void CNpcBehemothBoss::CtorStateDoll()
    {
        m_nSubStep = 0;
    }

    void CNpcBehemothBoss::LoopStateDoll()
    {
    }

    void CNpcBehemothBoss::DtorStateDoll()
    {
    }

    void CNpcBehemothBoss::CtorStateArtsEvent()
    {
        ARTS_EVENT_WORK *pWork = FsmqGetWork<ARTS_EVENT_WORK>();
        m_nSubStep = 0;
        m_Unk2BC = 0;
        SetStickDamageCount();
        m_Unk2AC = 1;
        m_Unk368 = 0;
        pWork->m_nStep = 0;
        pWork->m_fWait = 150.0f;
        pWork->m_nCount = 0;
    }

    // The players' arts attack: it roars, then waits for the result
    // (m_nCount, sent with message 0x24); a hit does the special damage.
    void CNpcBehemothBoss::LoopStateArtsEvent()
    {
        ARTS_EVENT_WORK *pWork = FsmqGetWork<ARTS_EVENT_WORK>();
        switch (pWork->m_nStep) {
        case 0:
            AnimPlay(0x17, 0, 8.0f, 0.0f);
            pWork->m_nStep++;
            break;
        case 1:
            if (IsAnimEnd() || AnimGetFrame() >= AnimGetFrameNum() - 40.0f) {
                PlaySe(0xDF9, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (IsAnimEnd()) {
                AnimPlay(0x18, 1, 0.0f, 0.0f);
                new (0x100) CGameBlurEffect(CGame::GetTempParent(), 0.0f, pWork->m_fWait - 32.0f, 32.0f, 2.0f,
                                            bcs::BCS_Color(0xFF, 0xFF, 0xFF, 0xA0));
                pWork->m_nStep++;
            }
            break;
        case 3:
            pWork->m_fWait -= GetFrameRateF();
            if (pWork->m_fWait <= 0.0f) {
                Eff_Stop(8);
                AnimPlay(0x19, 0, 0.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        case 4:
            // Waiting for the result.
            if (pWork->m_nCount == 0) {
                if (IsAnimEnd()) {
                    AnimPlay(1, 1, 0.0f, 0.0f);
                    pWork->m_nStep++;
                }
                break;
            }
        case 5:
            if (pWork->m_nCount > 0) {
                AnimPlay(0x14, 0, 0.0f, 0.0f);
                if (pWork->m_nCount > 1) {
                    DoDamage_(0x1B01, 0, m_vPos);
                    pWork->m_nStep++;
                } else {
                    pWork->m_nCount = 0;
                    pWork->m_nStep = 4;
                }
            }
            break;
        case 6:
            if (IsAnimEnd()) {
                AnimPlay(1, 0, 8.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        default:
            if (IsAnimEnd()) {
                FsmqDoChain();
            }
            break;
        }
    }

    void CNpcBehemothBoss::DtorStateArtsEvent()
    {
        Eff_Stop(8);
        m_nSubStep = 1;
        m_Unk2AC = 0;
    }

    // The meteors of LoopStateAttackMeteor: they start above the boss and
    // fall around a point in front of it, the first one (by chance) on
    // the character it is after.
    void CNpcBehemothBoss::ShotMeteor_()
    {
        long nNum = GetDatI(0x17, GetLvI());
        float fWait = 0.0f;
        int bAimed = 0;
        bcs::FVEC3 vAim(0.0f, 0.0f, 0.0f);
        if (m_Unk258 >= 0 && LotRate() < GetDatI(0x233, GetLvI())) {
            CChrCore *pChr = CChrMan::GetChrCore(m_Unk258);
            if (pChr != 0) {
                vAim = pChr->GetPos();
                bAimed = 1;
            }
        }
        float fDist = GetDatF(0x236, GetLvI());
        float fMin = GetDatF(0x239, GetLvI());
        float fMax = GetDatF(0x23C, GetLvI());
        for (long i = 0; i < nNum; i++) {
            bcs::FVEC3 vFrom = m_vPos;
            bcs::FVEC3 vOfs;
            float fRot = RoundRad(bcs::BCS_RandMax(6.2831855f));
            bcs::BCS_VecZ_RotY(vOfs, bcs::BCS_RandRange(2.0f, 5.0f), fRot);
            vOfs.y = bcs::BCS_RandRange(20.0f, 25.0f);
            bcs::FVEC3Add(&vFrom, &vFrom, &vOfs);

            bcs::FVEC3 vTo;
            if (bAimed == 1 && i == 0) {
                vTo = vAim;
            } else {
                vTo = m_vPos;
                bcs::BCS_VecZ_RotY(vOfs, fDist, m_vRot.y);
                vTo.x += vOfs.x;
                vTo.z += vOfs.z;
                fRot = RoundRad(bcs::BCS_RandMax(6.2831855f));
                bcs::BCS_VecZ_RotY(vOfs, GetRandValueF(fMin, fMax), fRot);
                vTo.x += vOfs.x;
                vTo.z += vOfs.z;
            }
            bcs::FVEC3 vDir;
            bcs::FVEC3Sub(&vDir, &vTo, &vFrom);
            bcs::BCS_Vec_Normalize(vDir, vDir);
            fWait += bcs::BCS_RandMax(20L) + 20;
            new (0x100) CNpcBehemothMeteor(CGame::GetTempParent(), m_pMdl->GetHandle(), vFrom, vDir, fWait, GetLvI());
        }
    }

    // A Figa fire ball from the mouth at every character still in the game.
    void CNpcBehemothBoss::ShotFiga_()
    {
        bcs::FMTX34 mtx;
        m_pMdl->GetScnMtxPos(&mtx, m_nVNode[9], bcs::CScnObj::SCNOBJMTXTYPE_1);
        bcs::FVEC3 vPos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        bcs::FVEC3 vTarget(0.0f, 0.0f, 20.0f);
        bcs::BCS_Vec_RotY(vTarget, vTarget, m_vRot.y);
        vTarget.x += m_vPos.x;
        vTarget.z += m_vPos.z;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable() || pChr->IsVsBossDead()) {
                continue;
            }
            CNpcBehemothFiga *pFiga = new (0x100) CNpcBehemothFiga(CGame::GetTempParent());
            if (pFiga != 0) {
                pFiga->Create(m_pMdl->GetHandle(), vPos, vTarget, pChr, GetLvI());
            }
        }
    }

    void CNpcBehemothBoss::ShotThunderga_()
    {
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable() || pChr->IsVsBossDead()) {
                continue;
            }
            bcs::FVEC3 vPos = pChr->GetPos();
            vPos.y = 0.0f;
            bcs::FVEC3 vDir;
            bcs::FVEC3Sub(&vDir, &vPos, &m_vPos);
            bcs::FVEC3 vXZ;
            vXZ.Set(vDir.x, 0.0f, vDir.z);
            bcs::BCS_Vec_Normalize(vDir, vXZ);
            new (0x100) CNpcBehemothThunderga(CGame::GetTempParent(), vPos, vDir, GetLvI());
        }
        bcs::CMainApp::Start3DFade(1, 0.0f);
        bcs::CMainApp::Start3DFade(4, 0.0625f);
    }
}
