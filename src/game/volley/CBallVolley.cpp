#include "game/msw/CGameTask.hpp"
#include "game/msw/CBallVolley.hpp"
#include "game/msw/BallVolleySpikeShotArg.hpp"
#include "game/msw/CBallInline.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrParasite.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CItemBall.hpp"
#include "game/msw/CItemBallVolleyKinds.hpp"
#include "game/msw/CStageMan.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmObj.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"
#include "nw4r/math/equation.h"
#include "nw4r/math/types.h"

#include <math.h>

// The ball of the volley game: msw::CBallVolley, the ball against a boss
// (msw::CBallVolleyVsBoss), the balls of the rhythm receive mission
// (msw::CBallVolleyRhythmReceive), the constructor of the spike parameters
// (msw::BALL::VOLLEY::SPIKE_SHOT_ARG) and the small tasks the ball starts
// (BALL_VOLLEY::CChrParasiteBanana, CBallTouchDownMark,
// CBallTouchDownMdlEffect, CBallVolleyRRReceiveEffect).

namespace msw {
    namespace BALL_VOLLEY {
        // The mark on the floor where the ball touched down (an effect
        // that drifts with the floor flow). Size 0x50.
        class CBallTouchDownMark : public CGameTask {
            public:
                CBallTouchDownMark(bcs::CTask *pParent, long nEffect, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel)
                    : CGameTask(pParent, 0x16), m_hEffect(0)
                {
                    m_Pos.Set(rPos.x, 0.0f, rPos.z);
                    float yaw = bcs::ROUND_PI(nw4r::math::Atan2Rad(-rVel.x, -rVel.z));
                    m_Rot.Set(0.0f, yaw, 0.0f);
                    unsigned long hEffect = CEffectCtrl::CreateAttachEffect(nEffect, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0);
                    m_hEffect = hEffect;
                    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                    if (pCluster != 0) {
                        pCluster->SetTranslate(&m_Pos);
                        pCluster->SetRotateRad(&m_Rot);
                    }
                }
                virtual ~CBallTouchDownMark();
                virtual void Main();

                unsigned long GetHandle() const { return m_hEffect; }

            private:
                bcs::FVEC3 m_Pos;          // 0x34
                bcs::FVEC3 m_Rot;          // 0x40
                unsigned long m_hEffect;   // 0x4C
        };


        // A model effect where a special shot touched down; it deletes
        // itself once every animation of its models has ended. Size 0x44.
        class CBallTouchDownMdlEffect : public CGameTask {
            public:
                CBallTouchDownMdlEffect(bcs::CTask *pParent, long nEffect, const bcs::FVEC3 &rPos,
                                        const bcs::FVEC3 &rVel)
                    : CGameTask(pParent, 0x16), m_hEffect(0)
                {
                    m_Pos.x = rPos.x;
                    m_Pos.y = 0.0f;
                    m_Pos.z = rPos.z;
                    m_hEffect = CEffectCtrl::CreateAttachEffect(nEffect, 0, 0, 0, 1, 1, 0, 3, 0, 0, 0);
                    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(m_hEffect);
                    if (pCluster != 0) {
                        pCluster->SetTranslate(&m_Pos);
                        bcs::FVEC3 rot(0.0f, 0.0f, 0.0f);
                        rot.y = bcs::ROUND_PI(nw4r::math::Atan2Rad(-rVel.x, -rVel.z));
                        pCluster->SetRotateRad(&rot);
                    }
                }
                virtual ~CBallTouchDownMdlEffect();
                virtual void Main();

                unsigned long GetHandle() const { return m_hEffect; }

            private:
                bcs::FVEC3 m_Pos;          // 0x34
                unsigned long m_hEffect;   // 0x40
        };

        // Plays the receive sound of a ball of the rhythm receive mission on
        // its beat. Size 0x44.
        class CBallVolleyRRReceiveEffect : public CGameTask {
            public:
                CBallVolleyRRReceiveEffect(bcs::CTask *pParent, long nSe, float fBeat, int bGood, long nSe2)
                    : CGameTask(pParent, -1), m_nSe(nSe), m_fBeat(fBeat), m_bGood(bGood), m_nSe2(nSe2)
                {
                    CheckPlay_();
                }
                virtual ~CBallVolleyRRReceiveEffect();
                virtual void Main();

            private:
                void CheckPlay_();

                long m_nSe;       // 0x34
                float m_fBeat;    // 0x38
                int m_bGood;      // 0x3C
                long m_nSe2;      // 0x40
        };


        // (meaning guessed) The attack collision a character carries for
        // a moment after a banana item hit it.
        class CChrParasiteBanana : public CC_CMN::CChrParasite {
            public:
                CChrParasiteBanana(bcs::CTask *pParent, long nChr);
                virtual ~CChrParasiteBanana();
                virtual void MainFunc();

            private:
                CAtkCollision m_AtkColl; // 0x40
        };

    }
}

namespace {

    // Tells every game camera where the ball is heading.
    inline void SetCameraBallTargetPos(int bFlag, const bcs::FVEC3 &rPos)
    {
        long n = msw::CGameCamera::GetInstanceNum();
        for (long i = 0; i < n; i++) {
            msw::CGameCamera *pCamera = msw::CGameCamera::GetInstance(i);
            if (pCamera != 0) {
                pCamera->SetBallTargetPos(bFlag, rPos);
            }
        }
    }

    // rOut = rIn flattened onto the XZ plane and scaled to length 1 (zero
    // for a vertical rIn).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::FVEC3 xz(rIn.x, 0.0f, rIn.z);
        bcs::BCS_Vec_Normalize(rOut, xz);
    }

    // The trail effect of each kind of serve (SERVE_SHOT_ARG::m_nType).
    const long s_aServeTailEffect[] = {
        0x4DA, 0x4E2, 0x4E4, 0,
    };

    // The trail effect of a pass, by (guessed) its height and its kind.
    const long s_aPassTailEffect[2][3] = {
        {0x4DA, 0x4DC, 0x4DD},
        {0x4DA, 0x4DD, 0x4DD},
    };

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    // deg in radians.
    inline float DegToRad(float deg)
    {
        return 0.017453292f * deg;
    }

    // The colours of the special shot effects (filled in by the static
    // initialiser).
    msw::CBallVolley::ARTS_EFCT_TBL::EFCT_COL_SET s_aColSet[3] = {
        {0x19,
         {bcs::BCS_Color(0x00, 0x00, 0xB4, 0xFF), bcs::BCS_Color(0x32, 0xAF, 0xFF, 0xFF), bcs::BCS_Color(0x00000000),
          bcs::BCS_Color(0x00000000), bcs::BCS_Color(0x32, 0xFF, 0xFF, 0xFF), bcs::BCS_Color(0x00, 0x46, 0xFF, 0xFF),
          bcs::BCS_Color(0x00000000)}},
        {0x19,
         {bcs::BCS_Color(0xFF, 0x00, 0x00, 0xFF), bcs::BCS_Color(0xFF, 0xA5, 0x3C, 0xFF), bcs::BCS_Color(0x00000000),
          bcs::BCS_Color(0x00000000), bcs::BCS_Color(0xFF, 0x78, 0x32, 0xFF), bcs::BCS_Color(0xE6, 0x5F, 0x00, 0xFF),
          bcs::BCS_Color(0x00000000)}},
        {0x19,
         {bcs::BCS_Color(0x5F, 0x5F, 0x5F, 0xFF), bcs::BCS_Color(0x91, 0x91, 0x91, 0xFF), bcs::BCS_Color(0x00000000),
          bcs::BCS_Color(0x00000000), bcs::BCS_Color(0xC8, 0xC8, 0xC8, 0xFF), bcs::BCS_Color(0x69, 0x69, 0x69, 0xFF),
          bcs::BCS_Color(0x00000000)}},
    };

    // The TEV colour 0 and K colour 0 of the balls of the rhythm receive
    // mission, by colour (filled in by the static initialiser).
    GXColor s_aRRTevColor[7] = {
        bcs::BCS_Color(0x0064FFFF), bcs::BCS_Color(0xFF1E1EFF), bcs::BCS_Color(0x00FF00FF), bcs::BCS_Color(0xFFFF00FF),
        bcs::BCS_Color(0xFFFFFFFF), bcs::BCS_Color(0xFFFFFFFF), bcs::BCS_Color(0xFFFFFFFF),
    };
    GXColor s_aRRKColor[7] = {
        bcs::BCS_Color(0x1E1E46FF), bcs::BCS_Color(0x503232FF), bcs::BCS_Color(0x1E1E78FF), bcs::BCS_Color(0x3C1E1EFF),
        bcs::BCS_Color(0xFFFFFFFF), bcs::BCS_Color(0xFFFFFFFF), bcs::BCS_Color(0xFFFFFFFF),
    };

    // The effects of the special shots, per phase (ARTS_EFCT_TBL::m_aEfct).
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct270[] = {
        {0x4e6, 0x0, 0},
        {0x4e7, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct288[] = {
        {-1, 0x0, 0},
        {0x4e6, 0x0, 0},
        {0x4e7, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct2B0[] = {
        {0x4e6, 0x0, 0},
        {0x4e7, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct2C8[] = {
        {0x55a, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct2D4[] = {
        {0x55b, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct2E0[] = {
        {0x565, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct2EC[] = {
        {0x56c, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct2F8[] = {
        {0x579, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct304[] = {
        {0x585, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct310[] = {
        {0x590, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct31C[] = {
        {0x5a0, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct328[] = {
        {0x5aa, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct334[] = {
        {0x5b3, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct340[] = {
        {0x5bf, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct34C[] = {
        {0x5cc, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct358[] = {
        {0x5d6, 0x0, 0},
        {0x5de, 0x4, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct370[] = {
        {0x5e5, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct37C[] = {
        {0x5f1, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct388[] = {
        {0x5fc, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct394[] = {
        {0x608, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct3A0[] = {
        {0x619, 0x0, &s_aColSet[0]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct3AC[] = {
        {0x619, 0x0, &s_aColSet[1]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct3B8[] = {
        {0x619, 0x0, &s_aColSet[2]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct3C4[] = {
        {0x631, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct3D0[] = {
        {0x63b, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct3DC[] = {
        {0x644, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct3E8[] = {
        {0x501, 0x8, 0},
        {0x558, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct400[] = {
        {0x500, 0x8, 0},
        {0x566, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct418[] = {
        {0x500, 0x8, 0},
        {0x572, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct430[] = {
        {0x4ff, 0x8, 0},
        {0x57c, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct448[] = {
        {0x4ff, 0x8, 0},
        {0x587, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct460[] = {
        {0x4ff, 0x8, 0},
        {0x591, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct478[] = {
        {0x501, 0x8, 0},
        {0x5a1, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct490[] = {
        {0x501, 0x8, 0},
        {0x5ab, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct4A8[] = {
        {0x500, 0x8, 0},
        {0x5b4, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct4C0[] = {
        {0x500, 0x8, 0},
        {0x5c0, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct4D8[] = {
        {0x501, 0x8, 0},
        {0x5cf, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct4F0[] = {
        {0x501, 0x8, 0},
        {0x5d7, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct508[] = {
        {0x500, 0x8, 0},
        {0x5e9, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct520[] = {
        {0x500, 0x8, 0},
        {0x5f2, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct538[] = {
        {0x500, 0x8, 0},
        {0x5fd, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct550[] = {
        {0x501, 0x8, 0},
        {0x60a, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct568[] = {
        {0x500, 0x8, 0},
        {0x620, 0x0, &s_aColSet[0]},
        {0x623, 0x10, &s_aColSet[0]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct58C[] = {
        {0x500, 0x8, 0},
        {0x620, 0x0, &s_aColSet[1]},
        {0x623, 0x10, &s_aColSet[1]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct5B0[] = {
        {0x500, 0x8, 0},
        {0x620, 0x0, &s_aColSet[2]},
        {0x623, 0x10, &s_aColSet[2]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct5D8[] = {
        {0x501, 0x8, 0},
        {0x632, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct5F0[] = {
        {0x501, 0x8, 0},
        {0x63c, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct608[] = {
        {0x501, 0x8, 0},
        {0x645, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct620[] = {
        {0x55d, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct62C[] = {
        {0x569, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct638[] = {
        {0x571, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct644[] = {
        {0x57a, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct650[] = {
        {0x589, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct65C[] = {
        {0x593, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct668[] = {
        {0x5a3, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct674[] = {
        {0x5ad, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct680[] = {
        {0x5b6, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct68C[] = {
        {0x5c2, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct698[] = {
        {0x5d1, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6A4[] = {
        {0x5d9, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6B0[] = {
        {0x5eb, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6BC[] = {
        {0x5f4, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6C8[] = {
        {0x5ff, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6D4[] = {
        {0x60b, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6E0[] = {
        {0x622, 0x0, &s_aColSet[0]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6EC[] = {
        {0x622, 0x0, &s_aColSet[1]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct6F8[] = {
        {0x622, 0x0, &s_aColSet[2]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct704[] = {
        {0x634, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct710[] = {
        {0x63e, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct71C[] = {
        {0x647, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct728[] = {
        {0x559, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct734[] = {
        {0x567, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct740[] = {
        {0x573, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct74C[] = {
        {0x57d, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct758[] = {
        {0x588, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct764[] = {
        {0x592, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct770[] = {
        {0x5a2, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct77C[] = {
        {0x5ac, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct788[] = {
        {0x5b5, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct794[] = {
        {0x5c1, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7A0[] = {
        {0x5d0, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7AC[] = {
        {0x5d8, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7B8[] = {
        {0x5ea, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7C4[] = {
        {-1, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7D0[] = {
        {0x5fe, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7DC[] = {
        {-1, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7E8[] = {
        {0x621, 0x0, &s_aColSet[0]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct7F4[] = {
        {0x621, 0x0, &s_aColSet[1]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct800[] = {
        {0x621, 0x0, &s_aColSet[2]},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct80C[] = {
        {0x633, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct818[] = {
        {0x63d, 0x0, 0},
    };
    const msw::CBallVolley::ARTS_EFCT_TBL::EFCT s_aEfct824[] = {
        {0x646, 0x0, 0},
    };

    // The effects and reactions of each special shot (GetArtsEffectTbl_);
    // kind 0x10 has two more variants, kinds 0x13 to 0x18 share two.
    const msw::CBallVolley::ARTS_EFCT_TBL s_aArtsEfctTbl[0x13] = {
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct2C8}, {1, s_aEfct2D4},
             {2, s_aEfct3E8}, {1, s_aEfct620}, {1, s_aEfct728}},
            0x0, 0x2800, 0x5, 0x559, 0x558, 2.0f,
            {0x3ea, 0x3eb, 0x3ec, 0x3ed, 0x3ec, 0x3ec},
            {0xf, 0xf, 0xf},
        },
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct2E0}, {1, s_aEfct2E0},
             {2, s_aEfct400}, {1, s_aEfct62C}, {1, s_aEfct734}},
            0x0, 0x0, 0x5, 0x567, 0x566, 2.0f,
            {0x3f3, 0x3f4, 0x3f5, 0x3f6, 0x3f5, 0x3f6},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct2EC}, {1, s_aEfct2EC},
             {2, s_aEfct418}, {1, s_aEfct638}, {1, s_aEfct740}},
            0x0, 0x0, 0x5, 0x573, 0x572, 2.0f,
            {0x3fc, 0x3fd, 0x3fe, 0x3ff, 0x3fe, 0x3ff},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct2F8}, {1, s_aEfct2F8},
             {2, s_aEfct430}, {1, s_aEfct644}, {1, s_aEfct74C}},
            0x0, 0x0, 0x5, 0x57d, 0x57c, 2.0f,
            {0x404, 0x405, 0x406, 0x407, 0x406, 0x407},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct304}, {1, s_aEfct304},
             {2, s_aEfct448}, {1, s_aEfct650}, {1, s_aEfct758}},
            0x0, 0x0, 0x5, 0x588, 0x587, 2.0f,
            {0x40d, 0x40e, 0x40f, 0x410, 0x40f, 0x410},
            {-1, -1, -1},
        },
        {
            {{3, s_aEfct288}, {2, s_aEfct2B0}, {1, s_aEfct310}, {1, s_aEfct310},
             {2, s_aEfct460}, {1, s_aEfct65C}, {1, s_aEfct764}},
            0x0, 0x0, 0x5, 0x592, 0x591, 2.0f,
            {0x415, 0x416, 0x417, 0x418, 0x417, 0x418},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct31C}, {1, s_aEfct31C},
             {2, s_aEfct478}, {1, s_aEfct668}, {1, s_aEfct770}},
            0x0, 0x0, 0x5, 0x5a2, 0x5a1, 2.0f,
            {0x41f, 0x420, 0x421, 0x422, 0x421, 0x422},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct328}, {1, s_aEfct328},
             {2, s_aEfct490}, {1, s_aEfct674}, {1, s_aEfct77C}},
            0x0, 0x0, 0x5, 0x5ac, 0x5ab, 2.0f,
            {0x428, 0x429, 0x42a, 0x42b, 0x42a, 0x42b},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct334}, {1, s_aEfct334},
             {2, s_aEfct4A8}, {1, s_aEfct680}, {1, s_aEfct788}},
            0x0, 0x0, 0x5, 0x5b5, 0x5b4, 2.0f,
            {0x432, 0x433, 0x434, 0x435, 0x434, 0x435},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct340}, {1, s_aEfct340},
             {2, s_aEfct4C0}, {1, s_aEfct68C}, {1, s_aEfct794}},
            0x0, 0x0, 0x5, 0x5c1, 0x5c0, 2.0f,
            {0x43c, 0x43d, 0x43e, 0x43f, 0x43e, 0x43f},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct34C}, {1, s_aEfct34C},
             {2, s_aEfct4D8}, {1, s_aEfct698}, {1, s_aEfct7A0}},
            0x0, 0x2800, 0x5, 0x5d0, 0x5cf, 2.0f,
            {0x447, 0x448, 0x449, 0x44a, 0x449, 0x449},
            {0x11, 0x11, 0x11},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {2, s_aEfct358}, {2, s_aEfct358},
             {2, s_aEfct4F0}, {1, s_aEfct6A4}, {1, s_aEfct7AC}},
            0x1000, 0x1000, 0x5, 0x5d8, 0x5d7, 2.0f,
            {0x451, 0x452, 0x453, 0x454, 0x453, 0x454},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct370}, {1, s_aEfct370},
             {2, s_aEfct508}, {1, s_aEfct6B0}, {1, s_aEfct7B8}},
            0x0, 0x0, 0x5, 0x5ea, 0x5e9, 2.0f,
            {0x45c, 0x45d, 0x45e, 0x4d3, 0x45e, 0x4d3},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct37C}, {1, s_aEfct37C},
             {2, s_aEfct520}, {1, s_aEfct6BC}, {1, s_aEfct7C4}},
            0x0, 0x0, 0x5, 0x5f2, 0x5f2, 2.0f,
            {0x463, 0x464, 0x465, 0x466, 0x465, 0x466},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct388}, {1, s_aEfct388},
             {2, s_aEfct538}, {1, s_aEfct6C8}, {1, s_aEfct7D0}},
            0x0, 0x0, 0x5, 0x5fe, 0x5fd, 2.0f,
            {0x472, 0x473, 0x474, 0x475, 0x474, 0x475},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct394}, {1, s_aEfct394},
             {2, s_aEfct550}, {1, s_aEfct6D4}, {1, s_aEfct7DC}},
            0x0, 0x0, 0x5, -1, 0x60a, 2.0f,
            {0x4e4, 0x47a, 0x47b, 0x47c, 0x47b, 0x47c},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct3A0}, {1, s_aEfct3A0},
             {3, s_aEfct568}, {1, s_aEfct6E0}, {1, s_aEfct7E8}},
            0x0, 0x0, 0x5, 0x621, 0x620, 2.0f,
            {0x4e9, 0x482, 0x483, 0x484, 0x483, 0x484},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct3C4}, {1, s_aEfct3C4},
             {2, s_aEfct5D8}, {1, s_aEfct704}, {1, s_aEfct80C}},
            0x0, 0x0, 0x5, 0x633, 0x632, 2.0f,
            {0x4f0, 0x48a, 0x48b, 0x48c, 0x48b, 0x48c},
            {-1, -1, -1},
        },
        {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0},
             {0, 0}, {0, 0}, {0, 0}},
            0x0, 0x0, 0x6, -1, -1, 2.0f,
            {-1, -1, -1, -1, -1, -1},
            {-1, -1, -1},
        },
    };
    const msw::CBallVolley::ARTS_EFCT_TBL s_aArtsEfctTbl10[2] = {
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct3AC}, {1, s_aEfct3AC},
             {3, s_aEfct58C}, {1, s_aEfct6EC}, {1, s_aEfct7F4}},
            0x0, 0x0, 0x5, 0x621, 0x620, 2.0f,
            {0x4e9, 0x482, 0x483, 0x484, 0x483, 0x484},
            {-1, -1, -1},
        },
        {
            {{2, s_aEfct270}, {0, 0}, {1, s_aEfct3B8}, {1, s_aEfct3B8},
             {3, s_aEfct5B0}, {1, s_aEfct6F8}, {1, s_aEfct800}},
            0x0, 0x0, 0x5, 0x621, 0x620, 2.0f,
            {0x4e9, 0x482, 0x483, 0x484, 0x483, 0x484},
            {-1, -1, -1},
        },
    };
    const msw::CBallVolley::ARTS_EFCT_TBL s_aArtsEfctTblEx[2] = {
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct3D0}, {1, s_aEfct3D0},
             {2, s_aEfct5F0}, {1, s_aEfct710}, {1, s_aEfct818}},
            0x0, 0x2800, 0x5, 0x63d, 0x63c, 2.0f,
            {0x491, 0x492, 0x493, 0x494, 0x493, 0x494},
            {0xf, 0xf, 0xf},
        },
        {
            {{2, s_aEfct270}, {2, s_aEfct2B0}, {1, s_aEfct3DC}, {1, s_aEfct3DC},
             {2, s_aEfct608}, {1, s_aEfct71C}, {1, s_aEfct824}},
            0x0, 0x2800, 0x5, 0x646, 0x645, 2.0f,
            {0x49a, 0x49b, 0x49c, 0x49d, 0x49c, 0x49d},
            {0xf, 0xf, 0xf},
        },
    };


    // Maps x from [x0, x1] onto [y0, y1], clamped at both ends.
    inline float MapRange(float x, float x0, float x1, float y0, float y1)
    {
        if (x <= x0) {
            return y0;
        }
        if (x >= x1) {
            return y1;
        }
        return (y1 - y0) * (x - x0) / (x1 - x0) + y0;
    }

    // The distance from rFrom to rTo in the XZ plane.
    inline float GetDistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 from(rFrom.x, 0.0f, rFrom.z);
        bcs::FVEC3 to(rTo.x, 0.0f, rTo.z);
        return nw4r::math::FSqrt(bcs::FVEC3DistSq(&from, &to));
    }

}

// (meaning not known) A name nothing in the file reads (its reader was
// stripped); the original keeps it between the effect tables and the literal
// pool. A Matching build needs `s_aJustBlock__15CBallVolley_cpp` in
// VolleyF's force_active list (config.yml) to keep it.
// uncalled stand-in (notes/decisions.md section 1).
namespace CBallVolley_cpp {
    extern const char s_aJustBlock[] = "JustBlock";
}

namespace {

    // One step of a flight: rPos moves by fTime frames of rVel plus half
    // of rAccel (the change of velocity over the step), then rVel takes
    // rAccel.
    inline void MoveFlight(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, const bcs::FVEC3 &rAccel, float fTime)
    {
        bcs::FVEC3 move;
        bcs::FVEC3 half;
        bcs::FVEC3Scale(&half, &rAccel, 0.5f * fTime);
        bcs::FVEC3Scale(&move, &rVel, fTime);
        bcs::FVEC3Add(&move, &move, &half);
        bcs::FVEC3Add(&rVel, &rVel, &rAccel);
        bcs::FVEC3Add(&rPos, &rPos, &move);
    }

    // One step of a flight: rMove = how far it goes in fTime frames, then
    // rVel takes rAccel.
    inline void StepFlight(bcs::FVEC3 &rMove, bcs::FVEC3 &rVel, const bcs::FVEC3 &rAccel, float fTime)
    {
        bcs::FVEC3 half;
        bcs::FVEC3Scale(&rMove, &rVel, fTime);
        bcs::FVEC3Scale(&half, &rAccel, 0.5f * fTime);
        bcs::FVEC3Add(&rMove, &rMove, &half);
        bcs::FVEC3Add(&rVel, &rVel, &rAccel);
    }

    // n clamped to the five entries of the per touch tables.
    inline long GetTouchIdx(long n)
    {
        return n > 4 ? 4 : (n < 0 ? 0 : n);
    }
}

// ---------------------------------------------------------------------------
// BALL_VOLLEY::CChrParasiteBanana

msw::BALL_VOLLEY::CChrParasiteBanana::CChrParasiteBanana(bcs::CTask *pParent, long nChr)
    : CChrParasite(pParent, nChr)
{
    m_AtkColl.Create(1, 4, 0, 0);
    m_AtkColl.SetAtkTypeAll(0x2704);
    m_AtkColl.SetTargetFlagAll(2);
    m_AtkColl.SetNoHitFlagAll(~(1 << nChr));
    m_AtkColl.SetHitEffectFrameAll(12.0f);
    m_AtkColl.SetRadiusAll(0.5f);
    m_AtkColl.SetDisableAll();
}

msw::BALL_VOLLEY::CChrParasiteBanana::~CChrParasiteBanana()
{
}

// Waits one frame while the character is down (status bits 0x400 or 1),
// then puts the attack collision on its trans node for one frame and turns
// it off; then asks for deletion.
void msw::BALL_VOLLEY::CChrParasiteBanana::MainFunc()
{
    switch (m_nState) {
        case 0: {
            unsigned long flag = GetChrCore_()->GetStatusFlag();
            if (!(flag & 0x400) && !(flag & 1)) {
                m_nState++;
            }
            break;
        }
        case 1: {
            CChrCore *pChr = GetChrCore_();
            unsigned long flag = pChr->GetStatusFlag();
            if (!(flag & 0x400) && !(flag & 1)) {
                bcs::FVEC3 pos;
                pChr->GetTransNodePos(pos);
                m_AtkColl.SetPosAll(pos);
                m_AtkColl.SetEnableAll();
                m_nState++;
            }
            break;
        }
        case 2:
            m_AtkColl.SetDisableAll();
            m_nState++;
            break;
        default:
            Delete();
            break;
    }
}

// ---------------------------------------------------------------------------
// CBallVolley

namespace msw {

    CBallVolley::CBallVolley(bcs::CTask *pParent, long nBallNo)
        : CBall(pParent, nBallNo), m_nVolleyFlag318(0), m_nVolleyFlag(0), m_bArtsOver(0), m_fFadeWait(0.0f),
          m_fAlpha(1.0f), m_fArtsOverRate(0.0f), m_nVolleyFlag444(0)
    {
        ResetParam_();
    }

    CBallVolley::~CBallVolley()
    {
    }

    void CBallVolley::Disp()
    {
        CBall::Disp();
    }

    inline void CBallVolley::ClearTouchReaction_()
    {
        for (long i = 0; i < 5; i++) {
            m_anReceiveReaction[i] = 0;
            m_anReceiveReaction3[i] = 0;
            m_anBlockReaction[i] = 0;
            m_afStaggerFrame[i] = 0.0f;
        }
    }

    inline void CBallVolley::ClearSpin_()
    {
        m_nSpinType = 0;
        m_fSpinCount = 0.0f;
        m_vSpinAccel.x = 0.0f;
        m_vSpinAccel.y = 0.0f;
        m_vSpinAccel.z = 0.0f;
    }

    inline void CBallVolley::ClearSpikeWork_()
    {
        m_fTimingCount = m_fTimingStart = m_fTimingMid = m_fTimingEnd = 0.0f;
        m_vSpikeJumpTgt.x = 0.0f;
        m_vSpikeJumpTgt.y = 0.0f;
        m_vSpikeJumpTgt.z = 0.0f;
        m_fSpikeJumpHeight = 0.0f;
        m_fSpikeJumpFrame = 0.0f;
        m_nVolleyFlag &= ~0x1C0;
    }

    inline void CBallVolley::ClearRide_()
    {
        m_nVolleyFlag &= ~1;
        m_nRideObjHandle = 0;
    }

    void CBallVolley::ResetParam_()
    {
        CBall::ResetParam_();
        m_nVolleyFlag = 0;
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        m_nShotChrIdx = 0;
        m_n32C = 0;
        m_n330 = -1;
        m_n334 = -1;
        m_nPassChrIdx = 0;
        m_nShotTeamIdx = 0;
        m_nServeCourt = 0;
        m_nTouchNum = 0;
        m_nTrueTouchNum = 0;
        m_n34C = 0;
        m_f350 = 0.0f;
        m_n358 = -1;
        m_f35C = 0.0f;
        m_f360 = 0.0f;
        m_fPassCounter = 0.0f;
        m_nSpikeType = 6;
        m_n36C = 1;
        m_nFloorHitSe = -1;
        m_n450 = -1;
        m_n448 = 0;
        m_n3E0 = -1;
        m_n3E4 = 0;
        m_fMarkerDispFrame = 100.0f;
        m_fMarkerDispFrame2 = 20.0f;
        m_fFadeWait = 0.0f;
        m_fAlpha = 1.0f;
        m_fArtsOverRate = 0.0f;
        m_vRideObjPos.x = 0.0f;
        m_vRideObjPos.y = 0.0f;
        m_vRideObjPos.z = 0.0f;
        m_nSpikeFlag = 0;
        ClearTouchReaction_();
        ClearSpin_();
        InitArtsShotStep_();
        m_fChargePower = 0.0f;
        ClearSpikeWork_();
        ClearRide_();
    }

    void CBallVolley::ResetOnAttach_()
    {
        m_bArtsAttach = 0;
        m_fScale = 1.0f;
        m_nVolleyFlag &= ~0xC;
        ClearRide_();
    }

    void CBallVolley::ResetOnDetach_()
    {
        m_bArtsAttach = 0;
        // dead comparison: the original's literal pool has the net margin
        // 0.04 here, ahead of its first use (CollSubCheckNet_).
        (void)(m_f324 == 0.04f);
    }

    void CBallVolley::CreateCollBody_()
    {
        if (m_pMdlObj != 0) {
            m_pBodyColl = new (0) CBodyCollision(2, 2, 0, m_pMdlObj->GetHandle());
            m_pBodyColl->SetPushEnable(0);
            m_pBodyColl->SetBodyEnable(1);
            m_pBodyColl->SetIndexAll(m_nBallNo);
            m_pBodyColl->SetMyNoHitFlagAll(1 << (m_nBallNo + 8));
            m_pBodyColl->SetPushRank(0, 1);
            m_pBodyColl->SetPushPower(0, 0);
            m_pBodyColl->SetBodyDamageCallbackAll(CallbackBodyDamage, this);
            m_pBodyColl->SetBodyPushCallbackAll(CallbackBodyPush, this);
        }
    }

    void CBallVolley::Reset()
    {
        DetachItemEffect();
        EffectStopTail_();
        ResetParam_();
        ChangeState(0);
    }

    int CBallVolley::IsActive() const
    {
        if (IsVolleyReqDelete()) {
            return 0;
        }
        return CBall::IsActive();
    }

    int CBallVolley::IsSleepMode() const
    {
        return CBall::IsSleepMode();
    }

    int CBallVolley::IsFreeMoveMode() const
    {
        return CBall::IsFreeMoveMode();
    }

    int CBallVolley::IsPassMode() const
    {
        return CBall::IsPassMode();
    }

    int CBallVolley::IsAlleyoopPassMode() const
    {
        return CBall::IsAlleyoopPassMode();
    }

    int CBallVolley::IsShootMode() const
    {
        return CBall::IsShootMode();
    }

    int CBallVolley::IsArtsShootMode() const
    {
        return CBall::IsArtsShootMode();
    }

    int CBallVolley::IsVolleyEnemySpike() const
    {
        return (m_nSpikeFlag >> 2) & 1;
    }

    int CBallVolley::IsVolleyThroughUnderNet() const
    {
        return (m_nVolleyFlag >> 3) & 1;
    }

    int CBallVolley::IsVolleyTargetPosInnerCourt(long nCourt, float fMargin)
    {
        bcs::FVEC3 pos;
        GetPosAfterFrame(pos, GetTouchDownFrame(GetBgCollRadius()));
        return CCollMan::CheckMapInner(pos, fMargin, (unsigned char)nCourt | 0x4000000) == 0;
    }

    int CBallVolley::IsVolleyChargeBall() const
    {
        return GetVolleyChargePower() >= 1.0f;
    }

    float CBallVolley::GetVolleyChargePower() const
    {
        return m_fChargePower;
    }

    int CBallVolley::GetVolleySpikeJumpTargetPos(bcs::FVEC3 &rPos) const
    {
        if (IsPassMode() && m_fSpikeJumpHeight > 0.0f && GetHeightPassFrame(m_fSpikeJumpHeight, 0) > m_fSpikeJumpFrame) {
            rPos = m_vSpikeJumpTgt;
            return 1;
        }
        return 0;
    }

    void CBallVolley::GetVolleyTouchDownPos(bcs::FVEC3 &rPos, int b) const
    {
        GetVolleyPosAfterFrame(rPos, GetTouchDownFrame(GetBgCollRadius()), b);
    }

    void CBallVolley::CmnShotInit_()
    {
        m_nVolleyFlag &= ~0x7F1C;
        m_nSpikeType = 6;
        m_n36C = 1;
        m_fPassCounter = 0.0f;
        m_n448 = 0;
        m_nFloorHitSe = -1;
        m_n450 = -1;
        m_n32C = 0;
        m_n330 = -1;
        m_n334 = -1;
        m_nSpikeFlag = 0;
        m_fScale = 1.0f;
        m_bEndure = 0;
        m_fMarkerDispFrame = 100.0f;
        m_fMarkerDispFrame2 = 20.0f;
        m_f35C = 0.0f;
        m_f3DC = 0.0f;
        SE_Stop(0, 0.0f);
        SE_Stop(1, 0.0f);
        ClearTouchReaction_();
        ClearSpin_();
        InitArtsShotStep_();
        ClearSpikeWork_();
        ClearRide_();
    }

    void CBallVolley::InitArtsShotStep_()
    {
        m_bArtsAttach = 0;
        m_n3EC = 0;
        m_f3F0 = 0.0f;
        m_nArtsKind = -1;
        m_f400 = 0.0f;
        m_n3F8 = 0;
        m_n3FC = 0;
        m_f404 = 0.0f;
        m_f408 = 0.0f;
        m_bEndure = 0;
        m_EndurePrm.m_fUnk00 = 0.0f;
        m_EndurePrm.m_fUnk04 = 0.0f;
        m_EndurePrm.m_fUnk08 = 0.0f;
        m_EndurePrm.m_afUnk0C[0] = 0.0f;
        m_EndurePrm.m_afUnk0C[1] = 0.0f;
        m_EndurePrm.m_afUnk0C[2] = 0.0f;
        m_EndurePrm.m_anUnk18[0] = -1;
        m_EndurePrm.m_anUnk18[1] = -1;
        m_EndurePrm.m_anUnk18[2] = -1;
    }

    void CBallVolley::PlayFloorHitSe(unsigned long nArg)
    {
        if (m_nFloorHitSe >= 0) {
            PlaySePos(m_nFloorHitSe, 0, nArg, -1, 0.0f, 1.0f, 0.0f);
            m_nFloorHitSe = -1;
        } else {
            CBall::PlayFloorHitSe(nArg);
        }
    }

    void CBallVolley::SetHighlight_()
    {
        CBall::SetHighlight_();
        if (m_bArtsOver != 0) {
            if (m_fFadeWait > 0.0f) {
                m_fFadeWait -= m_fMoveRate;
            } else {
                if (m_fAlpha > 0.0f) {
                    m_fAlpha -= m_fArtsOverRate * m_fMoveRate;
                    float a = m_fAlpha;
                    m_fAlpha = a > 1.0f ? 1.0f : (a < 0.0f ? 0.0f : a);
                    if (m_fAlpha <= 0.0f) {
                        ChangeState(0);
                    }
                }
                SetAlphaRate(m_fAlpha);
            }
        }
    }

    void CBallVolley::UpdateRainbow_()
    {
    }

    void CBallVolley::UpdateCollAtk_()
    {
        if (m_bArtsOver != 0) {
            if (m_pAtkColl != 0) {
                m_pAtkColl->SetDisableAll();
            }
        } else {
            CBall::UpdateCollAtk_();
        }
    }

    void CBallVolley::UpdateCollBody_()
    {
        if (m_bArtsOver != 0) {
            if (m_pBodyColl != 0) {
                m_pBodyColl->SetDisableAll();
            }
        } else {
            CBall::UpdateCollBody_();
        }
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            float add = pItem->GetVolleyCatchCollRadius();
            SetCollCatchRadius(GetCollCatchRadiusDefault() + add);
        } else {
            SetCollCatchRadius(GetCollCatchRadiusDefault());
        }
    }

    float CBallVolley::GetCollCatchRadiusDefault() const
    {
        return 0.1f;
    }

    void CBallVolley::UpdateDisp_()
    {
        if ((m_nVolleyFlag & 2) && !IsFlag2()) {
            m_nFlag |= 0x6;
            StopTailEffect();
            ChangeState(0);
            m_f360 = 99999.9f;
            SetAlphaRate(0.0f);
        }
        CBall::UpdateDisp_();
    }

    int CBallVolley::IsVolleyReqDelete() const
    {
        return m_bArtsOver != 0;
    }

    int CBallVolley::IsVolleyVanish() const
    {
        return m_fAlpha <= 0.0f;
    }

    int CBallVolley::IsVolleyChanceBallMode() const
    {
        return m_nVolleyFlag & 0x200;
    }

    int CBallVolley::IsVolleyPowerSpike() const
    {
        if (IsShootMode() == 0) {
            return 0;
        }
        return m_nSpikeType == 1;
    }

    int CBallVolley::IsVolleyCurveSpike() const
    {
        if (IsShootMode() == 0) {
            return 0;
        }
        return m_nSpikeType == 3;
    }

    int CBallVolley::IsVolleyFeintSpike() const
    {
        if (IsShootMode() == 0) {
            return 0;
        }
        return m_nSpikeType == 5;
    }

    int CBallVolley::IsVolleyNetHit() const
    {
        return m_nVolleyFlag & 0x4000;
    }

    float CBallVolley::GetRadius() const
    {
        float scale = m_fScale;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            scale *= pItem->GetScale();
        }
        return 0.25f * scale;
    }

    float CBallVolley::GetBgCollRadius() const
    {
        float scale = m_fScale;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            scale *= pItem->GetScale();
        }
        return 0.23f * scale;
    }

    float CBallVolley::GetBgCollRadiusXZ() const
    {
        return GetBgCollRadius();
    }

    int CBallVolley::GetTargetPos(bcs::FVEC3 &rPos) const
    {
        if (m_nFlag & FLAG_SCORED) {
            return 0;
        }
        if (IsAttach()) {
            return 0;
        }
        if (IsAlleyoopPassMode()) {
            return 0;
        }
        if (m_nVolleyFlag & 0x40000) {
            return 0;
        }
        rPos = m_vTargetPos;
        return 1;
    }

    long CBallVolley::CallbackBodyDamage_(const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos)
    {
        if (!IsAttach()) {
            if (!IsInvincible()) {
                unsigned long trg = TrgItemEffect_(7);
                // (the original also tests bit 0x8 here and does nothing
                // with it)
                if (trg & 0x10) {
                    m_nVolleyFlag |= 2;
                }
                if (trg & 4) {
                    // Knocked away from the hit, upwards.
                    bcs::FVEC3 dir;
                    bcs::FVEC3Sub(&dir, &m_Pos, &rPos);
                    if (bcs::BCS_Vec_GetLenXZ(dir) > 0.0f) {
                        NormalizeXZ(dir, dir);
                        dir.y = 1.0f;
                        bcs::BCS_Vec_Normalize(dir, dir);
                    } else {
                        dir.x = 0.0f;
                        dir.y = 1.0f;
                        dir.z = 0.0f;
                    }
                    bcs::FVEC3Scale(&dir, &dir, 0.08f);
                    SetBound_(dir);
                    return 0x101;
                }
            }
        } else if (m_pBodyDamageHook != 0) {
            return m_pBodyDamageHook(this, pAtk, rPos, m_pBodyDamageHookArg);
        }
        return 2;
    }

    void CBallVolley::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush == 0) {
            return;
        }
        if (IsAttach()) {
            pOther->DoBodyPushCallback(pBody, 0);
            return;
        }
        pOther->DoBodyPushCallback(pBody, 0);
        if ((m_nFlag & 1) && CGame::g_GameWork.m_nGameType == 0) {
            bcs::FVEC3 push = *pPush;
            m_Pos.x += push.x;
            m_Pos.z += push.z;
            m_PrevPos.x += push.x;
            m_PrevPos.z += push.z;
            if (m_pBodyColl != 0) {
                m_pBodyColl->AddPosXZAll(push, push);
                m_pBodyColl->CalcAABBAll();
            }
        }
    }

    void CBallVolley::SetBound_(const bcs::FVEC3 &rVel)
    {
        ClearSpin_();
        m_nSpikeType = 6;
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        EffectStopTail_();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_nFlag |= 0x20;
        m_Velocity = rVel;
        m_PrevVel = rVel;
        m_PrevPos = m_Pos;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 0.2617994f;
        UpdateCameraTarget_(0);
        m_n448 = 0;
        m_nFloorHitSe = -1;
        ChangeState(1);
    }

    inline void CBallVolley::UpdateCameraTarget_(int bFlag)
    {
        if (m_bArtsOver != 0) {
            bFlag = 0;
        }
        SetCameraBallTargetPos(bFlag, m_vTargetPos);
    }

    void CBallVolley::SetVolleyChargePower(float fPower)
    {
        if (IsItemEffect()) {
            m_fChargePower = 0.0f;
            return;
        }
        m_fChargePower = fPower;
        float p = m_fChargePower;
        m_fChargePower = 0.0f < p ? p : 0.0f;
    }

    void CBallVolley::ClearVolleyChargePower()
    {
        GetVolleyChargePower();
        m_fChargePower = 0.0f;
    }

    void CBallVolley::SetSpikeTimingWork_(float fStart, float fMid, float fEnd)
    {
        m_fTimingCount = 0.0f;
        m_fTimingStart = Max(0.0f, fStart);
        m_fTimingMid = Max(fMid, m_fTimingStart);
        m_fTimingEnd = Max(fEnd, m_fTimingMid);
        m_nVolleyFlag |= 0x40;
    }

    int CBallVolley::IsTimingSpike_() const
    {
        if ((m_nVolleyFlag & 0x40) && m_fTimingCount >= m_fTimingStart && m_fTimingCount <= m_fTimingEnd) {
            return 1;
        }
        return 0;
    }

    void CBallVolley::SetSpikeJumpTgtWork_(float fHeight)
    {
        float height = 2.9f;
        if (fHeight > 0.0f) {
            height = fHeight;
        }
        float frame = GetHeightPassFrame(height, 1);
        float frame2 = GetHeightPassFrame(height, 0);
        if (frame < frame2) {
            frame += 0.75f * (frame2 - frame);
        }
        bcs::FVEC3 pos;
        GetPosAfterFrame(pos, frame);
        m_vSpikeJumpTgt.x = pos.x;
        m_vSpikeJumpTgt.y = 0.0f;
        m_vSpikeJumpTgt.z = pos.z;
        bcs::FVEC3 from = m_Pos;
        bcs::FVEC3 to(pos.x, 0.0f, pos.z);
        CollCheckPassHitNet_(m_vSpikeJumpTgt, from, to);
        m_fSpikeJumpHeight = height;
        m_fSpikeJumpFrame = 32.0f;
    }

    void CBallVolley::GetVolleyBallMarkerDispFrame(float &rFrame, float &rFrame2) const
    {
        rFrame = m_fMarkerDispFrame;
        rFrame2 = m_fMarkerDispFrame2;
        if (m_nVolleyFlag & 0x40000) {
            rFrame2 = rFrame = m_fMarkerDispFrame;
        }
    }

    inline const CBallVolley::ARTS_EFCT_TBL *CBallVolley::GetArtsEffectTbl_() const
    {
        const ARTS_EFCT_TBL *pTbl = 0;
        if (m_nArtsKind >= 0 && m_nArtsKind <= 0x18) {
            pTbl = GetArtsEffectTbl_(m_nArtsKind, m_n32C);
        }
        return pTbl;
    }

    long CBallVolley::GetVolleyReceiveReaction(unsigned long nKind, long nTouch) const
    {
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            return pItem->GetVolleyReceiveReaction(nKind, 0);
        }
        const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_();
        if (pTbl != 0) {
            long n = pTbl->m_nReceiveReaction;
            if (nKind == 3) {
                n = pTbl->m_nReceiveReaction3;
            }
            return n;
        }
        long idx = GetTouchIdx(nTouch);
        long n = m_anReceiveReaction[idx];
        if (nKind == 3) {
            n = m_anReceiveReaction3[idx];
        }
        if (nKind != 1) {
            if (n & 0x400000) {
                n |= 0x100;
            }
            if (n & 0x800000) {
                n |= 0x200;
            }
            if (n & 0x1000000) {
                n |= 0x400;
            }
            if (n & 0x2000000) {
                n |= 0x800;
            }
            if (n & 0x4000000) {
                n |= 0x1000;
            }
            if (n & 0x8000000) {
                n |= 0x2000;
            }
            if (n & 0x20000000) {
                n |= 0x20000;
            }
        }
        return n;
    }

    void CBallVolley::TrgVolleyReceiveReaction(long bArts, unsigned long nArg)
    {
        if (bArts != 0) {
            const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_();
            if (pTbl != 0) {
                SetArtsTailEffect__(pTbl->m_aEfct[3]);
            }
            if (m_bEndure != 0) {
                if (m_EndurePrm.m_anUnk18[0] >= 0) {
                    PlaySePos(m_EndurePrm.m_anUnk18[0], 0, nArg, -1, 0.0f, 1.0f, 0.0f);
                }
                if (m_EndurePrm.m_anUnk18[1] >= 0) {
                    SE_PlayMove(1, m_EndurePrm.m_anUnk18[1], &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                }
            }
        } else if (TrgItemEffect_(0xA) & 0x10) {
            m_nVolleyFlag |= 2;
        }
    }

    long CBallVolley::GetVolleyBlockReaction(unsigned long nKind, long nTouch) const
    {
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            return pItem->GetVolleyBlockReaction(nKind, 0);
        }
        if (IsArtsShootMode()) {
            const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_();
            if (pTbl != 0) {
                return pTbl->m_nBlockReaction;
            }
            return 1;
        }
        return m_anBlockReaction[GetTouchIdx(nTouch)];
    }

    void CBallVolley::TrgVolleyBlockReaction(unsigned long)
    {
        if (TrgItemEffect_(0xB) & 0x10) {
            m_nVolleyFlag |= 2;
        }
    }

    long CBallVolley::GetVolleyReceiveDropCoinNum(long) const
    {
        return 0;
    }

    long CBallVolley::GetVolleyBlockDropCoinNum(long) const
    {
        return 0;
    }

    float CBallVolley::GetVolleyReceiveStaggerFrame(long nTouch) const
    {
        return m_afStaggerFrame[GetTouchIdx(nTouch)];
    }

    float CBallVolley::GetVolleyFlickDownPower(long nKind) const
    {
        float power = 0.0f;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            return pItem->GetVolleyFlickDownPower(nKind, power);
        }
        return power;
    }

    int CBallVolley::GetVolleyEndureParam(BALL::VOLLEY::ENDURE_PRM &rPrm)
    {
        if (m_bEndure == 0) {
            return 0;
        }
        rPrm.m_fUnk00 = m_EndurePrm.m_fUnk00;
        rPrm.m_fUnk04 = m_EndurePrm.m_fUnk04;
        rPrm.m_fUnk08 = m_EndurePrm.m_fUnk08;
        rPrm.m_afUnk0C[0] = m_EndurePrm.m_afUnk0C[0];
        rPrm.m_afUnk0C[1] = m_EndurePrm.m_afUnk0C[1];
        rPrm.m_afUnk0C[2] = m_EndurePrm.m_afUnk0C[2];
        rPrm.m_anUnk18[0] = m_EndurePrm.m_anUnk18[0];
        rPrm.m_anUnk18[1] = m_EndurePrm.m_anUnk18[1];
        rPrm.m_anUnk18[2] = m_EndurePrm.m_anUnk18[2];
        return 1;
    }

    int CBallVolley::TrgVolleyPreparationTouch(long nChr, long, long nKind, long bArg)
    {
        unsigned long trg = TrgItemEffect_(0xD);
        if ((nKind == 9 || nKind == 0xF || nKind == 0x11) && bArg != 0) {
            trg &= ~0x40;
        }
        if ((trg & 0x40) && !(m_nFlag & 1)) {
            new (0x100) BALL_VOLLEY::CChrParasiteBanana(CGame::GetTempParent(), nChr);
        }
        return 1;
    }

    // Counts the timers of the ball down (and the spike timing up).
    void CBallVolley::UpdateCountWork_()
    {
        if (m_f35C > 0.0f) {
            m_f35C -= m_fMoveRate;
            if (m_f35C < 0.0f) {
                m_f35C = 0.0f;
            }
        }
        if (m_f360 > 0.0f) {
            m_f360 -= m_fMoveRate;
            if (m_f360 < 0.0f) {
                m_f360 = 0.0f;
            }
        }
        if (m_fPassCounter > 0.0f) {
            m_fPassCounter -= m_fMoveRate;
            if (m_fPassCounter < 0.0f) {
                m_fPassCounter = 0.0f;
            }
        }
        if (m_f3DC > 0.0f) {
            m_f3DC -= m_fMoveRate;
            if (m_f3DC < 0.0f) {
                m_f3DC = 0.0f;
            }
        }
        if (m_nVolleyFlag & 0x40) {
            m_fTimingCount += m_fMoveRate;
            float max = 1.0f + m_fTimingEnd;
            float t = m_fTimingCount;
            m_fTimingCount = max > t ? t : max;
        }
    }

    void CBallVolley::AttachTailEffect_()
    {
        if (m_bArtsOver != 0) {
            EffectStopTail_();
        } else if (m_n358 >= 0) {
            EffectSetTail_(m_n358);
        }
    }

    void CBallVolley::CtorStatePass()
    {
        m_nFlag |= 0x3000;
        AttachTailEffect_();
    }

    void CBallVolley::LoopStatePass()
    {
        UpdateCountWork_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
    }

    void CBallVolley::DtorStatePass()
    {
    }

    void CBallVolley::CtorStateFreeMove()
    {
        CBall::CtorStateFreeMove();
    }

    void CBallVolley::LoopStateFreeMove()
    {
        UpdateCountWork_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
        if (CGame::g_GameWork.m_nFlag18 & 1) {
            AdjustGravity_();
        }
    }

    void CBallVolley::DtorStateFreeMove()
    {
        CBall::DtorStateFreeMove();
    }

    void CBallVolley::CtorStateAttach()
    {
        CBall::CtorStateAttach();
    }

    // While a character holds a special shot ready (m_bArtsAttach) the ball
    // shows the charge effect, then waits m_f3F0 frames and grows its glow
    // (m_f18C) up to 10.
    void CBallVolley::LoopStateAttach()
    {
        if (m_bArtsAttach != 0 && (CGame::g_uPauseBit & 0x4)) {
            switch (m_n3EC) {
                case 0:
                    Eff_CreateSimple(0x4FD, m_Pos, 3, 0);
                    PlaySePos(0xC99, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    m_n3EC++;
                case 1:
                    m_f3F0 -= m_fMoveRate;
                    if (m_f3F0 < 0.0f) {
                        m_f3F0 = 0.0f;
                        m_n3EC++;
                    }
                    break;
                default:
                    m_f3F0 += 0.33333334f * m_fMoveRate;
                    m_f3F0 = m_f3F0 > 10.0f ? 10.0f : (m_f3F0 < 0.0f ? 0.0f : m_f3F0);
                    m_f18C = m_f3F0;
                    break;
            }
        } else {
            CBall::LoopStateAttach();
        }
    }

    void CBallVolley::DtorStateAttach()
    {
        CBall::DtorStateAttach();
    }

    void CBallVolley::CtorStateAlleyoop()
    {
        AttachTailEffect_();
        m_nFlag |= 0x3000;
    }

    void CBallVolley::LoopStateAlleyoop()
    {
        LoopStatePass();
    }

    void CBallVolley::DtorStateAlleyoop()
    {
    }

    void CBallVolley::CtorStateChargeShoot()
    {
    }

    void CBallVolley::LoopStateChargeShoot()
    {
        ChangeState(6);
    }

    void CBallVolley::DtorStateChargeShoot()
    {
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
    }

    void CBallVolley::CtorStateNormalShoot()
    {
        m_nFlag &= ~0x40;
        m_nFlag |= 0x3000;
        if (m_n3E0 >= 0) {
            PlaySePos(m_n3E0, 0, m_n3E4, -1, 0.0f, 1.0f, 0.0f);
            m_n3E0 = -1;
        }
        AttachTailEffect_();
        if (IsItemEffect()) {
            if (m_pBodyColl != 0) {
                m_pBodyColl->ResetTargetFlag(1, 2);
            }
        }
        ClearVolleyChargePower();
    }

    inline void CBallVolley::UpdateSpin_()
    {
        switch (m_nSpinType) {
            case 0:
                m_fSpinCount = 0.0f;
                m_vSpinAccel.x = 0.0f;
                m_vSpinAccel.y = 0.0f;
                m_vSpinAccel.z = 0.0f;
                break;
            case 1:
            case 2:
            case 3:
                if (m_f360 <= 0.0f) {
                    m_fSpinCount -= m_fMoveRate;
                    if (m_fSpinCount <= 0.0f) {
                        m_fSpinCount = 0.0f;
                        m_vSpinAccel.x = 0.0f;
                        m_vSpinAccel.y = 0.0f;
                        m_vSpinAccel.z = 0.0f;
                        m_nSpinType = 0;
                    }
                }
                break;
        }
    }

    void CBallVolley::LoopStateNormalShoot()
    {
        UpdateCountWork_();
        UpdateSpin_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
    }

    void CBallVolley::DtorStateNormalShoot()
    {
        if (m_pBodyColl != 0) {
            m_pBodyColl->AddTargetFlag(1, 2);
        }
    }

    void CBallVolley::CtorStateArtsShoot()
    {
        m_nFlag &= ~0x2000;
        m_nFlag |= 0x3000;
        const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(m_nArtsKind, m_n32C);
        if (pTbl != 0) {
            SetArtsTailEffect__(pTbl->m_aEfct[2]);
        }
        if (TrgItemEffect_(0) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        ClearVolleyChargePower();
    }

    void CBallVolley::LoopStateArtsShoot()
    {
        UpdateCountWork_();
        UpdateSpin_();
        ArtsBallBeforeMoveFunc_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        ArtsBallAfterMoveFunc_();
        RotNormal_();
        CheckCollision_();
    }

    void CBallVolley::DtorStateArtsShoot()
    {
        ClearOffsetPos();
    }

    // The turn of an effect along velocity rVel (x: the pitch, y: the
    // heading).
    inline void CBallVolley::GetFlightRot_(bcs::FVEC3 &rRot) const
    {
        bcs::FVEC3 vel = m_Velocity;
        rRot.x = bcs::ROUND_PI(nw4r::math::Atan2Rad(bcs::BCS_Vec_GetLenXZ(vel), vel.y));
        rRot.x = bcs::ROUND_PI(rRot.x - 1.5707964f);
        rRot.y = bcs::ROUND_PI(nw4r::math::Atan2Rad(vel.x, vel.z));
        rRot.z = 0.0f;
    }

    // The special shot of kind 4 (a curving shot whose side the shooter
    // picks): it first turns aside towards a new target, then shows its
    // trail.
    void CBallVolley::ArtsBallBeforeMoveFunc_()
    {
        if (m_nArtsKind != 4) {
            return;
        }
        switch (m_n3F8) {
            case 0:
                if (m_fSpinCount <= 0.0f) {
                    bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
                    bcs::FVEC3 dir;
                    {
                        bcs::FVEC3 move;
                        bcs::FVEC3Sub(&move, &m_vTargetPos, &m_Pos);
                        bcs::BCS_Vec_Normalize(dir, move);
                    }
                    bcs::FVEC3 side;
                    PSVECCrossProduct((const Vec *)&up, (const Vec *)&dir, (Vec *)&side);
                    NormalizeXZ(side, side);
                    float len = 7.0f;
                    if (m_n3FC <= 0) {
                        if (bcs::BCS_RandMax(1L)) {
                            len = -len;
                        }
                    } else if (m_n3FC == 2) {
                        len = -len;
                    }
                    side.x *= len;
                    side.z *= len;
                    m_vTargetPos.x += side.x;
                    m_vTargetPos.z += side.z;
                    len = 0.0f;
                    bcs::BCS_VecZ_RotY(side, bcs::BCS_RandMax(len), bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f)));
                    m_vTargetPos.x += side.x;
                    m_vTargetPos.z += side.z;
                    bcs::FVEC3 hitPos;
                    if (CCollMan::DetectMapHit(hitPos, side, len, m_vTargetPos, m_vTargetPos, 0.05f, 2,
                                               (unsigned char)m_nServeCourt | 0x4000000)) {
                        m_vTargetPos.x = hitPos.x;
                        m_vTargetPos.z = hitPos.z;
                    }
                    {
                        bcs::FVEC3 move;
                        bcs::FVEC3Sub(&move, &m_vTargetPos, &m_Pos);
                        bcs::BCS_Vec_Normalize(dir, move);
                    }
                    bcs::FVEC3Scale(&m_Velocity, &dir, m_f400);
                    m_fGravity = 0.0f;
                    m_nSpinType = 3;
                    m_fSpinCount = GetTouchDownFrame(GetBgCollRadius());
                    PSVECCrossProduct((const Vec *)&up, (const Vec *)&dir, (Vec *)&side);
                    PSVECCrossProduct((const Vec *)&dir, (const Vec *)&side, (Vec *)&m_vSpinAccel);
                    bcs::BCS_Vec_Normalize(side, m_vSpinAccel);
                    float vel = 0.0f;
                    float accel = 0.0f;
                    float dist = bcs::FVEC3Dist(&m_vTargetPos, &m_Pos);
                    bcs::BCS_Phys_CalcYVG(m_fSpinCount, 0.0f, 0.0f, 0.35f * dist, vel, accel);
                    bcs::FVEC3Scale(&m_vSpinAccel, &side, accel);
                    bcs::FVEC3 add;
                    bcs::FVEC3Scale(&add, &side, vel);
                    bcs::FVEC3Add(&m_Velocity, &m_Velocity, &add);
                    UpdateCameraTarget_(1);
                    CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x89), m_nShotChrIdx, 4, 0, 0);
                    m_f360 = 2.0f;
                    m_n3F8++;
                } else {
                    // The shooter may still pick the side.
                    long nChr = m_nShotChrIdx;
                    if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
                        CChrCore *pChr = CChrMan::GetChrCore(nChr);
                        if (pChr != 0 && pChr->IsEnable()) {
                            long n = pChr->GetVolleyArtsExInput();
                            if (n > 0) {
                                m_n3FC = n;
                            }
                        }
                    }
                }
                break;
            case 1:
                if (m_f360 <= 0.0f) {
                    StopTailEffect();
                    CEffectCtrl::SimpleEffect(0x586, m_Pos, 3, 0, 0);
                    PlaySePos(0x412, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(m_nArtsKind, m_n32C);
                    if (pTbl != 0) {
                        SetArtsTailEffect__(pTbl->m_aEfct[2]);
                    }
                    m_n3F8++;
                }
                break;
        }
    }

    // The special shot of kind 12 (a ball that vanishes on its way and
    // comes back to the target marker of the shooter).
    void CBallVolley::ArtsBallAfterMoveFunc_()
    {
        if (m_nArtsKind != 0xC) {
            return;
        }
        switch (m_n3F8) {
            case 0:
                if (!(m_nVolleyFlag & 0x20000)) {
                    CGame::SendMessage(0x5B, m_nShotChrIdx, 0, 0);
                    m_n3F8 = 0x63;
                    return;
                }
                if (m_f404 < m_f408) {
                    m_f404 += m_fMoveRate;
                }
                if (m_f404 >= m_f408) {
                    bcs::FVEC3 rot;
                    GetFlightRot_(rot);
                    unsigned long hEffect = CEffectCtrl::CreateEffect(0x5E6, 3, 0, 0, 0);
                    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                    if (pCluster != 0) {
                        pCluster->SetTranslate(&m_Pos);
                        pCluster->SetRotateRad(&rot);
                    }
                    PlaySePos(0x460, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    SE_Stop(0, 0.0f);
                    m_nFlag |= 4;
                    StopTailEffect();
                    m_f404 = 0.0f;
                    float frame = 60.0f;
                    m_f408 = frame;
                    m_f360 = 3.0f * frame;
                    m_nVolleyFlag |= 0x40000;
                    CGame::SendMessage(0x5A, m_nShotChrIdx, 0, 0);
                    m_n3F8++;
                    return;
                }
                break;
            case 1:
                if (m_f404 < m_f408) {
                    m_f404 += m_fMoveRate;
                }
                if (m_f404 >= m_f408) {
                    long nChr = m_nShotChrIdx;
                    if (nChr >= 0) {
                        CChrCore *pChr = CChrMan::GetChrCore(nChr);
                        if (pChr != 0 && !pChr->IsDisable()) {
                            bcs::FVEC3 pos = m_vTargetPos;
                            if (pChr->GetVolleyTargetMarkerPos(pos, 0)) {
                                bcs::FVEC3Sub(&pos, &pos, &m_vTargetPos);
                                m_Pos.x += pos.x;
                                m_Pos.z += pos.z;
                                m_nFlag |= 0x40;
                                m_PosD4 = m_Pos;
                            }
                        }
                        CGame::SendMessage(0x5B, m_nShotChrIdx, 0, 0);
                    }
                    bcs::FVEC3 rot;
                    GetFlightRot_(rot);
                    unsigned long hEffect = Eff_CreateAttach(8, 0x5E7, 0, 0, 3);
                    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                    if (pCluster != 0) {
                        pCluster->SetRotateRad(&rot);
                    }
                    CGameSnd::Play(0x461, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    m_f404 = 0.0f;
                    float frame = 13.0f;
                    m_f408 = frame;
                    m_f360 = 3.0f * frame;
                    m_n3F8++;
                    return;
                }
                break;
            case 2:
                if (m_f404 < m_f408) {
                    m_f404 += m_fMoveRate;
                }
                if (m_f404 >= m_f408) {
                    bcs::FVEC3 rot;
                    GetFlightRot_(rot);
                    Eff_Stop(8);
                    unsigned long hEffect = CEffectCtrl::CreateEffect(0x5E8, 3, 0, 0, 0);
                    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                    if (pCluster != 0) {
                        pCluster->SetTranslate(&m_Pos);
                        pCluster->SetRotateRad(&rot);
                    }
                    m_f404 = 0.0f;
                    float frame = 15.0f;
                    m_f408 = frame;
                    m_f360 = 3.0f * frame;
                    m_n3F8++;
                    return;
                }
                break;
            case 3:
                if (m_f404 < m_f408) {
                    m_f404 += m_fMoveRate;
                }
                if (m_f404 >= m_f408) {
                    m_nVolleyFlag &= ~0x40000;
                    m_nFlag &= ~4;
                    const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(m_nArtsKind, m_n32C);
                    if (pTbl != 0) {
                        SetArtsTailEffect__(pTbl->m_aEfct[2]);
                    }
                    SE_PlayMove(0, 0x45D, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    m_f404 = 0.0f;
                    m_f408 = 0.0f;
                    m_f360 = 0.0f;
                    GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
                    m_vTargetPos.y = 0.0f;
                    UpdateCameraTarget_(1);
                    m_n3F8++;
                }
                break;
        }
    }

    void CBallVolley::CtorStateJumpBall()
    {
    }

    // The jump ball of the volley game is a serve by the first character.
    void CBallVolley::LoopStateJumpBall()
    {
        if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
            return;
        }
        CChrCore *pChr = CChrMan::GetChrCore(0);
        long nCourt = 0;
        bcs::FVEC3 pos(2.5f, 0.0f, 8.0f);
        if (pChr != 0) {
            const bcs::FVEC3 &rChrPos = pChr->GetPos();
            pos.x = rChrPos.x;
            pos.z = rChrPos.z;
            pos.y = rChrPos.y + 0.8f;
            nCourt = pChr->GetGuardCourtIdx();
        }
        BALL::VOLLEY::SERVE_SHOT_ARG arg;
        arg.m_Pos = pos;
        arg.m_nUnk0C = 0;
        arg.m_nUnk10 = 0;
        arg.m_nPlayerNo = 1;
        arg.m_nTeamNo = 1;
        arg.m_nCourt = nCourt;
        arg.m_nType = 1;
        arg.m_nUnk24 = -1;
        arg.m_fUnk28 = 0.0f;
        arg.m_nSe = 0xC81;
        arg.m_nEffect = -1;
        arg.m_nUnk34 = 1;
        arg.m_fUnk38 = 26.0f;
        arg.m_fUnk3C = 20.0f;
        arg.m_fUnk40 = 0.6f;
        arg.m_fUnk44 = 0.0f;
        arg.m_fUnk48 = 30.0f;
        arg.m_fUnk4C = 4.7f;
        arg.m_fUnk50 = 4.7f;
        arg.m_fUnk54 = 100.0f;
        arg.m_fUnk58 = 10.0f;
        arg.m_nUnk5C = 0;
        SetVolleyServeShotMode(arg);
    }

    void CBallVolley::DtorStateJumpBall()
    {
    }

    void CBallVolley::CtorStateTouchDown()
    {
        m_nSpinType = 0;
        m_f350 = 3.0f;
        m_f35C = 0.0f;
        m_f360 = 0.0f;
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        m_fSpinCount = 0.0f;
        m_vSpinAccel.x = 0.0f;
        m_vSpinAccel.y = 0.0f;
        m_vSpinAccel.z = 0.0f;
        bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
        if (m_f350 > 0.0f) {
            m_nStateStep = 0;
            SetSquashParam(up, 1.0f, m_f350, 2.0f, 6.0f);
        } else {
            m_nStateStep = 1;
            SetSquashParam(up, 1.0f, 1.0f, 2.0f, 6.0f);
        }
    }

    // The velocity reflected off a surface with normal rNormal.
    inline void CBallVolley::Reflect_(bcs::FVEC3 &rOut, const bcs::FVEC3 &rNormal) const
    {
        float dot = 2.0f * bcs::FVEC3Dot(&m_Velocity, &rNormal);
        bcs::FVEC3 reflect;
        bcs::FVEC3Scale(&reflect, &rNormal, dot);
        bcs::FVEC3Sub(&rOut, &m_Velocity, &reflect);
    }

    // (name guessed) The ball is loose: the game goes into loose ball mode
    // and a ball that was passed or shot goes back to free move.
    inline void CBallVolley::SetLooseBall_()
    {
        EffectStopTail_();
        CGame::SetLooseBallMode(1);
        if (TrgItemEffect_(4) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        m_nFlag |= 0x3000;
        long state = GetState();
        if (state >= 3 && state <= 9) {
            ChangeState(1);
        }
    }

    void CBallVolley::LoopStateTouchDown()
    {
        int bEnd = 0;
        switch (m_nStateStep) {
            case 0:
                if (m_f350 <= 0.0f) {
                    if (CGame::IsReqArtsEvent() || CGame::IsArtsEventMode()) {
                        break;
                    }
                    m_nStateStep++;
                } else {
                    m_f350 -= m_fMoveRate;
                }
                if (m_pMdlObj != 0 && m_n1E0 > 0) {
                    float squash;
                    switch (m_n1E0) {
                        case 3:
                            squash = (1.0f - m_f1E4) * m_f1DC;
                            break;
                        case 2:
                            squash = m_f1DC;
                            break;
                        default:
                            squash = m_f1E4 * m_f1DC;
                            break;
                    }
                    m_pMdlObj->m_vTransOffset.y = -1.0f * (squash * GetBgCollRadius());
                }
                break;
            case 1: {
                bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
                if (m_Velocity.y < -0.0375f) {
                    PlayFloorHitSe(1);
                    VelReduce_(CBallMan::GetFloorBoundDecRateXZ());
                    m_Velocity.y *= CBallMan::GetFloorBoundDecRateY();
                } else {
                    VelReduce_(CBallMan::GetFloorRoundDecRateXZ());
                    m_Velocity.y = 0.0f;
                }
                Reflect_(m_Velocity, up);
                if (m_pMdlObj != 0) {
                    m_pMdlObj->m_vTransOffset.Set(0.0f, 0.0f, 0.0f);
                }
                TrgHitFloor_(0);
                ClearSpin_();
                m_nSpikeType = 6;
                m_f320 = 0.0f;
                m_f324 = 0.0f;
                m_nFlag &= ~0x40;
                SetBlinkDisable();
                RotReduce_(0.95f);
                SetLooseBall_();
                bEnd = 1;
                m_nStateStep++;
                break;
            }
            default:
                bEnd = 1;
                break;
        }
        RotNormal_();
        if (bEnd) {
            ChangeState(1);
        }
    }

    void CBallVolley::DtorStateTouchDown()
    {
        m_f350 = 0.0f;
        if (m_pMdlObj != 0) {
            m_pMdlObj->m_vTransOffset.Set(0.0f, 0.0f, 0.0f);
        }
    }

    void CBallVolley::CheckCollision_()
    {
        bcs::FVEC3 pos;
        bcs::FVEC3 normal;
        CollSubCheckTgtPos_();
        int hit = CollSubCheckNet_(pos, normal);
        if (hit == 0) {
            CollSubCheckDef_(pos, normal);
        }
        if (hit == 0) {
            hit = CollSubCheckBG_(pos, normal);
        }
        if (hit == 0) {
            m_nFlag &= ~0x80;
            return;
        }
        int bWall = 0;
        m_nFlag &= ~0x40;
        m_Pos = pos;
        switch (hit) {
            case 2: {
                // The net: the ball goes back to the side it bounces to.
                bcs::FVEC3 vel;
                Reflect_(vel, normal);
                long nTeam;
                if (vel.z < 0.0f) {
                    nTeam = CGame::GetTargetGoalIdx(0) == 0;
                } else {
                    nTeam = CGame::GetTargetGoalIdx(0) != 0;
                }
                CGame::SetOffenseTeam(nTeam);
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x17), nTeam, 0, 0, 0);
                m_nVolleyFlag &= ~0x400;
                m_f360 = 10.0f;
                m_n34C = 0;
            }
            case 1:
                ClearSpin_();
                m_nSpikeType = 6;
                m_f320 = 0.0f;
                m_f324 = 0.0f;
                if (!(m_nFlag & 0x80)) {
                    if (hit == 2) {
                        long nSe = CStageMan::GetInstance()->GetGoalSE(CStageMan::GOAL_SE_TYPE(1));
                        if (nSe >= 0) {
                            PlaySePos(nSe, &pos, 1, -1, 0.0f, 1.0f, 0.0f);
                        }
                        CEffectCtrl::SimpleEffect(0x4EE, pos, 3, 0, 0);
                        bcs::FVEC3 dir = m_Velocity;
                        if (bcs::FVEC3LenSq(&dir) != 0.0f) {
                            bcs::BCS_Vec_Normalize(dir, dir);
                            CStageMan::GetInstance()->SetNetAnm(pos, dir);
                        }
                    } else {
                        PlayWallHitSe(1);
                    }
                    m_nFlag |= 0x80;
                }
                VelReduce_(CBallMan::GetWallBoundDecRate());
                if (hit == 2) {
                    if (!(m_Pos.y <= 1.0f)) {
                        m_Velocity.y = 0.0f;
                        m_fGravity = GetGravityDefault();
                    }
                } else {
                    m_fGravity = GetGravityDefault();
                }
                RotReduce_(0.95f);
                bWall = 1;
                break;
        }
        SetBlinkDisable();
        Reflect_(m_Velocity, normal);
        SetLooseBall_();
        if (hit == 2) {
            m_nVolleyFlag |= 0x4000;
        }
        if (bWall) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
            m_vTargetPos.y = 0.0f;
            UpdateCameraTarget_(0);
        }
        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x87), hit == 2, 0, 0, 0);
    }

    // Changes the attacking team when a ball heading over the net has
    // crossed it.
    void CBallVolley::CollSubCheckTgtPos_()
    {
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        // dead comparisons: the original's literal pool has -6.5 and 6.5
        // here, most likely from a function the module link stripped.
        (void)(netPos.x == -6.5f);
        (void)(netPos.x == 6.5f);
        if (CGame::g_GameWork.m_nFlag18 & 8) {
            return;
        }
        if (m_vTargetPos.z < netPos.z) {
            if (m_Pos.z < netPos.z) {
                long nTeam = CGame::GetTeamIdxByGoalIdx(1);
                if (nTeam != CGame::g_GameWork.GetUserTeam()) {
                    CGame::SetOffenseTeam(nTeam);
                    CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x17), nTeam, 0, 0, 0);
                }
            }
        } else if (m_Pos.z >= netPos.z) {
            long nTeam = CGame::GetTeamIdxByGoalIdx(0);
            if (nTeam != CGame::g_GameWork.GetUserTeam()) {
                CGame::SetOffenseTeam(nTeam);
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x17), nTeam, 0, 0, 0);
            }
        }
    }

    // The net: 2 when the ball hits it this frame (rPos, rNormal), 0
    // otherwise. A ball passing under it or beside it is marked.
    int CBallVolley::CollSubCheckNet_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal)
    {
        if (m_nVolleyFlag & 0x2000) {
            return 0;
        }
        CStageMan *pStage = CStageMan::GetInstance();
        bcs::FVEC3 netPos = pStage->GetNetPos();
        float top = pStage->GetNetHeightTop();
        float bottom = pStage->GetNetHeightBottom();
        int bAlways = 0;
        if (IsPassMode()) {
            bAlways = 1;
            top += 0.8f;
        } else if (IsFreeMoveMode() && !IsScored() && m_bArtsOver == 0) {
            bAlways = 1;
            top += 0.8f;
        }
        if (bottom > top) {
            float t = top;
            top = bottom;
            bottom = t;
        }
        bcs::FVEC4 rect;
        pStage->GetGroundRect(&rect.x, &rect.y, &rect.z, &rect.w);
        float minX = rect.x;
        float maxX = rect.y;

        bcs::FVEC3 normal;
        bcs::FVEC3 point;
        bcs::FVEC3 cross;
        float t;

        normal.x = 0.0f;
        normal.y = 0.0f;
        normal.z = -1.0f;
        {
            bcs::FVEC3 ofs;
            bcs::FVEC3Scale(&ofs, &normal, 0.04f);
            bcs::FVEC3Add(&point, &netPos, &ofs);
        }
        t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, normal, point, m_PrevPos, m_Pos);
        if (t > 0.0f && t < 1.0f) {
            if (bAlways || (cross.x >= minX && cross.x <= maxX && cross.y >= bottom && cross.y <= top)) {
                rPos = cross;
                rNormal = normal;
                m_Pos = cross;
                return 2;
            }
            if (cross.x >= minX && cross.x <= maxX && cross.y <= bottom) {
                m_nVolleyFlag |= 8;
            } else {
                m_nVolleyFlag |= 4;
                if (m_n330 >= 0 && m_n334 >= 0 && !(m_nVolleyFlag & 0x1000) &&
                    CGame::GetTeamIdxByGoalIdx(0) != m_n334) {
                    CGame::SendMessage(0x57, m_n330, (m_nSpikeFlag >> 1) & 1, 0);
                    m_nVolleyFlag |= 0x1000;
                }
            }
        }

        normal.x = 0.0f;
        normal.y = 0.0f;
        normal.z = 1.0f;
        {
            bcs::FVEC3 ofs;
            bcs::FVEC3Scale(&ofs, &normal, 0.04f);
            bcs::FVEC3Add(&point, &netPos, &ofs);
        }
        t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, normal, point, m_PrevPos, m_Pos);
        if (t > 0.0f && t < 1.0f) {
            if (bAlways || (cross.x >= minX && cross.x <= maxX && cross.y >= bottom && cross.y <= top)) {
                rPos = cross;
                rNormal = normal;
                m_Pos = cross;
                return 2;
            }
            if (cross.x >= minX && cross.x <= maxX && cross.y <= bottom) {
                m_nVolleyFlag |= 8;
            } else {
                m_nVolleyFlag |= 4;
                if (m_n330 >= 0 && m_n334 >= 0 && !(m_nVolleyFlag & 0x1000) &&
                    CGame::GetTeamIdxByGoalIdx(1) != m_n334) {
                    CGame::SendMessage(0x57, m_n330, (m_nSpikeFlag >> 1) & 1, 0);
                    m_nVolleyFlag |= 0x1000;
                }
            }
        }
        return 0;
    }

    // rOut = rTo, or where the flight from rFrom to rTo crosses the net.
    void CBallVolley::CollCheckPassHitNet_(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo) const
    {
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        bcs::FVEC3 from = rFrom;
        bcs::FVEC3 to = rTo;
        rOut = to;
        bcs::FVEC3 normal;
        bcs::FVEC3 cross;
        float t;

        normal.Set(0.0f, 0.0f, -1.0f);
        bcs::FVEC3 point(netPos.x, netPos.y, netPos.z - 0.01f);
        t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, normal, point, from, to);
        if (t > 0.0f && t < 1.0f) {
            rOut.x = cross.x;
            rOut.z = cross.z;
        }

        normal.Set(0.0f, 0.0f, 1.0f);
        bcs::FVEC3 point2(netPos.x, netPos.y, 0.01f + netPos.z);
        t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, normal, point2, from, to);
        if (t > 0.0f && t < 1.0f) {
            rOut.x = cross.x;
            rOut.z = cross.z;
        }
    }

    // A block (or another defence volume) in the ball's way.
    void CBallVolley::CollSubCheckDef_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal)
    {
        if (m_n34C == 0) {
            return;
        }
        const CCollDefUnit *pDef = CCollMan::CheckBallReflect(m_PrevPos, m_Pos, GetRadius(), rPos, rNormal);
        if (pDef == 0) {
            return;
        }
        if (!(m_n34C & (1 << pDef->GetOwnerKind()))) {
            return;
        }
        unsigned long ret = pDef->DoReflectHitCallback(this, rPos, rNormal);
        if (ret == 0) {
            return;
        }
        unsigned long nKind = 2;
        long nTeam = m_nShotTeamIdx;
        unsigned long nOutline = 1;
        long nChr = m_nShotChrIdx;
        long nSkill = 0;
        int bChr = 0;
        long nChrId = -1;
        long nSe = -1;
        long nEffect = -1;
        if (pDef->GetOwnerKind() == 1) {
            nChr = (ret >> 8) & 0xFF;
            nKind = (ret >> 16) & 0xFF;
            nTeam = pDef->GetTeamNo();
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr != 0 && pChr->IsEnable()) {
                nOutline = pChr->PlayGetOutline_(1);
                nSkill = pChr->GetPrmSkillType();
                bChr = 1;
                nChrId = pChr->GetChrTypeID();
            }
        } else if (pDef->GetOwnerKind() == 5) {
            nKind = 1;
            switch ((ret >> 8) & 0xFF) {
                case 1:
                    nKind = 5;
                    nSe = 0xD15;
                    nEffect = 0x6C1;
                    break;
                case 4:
                    nKind = 5;
                    nSe = 0xC2F;
                    nEffect = 0xC;
                    break;
                case 7:
                    nKind = 5;
                    nSe = 0xC2F;
                    nEffect = 0xC;
                    break;
            }
        }
        unsigned long nReaction = GetVolleyBlockReaction(nKind, nSkill);
        if (nKind == 4) {
            nReaction = 0;
        }
        if (nReaction & 1) {
            // The ball goes through.
            if (nKind == 5) {
                if (!(m_nVolleyFlag & 0x20)) {
                    if (nSe >= 0) {
                        PlaySePos(nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    }
                    if (nEffect >= 0) {
                        CEffectCtrl::SimpleEffect(nEffect, rPos, 3, 0, 0);
                    }
                    m_nVolleyFlag |= 0x20;
                }
            } else {
                m_nVolleyFlag &= ~0x20;
                if (IsArtsShootMode()) {
                    const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_();
                    if (pTbl != 0) {
                        bcs::FVEC3 pos = rPos;
                        SetArtsTouchdownEffect_(pTbl->m_aEfct[6], pos);
                        if (pTbl->m_anUnk50[5] >= 0) {
                            PlaySePos(pTbl->m_anUnk50[5], rPos, nOutline, -1, 0.0f, 1.0f, 0.0f);
                        }
                        CGame::StartCameraQuake(pTbl->m_anUnk68[2], -1, -1);
                    }
                }
            }
        } else {
            m_n34C = 0;
            SetPos(rPos);
            int bReflect = 1;
            if (IsArtsShootMode()) {
                TrgVolleyArtsOver(3, nChr, 1);
                bReflect = 0;
            }
            if (IsItemBall()) {
                if (TrgItemEffect_(0xB) & 0x10) {
                    m_nVolleyFlag |= 2;
                }
                m_n358 = 0x4DA;
                AttachTailEffect_();
            }
            if (nReaction & 2) {
                nKind = 2;
            }
            if (nReaction & 8) {
                nKind = 1;
            }
            DoBlockReflect_(nKind, nChrId, nChr, nTeam, rNormal, nSe, nEffect, bReflect, nOutline, bChr);
        }
    }

    // What a block of kinds 0, 3 and 4 (a character's) does: the block
    // effect and sound, the attacker's message and the side change, then
    // the ball goes back steeply.
    inline void CBallVolley::SetBlockNormal_(long nKind, long nChrId, long nChr, long nTeam, const bcs::FVEC3 &rNormal,
                                             int bChr, long &rSe, long &rEffect, long &rVoice, int &rbRot)
    {
        rSe = 0xC94;
        rEffect = -1;
        if ((nKind >= 3 && nKind <= 4) || nKind == 0) {
            rEffect = 0x4EC;
            switch (nChrId) {
                case 2:
                    rEffect = 0x65E;
                    break;
                case 3:
                    rEffect = 0x664;
                    break;
                case 11:
                    rEffect = -1;
                    break;
                case 15:
                    rEffect = -1;
                    break;
                case 9:
                    rEffect = -1;
                    break;
            }
        }
        rbRot = 1;
        if (nChrId >= 0 && nChrId <= 0x18) {
            rSe = -1;
            rVoice = 0xC;
        }
        m_n330 = nChr;
        m_n334 = nTeam;
        if (IsScoreBookCountChr_(nChr)) {
            m_nSpikeFlag |= 2;
        }
        if (CGame::g_GameWork.m_nGameType == 0) {
            m_nVolleyFlag &= ~4;
            if (m_n330 >= 0 && m_n334 >= 0) {
                bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
                long nGoal = CGame::GetTargetGoalIdx(m_n334);
                if ((nGoal == 0 && 0.04f + netPos.z >= m_Pos.z) || (nGoal == 1 && netPos.z - 0.04f <= m_Pos.z)) {
                    CGame::SendMessage(0x57, m_n330, (m_nSpikeFlag >> 1) & 1, 0);
                    m_nVolleyFlag |= 0x1004;
                }
            }
        }
        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x86), 0, 0, 0, 0);
        if (nTeam != m_nShotTeamIdx) {
            m_nShotTeamIdx = nTeam;
            m_nTrueTouchNum = 0;
            m_nTouchNum = 0;
            long nOther = 1 - nTeam;
            CGame::SetOffenseTeam(nOther);
            CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x17), nOther, 0, 0, 0);
        }
        DoBlockAcuteReflect_(rNormal, CGame::GetTargetGoalIdx(nTeam), 0.75f, 0.1f, 99.9f, 18.0f, 15.0f);
        if (bChr) {
            CGame::StartStopEffect(20.0f, 15.0f, CGame::GetGuardGoalIdx(m_nShotTeamIdx));
            CGame::SendMessage(0x51, m_nShotChrIdx, 0, 0);
        }
        m_nVolleyFlag |= 0x800;
    }

    // Clears bit 0x200000 of the receive reactions.
    inline void CBallVolley::ClearReceiveReaction200000_()
    {
        for (long i = 0; i < 5; i++) {
            m_anReceiveReaction[i] &= ~0x200000;
            m_anReceiveReaction3[i] &= ~0x200000;
        }
    }

    // The ball bounced off a block (nKind: 0, 3 and 4 a character's block,
    // 1 and 2 a weak block, 5 a block that sends the ball back over).
    void CBallVolley::DoBlockReflect_(long nKind, long nChrId, long nChr, long nTeam, const bcs::FVEC3 &rNormal,
                                      long nSe, long nEffect, int bReflect, unsigned long nOutline, int bChr)
    {
        long nVoice = -1;
        m_PosD4 = m_Pos;
        int bRot = 0;
        bcs::FVEC3 normal = rNormal;
        m_nShotChrIdx = nChr;
        m_n330 = -1;
        m_n334 = -1;
        m_nVolleyFlag &= ~0x1000;
        if (m_f360 < 1.0f) {
            m_f360 = 1.0f;
        }
        if (nKind != 5) {
            SetSquashParam(rNormal, 1.0f, 0.0f, 2.0f, 4.0f);
        }
        SetBlinkDisable();
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        m_nVolleyFlag &= ~0x180;
        ClearTouchReaction_();
        ClearSpin_();
        m_nSpikeType = 6;
        switch (nKind) {
            case 3:
            case 4:
                SetBlockNormal_(nKind, nChrId, nChr, nTeam, rNormal, bChr, nSe, nEffect, nVoice, bRot);
                if (nChrId == 10 || nChrId == 7) {
                    ClearTouchReaction_();
                    for (long i = 0; i < 5; i++) {
                        if (i != 2) {
                            m_anReceiveReaction[i] |= 0x800000;
                            m_afStaggerFrame[i] = 60.0f;
                        }
                    }
                    for (long i = 0; i < 5; i++) {
                        m_anReceiveReaction3[i] |= 0x40000;
                        if (i != 1) {
                            m_anReceiveReaction3[i] |= 0x103;
                        }
                    }
                    m_n448 = 2;
                    m_nFloorHitSe = 0xC33;
                }
                break;
            case 0:
                SetBlockNormal_(nKind, nChrId, nChr, nTeam, rNormal, bChr, nSe, nEffect, nVoice, bRot);
                break;
            case 5: {
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x87), 2, 0, 0, 0);
                m_nTrueTouchNum = 0;
                m_nShotTeamIdx = 1 - m_nShotTeamIdx;
                long nOther = 1 - m_nShotTeamIdx;
                m_nTouchNum = 0;
                CGame::SetOffenseTeam(nOther);
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x17), nOther, 0, 0, 0);
                Reflect_(m_Velocity, rNormal);
                SetParabolaParam_(0.4f, 0.083333336f, 0.083333336f, 75.0f, 75.0f, 1.4f, 1.4f);
                m_fRotSpeed = 0.27925268f;
                m_n448 = 0;
                m_nFloorHitSe = -1;
                m_nVolleyFlag &= ~0x10;
                m_nVolleyFlag &= ~0x400;
                float fStop = 5.0f;
                if (fStop > 0.0f) {
                    CGame::StartStopEffect(fStop, 15.0f, 0);
                }
                break;
            }
            case 1:
                nEffect = 0x4EB;
                nSe = 0xC95;
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x86), 1, 0, 0, 0);
                if (nTeam != m_nShotTeamIdx) {
                    m_nShotTeamIdx = nTeam;
                    m_nTrueTouchNum = 0;
                    m_nTouchNum = 0;
                    long nOther = 1 - nTeam;
                    CGame::SetOffenseTeam(nOther);
                    CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x17), nOther, 0, 0, 0);
                }
                Reflect_(m_Velocity, rNormal);
                SetParabolaParam_(0.8f, 0.083333336f, 0.10833334f, 40.0f, 55.0f, 0.4f, 0.4f);
                m_fRotSpeed = 0.27925268f;
                m_n448 = 0;
                m_nFloorHitSe = -1;
                m_nVolleyFlag &= ~0x10;
                m_nVolleyFlag |= 0x800;
                ClearReceiveReaction200000_();
                break;
            default:
                nEffect = 0x4EA;
                nSe = 0xC96;
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x86), 2, 0, 0, 0);
                switch (m_nShootType) {
                    case 0: {
                        m_n358 = 0x4DA;
                        CItemBall *pItem = GetItemEffect_();
                        if (pItem != 0) {
                            m_n358 = pItem->GetVolleyPassTailEffect(m_n358, 0, 0);
                        }
                        AttachTailEffect_();
                        break;
                    }
                    case 1:
                    case 3:
                    case 4: {
                        m_n358 = 0x4DA;
                        CItemBall *pItem = GetItemEffect_();
                        if (pItem != 0) {
                            m_n358 = pItem->GetVolleySpikeTailEffect(m_n358, 0);
                        }
                        AttachTailEffect_();
                        break;
                    }
                }
                SetParabolaParam_(0.55f, 0.075f, 0.086666666f, 58.0f, 63.0f, 2.1f, 2.2f);
                m_fRotSpeed = 0.27925268f;
                m_n448 = 0;
                m_nFloorHitSe = -1;
                m_nVolleyFlag &= ~0x10;
                m_nVolleyFlag |= 0x800;
                m_nVolleyFlag &= ~0x400;
                ClearReceiveReaction200000_();
                break;
        }
        if (nEffect >= 0 && bReflect) {
            if (bRot) {
                bcs::FVEC3 rot;
                rot.x = 0.0f;
                rot.y = bcs::ROUND_PI(nw4r::math::Atan2Rad(normal.x, normal.z));
                rot.z = 0.0f;
                unsigned long hEffect = CEffectCtrl::CreateEffect(nEffect, 3, 0, 0, 0);
                CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                if (pCluster != 0) {
                    pCluster->SetTranslate(&m_PosD4);
                    pCluster->SetRotateRad(&rot);
                }
            } else {
                CEffectCtrl::SimpleEffect(nEffect, m_PosD4, 3, 0, 0);
            }
        }
        if (nSe >= 0) {
            PlaySePos(nSe, 0, nOutline, -1, 0.0f, 1.0f, 0.0f);
        }
        if (nVoice >= 0) {
            CGameSnd::PlayWrapAct(nChrId, nVoice, m_Pos, nOutline, -1, 0.0f, 1.0f);
        }
        m_nVolleyFlag &= ~0x20000;
        GetPosAfterFrame(m_vTargetPos, floor(GetTouchDownFrame(GetBgCollRadius())));
        m_vTargetPos.y = 0.0f;
        int bFlag = 1;
        if (m_bArtsOver != 0) {
            bFlag = 0;
        }
        SetCameraBallTargetPos(bFlag, m_vTargetPos);
    }

    // The ball bounces off a block towards the court of goal nGoal: its
    // speed (times fRate, kept between fMin and fMax per second) and an
    // angle down that grows from fAngleNear (at the net) to fAngleFar (at
    // the back of the front zone).
    void CBallVolley::DoBlockAcuteReflect_(const bcs::FVEC3 &rNormal, long nGoal, float fRate, float fMin, float fMax,
                                           float fAngleNear, float fAngleFar)
    {
        float speed = bcs::FVEC3Len(&m_Velocity);
        if (speed <= 0.0f) {
            return;
        }
        speed *= fRate;
        float max = (1.0f / 60.0f) * fMax;
        float min = (1.0f / 60.0f) * fMin;
        speed = speed > max ? max : (speed < min ? min : speed);
        bcs::BCS_Vec_Normalize(m_Velocity, m_Velocity);
        Reflect_(m_Velocity, rNormal);

        float front;
        float back;
        CStageMan::GetInstance()->GetTeamCourtFrontZoneRect(nGoal, 0, 0, &front, &back);
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        float depth = back - netPos.z;
        float dist = m_Pos.z - netPos.z;
        float t;
        if (depth > 0.0f) {
            float r = dist / depth;
            t = r > 1.0f ? 1.0f : (r < 0.0f ? 0.0f : r);
        } else {
            t = 0.0f;
        }
        float angle = t * (fAngleFar - fAngleNear) + fAngleNear;
        if (angle > 0.0f) {
            float add = DegToRad(angle);
            float lenXZ = m_Velocity.x * m_Velocity.x + m_Velocity.z * m_Velocity.z;
            if (lenXZ > 0.0f) {
                lenXZ = nw4r::math::FSqrt(lenXZ);
            }
            float rotY = nw4r::math::Atan2Rad(m_Velocity.x, m_Velocity.z);
            float rotX = nw4r::math::Atan2Rad(-1.0f * m_Velocity.y, lenXZ);
            float r = 1.0f - __fabsf(rotX) / 1.5707964f;
            r = r > 1.0f ? 1.0f : (r < 0.0f ? 0.0f : r);
            add = r * add;
            if (rotX < 0.0f) {
                rotX -= add;
            } else {
                rotX += add;
            }
            rotX = bcs::ROUND_PI(rotX);
            nw4r::math::MTX34 mtx;
            nw4r::math::MTX34 mtxX;
            nw4r::math::MTX34RotXYZRad(&mtx, 0.0f, rotY, 0.0f);
            nw4r::math::MTX34RotXYZRad(&mtxX, rotX, 0.0f, 0.0f);
            PSMTXConcat(mtx.m, mtxX.m, mtx.m);
            m_Velocity.x = 0.0f;
            m_Velocity.y = 0.0f;
            m_Velocity.z = 1.0f;
            nw4r::math::VEC3TransformNormal(reinterpret_cast<nw4r::math::VEC3 *>(&m_Velocity), &mtx,
                                            reinterpret_cast<const nw4r::math::VEC3 *>(&m_Velocity));
        }
        bcs::FVEC3Scale(&m_Velocity, &m_Velocity, speed);
    }

    // A flight from here: the XZ speed of the ball times fRate kept
    // between fMinSpeed and fMaxSpeed, a height of fMinHeight to
    // fMaxHeight reached and fMinFrame to fMaxFrame frames to the floor.
    void CBallVolley::SetParabolaParam_(float fRate, float fMinSpeed, float fMaxSpeed, float fMinFrame, float fMaxFrame,
                                        float fMinHeight, float fMaxHeight)
    {
        float speed = bcs::BCS_Vec_GetLenXZ(m_Velocity) * fRate;
        if (fMinSpeed > fMaxSpeed) {
            float t = fMinSpeed;
            fMinSpeed = fMaxSpeed;
            fMaxSpeed = t;
        }
        if (fMinFrame > fMaxFrame) {
            float t = fMinFrame;
            fMinFrame = fMaxFrame;
            fMaxFrame = t;
        }
        if (fMinHeight > fMaxHeight) {
            float t = fMinHeight;
            fMinHeight = fMaxHeight;
            fMaxHeight = t;
        }
        if (fMinSpeed > 0.0f) {
            speed = fMinSpeed < speed ? speed : fMinSpeed;
        }
        if (fMaxSpeed > 0.0f) {
            speed = fMaxSpeed > speed ? speed : fMaxSpeed;
        }
        NormalizeXZ(m_Velocity, m_Velocity);
        m_Velocity.x *= speed;
        m_Velocity.z *= speed;
        float frame = bcs::BCS_RandRange(fMinFrame, fMaxFrame);
        float height = bcs::BCS_RandRange(fMinHeight, fMaxHeight);
        CalcYVG(m_Pos.y, m_Pos.y + height, GetBgCollRadius(), frame, m_Velocity.y, m_fGravity);
    }

    // An item ball hit straight back by character nChr of team nTeam.
    // Returns the block reaction (0 when the ball is no item ball).
    int CBallVolley::DoVolleyItemBallDirectSpike(long nChr, long nTeam)
    {
        if (!IsItemBall()) {
            return 0;
        }
        if (IsStarBall()) {
            return 0x20;
        }
        if (GetItemBallType() == 3) {
            return 8;
        }
        long nReaction = GetVolleyBlockReaction(2, 0);
        if (TrgItemEffect_(0xB) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        m_PosD4 = m_Pos;
        SetTouch_(nChr, nTeam, 0);
        m_n330 = -1;
        m_n334 = -1;
        SetBlinkDisable();
        m_nVolleyFlag &= ~0x180;
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        ClearTouchReaction_();
        ClearSpin_();
        m_nSpikeType = 6;
        CEffectCtrl::SimpleEffect(0x4EA, m_PosD4, 3, 0, 0);
        PlaySePos(0xC96, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x86), 2, 0, 0, 0);
        switch (m_nShootType) {
            case 0:
            case 1:
            case 3:
            case 4:
                m_n358 = 0x4DA;
                AttachTailEffect_();
                break;
        }
        SetParabolaParam_(0.55f, 0.075f, 0.086666666f, 58.0f, 63.0f, 2.1f, 2.2f);
        m_fRotSpeed = 0.27925268f;
        m_n448 = 0;
        m_nFloorHitSe = -1;
        ClearReceiveReaction200000_();
        m_nVolleyFlag &= ~0x10;
        m_nVolleyFlag |= 0x800;
        m_nVolleyFlag &= ~0x400;
        m_nVolleyFlag &= ~0x20000;
        GetPosAfterFrame(m_vTargetPos, floor(GetTouchDownFrame(GetBgCollRadius())));
        m_vTargetPos.y = 0.0f;
        int bFlag = 1;
        if (m_bArtsOver != 0) {
            bFlag = 0;
        }
        SetCameraBallTargetPos(bFlag, m_vTargetPos);
        return nReaction;
    }

    int CBallVolley::IsScoreBookCountChr_(long nChr)
    {
        if (!CChrMan::IsActive()) {
            return 0;
        }
        if (nChr < 0 || nChr >= CChrMan::GetChrCoreNum()) {
            return 0;
        }
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr == 0) {
            return 0;
        }
        if (pChr->IsDisable()) {
            return 0;
        }
        return pChr->IsScoreBookCountChr_();
    }

    // The floor and the walls: 1 when the ball hits a wall (rPos,
    // rNormal), 0 otherwise. A ball reaching the floor bounces or, when it
    // can, touches down.
    int CBallVolley::CollSubCheckBG_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal)
    {
        if (GetState() == 12) {
            return 0;
        }
        float radius = GetBgCollRadius();
        bcs::FVEC3 flow(0.0f, 0.0f, 0.0f);
        unsigned short bHit = 0;
        int ret = 0;
        if (!(m_nVolleyFlag & 0x8000) && m_Pos.y <= radius && m_Velocity.y <= 0.0f) {
            unsigned long attr;
            CCollMan::GetGrdAttr(m_Pos, attr, flow);
            if (CCollMan::IsGrdAttrFlow(attr)) {
                if (!CCollMan::IsGrdMove(m_Pos)) {
                    RollBall_(flow);
                }
                m_Pos.x += flow.x;
                m_Pos.z += flow.z;
            } else {
                flow.Set(0.0f, 0.0f, 0.0f);
            }
        }
        if (m_nVolleyFlag & 1) {
            bcs::CScnObj *pObj = bcs::CScnObj::GetScnObj(m_nRideObjHandle);
            if (pObj == 0) {
                m_nRideObjHandle = 0;
                m_nVolleyFlag &= ~1;
            } else {
                bcs::FVEC3 objPos = pObj->m_vTrans;
                bcs::FVEC3 move;
                move.x = objPos.x - m_vRideObjPos.x;
                move.z = objPos.z - m_vRideObjPos.z;
                m_Pos.x += move.x;
                m_Pos.z += move.z;
                m_PrevPos.x += move.x;
                m_PrevPos.z += move.z;
                m_vRideObjPos = objPos;
            }
        }
        bcs::FVEC3 hitPos;
        float time;
        if (CCollMan::DetectMapHit(hitPos, rNormal, time, m_PrevPos, m_Pos, 0.0f, 2, 0)) {
            bHit |= 1;
        }
        if (m_nFlag & 0x40) {
            if (!bHit) {
                m_nFlag &= ~0x40;
            } else {
                hitPos = m_Pos;
                bHit = 0;
            }
        }
        if (!(!IsScored() && m_bArtsOver == 0) && (long)CGame::g_GameWork.m_nStage == 5) {
            hitPos = m_Pos;
            bHit = 0;
        }
        int bBound = 1;
        int bWater = 0;
        if (m_nVolleyFlag & 0x8000) {
            hitPos = m_Pos;
            bHit = 0;
            bBound = 0;
            bWater = 1;
        } else if (hitPos.y < radius && m_Velocity.y < 0.0f &&
                   (unsigned long)CStageMan::GetInstance()->GetFloorAttribute(hitPos, CStageMan::FLOW_TYPE(6)) == 8) {
            bBound = 0;
            bWater = 1;
        }
        if (bHit) {
            SetSquashParam(rNormal, 1.0f, 0.0f, 0.0f, 6.0f);
            hitPos.y = hitPos.y < radius ? radius : hitPos.y;
            ret = 1;
            rPos = hitPos;
            if (hitPos.y <= radius && m_Velocity.y < 0.0f) {
                if (m_Velocity.y >= -0.0375f) {
                    m_Velocity.y = 0.0f;
                } else {
                    m_Velocity.y *= CBallMan::GetFloorBoundDecRateY();
                }
            }
        } else if (m_Pos.y <= radius && m_Velocity.y < 0.0f && bBound) {
            if (m_Pos.y <= radius && m_Velocity.y < 0.0f && m_fMoveRate > 0.0f) {
                m_PrevPos.x += flow.x;
                m_PrevPos.z += flow.z;
                rNormal.Set(0.0f, 1.0f, 0.0f);
                bcs::FVEC3 floorPoint(0.0f, radius, 0.0f);
                bcs::FVEC3 cross;
                float t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, rNormal, floorPoint, m_PrevPos,
                                                                           m_Pos);
                if (t >= 0.0f && t <= 1.0f) {
                    m_Velocity = m_PrevVel;
                    m_Pos = m_PrevPos;
                    FuncMove_(m_Pos, m_Velocity, m_fGravity, t * m_fMoveRate);
                    if (CanbeTouchDown_()) {
                        if (m_Pos.y < radius) {
                            m_Pos.y = radius;
                        }
                        ChangeState(12);
                        return 0;
                    }
                    TrgHitFloor_(0);
                    if (m_Velocity.y < -0.0375f) {
                        PlayFloorHitSe(1);
                        VelReduce_(CBallMan::GetFloorBoundDecRateXZ());
                        m_Velocity.y *= CBallMan::GetFloorBoundDecRateY();
                    } else {
                        VelReduce_(CBallMan::GetFloorRoundDecRateXZ());
                        m_Velocity.y = 0.0f;
                    }
                    Reflect_(m_Velocity, rNormal);
                }
            }
            if (m_Pos.y < radius) {
                m_Pos.y = radius;
            }
            ClearSpin_();
            m_nSpikeType = 6;
            m_f320 = 0.0f;
            m_f324 = 0.0f;
            m_nFlag &= ~0x40;
            SetBlinkDisable();
            RotReduce_(0.95f);
            SetLooseBall_();
            ret = 0;
        } else if (bWater) {
            // Into the water (the ball sinks; stage 5 splashes).
            if (!(m_nVolleyFlag & 0x8000)) {
                m_nVolleyFlag |= 0x8000;
                m_nFlag |= 0x200000;
                TrgHitFloor_(1);
            }
            ret = 0;
            m_f320 = 0.0f;
            m_f324 = 0.0f;
            m_nFlag &= ~0x40;
            if (!(m_nVolleyFlag & 0x10000)) {
                long nEffect = -1;
                long nSe = -1;
                if ((long)CGame::g_GameWork.m_nStage == 5) {
                    nEffect = 0x14D;
                    nSe = 0xD2A;
                }
                if (m_Pos.y <= -3.0f) {
                    m_nVolleyFlag |= 0x10000;
                    if (nEffect >= 0) {
                        bcs::FVEC3 pos(m_Pos.x, -3.0f, m_Pos.z);
                        Eff_CreateSimple(nEffect, pos, 3, 0);
                    }
                    if (nSe >= 0) {
                        PlaySePos(nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    }
                }
            }
        }
        return ret;
    }

    int CBallVolley::CanbeTouchDown_()
    {
        if (!IsScored() && GetState() != 12) {
            return 1;
        }
        return 0;
    }

    // The ball reached the floor (bWater: the water of stage 5): a point
    // for the side it fell in, or for the other side when it fell out.
    void CBallVolley::TrgHitFloor_(int bWater)
    {
        if (IsScored()) {
            return;
        }
        if (m_bArtsOver != 0) {
            return;
        }
        if (bWater == 0) {
            ClearVolleyChargePower();
        }
        long nChr = m_nShotChrIdx;
        if (nChr < 0 || nChr >= CChrMan::GetChrCoreNum()) {
            return;
        }
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr == 0) {
            return;
        }
        if (pChr->IsDisable()) {
            return;
        }
        long nCourt = pChr->GetTargetCourtIdx();
        bcs::FVEC4 rect;
        Util_GetCourtRect_(nCourt, rect, 1);
        if (m_Pos.x >= rect.x && m_Pos.x <= rect.y && m_Pos.z >= rect.z && m_Pos.z <= rect.w &&
            !(m_nVolleyFlag & 8)) {
            TrgGoalIn_(0, 0, bWater);
            return;
        }
        CChrCore *pNext = CChrMan::GetChrCore((nChr + 1) % CChrMan::GetChrCoreNum());
        int bOut = 1;
        m_nShotTeamIdx = pNext->GetTeamNo();
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        int bOwnSide;
        if (nCourt != 0) {
            if (m_Pos.z <= netPos.z && !(m_nVolleyFlag & 8)) {
                bOwnSide = 1;
            } else {
                bOwnSide = 0;
            }
        } else {
            if (m_Pos.z >= netPos.z && !(m_nVolleyFlag & 8)) {
                bOwnSide = 1;
            } else {
                bOwnSide = 0;
            }
        }
        if (bOwnSide) {
            bcs::FVEC4 rect2;
            Util_GetCourtRect_(nCourt, rect2, 1);
            if (m_Pos.x >= rect2.x && m_Pos.x <= rect2.y && m_Pos.z >= rect2.z && m_Pos.z <= rect2.w &&
                !(m_nVolleyFlag & 8)) {
                bOut = 0;
            }
        } else {
            bOut = 0;
            if ((m_nVolleyFlag & 4) && (m_nVolleyFlag & 8)) {
                bOut = 1;
            }
        }
        TrgGoalIn_(1, bOut, bWater);
    }

    // The court of nCourt (x: left, y: right, z: front, w: back), with a
    // margin of a ball around the lines when bMargin is set.
    void CBallVolley::Util_GetCourtRect_(long nCourt, bcs::FVEC4 &rRect, int bMargin)
    {
        float left;
        float right;
        float front;
        float back;
        CStageMan::GetInstance()->GetTeamCourtRect(nCourt, &left, &right, &front, &back);
        if (left > right) {
            float t = left;
            left = right;
            right = t;
        }
        if (front > back) {
            float t = back;
            back = front;
            front = t;
        }
        if (bMargin) {
            float margin;
            if (CGame::g_GameWork.m_nStage == 5) {
                margin = 0.0f;
            } else {
                margin = 0.25f;
            }
            if (margin > 0.0f) {
                left -= margin;
                right += margin;
                bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
                if (netPos.z <= front) {
                    back += margin;
                } else if (netPos.z >= back) {
                    front -= margin;
                } else {
                    front -= margin;
                    back += margin;
                }
            }
        }
        rRect.x = left;
        rRect.y = right;
        rRect.z = front;
        rRect.w = back;
    }

    // rOut = rPos, moved inside the court lines when it is on the margin
    // of one of the two courts.
    void CBallVolley::Util_GetModifyTouchDownPos_(const bcs::FVEC3 &rPos, bcs::FVEC3 &rOut)
    {
        rOut = rPos;
        for (long i = 0; i < 2; i++) {
            bcs::FVEC4 rect;
            Util_GetCourtRect_(i, rect, 1);
            if (rPos.x >= rect.x && rPos.x <= rect.y && rPos.z >= rect.z && rPos.z <= rect.w) {
                Util_GetCourtRect_(i, rect, 0);
                rOut.x = rOut.x > rect.y ? rect.y : (rOut.x < rect.x ? rect.x : rOut.x);
                rOut.z = rOut.z > rect.w ? rect.w : (rOut.z < rect.z ? rect.z : rOut.z);
                return;
            }
        }
    }

    // The speed, the height rate and the frames of a pass of kind nKind,
    // by the game level.
    void CBallVolley::Util_GetPassParam_(long nKind, float &rSpeed, float &rRate, float &rFrame)
    {
        long nLevel = CGame::GetGameLevel();
        switch (nKind) {
            case 2:
                rSpeed = 0.15f;
                rRate = 0.6f;
                rFrame = 35.0f;
                return;
            case 1:
            case 4:
                switch (nLevel) {
                    case 0:
                        rSpeed = 0.116666675f;
                        rRate = 0.7f;
                        rFrame = 80.0f;
                        return;
                    case 1:
                        rSpeed = 0.116666675f;
                        rRate = 0.7f;
                        rFrame = 70.0f;
                        return;
                    case 3:
                        rSpeed = 0.116666675f;
                        rRate = 0.7f;
                        rFrame = 70.0f;
                        return;
                    default:
                        rSpeed = 0.116666675f;
                        rRate = 0.7f;
                        rFrame = 70.0f;
                        return;
                }
                break;
            default:
                switch (nLevel) {
                    case 0:
                        rSpeed = 0.13333334f;
                        rRate = 1.0f;
                        rFrame = 80.0f;
                        return;
                    case 1:
                        rSpeed = 0.13333334f;
                        rRate = 1.0f;
                        rFrame = 75.0f;
                        return;
                    case 3:
                        rSpeed = 0.13333334f;
                        rRate = 1.0f;
                        rFrame = 75.0f;
                        return;
                    default:
                        rSpeed = 0.13333334f;
                        rRate = 1.0f;
                        rFrame = 75.0f;
                        return;
                }
                break;
        }
    }

    int CBallVolley::GetColorReg0Color_(bcs::BCS_Color &rColor)
    {
        rColor = 0xFF000000;
        return 0;
    }

    int CBallVolley::GetColorAmbientColor_(bcs::BCS_Color &rColor)
    {
        rColor = 0x00000000;
        return 0;
    }

    void CBallVolley::TrgGoalIn(long nChr, long nTeam, long, long, long nShootType)
    {
        if (m_bArtsOver != 0) {
            return;
        }
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = nTeam;
        m_nShootType = nShootType;
        TrgGoalIn_(0, 0, 0);
    }

    // The point: bOut when the ball fell out (bWater: into the water).
    void CBallVolley::TrgGoalIn_(int bFall, int bOut, int bWater)
    {
        if (bWater == 0) {
            if (TrgItemEffect_(3) & 0x10) {
                m_nVolleyFlag |= 2;
            }
        }
        if (m_bArtsOver != 0) {
            return;
        }
        if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
            return;
        }
        if (!CGame::IsMainLoopState(1)) {
            return;
        }
        if (IsScored()) {
            return;
        }
        long nCoin = 0;
        long nScore = m_n36C;
        int bCoin = 0;
        if (m_nVolleyFlag & 0x400) {
            if (!(m_nVolleyFlag318 & 2) || bFall == 0) {
                bCoin = 1;
            }
            if (bOut) {
                bCoin = 0;
            }
            if (bWater) {
                bCoin = 0;
            }
        }
        if (bCoin) {
            if ((unsigned long)m_nShotTeamIdx <= 3) {
                nCoin = CGame::GetTeamCoin(m_nShotTeamIdx);
            }
        }
        bcs::FVEC3 pos = m_Pos;
        pos.y = 0.0f;
        unsigned long nFlag = m_nVolleyFlag444;
        if (bOut == 0 && !(m_nVolleyFlag & 8)) {
            bcs::FVEC3 floorPos;
            Util_GetModifyTouchDownPos_(pos, floorPos);
            switch (CGimmickMan::FloorHitCheck(floorPos, m_nShotTeamIdx)) {
                case 1:
                    nFlag |= 0x20;
                    break;
                case 3:
                    nFlag |= 0x40;
                    break;
            }
        }
        if (TrgVolleyArtsOver(0, -1, 1) == 0 && bWater == 0) {
            long nEffect = 0x4FF;
            switch (m_nShootType) {
                case 1:
                case 3:
                case 4:
                case 9:
                    nEffect = 0x500;
                    break;
            }
            new (0x100) BALL_VOLLEY::CBallTouchDownMark(CGame::GetTempParent(), nEffect, m_Pos, m_Velocity);
            CEffectCtrl::SimpleEffect(0x4E8, pos, 3, 0, 0);
        }
        if (nCoin > 0) {
            bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
            long n = nCoin - CGame::g_GameWork.m_Unk160;
            if (n > 0) {
                CGimmickMan::EffectCoinScoreCreate(pos, up, n);
            }
        }
        long nShootType = m_nShootType;
        if (m_nVolleyFlag & 0x800) {
            nShootType = 0xB;
        }
        if (bOut) {
            switch (nShootType) {
                case 0:
                    nFlag |= 8;
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 9:
                case 10:
                case 11:
                    nFlag |= 8;
                    break;
            }
        }
        long nPass = 1;
        switch (nShootType) {
            case 1:
            case 3:
            case 4:
                nPass = m_nTrueTouchNum;
                break;
        }
        if (m_nSpikeFlag & 1) {
            nFlag |= 0x80;
        }
        if (bWater) {
            nFlag |= 0x200;
        }
        if (!(m_nVolleyFlag318 & 2) || bFall == 0) {
            CGame::TrgVolleyGoalIn(m_nShotChrIdx, m_nShotTeamIdx, nShootType, nScore, nCoin, nPass, pos, nFlag);
        }
        m_fTimingCount = m_fTimingStart = m_fTimingMid = m_fTimingEnd = 0.0f;
        m_nVolleyFlag &= ~0x1C0;
        m_nFlag |= 0x3001;
        UpdateCameraTarget_(0);
        if (m_n450 >= 0 && bWater == 0) {
            CGame::StartCameraQuake(m_n450, -1, -1);
            m_n450 = -1;
        }
    }

    // The touches of team nTeam (of the attacking team for -1).
    long CBallVolley::GetVolleyTouchNum(long nTeam) const
    {
        long n = 0;
        if (nTeam < 0) {
            if (m_nShotTeamIdx == CGame::g_GameWork.m_nUserTeam) {
                n = m_nTouchNum;
            }
        } else if (m_nShotTeamIdx == nTeam) {
            n = m_nTouchNum;
        }
        return n;
    }

    // How character nChr of team nTeam may touch the ball.
    long CBallVolley::GetVolleyCanbeTouchKind(long nTeam, long nChr)
    {
        long nKind = 3;
        int bLast = 0;
        if (m_nShotTeamIdx == nTeam) {
            if (m_nShotChrIdx != nChr) {
                if (m_nTouchNum >= 3) {
                    bLast = 1;
                }
                if (IsAlleyoopPassMode()) {
                    bLast = 1;
                }
            } else if (!IsAlleyoopPassMode()) {
                bLast = 1;
            }
            if (m_nTouchNum >= 2) {
                nKind |= 0x10000;
            }
        } else {
            nKind = 1;
        }
        if (bLast) {
            nKind |= 0x20000;
        }
        return nKind;
    }

    void CBallVolley::SetTouch_(long nTeam, long nChr, int bForce)
    {
        if (m_nShotTeamIdx == nTeam) {
            if (m_nShotChrIdx != nChr) {
                m_nTouchNum++;
                m_nTrueTouchNum = m_nTouchNum;
            } else if (m_nTouchNum <= 0 && bForce) {
                m_nTouchNum++;
                m_nTrueTouchNum = m_nTouchNum;
            }
        } else {
            m_nTouchNum = 1;
        }
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = nTeam;
    }

    inline void CBallVolley::SetShotChrNoHit_(long nChr)
    {
        unsigned long bit = 1 << nChr;
        m_Unk17C |= bit;
        if (m_pBodyColl != 0) {
            m_pBodyColl->SetTmpNoHitFlag(0, bit);
        }
    }

    inline void CBallVolley::InitFlight_(const bcs::FVEC3 &rTarget)
    {
        ResetVelGravity_();
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        ClearSpin_();
        m_PrevPos = m_Pos;
        m_PosD4 = m_Pos;
        m_vTargetPos = rTarget;
    }

    // The toss of a serve: up from the server's hand (or with the server's
    // model, which the ball then rides on).
    void CBallVolley::SetVolleyServeTossMode(BALL::VOLLEY::SERVE_TOSS_ARG &rArg)
    {
        Detach_();
        CmnShotInit_();
        m_nShootType = 8;
        m_fScale = rArg.m_fScale;
        m_n358 = -1;
        m_nShotChrIdx = rArg.m_nPlayerNo;
        m_nPassChrIdx = rArg.m_nPlayerNo;
        m_nShotTeamIdx = rArg.m_nTeamNo;
        m_nTrueTouchNum = 2;
        m_nTouchNum = 2;
        SetShotChrNoHit_(rArg.m_nPlayerNo);
        m_nFlag |= 0x40;
        InitFlight_(rArg.m_Pos);
        m_vTargetPos.y = rArg.m_fCatchY;
        float frame = rArg.m_fUnk20;
        CalcXZVel(m_Pos, m_vTargetPos, 0.0f, m_Velocity, frame, 0.15f, rArg.m_fUnk20, rArg.m_fUnk20);
        float top = rArg.m_fTopY;
        if (top < m_Pos.y) {
            top = m_Pos.y;
        }
        if (top < m_vTargetPos.y) {
            top = m_vTargetPos.y;
        }
        CalcYVG(m_Pos.y, top, m_vTargetPos.y, frame, m_Velocity.y, m_fGravity);
        m_fRotSpeed = 0.2617994f;
        UpdateCameraTarget_(1);
        if (rArg.m_pMdl == 0) {
            m_nRideObjHandle = 0;
            m_nVolleyFlag &= ~1;
        } else {
            m_nRideObjHandle = rArg.m_pMdl->GetHandle();
            m_vRideObjPos = rArg.m_pMdl->m_vTrans;
            m_nVolleyFlag |= 1;
        }
        float start = Max(0.0f, rArg.m_fUnk28);
        float end = Max(0.0f, rArg.m_fUnk2C);
        SetSpikeTimingWork_(start, start, end);
        ChangeState(5);
    }

    // Every receive of the ball makes the receiver stagger (but the third
    // touch).
    inline void CBallVolley::SetReceiveStagger_()
    {
        for (long i = 0; i < 5; i++) {
            if (i != 2) {
                m_anReceiveReaction[i] |= 0x800000;
                m_afStaggerFrame[i] = 60.0f;
            }
        }
    }

    inline void CBallVolley::SetReceiveReaction3_()
    {
        for (long i = 0; i < 5; i++) {
            m_anReceiveReaction3[i] |= 0x40000;
            if (i != 1) {
                m_anReceiveReaction3[i] |= 0x103;
            }
        }
    }

    // A serve. Returns 0.
    int CBallVolley::SetVolleyServeShotMode(BALL::VOLLEY::SERVE_SHOT_ARG &rArg)
    {
        Detach_();
        CmnShotInit_();
        ClearVolleyChargePower();
        m_nTrueTouchNum = 1;
        m_nTouchNum = 0;
        if (CGame::g_GameWork.m_nGameType == 0) {
            m_nTouchNum = 3;
        }
        m_nShootType = 6;
        m_n34C = 0;
        m_n358 = s_aServeTailEffect[rArg.m_nType];
        m_nShotChrIdx = rArg.m_nPlayerNo;
        m_nShotTeamIdx = rArg.m_nTeamNo;
        m_nServeCourt = rArg.m_nCourt;
        CheckServeSide_();
        SetShotChrNoHit_(m_nShotChrIdx);
        bcs::FVEC3 target = rArg.m_Pos;
        target.y = GetBgCollRadius();
        InitFlight_(target);
        m_n448 = 1;
        m_nFloorHitSe = 0xC32;
        float speed = rArg.m_fUnk3C;
        float minHeight = rArg.m_fUnk4C;
        float maxHeight = rArg.m_fUnk50;
        if (rArg.m_nType >= 2) {
            speed = rArg.m_fUnk38;
            if (rArg.m_nUnk10 == 1) {
                SetReceiveStagger_();
                m_n448 = 2;
                m_nFloorHitSe = 0xC33;
            }
            SetReceiveReaction3_();
            SetReceiveStaggerHard_();
            m_nShootType = 7;
            m_nVolleyFlag |= 0x80;
        }
        speed = (1.0f / 60.0f) * speed;
        for (long i = 0; i < 5; i++) {
            m_anReceiveReaction3[i] |= 0x200000;
            m_anReceiveReaction[i] |= 0x200000;
        }
        float frame;
        float t = CalcXZVel2(m_Pos, target, m_Velocity, m_f320, m_f324, frame, speed, speed * rArg.m_fUnk40,
                             rArg.m_fUnk48, 0.0f, (1.0f / 60.0f) * rArg.m_fUnk44);
        if (minHeight > maxHeight) {
            float h = minHeight;
            minHeight = maxHeight;
            maxHeight = h;
        }
        if (minHeight < maxHeight) {
            minHeight = bcs::BCS_RandRange(minHeight, maxHeight);
        }
        if (t > 0.0f) {
            float frames = ceil(frame);
            bcs::FVEC3 cross;
            if (!CanbeStraightSpike_(m_PosD4, m_vTargetPos, cross)) {
                CalcYVG(m_Pos.y, minHeight, target.y, frame, m_Velocity.y, m_fGravity);
            } else {
                m_fGravity = 0.0f;
                m_Velocity.y = (1.0f / frames) * (target.y - m_Pos.y);
            }
            m_fRotSpeed = 0.27925268f;
            float radius = GetBgCollRadius();
            if (m_vTargetPos.y > radius) {
                GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(radius));
                m_vTargetPos.y = 0.0f;
            }
        }
        UpdateCameraTarget_(1);
        m_fMarkerDispFrame = rArg.m_fUnk54;
        m_fMarkerDispFrame2 = rArg.m_fUnk58;
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_nVolleyFlag |= 0x10;
        if (rArg.m_nSe >= 0) {
            PlaySePos(rArg.m_nSe, 0, rArg.m_nUnk34, -1, 0.0f, 1.0f, 0.0f);
        }
        if (rArg.m_nEffect >= 0) {
            Eff_CreateSimple(rArg.m_nEffect, m_Pos, rArg.m_fUnk28, 3, 0);
        }
        m_nVolleyFlag |= 0x400;
        if (rArg.m_nUnk5C != 0) {
            m_nSpikeFlag |= 1;
        }
        ChangeState(6);
        CGame::SendMessage(0x16, 0, 0, 0);
        return 0;
    }

    // A pass of kind nType from character nChr of team nTeam to rTarget.
    void CBallVolley::SetVolleyPassMode(const bcs::FVEC3 &rTarget, long nType, long nChr, long nTeam, long nPassChr,
                                        float, long nTail, float fHeight, unsigned long)
    {
        int bNetHit = IsVolleyNetHit();
        Detach_();
        CmnShotInit_();
        m_nShootType = 0;
        SetTouch_(nTeam, nChr, bNetHit);
        m_nPassChrIdx = nPassChr;
        long nRow = 1;
        if (m_nTouchNum - 1 <= 1) {
            nRow = m_nTouchNum - 1;
        }
        m_n358 = s_aPassTailEffect[nRow][nTail];
        if (GetVolleyChargePower() >= 1.0f) {
            m_n358 = 0x4F8;
        }
        SetShotChrNoHit_(m_nShotChrIdx);
        float frame = fHeight;
        m_nFlag &= ~0x40;
        m_fPassCounter = 20.0f;
        float speed;
        float rate;
        switch (nType) {
            case 2:
                speed = 0.15f;
                frame = 35.0f;
                rate = 0.6f;
                break;
            case 1: {
                float maxFrame;
                Util_GetPassParam_(nType, speed, rate, maxFrame);
                if (fHeight <= 0.0f) {
                    frame = maxFrame;
                }
                break;
            }
            default: {
                float maxFrame;
                Util_GetPassParam_(nType, speed, rate, maxFrame);
                if (fHeight <= 0.0f) {
                    frame = maxFrame;
                }
                break;
            }
        }
        SetPassModeParam_(rTarget, speed, rate, frame, fHeight);
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_f35C = 8.0f;
        ChangeState(3);
    }

    // A flight to rTarget: fSpeed per frame (fSpeed * fRate at the end), at
    // most fFrame frames; a flat one when fHeight is set. Returns the frames
    // it takes (0 when it goes nowhere).
    float CBallVolley::SetPassModeParam_(const bcs::FVEC3 &rTarget, float fSpeed, float fRate, float fFrame,
                                         float fHeight)
    {
        bcs::FVEC3 target = rTarget;
        float frame = fFrame;
        InitFlight_(target);
        float dist = CalcXZVel2(m_Pos, target, m_Velocity, m_f320, m_f324, frame, fSpeed, fSpeed * fRate, fFrame,
                                fHeight, 0.0f);
        if (dist <= 0.0f && frame <= 0.0f) {
            UpdateCameraTarget_(1);
            return 0.0f;
        }
        float result = frame;
        float top = MapRange(dist, 4.0f, 10.0f, 3.0f, 5.0f);
        float minTop = 5.0f;
        float add = 1.0f;
        if (fHeight > 0.0f) {
            minTop = 0.0f;
            top = 0.0f;
            add *= 0.25f;
        }
        if (m_Pos.y > top) {
            top = m_Pos.y + add;
        }
        if (target.y > top) {
            top = target.y + add;
        }
        if (minTop < top) {
            minTop = top;
        }
        CalcYVG(m_Pos.y, minTop, target.y, frame, m_Velocity.y, m_fGravity);
        m_fRotSpeed = 0.2617994f;
        if (m_vTargetPos.y > 0.0f) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
            m_vTargetPos.y = 0.0f;
        }
        CollCheckPassHitNet_(m_vTargetPos, m_Pos, m_vTargetPos);
        UpdateCameraTarget_(1);
        return result;
    }

    // A pass of kind nType that may go wrong (nBad: how).
    void CBallVolley::SetVolleyPassMode(const bcs::FVEC3 &rTarget, long nType, long nChr, long nTeam, long nPassChr,
                                        long nBad, unsigned long)
    {
        int bNetHit = IsVolleyNetHit();
        Detach_();
        CmnShotInit_();
        m_nShootType = 0;
        SetTouch_(nTeam, nChr, bNetHit);
        m_nPassChrIdx = nPassChr;
        bcs::FVEC3 target = rTarget;
        SetShotChrNoHit_(m_nShotChrIdx);
        m_nFlag &= ~0x40;
        m_fPassCounter = 20.0f;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            nBad = pItem->GetVolleyPassParam(nBad, nType);
        }
        m_n358 = 0x4DD;
        if (nBad > 0) {
            m_n358 = 0x4DA;
        }
        if (GetVolleyChargePower() >= 1.0f) {
            m_n358 = 0x4F8;
        }
        if (pItem != 0) {
            m_n358 = pItem->GetVolleyPassTailEffect(m_n358, nType, nBad);
        }
        PASS_PARAM_ param;
        SetBadPassParam_(nBad, nType, param);
        float fRate = 0.0f;
        if (nBad > 0) {
            CorruptTargetPos_(target, param.m_f10, param.m_f14, param.m_f18, param.m_f1C, fRate, param.m_n30);
            // Kept inside the receiving court, away from the net.
            float front;
            float back;
            CStageMan::GetInstance()->GetTeamCourtRect(m_nServeCourt, 0, 0, &front, &back);
            if (__fabsf(front) > __fabsf(back)) {
                float t = back;
                back = front;
                front = t;
            }
            if (back < 0.0f) {
                back += 0.05f;
                front -= 0.8f;
            } else {
                back -= 0.05f;
                front += 0.8f;
            }
            if (front > back) {
                float t = back;
                back = front;
                front = t;
            }
            target.z = target.z > back ? back : (target.z < front ? front : target.z);
        }
        float dist = GetDistXZ(m_Pos, target);
        float minTop = 5.0f;
        float top = MapRange(dist, 4.0f, 10.0f, 3.0f, 5.0f);
        float add = 0.0f;
        if (param.m_f0C > 0.0f) {
            minTop = 0.0f;
            top = 0.0f;
        }
        if (m_Pos.y > top) {
            top = m_Pos.y + add;
        }
        if (target.y > top) {
            top = target.y + add;
        }
        if (minTop < top) {
            minTop = top;
        }
        if (nBad > 0 && (param.m_f20 > 0.0f || param.m_f24 > 0.0f)) {
            float r = bcs::BCS_RandRange(-1.0f * param.m_f20, param.m_f24);
            minTop += r;
            param.m_f08 -= param.m_f28;
            if (r < 0.0f && param.m_f20 > 0.0f) {
                float t = __fabsf(r) / param.m_f20;
                t = t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
                param.m_f08 -= t * param.m_f2C;
            }
        }
        float frame = SetPassModeParam2_(target, minTop, param.m_f00, param.m_f00 * param.m_f04, param.m_f08,
                                         param.m_f0C);
        if (frame > 0.0f && nType > 0 && !IsItemBall() && nBad <= 0) {
            float mid;
            float start;
            float end;
            if (nType == 2) {
                mid = -8.0f + frame;
                start = mid - 7.0f;
                end = 5.0f + mid;
            } else {
                mid = -28.0f + frame;
                end = mid;
                start = mid - 7.0f;
            }
            mid = Max(0.0f, mid);
            start = Max(0.0f, start);
            end = Max(0.0f, end);
            SetSpikeTimingWork_(start, mid, end);
        }
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_f35C = 8.0f;
        m_fRotSpeed = 0.2617994f;
        ChangeState(3);
    }

    // The flight of a pass of kind nType (nBad: how it goes wrong).
    void CBallVolley::SetBadPassParam_(long nBad, long nType, PASS_PARAM_ &rParam)
    {
        rParam.m_f10 = 0.0f;
        rParam.m_f14 = 0.0f;
        rParam.m_f18 = 0.0f;
        rParam.m_f1C = 0.0f;
        rParam.m_f20 = 0.0f;
        rParam.m_f24 = 0.0f;
        rParam.m_f28 = 0.0f;
        rParam.m_f2C = 0.0f;
        rParam.m_n30 = 0;
        switch (nType) {
            case 2:
                rParam.m_f00 = 0.15f;
                rParam.m_f04 = 0.6f;
                rParam.m_f0C = 40.0f;
                rParam.m_f08 = 35.0f;
                break;
            case 1:
                Util_GetPassParam_(nType, rParam.m_f00, rParam.m_f04, rParam.m_f08);
                rParam.m_f0C = 0.0f;
                break;
            case 3:
                rParam.m_f00 = 0.13333334f;
                rParam.m_f04 = 1.0f;
                rParam.m_f0C = 0.0f;
                rParam.m_f08 = 70.0f;
                break;
            case 4:
                rParam.m_f00 = 0.116666675f;
                rParam.m_f04 = 0.7f;
                rParam.m_f0C = 0.0f;
                rParam.m_f08 = 70.0f;
                break;
            default:
                Util_GetPassParam_(nType, rParam.m_f00, rParam.m_f04, rParam.m_f08);
                rParam.m_f0C = 0.0f;
                break;
        }
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0 && pItem->GetType() == 2) {
            if (nBad == 5) {
                rParam.m_f00 = 0.10000001f;
                rParam.m_f04 = 1.0f;
                rParam.m_f0C = 0.0f;
                rParam.m_f08 = 90.0f;
            } else {
                switch (nType) {
                    case 2:
                        rParam.m_f00 = 0.09166667f;
                        rParam.m_f04 = 0.9f;
                        rParam.m_f0C = 80.0f;
                        rParam.m_f08 = 80.0f;
                        break;
                    case 1:
                        rParam.m_f00 = 0.09166667f;
                        rParam.m_f04 = 0.9f;
                        rParam.m_f0C = 0.0f;
                        rParam.m_f08 = 80.0f;
                        break;
                    default:
                        rParam.m_f00 = 0.10000001f;
                        rParam.m_f04 = 1.0f;
                        rParam.m_f0C = 0.0f;
                        rParam.m_f08 = 90.0f;
                        break;
                }
            }
        }
        switch (nBad) {
            case 2:
                rParam.m_f10 = 0.0f;
                rParam.m_f14 = 0.0f;
                rParam.m_f18 = 0.0f;
                rParam.m_f1C = 0.0f;
                rParam.m_f20 = 0.0f;
                rParam.m_f24 = 0.0f;
                rParam.m_f28 = 0.0f;
                rParam.m_f2C = 0.0f;
                return;
            case 3:
                switch (nType) {
                    case 2:
                        rParam.m_f10 = 0.0f;
                        rParam.m_f14 = 0.0f;
                        rParam.m_f18 = 0.0f;
                        rParam.m_f1C = 0.0f;
                        rParam.m_f20 = 0.0f;
                        rParam.m_f24 = 0.0f;
                        rParam.m_f28 = 0.0f;
                        rParam.m_f2C = 0.0f;
                        return;
                    case 1:
                        rParam.m_f10 = 0.0f;
                        rParam.m_f14 = 0.0f;
                        rParam.m_f18 = 0.0f;
                        rParam.m_f1C = 0.0f;
                        rParam.m_f20 = 0.0f;
                        rParam.m_f24 = 0.0f;
                        rParam.m_f28 = 0.0f;
                        rParam.m_f2C = 0.0f;
                        return;
                    default:
                        rParam.m_f10 = 0.0f;
                        rParam.m_f14 = 0.0f;
                        rParam.m_f18 = 0.0f;
                        rParam.m_f1C = 0.0f;
                        rParam.m_f20 = 0.0f;
                        rParam.m_f24 = 0.0f;
                        rParam.m_f28 = 0.0f;
                        rParam.m_f2C = 0.0f;
                        return;
                }
                break;
            case 4:
                switch (nType) {
                    case 2:
                        rParam.m_f10 = 70.0f;
                        rParam.m_f14 = 70.0f;
                        rParam.m_f18 = 0.0f;
                        rParam.m_f1C = 30.0f;
                        rParam.m_f20 = 0.0f;
                        rParam.m_f24 = 0.0f;
                        rParam.m_f28 = 0.0f;
                        rParam.m_f2C = 0.0f;
                        return;
                    case 1:
                        rParam.m_f10 = 70.0f;
                        rParam.m_f14 = 70.0f;
                        rParam.m_f18 = 0.0f;
                        rParam.m_f1C = 30.0f;
                        rParam.m_f20 = 0.0f;
                        rParam.m_f24 = 0.0f;
                        rParam.m_f28 = 15.0f;
                        rParam.m_f2C = 20.0f;
                        return;
                    default:
                        rParam.m_f10 = 70.0f;
                        rParam.m_f14 = 70.0f;
                        rParam.m_f18 = 0.0f;
                        rParam.m_f1C = 30.0f;
                        rParam.m_f20 = 0.0f;
                        rParam.m_f24 = 1.0f;
                        rParam.m_f28 = 0.0f;
                        rParam.m_f2C = 5.0f;
                        return;
                }
                break;
            case 5:
                rParam.m_f10 = 70.0f;
                rParam.m_f14 = 70.0f;
                rParam.m_f18 = 9.0f;
                rParam.m_f1C = 9.0f;
                rParam.m_f20 = 0.0f;
                rParam.m_f24 = 1.0f;
                rParam.m_f28 = 0.0f;
                rParam.m_f2C = 0.0f;
                rParam.m_n30 = 1;
                return;
            case 6:
                rParam.m_f10 = 45.0f;
                rParam.m_f14 = 45.0f;
                rParam.m_f18 = -20.0f;
                rParam.m_f1C = 10.0f;
                rParam.m_f20 = 2.0f;
                rParam.m_f24 = 0.0f;
                rParam.m_f28 = 0.0f;
                rParam.m_f2C = 10.0f;
                return;
            case 7:
                rParam.m_f10 = 40.0f;
                rParam.m_f14 = 40.0f;
                rParam.m_f18 = -20.0f;
                rParam.m_f1C = 10.0f;
                rParam.m_f20 = 2.0f;
                rParam.m_f24 = 0.0f;
                rParam.m_f28 = 0.0f;
                rParam.m_f2C = 10.0f;
                return;
            case 8:
                rParam.m_f10 = 45.0f;
                rParam.m_f14 = 45.0f;
                rParam.m_f18 = -20.0f;
                rParam.m_f1C = 10.0f;
                rParam.m_f20 = 2.0f;
                rParam.m_f24 = 0.0f;
                rParam.m_f28 = 0.0f;
                rParam.m_f2C = 10.0f;
                return;
        }
    }

    // A flight to rTarget at most fTop high: fSpeed per frame (fEndSpeed
    // at the end), at most fFrame frames, flat when fHeight is set. Returns
    // the frames it takes (0 when it goes nowhere).
    float CBallVolley::SetPassModeParam2_(const bcs::FVEC3 &rTarget, float fTop, float fSpeed, float fEndSpeed,
                                          float fFrame, float fHeight)
    {
        InitFlight_(rTarget);
        float frame = fFrame;
        float dist = CalcXZVel2(m_Pos, rTarget, m_Velocity, m_f320, m_f324, frame, fSpeed, fEndSpeed, fFrame, fHeight,
                                0.0f);
        if (dist <= 0.0f && frame <= 0.0f) {
            UpdateCameraTarget_(1);
            return 0.0f;
        }
        CalcYVG(m_Pos.y, fTop, rTarget.y, frame, m_Velocity.y, m_fGravity);
        if (m_vTargetPos.y > 0.0f) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
            m_vTargetPos.y = 0.0f;
        }
        SetSpikeJumpTgtWork_(rTarget.y);
        CollCheckPassHitNet_(m_vTargetPos, m_Pos, m_vTargetPos);
        UpdateCameraTarget_(1);
        return frame;
    }

    // rTarget moved off its line (fAngleMin to fAngleMax degrees to a
    // random side) and in distance (fDistMin to fDistMax: metres when
    // bAbsolute is set, hundredths of the distance otherwise; rRate gets
    // the change).
    void CBallVolley::CorruptTargetPos_(bcs::FVEC3 &rTarget, float fAngleMin, float fAngleMax, float fDistMin,
                                        float fDistMax, float &rRate, int bAbsolute)
    {
        float dist = GetDistXZ(m_Pos, rTarget);
        bcs::FVEC3 dir;
        dir.x = rTarget.x - m_Pos.x;
        dir.z = rTarget.z - m_Pos.z;
        float rotY = bcs::ROUND_PI(nw4r::math::Atan2Rad(dir.x, dir.z));
        float distMin = fDistMin;
        if (fDistMin > fDistMax) {
            distMin = fDistMax;
            fDistMax = fDistMin;
        }
        if (fAngleMin > fAngleMax) {
            float t = fAngleMin;
            fAngleMin = fAngleMax;
            fAngleMax = t;
        }
        if (fAngleMin + fAngleMax > 0.0f) {
            if (fAngleMax > 0.0f) {
                fAngleMin += bcs::BCS_RandMax(fAngleMax - fAngleMin);
            }
            float angle = __fabsf(bcs::ROUND_PI(DegToRad(fAngleMin)));
            if (bcs::BCS_Rand() & 1) {
                angle *= -1.0f;
            }
            rotY = bcs::ROUND_PI(rotY + angle);
        }
        if (bAbsolute) {
            if (distMin > 0.0f || fDistMax > 0.0f) {
                if (distMin < fDistMax) {
                    distMin = bcs::BCS_RandRange(distMin, fDistMax);
                }
                if (dist > 0.0f) {
                    rRate = (distMin - dist) / dist;
                }
                dist = distMin;
            }
        } else if (0.0f != distMin || 0.0f != fDistMax) {
            if (distMin < fDistMax) {
                distMin = bcs::BCS_RandRange(distMin, fDistMax);
            }
            rRate = 0.01f * distMin;
            float prod = dist * distMin;
            float add = 0.01f * prod;
            dist += add;
        }
        bcs::FVEC3 move;
        bcs::BCS_VecZ_RotY(move, dist, rotY);
        move.x += m_Pos.x;
        move.z += m_Pos.z;
        rTarget.x = move.x;
        rTarget.z = move.z;
    }

}

namespace msw {
    namespace BALL {
        namespace VOLLEY {
            SPIKE_SHOT_ARG::SPIKE_SHOT_ARG()
            {
                m_fUnk00 = 0.0f;
                m_fUnk04 = 0.0f;
                m_fUnk08 = 0.0f;
                m_nUnk0C = 0;
                m_nUnk10 = 0;
                m_nUnk14 = 0;
                m_nUnk18 = 0;
                m_nUnk1C = 0;
                m_nUnk20 = 0;
                m_nUnk24 = 0;
                m_fUnk28 = 0.0f;
                m_nUnk2C = -1;
                m_nUnk30 = -1;
                m_nUnk34 = -1;
                m_fUnk38 = 1.0f;
                m_nUnk3C = -1;
                m_fUnk40 = 0.0f;
                m_nUnk44 = -1;
                m_nSe = -1;
                m_nUnk4C = -1;
                m_nTailEffect = -1;
                m_nUnk54 = 1;
                m_fUnk58 = 10.0f;
                m_fUnk5C = 10.0f;
                m_fUnk60 = 10.0f;
                m_fUnk64 = 0.0f;
                m_fUnk68 = 1.0f;
                m_fUnk6C = 0.0f;
                m_fUnk70 = 100.0f;
                m_fUnk74 = 0.0f;
                m_fUnk78 = 0.0f;
                m_fUnk7C = 0.0f;
                m_fUnk80 = 0.5f;
                m_fUnk84 = 0.5f;
                m_fUnk88 = 0.0f;
                m_fUnk8C = 0.0f;
                m_fUnk90 = 0.0f;
                m_fUnk94 = 0.0f;
                m_fUnk98 = 0.0f;
                m_fUnk9C = 0.0f;
                m_nUnkA0 = 0;
                m_nUnkA4 = 0;
            }
        }
    }
}

namespace msw {

    // Marks a ball still on the server's (or spiker's) side of the net.
    inline void CBallVolley::CheckServeSide_()
    {
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        if ((m_nServeCourt == 1 && 0.04f + netPos.z >= m_Pos.z) ||
            (m_nServeCourt == 0 && netPos.z - 0.04f <= m_Pos.z)) {
            m_nVolleyFlag |= 4;
        }
    }

    // A strong shot: every receiver staggers (longer when it would
    // already).
    inline void CBallVolley::SetReceiveStaggerHard_()
    {
        for (long i = 0; i < 5; i++) {
            if (m_anReceiveReaction[i] & 0x200) {
                m_anReceiveReaction[i] |= 0x400;
                m_afStaggerFrame[i] = 70.0f;
            } else if (m_anReceiveReaction[i] & 0x800000) {
                m_anReceiveReaction[i] |= 0x1000000;
                m_afStaggerFrame[i] = 70.0f;
            } else {
                m_anReceiveReaction[i] |= 0x800000;
                m_afStaggerFrame[i] = 60.0f;
            }
        }
    }

    // A spike (or a feint, kind 5). Returns flags: 1 a strong spike, 2 a
    // power or curve spike, 8 an item attached, 0x10 an item ball.
    int CBallVolley::SetVolleySpikeShotMode(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg)
    {
        int ret = 0;
        CItemBall *pItem = GetItemEffect_();
        Detach_();
        CmnShotInit_();
        if (rArg.m_nUnk0C == 5) {
            ClearVolleyChargePower();
        } else {
            m_fChargePower = 0.0f;
        }
        SetTouch_(rArg.m_nUnk30, rArg.m_nUnk2C, 0);
        m_nShotTeamIdx = rArg.m_nUnk30;
        m_nShotChrIdx = rArg.m_nUnk2C;
        m_nServeCourt = rArg.m_nUnk34;
        SetShotChrNoHit_(rArg.m_nUnk2C);
        m_nShootType = 1;
        m_n34C = 0x7F;
        if (rArg.m_nUnkA0 != 0) {
            m_nSpikeFlag |= 1;
        }
        if (rArg.m_nUnk0C == 5) {
            m_nShootType = 2;
            m_n358 = 0x4DA;
            m_n34C &= ~2;
            SetSpikeFeintParam_(rArg);
        } else {
            if (rArg.m_nUnkA4 != 0) {
                m_nSpikeFlag |= 4;
            }
            m_n358 = 0x4E3;
            if (rArg.m_nUnk3C >= 0 && AttachItemEffect(rArg.m_nUnk3C)) {
                ret |= 8;
            }
            pItem = GetItemEffect_();
            ret |= SetSpikeParam_(rArg);
            if (TrgItemEffect_(0) & 0x10) {
                m_nVolleyFlag |= 2;
            }
        }
        if (rArg.m_nUnk44 >= 0) {
            PlaySePos(rArg.m_nUnk44, 0, rArg.m_nUnk54, -1, 0.0f, 1.0f, 0.0f);
        }
        m_n3E0 = rArg.m_nSe;
        m_n3E4 = rArg.m_nUnk54;
        if (rArg.m_nUnk4C >= 0) {
            Eff_CreateSimple(rArg.m_nUnk4C, m_Pos, rArg.m_fUnk40, 3, 0);
        }
        m_nVolleyFlag |= 0x410;
        if (rArg.m_nTailEffect >= 0) {
            m_n358 = rArg.m_nTailEffect;
        }
        m_f35C = 8.0f;
        CheckServeSide_();
        if (pItem != 0) {
            ret |= 0x10;
            ChangeState(7);
        } else {
            SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
            ChangeState(6);
        }
        return ret;
    }

    // The flight of a spike (kinds: 1 power, 3 curve, 4 weak, others
    // normal).
    int CBallVolley::SetSpikeParam_(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg)
    {
        bcs::FVEC3 target(rArg.m_fUnk00, rArg.m_fUnk04, rArg.m_fUnk08);
        int ret = 0;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            pItem->GetVolleySpikeParam(rArg);
        }
        m_nSpikeType = rArg.m_nUnk0C;
        if (!((1 << m_nSpikeType) & CanbeVolleySpikeVariation())) {
            m_nSpikeType = 0;
        }
        float radius = GetBgCollRadius();
        if (target.y < radius) {
            target.y = radius;
        }
        InitFlight_(target);
        float speedMin = rArg.m_fUnk58;
        float speedMid = rArg.m_fUnk5C;
        float speedMax = rArg.m_fUnk60;
        float accelRate = rArg.m_fUnk64;
        float rate = 0.01f * rArg.m_fUnk70;
        float endRate = rArg.m_fUnk68;
        float minFrame = rArg.m_fUnk6C;
        float heightMin = rArg.m_fUnk80;
        speedMin *= rate;
        float heightMax = rArg.m_fUnk84;
        float slowRate = rArg.m_fUnk7C;
        speedMid *= rate;
        speedMax *= rate;
        if (0.0f != rArg.m_fUnk74) {
            float add = 0.01f * rArg.m_fUnk74;
            speedMin = speedMin * add + speedMin;
            speedMid = speedMid * add + speedMid;
            speedMax = speedMax * add + speedMax;
        }
        if (0.0f != rArg.m_fUnk78) {
            minFrame -= minFrame * (0.01f * rArg.m_fUnk78);
        }
        int bDirect = 1;
        switch (rArg.m_nUnk20) {
            case 0: {
                bcs::FVEC3 cross;
                bDirect = CanbeStraightSpike_(m_Pos, target, cross);
                break;
            }
            case 2:
                bDirect = 0;
                break;
        }
        float slow = 1.0f;
        if (!bDirect && slowRate > 0.0f) {
            slow -= 0.01f * slowRate;
        }
        slow = slow < 0.1f ? 0.1f : slow;
        if (rArg.m_nUnk14 != 0) {
            m_nVolleyFlag |= 0x80;
        }
        switch (rArg.m_nUnk0C) {
            case 4:
                m_n448 = 0;
                m_nFloorHitSe = 0xC31;
                m_n358 = 0x4DA;
                m_nVolleyFlag &= ~0x80;
                m_nShootType = 1;
                break;
            case 1:
                m_n448 = 2;
                m_nFloorHitSe = 0xC33;
                m_n358 = 0x4F8;
                if (rArg.m_nUnk10 == 1) {
                    for (long i = 0; i < 5; i++) {
                        if (i == 2) {
                            m_anReceiveReaction[i] |= 0x20C40002;
                        } else {
                            m_anReceiveReaction[i] |= 0x2460202;
                        }
                        m_afStaggerFrame[i] = 60.0f;
                    }
                } else {
                    for (long i = 0; i < 5; i++) {
                        if (i == 2) {
                            m_anReceiveReaction[i] |= 0x40002;
                        } else {
                            m_anReceiveReaction[i] |= 0x20C40002;
                            m_afStaggerFrame[i] = 60.0f;
                        }
                    }
                }
                SetReceiveReaction3_();
                for (long i = 0; i < 5; i++) {
                    if (i != 2) {
                        m_anBlockReaction[i] |= 4;
                    }
                }
                ret |= 2;
                if (pItem == 0) {
                    m_nVolleyFlag |= 0x100;
                }
                if (rArg.m_nUnk14 != 0) {
                    m_n358 = 0x4E5;
                    ret |= 1;
                    SetReceiveStaggerHard_();
                }
                m_nShootType = 3;
                break;
            case 3:
                m_n448 = 1;
                m_nFloorHitSe = 0xC32;
                SetReceiveReaction3_();
                ret |= 2;
                if (pItem == 0) {
                    m_nVolleyFlag |= 0x100;
                }
                m_nShootType = 3;
                m_nVolleyFlag &= ~0x80;
                break;
            default:
                if (rArg.m_nUnk10 == 1) {
                    SetReceiveStagger_();
                    m_n448 = 2;
                    m_nFloorHitSe = 0xC33;
                } else {
                    m_n448 = 1;
                    m_nFloorHitSe = 0xC32;
                }
                SetReceiveReaction3_();
                if (rArg.m_nUnk14 != 0) {
                    m_n358 = 0x4E4;
                    ret |= 1;
                    SetReceiveStaggerHard_();
                }
                m_nShootType = 1;
                break;
        }
        for (long i = 0; i < 5; i++) {
            m_anReceiveReaction[i] |= 0x200000;
            m_anReceiveReaction3[i] |= 0x200000;
        }
        if (pItem != 0) {
            m_nShootType = 4;
            ret &= ~3;
        }
        if (rArg.m_nUnk24 != 0) {
            m_nVolleyFlag |= 0x2000;
        }
        float t = CalcSpikeTargetPosRate_(target, rArg.m_nUnk34, speedMin, speedMid, speedMax);
        float speed = (1.0f / 60.0f) * (slow * (t * (speedMax - speedMin) + speedMin));
        float frame;
        float dist = CalcXZVel2(m_Pos, target, m_Velocity, m_f320, m_f324, frame, speed, speed * endRate, minFrame, 0.0f,
                                (1.0f / 60.0f) * accelRate);
        if (dist <= 0.0f) {
            UpdateCameraTarget_(1);
            return ret;
        }
        float frameUp = ceil(frame);
        if (rArg.m_fUnk28 > 0.0f) {
            m_f35C = ceil(frameUp * rArg.m_fUnk28);
        }
        float curve = 0.0f;
        int bSpin = 0;
        float drop = curve;
        long nSpin = rArg.m_nUnk18;
        if (rArg.m_nUnk0C == 3) {
            curve = __fabsf(dist * rArg.m_fUnk88);
            if (curve < rArg.m_fUnk8C) {
                curve = rArg.m_fUnk8C;
            }
            if (curve > rArg.m_fUnk90) {
                curve = rArg.m_fUnk90;
            }
            drop = __fabsf(dist * rArg.m_fUnk94);
            if (drop < rArg.m_fUnk98) {
                drop = rArg.m_fUnk98;
            }
            if (drop > rArg.m_fUnk9C) {
                drop = rArg.m_fUnk9C;
            }
            int bFlip = rArg.m_nUnk1C;
            if (rArg.m_fUnk88 < 0.0f) {
                curve += drop;
                drop = -drop;
                bFlip = !bFlip;
                if (rArg.m_fUnk94 < 0.0f) {
                    curve = -curve;
                    drop = -drop;
                }
            } else if (rArg.m_fUnk94 < 0.0f) {
                curve += drop;
                drop = -drop;
            }
            bSpin = 1;
            switch (nSpin) {
                case 1:
                    if (bFlip) {
                        nSpin = 2;
                    }
                    break;
                case 2:
                    if (bFlip) {
                        nSpin = 1;
                    }
                    break;
                default: {
                    // Bend towards the middle of the court.
                    float left;
                    float right;
                    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &left, &right, 0, 0);
                    float center = 0.5f * (left + right);
                    bcs::FVEC3 toTarget = target;
                    bcs::FVEC3 toMirror = target;
                    toMirror.x = center + (center - toTarget.x);
                    toTarget.x -= m_Pos.x;
                    toTarget.z -= m_Pos.z;
                    toMirror.x -= m_Pos.x;
                    toMirror.z = toTarget.z;
                    bcs::FVEC3 cross;
                    PSVECCrossProduct((const Vec *)&toTarget, (const Vec *)&toMirror, (Vec *)&cross);
                    if (cross.y > 0.0f) {
                        nSpin = drop < 0.0f ? 2 : 1;
                    } else {
                        nSpin = drop < 0.0f ? 1 : 2;
                    }
                    break;
                }
            }
        }
        if (bSpin) {
            m_nSpinType = nSpin;
            m_fSpinCount = 0.0f;
            bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
            bcs::FVEC3 dir = m_Velocity;
            NormalizeXZ(dir, dir);
            PSVECCrossProduct((const Vec *)&up, (const Vec *)&dir, (Vec *)&m_vSpinAccel);
            bcs::FVEC3 side;
            NormalizeXZ(side, m_vSpinAccel);
            float vel = 0.0f;
            float accel = 0.0f;
            bcs::BCS_Phys_CalcYVG(frameUp, 0.0f, -drop, curve, vel, accel);
            if (nSpin == 1) {
                vel = -vel;
                accel = -accel;
            }
            m_vSpinAccel.x = side.x * accel;
            m_vSpinAccel.z = side.z * accel;
            bcs::FVEC3 add;
            add.x = side.x * vel;
            add.z = side.z * vel;
            m_fSpinCount = frameUp;
            m_Velocity.x += add.x;
            m_Velocity.z += add.z;
        }
        if (!bDirect) {
            float net = CStageMan::GetInstance()->GetNetHeightTop();
            if (heightMin > heightMax) {
                float tmp = heightMin;
                heightMin = heightMax;
                heightMax = tmp;
            }
            float top;
            if (heightMin != heightMax) {
                top = net + bcs::BCS_RandRange(heightMin, heightMax);
            } else {
                top = net + heightMin;
            }
            CalcYVG(m_Pos.y, top, target.y, frame, m_Velocity.y, m_fGravity);
        } else {
            m_fGravity = 0.0f;
            m_Velocity.y = (1.0f / frameUp) * (target.y - m_Pos.y);
        }
        m_fRotSpeed = 0.27925268f;
        float r = GetBgCollRadius();
        if (m_vTargetPos.y > r) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(r));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
        return ret;
    }

    // Where rTarget lies along the court of nCourt (0 at the front zone, 1
    // at the back line); rMin or rMax takes rMid for a target in the
    // front or the back half.
    float CBallVolley::CalcSpikeTargetPosRate_(const bcs::FVEC3 &rTarget, long nCourt, float &rMin, float &rMid,
                                               float &rMax)
    {
        bcs::FVEC3 target = rTarget;
        float rate = 0.0f;
        float front;
        float back;
        CStageMan::GetInstance()->GetTeamCourtRect(nCourt, 0, 0, &front, &back);
        if (__fabsf(front) > __fabsf(back)) {
            back -= 2.0f;
            if (target.z >= back - 2.0f) {
                rMax = rMid;
                front = back - 2.0f;
            } else {
                rMin = rMid;
                back -= 2.0f;
            }
        } else {
            front += 2.0f;
            if (target.z <= 2.0f + front) {
                rMax = rMid;
                back = 2.0f + front;
            } else {
                rMin = rMid;
                front += 2.0f;
            }
        }
        if (target.z <= front) {
            rate = 0.0f;
        } else if (target.z >= back) {
            rate = 1.0f;
        } else {
            float depth = back - front;
            if (0.0f != depth) {
                float r = (target.z - front) / depth;
                rate = r > 1.0f ? 1.0f : (r < 0.0f ? 0.0f : r);
            }
        }
        if (__fabsf(front) > __fabsf(back)) {
            rate = 1.0f - rate;
        }
        return rate;
    }

    // Whether a spike from here at rTarget can fly straight.
    int CBallVolley::CanbeVolleyStraightSpike(const bcs::FVEC3 &rTarget)
    {
        bcs::FVEC3 target = rTarget;
        target.y += GetBgCollRadius();
        bcs::FVEC3 cross;
        return CanbeStraightSpike_(m_Pos, target, cross);
    }

    int CBallVolley::CanbeVolleySpikeVariation() const
    {
        unsigned long nVariation = 0x21;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            nVariation = pItem->GetVolleyValidSpikeVariation(nVariation);
        } else {
            nVariation |= 0xE;
        }
        return nVariation;
    }

    // A feint: a slow lob over the block.
    void CBallVolley::SetSpikeFeintParam_(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg)
    {
        bcs::FVEC3 target(rArg.m_fUnk00, rArg.m_fUnk04, rArg.m_fUnk08);
        m_nSpikeType = rArg.m_nUnk0C;
        float rate = rArg.m_fUnk38;
        rate = rate > 1.0f ? 1.0f : (rate < 0.0f ? 0.0f : rate);
        float minTop = 1.0f;
        float top = 2.4f;
        if (minTop > top) {
            top = minTop;
        }
        target.y = GetBgCollRadius();
        InitFlight_(target);
        float speed = (1.0f / 60.0f) * rArg.m_fUnk58;
        float frame;
        CalcXZVel2(m_Pos, target, m_Velocity, m_f320, m_f324, frame, speed, speed * rArg.m_fUnk68, rArg.m_fUnk6C, 0.0f,
                   (1.0f / 60.0f) * rArg.m_fUnk64);
        ceil(frame);
        float t = m_Pos.y + rArg.m_fUnk80;
        top = t < top ? top : t;
        CalcYVG(m_Pos.y, top, target.y, frame, m_Velocity.y, m_fGravity);
        m_fRotSpeed = 0.13962634f;
        float r = GetBgCollRadius();
        if (m_vTargetPos.y > r) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(r));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
        m_nVolleyFlag |= 0x200;
        for (long i = 0; i < 5; i++) {
            m_anReceiveReaction[i] |= 0x80000;
        }
    }

    // A special shot (nArts) of character nChr.
    void CBallVolley::SetVolleyArtsMode(const bcs::FVEC3 &rTarget, long nArts, long nChr, long nKind, long nTeam,
                                        long nCourt, long nScore, float f1, float f2, float f3, unsigned long nOutline)
    {
        Detach_();
        CmnShotInit_();
        ClearVolleyChargePower();
        m_nShootType = 9;
        m_n36C = nScore;
        m_n32C = nKind;
        SetTouch_(nTeam, nChr, 0);
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = nTeam;
        m_nServeCourt = nCourt;
        SetShotChrNoHit_(nChr);
        SetArtsSpikeParam_(nArts, rTarget, f1, f2, f3);
        const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(m_nArtsKind, m_n32C);
        if (pTbl != 0 && pTbl->m_anUnk50[0] >= 0) {
            PlaySePos(pTbl->m_anUnk50[0], 0, nOutline, -1, 0.0f, 1.0f, 0.0f);
        }
        CheckServeSide_();
        m_nVolleyFlag |= 0x2400;
        ChangeState(9);
    }

    // The flight of special shot nArts to rTarget (fSpeedMin to fSpeedMax
    // by where it lands), and what it does to the receivers.
    void CBallVolley::SetArtsSpikeParam_(long nArts, const bcs::FVEC3 &rTarget, float fSpeedMin, float fSpeedMid,
                                         float fSpeedMax)
    {
        m_n34C = 0x7F;
        m_nArtsKind = nArts;
        m_n358 = -1;
        if (nArts == 0xF) {
            // Dropped from high above the target.
            m_Pos.x = rTarget.x;
            m_Pos.y = 20.0f + (rTarget.y + GetBgCollRadius());
            m_Pos.z = rTarget.z;
        }
        bcs::FVEC3 target = rTarget;
        target.y = GetBgCollRadius();
        InitFlight_(target);
        const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(nArts, m_n32C);
        float endure = 0.0f;
        m_n3F8 = 0;
        float heightMin = endure;
        float rate = 0.5f;
        float heightMax = endure;
        m_f404 = endure;
        float staggerMin = 0.2f;
        m_f408 = endure;
        long nSe = -1;
        float staggerMax = 1.5f;
        long nSe2 = -1;
        long nVoice = -1;
        switch (nArts) {
            case 0x13:
            case 0x14: {
                const long aRate[5] = {5, 10, 15, 10, 60};
                const float aAdd[5] = {-5.0f, -3.0f, 3.0f, 5.0f, 0.0f};
                static const long RATE_TBL_MAX = 5;
                float add = endure;
                nSe = 0xC94;
                nSe2 = 0xC09;
                nVoice = 0x2F;
                long sum = 0;
                for (long i = 0; i < RATE_TBL_MAX; i++) {
                    sum += aRate[i];
                }
                endure = 140.0f;
                float low = 25.0f;
                float high = 45.0f;
                staggerMin = 2.0f;
                staggerMax = 6.0f;
                if (sum > 0) {
                    long r = bcs::BCS_RandMax(sum - 1);
                    long n = 0;
                    for (long i = 0; i < RATE_TBL_MAX; i++) {
                        if (aRate[i] > 0) {
                            n += aRate[i];
                            if (r < n) {
                                add = aAdd[i];
                                break;
                            }
                        }
                    }
                }
                heightMin = Max(0.0f, low + add);
                heightMax = Max(0.0f, high + add);
                break;
            }
            case 0: {
                const long aRate[5] = {5, 10, 15, 10, 60};
                const float aAdd[5] = {-5.0f, -3.0f, 3.0f, 5.0f, 0.0f};
                static const long RATE_TBL_MAX = 5;
                float add = endure;
                nSe = 0xC94;
                nSe2 = 0xC09;
                nVoice = 0x2F;
                long sum = 0;
                for (long i = 0; i < RATE_TBL_MAX; i++) {
                    sum += aRate[i];
                }
                endure = 140.0f;
                float low = 25.0f;
                float high = 45.0f;
                staggerMin = 2.0f;
                staggerMax = 6.0f;
                if (sum > 0) {
                    long r = bcs::BCS_RandMax(sum - 1);
                    long n = 0;
                    for (long i = 0; i < RATE_TBL_MAX; i++) {
                        if (aRate[i] > 0) {
                            n += aRate[i];
                            if (r < n) {
                                add = aAdd[i];
                                break;
                            }
                        }
                    }
                }
                heightMin = Max(0.0f, low + add);
                heightMax = Max(0.0f, high + add);
                break;
            }
        }
        if (staggerMin > staggerMax) {
            float t = staggerMin;
            staggerMin = staggerMax;
            staggerMax = t;
        }
        if (heightMin > heightMax) {
            float t = heightMin;
            heightMin = heightMax;
            heightMax = t;
        }
        heightMin = Max(0.0f, heightMin);
        heightMax = Max(0.0f, heightMax);
        m_EndurePrm.m_fUnk00 = endure;
        m_EndurePrm.m_fUnk04 = heightMin;
        m_EndurePrm.m_fUnk08 = heightMax;
        m_EndurePrm.m_afUnk0C[0] = rate;
        m_EndurePrm.m_afUnk0C[1] = staggerMin;
        m_EndurePrm.m_afUnk0C[2] = staggerMax;
        m_EndurePrm.m_anUnk18[0] = nSe;
        m_EndurePrm.m_anUnk18[1] = nSe2;
        m_EndurePrm.m_anUnk18[2] = nVoice;
        if (endure > 0.0f) {
            m_bEndure = 1;
        }
        if (pTbl != 0) {
            m_n448 = 3;
            m_nFloorHitSe = pTbl->m_anUnk50[2];
            m_n450 = pTbl->m_anUnk68[0];
            SE_Stop(0, 0.0f);
            SE_Stop(1, 0.0f);
            if (pTbl->m_anUnk50[1] >= 0) {
                SE_PlayMove(0, pTbl->m_anUnk50[1], &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
        }
        float speedMin = fSpeedMin;
        float speedMid = fSpeedMid;
        float speedMax = fSpeedMax;
        float t = CalcSpikeTargetPosRate_(target, m_nServeCourt, speedMin, speedMid, speedMax);
        m_f400 = (1.0f / 60.0f) * (t * (speedMax - speedMin) + speedMin);
        bcs::FVEC3 dir;
        {
            bcs::FVEC3 move;
            bcs::FVEC3Sub(&move, &target, &m_PosD4);
            bcs::BCS_Vec_Normalize(dir, move);
        }
        bcs::FVEC3Scale(&m_Velocity, &dir, m_f400);
        m_fGravity = 0.0f;
        m_fRotSpeed = 0.27925268f;
        float r = GetBgCollRadius();
        if (m_vTargetPos.y > r) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(r));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
        switch (nArts) {
            case 3: {
                // A rising arc over the flight.
                float height = 0.5f * (m_PosD4.y - m_vTargetPos.y);
                float vel;
                bcs::BCS_Phys_CalcYVG(GetTouchDownFrame(GetBgCollRadius()), 0.0f, 0.0f, height, vel, m_fGravity);
                m_Velocity.y += vel;
                break;
            }
            case 4: {
                // A sideways wave.
                m_nSpinType = 3;
                m_fSpinCount = 0.45f * GetTouchDownFrame(GetBgCollRadius());
                bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
                bcs::FVEC3 side;
                PSVECCrossProduct((const Vec *)&up, (const Vec *)&dir, (Vec *)&side);
                PSVECCrossProduct((const Vec *)&dir, (const Vec *)&side, (Vec *)&m_vSpinAccel);
                bcs::BCS_Vec_Normalize(side, m_vSpinAccel);
                float vel = 0.0f;
                float accel = 0.0f;
                float dist = bcs::FVEC3Dist(&target, &m_PosD4);
                bcs::BCS_Phys_CalcYVG(m_fSpinCount, 0.0f, 0.0f, 0.35f * (0.45f * dist), vel, accel);
                bcs::FVEC3Scale(&m_vSpinAccel, &side, accel);
                bcs::FVEC3 add;
                bcs::FVEC3Scale(&add, &side, vel);
                bcs::FVEC3Add(&m_Velocity, &m_Velocity, &add);
                break;
            }
            case 5: {
                float height = 0.4f * (m_PosD4.y - m_vTargetPos.y);
                float vel;
                bcs::BCS_Phys_CalcYVG(GetTouchDownFrame(GetBgCollRadius()), 0.0f, 0.0f, height, vel, m_fGravity);
                m_Velocity.y += vel;
                break;
            }
            case 11: {
                float height = 0.5f * (m_PosD4.y - m_vTargetPos.y);
                float vel;
                bcs::BCS_Phys_CalcYVG(GetTouchDownFrame(GetBgCollRadius()), 0.0f, 0.0f, height, vel, m_fGravity);
                m_Velocity.y += vel;
                break;
            }
            case 12: {
                float frame = GetTouchDownFrame(GetBgCollRadius());
                m_nVolleyFlag |= 0x20000;
                m_f408 = frame * 0.5f;
                break;
            }
        }
        if (nArts != 1 && nArts != 6) {
            SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        }
    }

    // A return over the net (a chance ball) of kind nShootType, maybe a
    // bad one (nBad).
    void CBallVolley::SetVolleyShootMode(const bcs::FVEC3 &rTarget, long nShootType, long nChr, long nTeam,
                                         long nCourt, long nBad, unsigned long)
    {
        bcs::FVEC3 target = rTarget;
        Detach_();
        CmnShotInit_();
        ClearVolleyChargePower();
        SetTouch_(nTeam, nChr, 0);
        m_nShootType = nShootType;
        m_n34C = 0x7F;
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = nTeam;
        m_nServeCourt = nCourt;
        SetShotChrNoHit_(nChr);
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            nBad = pItem->GetVolleyChanceBallParam(nBad);
        }
        m_n358 = 0x4DA;
        if (pItem != 0) {
            m_n358 = pItem->GetVolleyPassTailEffect(0x4DA, 0, 0);
        }
        PASS_PARAM_ param;
        param.m_f00 = 0.116666675f;
        param.m_f04 = 1.1f;
        param.m_f08 = 70.0f;
        param.m_f0C = 0.0f;
        if (nBad > 0) {
            SetBadReturnParam_(nBad, param);
            float rate = 0.0f;
            CorruptTargetPos_(target, param.m_f10, param.m_f14, param.m_f18, param.m_f1C, rate, param.m_n30);
        }
        SetChanceBallParam_(target, param);
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_fRotSpeed = 0.13962634f;
        m_nVolleyFlag |= 0x410;
        CheckServeSide_();
        ChangeState(6);
    }

    void CBallVolley::SetBadReturnParam_(long nBad, PASS_PARAM_ &rParam)
    {
        rParam.m_f10 = 0.0f;
        rParam.m_f14 = 0.0f;
        rParam.m_f18 = 0.0f;
        rParam.m_f1C = 0.0f;
        rParam.m_f20 = 0.0f;
        rParam.m_f24 = 0.0f;
        rParam.m_f28 = 0.0f;
        rParam.m_f2C = 0.0f;
        rParam.m_n30 = 0;
        switch (nBad) {
            case 4:
            case 5:
                rParam.m_f10 = 0.0f;
                rParam.m_f14 = 0.0f;
                rParam.m_f18 = 0.0f;
                rParam.m_f1C = 25.0f;
                break;
            case 2:
                rParam.m_f10 = 0.0f;
                rParam.m_f14 = 0.0f;
                rParam.m_f18 = 0.0f;
                rParam.m_f1C = 0.0f;
                break;
            case 3:
                rParam.m_f10 = 0.0f;
                rParam.m_f14 = 0.0f;
                rParam.m_f18 = 0.0f;
                rParam.m_f1C = 0.0f;
                break;
            case 9:
                rParam.m_f00 = 0.06666667f;
                rParam.m_f04 = 1.1f;
                rParam.m_f08 = 100.0f;
                rParam.m_f10 = 0.0f;
                rParam.m_f14 = 0.0f;
                rParam.m_f18 = 0.0f;
                rParam.m_f1C = 0.0f;
                break;
        }
    }

    // The flight of a chance ball to rTarget. Returns its frames.
    float CBallVolley::SetChanceBallParam_(const bcs::FVEC3 &rTarget, PASS_PARAM_ &rParam)
    {
        bcs::FVEC3 target = rTarget;
        target.y = GetBgCollRadius();
        InitFlight_(target);
        float frame;
        float dist = CalcXZVel2(m_Pos, target, m_Velocity, m_f320, m_f324, frame, rParam.m_f00,
                                rParam.m_f00 * rParam.m_f04, rParam.m_f08, 0.0f, 0.0f);
        if (dist <= 0.0f) {
            UpdateCameraTarget_(1);
            return 0.0f;
        }
        float frameUp = ceil(frame);
        float top = MapRange(dist, 4.0f, 10.0f, 3.0f, 5.0f);
        float minTop = 5.0f;
        if (m_Pos.y > top) {
            top = 1.0f + m_Pos.y;
        }
        if (target.y > top) {
            top = 1.0f + target.y;
        }
        if (minTop < top) {
            minTop = top;
        }
        CalcYVG(m_Pos.y, minTop, target.y, frame, m_Velocity.y, m_fGravity);
        if (m_vTargetPos.y > 0.0f) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
        m_nVolleyFlag |= 0x200;
        for (long i = 0; i < 5; i++) {
            m_anReceiveReaction[i] |= 0x80000;
        }
        return frameUp;
    }

    // 0 (and where, rCross) when the line from rFrom to rTo meets the
    // net, 1 when a straight spike clears it.
    int CBallVolley::CanbeStraightSpike_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rCross)
    {
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        float top = CStageMan::GetInstance()->GetNetHeightTop();
        float bottom = CStageMan::GetInstance()->GetNetHeightBottom();
        if (bottom > top) {
            top = bottom;
        }
        bcs::FVEC3 normal;
        bcs::FVEC3 cross;
        float t;
        normal.Set(0.0f, 0.0f, -1.0f);
        t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, normal, netPos, rFrom, rTo);
        if (t > 0.0f && t < 1.0f && cross.y <= 0.1f + top) {
            rCross = cross;
            return 0;
        }
        normal.Set(0.0f, 0.0f, 1.0f);
        t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, normal, netPos, rFrom, rTo);
        if (t > 0.0f && t < 1.0f && cross.y <= 0.1f + top) {
            rCross = cross;
            return 0;
        }
        return 1;
    }

    // The ball hit something on the way (a shot or a pass that is not too
    // fresh): it bounces off in a random direction.
    void CBallVolley::SetVolleyAnyHitMode(const bcs::FVEC3 *, long nKind)
    {
        int bStar = IsStarBall();
        if ((unsigned long)nKind <= 3 && bStar) {
            return;
        }
        float scale = 1.0f;
        int bHit = 0;
        if (IsShootMode()) {
            bHit = 1;
        } else if (IsPassMode() && m_f35C <= 0.0f) {
            scale = 1.5f;
            bHit = 1;
        }
        if (!bHit) {
            return;
        }
        bcs::FVEC3 target;
        bcs::BCS_VecZ_RotY(target, 4.0f, bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f)));
        target.x += m_Pos.x;
        target.z += m_Pos.z;
        Detach_();
        ClearVolleyChargePower();
        if (TrgItemEffect_(0xB) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        m_nShootType = 0;
        m_n34C = 0x7F;
        m_n358 = 0x4DA;
        PASS_PARAM_ param;
        param.m_f00 = 0.116666675f;
        param.m_f04 = 1.1f;
        param.m_f08 = 70.0f * scale;
        param.m_f0C = 0.0f;
        SetChanceBallParam_(target, param);
        PlaySePos(0xC8E, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        CEffectCtrl::SimpleEffect(0x4D7, m_Pos, 3, 0, 0);
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_fRotSpeed = 0.13962634f;
        m_nVolleyFlag &= ~0x10;
        m_nVolleyFlag &= ~0x20000;
        m_nVolleyFlag &= ~0x400;
        m_f35C = 8.0f;
        GetPosAfterFrame(m_vTargetPos, floor(GetTouchDownFrame(GetBgCollRadius())));
        m_vTargetPos.y = 0.0f;
        UpdateCameraTarget_(1);
        ChangeState(6);
    }

    // The ball is let go of with velocity rVel by character nChr of team
    // nTeam.
    void CBallVolley::SetVolleyDropMode(const bcs::FVEC3 &rVel, long nChr, long nTeam, long nKind, unsigned long)
    {
        unsigned long nKeep = 0;
        if (m_nArtsKind >= 0 && m_nArtsKind <= 0x18) {
            nKeep = m_nVolleyFlag & 0x400;
            TrgVolleyArtsOver(nKind, nChr, 1);
        }
        Detach_();
        CmnShotInit_();
        ResetVelGravity_();
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        ClearSpin_();
        m_PrevPos = m_Pos;
        m_PosD4 = m_Pos;
        m_n358 = -1;
        m_nShootType = 0;
        m_n34C = 0x7F;
        SetTouch_(nTeam, nChr, 0);
        m_nPassChrIdx = nChr;
        SetShotChrNoHit_(m_nShotChrIdx);
        m_Velocity = rVel;
        m_fRotSpeed = 0.06981317f;
        if (m_vTargetPos.y > 0.0f) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
        m_nVolleyFlag |= nKeep;
        ChangeState(1);
    }

    // A character holds special shot nArts ready on the ball.
    void CBallVolley::TrgVolleyArtsAttach(long, long, long nArts, int bTail)
    {
        ClearVolleyChargePower();
        m_nVolleyFlag &= ~0x1C0;
        if (TrgItemEffect_(0xC) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        long nMin = 0;
        long nMax = -1;
        Eff_GetGroupSlotMinMax_(0, nMin, nMax);
        for (long i = nMin; i <= nMax; i++) {
            CEffectCluster *pCluster = static_cast<CEffectCluster *>(Eff_GetCluster(i));
            if (pCluster != 0) {
                pCluster->ClearPauseFlag();
            }
        }
        Eff_EraseGroup(0);
        if (bTail) {
            const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(nArts, m_n32C);
            if (pTbl != 0) {
                SetArtsTailEffect__(pTbl->m_aEfct[0]);
            }
        }
        CmnShotInit_();
        m_bArtsAttach = 1;
        m_n3EC = 0;
        m_f3F0 = 60.0f;
        m_f360 = 0.0f;
        m_nFlag &= ~4;
        if (GetAttachMdlObj() != 0) {
            m_vTargetPos = GetAttachMdlObj()->m_vTrans;
        }
        UpdateCameraTarget_(0);
    }

    // The trail effects of a special shot (eight slots of group 0).
    void CBallVolley::SetArtsTailEffect__(const ARTS_EFCT_TBL::EFCT_TBL &rTbl)
    {
        long nNum = rTbl.m_nNum;
        const ARTS_EFCT_TBL::EFCT *pEfct = rTbl.m_pEfct;
        if (nNum <= 0 || pEfct == 0) {
            Eff_StopGroup(0);
            return;
        }
        for (long i = 0; i < 8; i++) {
            int bStop = 1;
            if (i < nNum) {
                if (pEfct->m_nEffect >= 0) {
                    unsigned long nFlag = 0;
                    if (pEfct->m_nFlag & 1) {
                        nFlag |= 1;
                    }
                    if (pEfct->m_nFlag & 2) {
                        nFlag |= 2;
                    }
                    if (pEfct->m_nFlag & 4) {
                        nFlag |= 4;
                    }
                    Eff_CreateAttach(i, pEfct->m_nEffect, 0, nFlag, 3);
                    Eff_SetMiiColor(i, m_n32C);
                    const ARTS_EFCT_TBL::EFCT_COL_SET *pColor = pEfct->m_pColor;
                    if (pColor != 0) {
                        unsigned long nColorFlag = pColor->m_nFlag;
                        if (nColorFlag & 1) {
                            Eff_SetPointLightColor(i, bcs::BCS_Color(pColor->m_aColor[0]),
                                                   bcs::BCS_Color(pColor->m_aColor[1]));
                        }
                        if (nColorFlag & 2) {
                            Eff_SetMdlColorReg(i, GX_TEVREG0, bcs::BCS_Color(pColor->m_aColor[2]));
                        }
                        if (nColorFlag & 4) {
                            Eff_SetMdlColorReg(i, GX_TEVREG1, bcs::BCS_Color(pColor->m_aColor[3]));
                        }
                        if (nColorFlag & 8) {
                            Eff_SetMdlKColorReg(i, GX_KCOLOR0, bcs::BCS_Color(pColor->m_aColor[4]));
                        }
                        if (nColorFlag & 0x10) {
                            Eff_SetMdlKColorReg(i, GX_KCOLOR1, bcs::BCS_Color(pColor->m_aColor[5]));
                        }
                        if (nColorFlag & 0x20) {
                            Eff_SetMdlKColorReg(i, GX_KCOLOR2, bcs::BCS_Color(pColor->m_aColor[6]));
                        }
                    }
                    bStop = 0;
                }
                pEfct++;
            }
            if (bStop) {
                Eff_Stop(i);
            }
        }
    }

    // The effects of a special shot where it touched down (flag 0x10: a
    // model effect, 8: a mark on the floor). Returns 1 when one started.
    int CBallVolley::SetArtsTouchdownEffect_(const ARTS_EFCT_TBL::EFCT_TBL &rTbl, const bcs::FVEC3 &rPos)
    {
        long nNum = rTbl.m_nNum;
        const ARTS_EFCT_TBL::EFCT *pEfct = rTbl.m_pEfct;
        if (nNum <= 0 || pEfct == 0) {
            return 0;
        }
        int ret = 0;
        for (long i = 0; i < nNum; i++, pEfct++) {
            long nEffect = pEfct->m_nEffect;
            if (nEffect < 0) {
                continue;
            }
            ret = 1;
            unsigned long hEffect = 0;
            if (pEfct->m_nFlag & 0x10) {
                BALL_VOLLEY::CBallTouchDownMdlEffect *pTask =
                    new BALL_VOLLEY::CBallTouchDownMdlEffect(CGame::GetTempParent(), nEffect, rPos, m_Velocity);
                if (pTask != 0) {
                    hEffect = pTask->GetHandle();
                }
            } else if (pEfct->m_nFlag & 8) {
                BALL_VOLLEY::CBallTouchDownMark *pTask =
                    new BALL_VOLLEY::CBallTouchDownMark(CGame::GetTempParent(), nEffect, rPos, m_Velocity);
                if (pTask != 0) {
                    hEffect = pTask->GetHandle();
                }
            } else {
                hEffect = Eff_CreateSimple(nEffect, rPos, 3, 0);
            }
            const ARTS_EFCT_TBL::EFCT_COL_SET *pColor = pEfct->m_pColor;
            if (pColor != 0 && hEffect != 0) {
                CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                if (pCluster != 0) {
                    unsigned long nColorFlag = pColor->m_nFlag;
                    if (nColorFlag & 1) {
                        pCluster->SetPointLightColor(pColor->m_aColor[0], pColor->m_aColor[1]);
                    }
                    long nMdlNum = pCluster->GetMdlObjNum();
                    for (long j = 0; j < nMdlNum; j++) {
                        bcs::CMdlObj *pMdl = pCluster->GetMdlObj(j);
                        if (pMdl != 0) {
                            if (nColorFlag & 2) {
                                pMdl->SetMatTevColor(GX_TEVREG0, bcs::BCS_Color(pColor->m_aColor[2]));
                            }
                            if (nColorFlag & 4) {
                                pMdl->SetMatTevColor(GX_TEVREG1, bcs::BCS_Color(pColor->m_aColor[3]));
                            }
                            if (nColorFlag & 8) {
                                pMdl->SetMatTevKColor(GX_KCOLOR0, bcs::BCS_Color(pColor->m_aColor[4]));
                            }
                            if (nColorFlag & 0x10) {
                                pMdl->SetMatTevKColor(GX_KCOLOR1, bcs::BCS_Color(pColor->m_aColor[5]));
                            }
                            if (nColorFlag & 0x20) {
                                pMdl->SetMatTevKColor(GX_KCOLOR2, bcs::BCS_Color(pColor->m_aColor[6]));
                            }
                        }
                    }
                }
            }
        }
        return ret;
    }

    void CBallVolley::TrgVolleyArtsReady(long nArts, int bTail)
    {
        if (bTail) {
            const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(nArts, m_n32C);
            if (pTbl != 0) {
                SetArtsTailEffect__(pTbl->m_aEfct[1]);
            }
        }
    }

    // The special shot is over (nKind 0: on the floor, 1: blocked, 2:
    // received, 3: reflected).
    int CBallVolley::TrgVolleyArtsOver(long nKind, long nChr, unsigned long nArg)
    {
        float fade = 40.0f;
        int ret = TrgVolleyArtsOver_(nKind, nChr, nArg);
        if (nKind == 0) {
            fade = 70.0f;
        }
        CGame::SendMessage(0xE, 0, 0, 0);
        CGame::SendMessage(0x16, 0, 0, 0);
        CGame::BgSetFadeIn(fade, 30.0f);
        return ret;
    }

    int CBallVolley::TrgVolleyArtsOver_(long nKind, long nChr, unsigned long nArg)
    {
        int ret = 0;
        long nEffect = -1;
        if (m_nArtsKind >= 0 && m_nArtsKind <= 0x18) {
            SE_Stop(0, 8.0f);
            SE_Stop(1, 8.0f);
            const ARTS_EFCT_TBL *pTbl = GetArtsEffectTbl_(m_nArtsKind, m_n32C);
            if (pTbl != 0) {
                bcs::FVEC3 pos = m_Pos;
                switch (nKind) {
                    case 0:
                        if (!(m_nVolleyFlag & 0x8000)) {
                            bcs::FVEC3 floorPos(pos.x, 0.0f, pos.z);
                            ret = SetArtsTouchdownEffect_(pTbl->m_aEfct[4], floorPos);
                        }
                        break;
                    case 1:
                        if (pTbl->m_anUnk50[4] >= 0) {
                            PlaySePos(pTbl->m_anUnk50[4], 0, nArg, -1, 0.0f, 1.0f, 0.0f);
                        }
                        CGame::StartCameraQuake(pTbl->m_anUnk68[1], nChr, -1);
                        if (pos.y >= pTbl->m_fUnk4C) {
                            nEffect = pTbl->m_nUnk44;
                        } else {
                            nEffect = pTbl->m_nUnk48;
                            pos.y = 0.0f;
                        }
                        break;
                    case 2:
                        if (pTbl->m_anUnk50[3] >= 0) {
                            PlaySePos(pTbl->m_anUnk50[3], 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        }
                        PlayVolleySpReceiveSuccessSe(nArg);
                        ret = SetArtsTouchdownEffect_(pTbl->m_aEfct[5], pos);
                        break;
                    case 3: {
                        bcs::FVEC3 reflectPos = m_Pos;
                        long nSe = pTbl->m_anUnk50[5];
                        ret = SetArtsTouchdownEffect_(pTbl->m_aEfct[6], reflectPos);
                        if (nSe >= 0) {
                            PlaySePos(nSe, 0, nArg, -1, 0.0f, 1.0f, 0.0f);
                        }
                        ClearSpin_();
                        InitArtsShotStep_();
                        ClearVolleyChargePower();
                        ChangeState(1);
                        break;
                    }
                }
                if (nEffect >= 0) {
                    CEffectCtrl::SimpleEffect(nEffect, pos, 3, 0, 0);
                    ret = 1;
                }
                if (!(m_nVolleyFlag & 0x8000)) {
                    EffectStopTail_();
                    m_n358 = -1;
                }
            }
            if (nKind != 0 && CGame::IsVolleyScored() && !(m_nVolleyFlag444 & 1)) {
                CGame::TrgVolleyGoalIn(m_nShotChrIdx, m_nShotTeamIdx, m_nShootType, 0, 0, m_nTrueTouchNum, m_Pos, 4);
            }
        }
        return ret;
    }

    void CBallVolley::PlayVolleySpReceiveSuccessSe(unsigned long nArg)
    {
        PlaySePos(0xC0B, 0, nArg, -1, 0.0f, 1.0f, 0.0f);
    }

    void CBallVolley::SetSleepMode()
    {
        CBall::SetSleepMode();
        ClearSpin_();
        m_nSpikeType = 6;
        UpdateCameraTarget_(0);
    }

    int CBallVolley::IsCanbeTurnOver() const
    {
        return 1;
    }

    int CBallVolley::IsInvincible() const
    {
        return (m_nFlag >> 4) & 1;
    }

    int CBallVolley::CanbeCatch() const
    {
        if (m_bArtsOver != 0) {
            return 0;
        }
        if (m_nVolleyFlag & 2) {
            return 0;
        }
        if (GetAttachMdlObj() != 0 && !(m_nFlag & 0x8000)) {
            return 0;
        }
        if ((m_nFlag & 0x2) || (m_nFlag & 0x4)) {
            return 0;
        }
        if (m_f35C > 0.0f) {
            return 0;
        }
        return 1;
    }

    void CBallVolley::PlayBoundSe()
    {
        PlayFloorHitSe(1);
    }

    // The jump ball: the ball waits beside the net on the first
    // character's side.
    inline CChrCore *CBallVolley::InitJumpBall_()
    {
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        ClearSpin_();
        m_nSpikeType = 6;
        EffectStopTail_();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_Pos.Set(-3.0f, 2.0f, -11.5f);
        m_Velocity.Set(0.0f, 0.0f, 0.0f);
        m_PrevVel.Set(0.0f, 0.0f, 0.0f);
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 0.2617994f;
        CChrCore *pChr = CChrMan::GetChrCore(0);
        if (pChr != 0 && pChr->GetTargetCourtIdx() == 0) {
            m_Pos.x *= -1.0f;
            m_Pos.z *= -1.0f;
        }
        m_PrevPos = m_Pos;
        return pChr;
    }

    void CBallVolley::SetJumpBallMode()
    {
        InitJumpBall_();
        ChangeState(11);
    }

    // The ball bounces off character nChr (at fSpeed, or the ball
    // manager's speed).
    void CBallVolley::SetChrReflectMode(long nChr, float fSpeed)
    {
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        ClearSpin_();
        m_nSpikeType = 6;
        m_f320 = 0.0f;
        m_f324 = 0.0f;
        EffectStopTail_();
        UpdateCameraTarget_(0);
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_nFlag |= 0x20;
        bcs::FVEC3 dir;
        pChr->GetCompPos(dir);
        bcs::FVEC3Sub(&dir, &m_Pos, &dir);
        if (bcs::BCS_Vec_GetLenXZ(dir) > 0.0f) {
            NormalizeXZ(dir, dir);
            dir.y = 1.0f;
            bcs::BCS_Vec_Normalize(dir, dir);
        } else {
            dir.Set(0.0f, 1.0f, 0.0f);
        }
        if (fSpeed <= 0.0f) {
            bcs::FVEC3Scale(&m_Velocity, &dir, CBallMan::GetChrReflectSpeed());
        } else {
            bcs::FVEC3Scale(&m_Velocity, &dir, fSpeed);
        }
        m_PrevPos = m_Pos;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 0.2617994f;
        if (TrgItemEffect_(5) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        m_n448 = 0;
        m_nFloorHitSe = -1;
        ChangeState(1);
    }

    float CBallVolley::GetTouchDownFrame(float fHeight) const
    {
        return GetTouchDownFrame2(fHeight, 0.0f);
    }

    // The frames until the ball is down to fHeight (its vertical speed
    // less fVelOfs).
    float CBallVolley::GetTouchDownFrame2(float fHeight, float fVelOfs) const
    {
        float gravity = m_fGravity + m_vSpinAccel.y;
        float vel = m_Velocity.y - fVelOfs;
        float y = m_Pos.y;
        float frame = 0.0f;
        if (__fabsf(gravity) <= 0.00001f) {
            if (!(y > fHeight && vel >= 0.0f) && !(y < fHeight && vel <= 0.0f)) {
                frame = (y - fHeight) / (-1.0f * vel);
            }
        } else {
            float roots[2];
            roots[1] = 0.0f;
            roots[0] = 0.0f;
            int num = nw4r::math::SolveEquation2(roots, 0.5f * gravity, vel, y - fHeight);
            if (num > 0) {
                if (num <= 1) {
                    frame = roots[0];
                } else {
                    frame = Max(roots[0], roots[1]);
                    if (frame < 0.0f) {
                        frame = 0.0f;
                    }
                }
            }
        }
        if (frame > 0.0f) {
            return frame + m_f360;
        }
        return frame;
    }

    // The frames until the ball passes fHeight (going up first when
    // bFirst is set).
    float CBallVolley::GetHeightPassFrame(float fHeight, int bFirst) const
    {
        float gravity = m_fGravity + m_vSpinAccel.y;
        float vel = m_Velocity.y;
        float y = m_Pos.y;
        float frame = 0.0f;
        if (__fabsf(gravity) <= 0.00001f) {
            frame = (y - fHeight) / (-1.0f * vel);
        } else {
            float roots[2];
            roots[1] = 0.0f;
            roots[0] = 0.0f;
            int num = nw4r::math::SolveEquation2(roots, 0.5f * gravity, vel, y - fHeight);
            if (num > 0) {
                if (num <= 1) {
                    frame = roots[0];
                } else if (bFirst) {
                    frame = Min(roots[0], roots[1]);
                    if (frame <= 0.0f) {
                        frame = Max(roots[0], roots[1]);
                    }
                } else {
                    frame = Max(roots[0], roots[1]);
                }
            }
        }
        if (frame > 0.0f) {
            return frame + m_f360;
        }
        return frame;
    }

    // The change of velocity over fTime frames of the flight (bSpin: with
    // the spin).
    inline void CBallVolley::CalcFlightAccel_(bcs::FVEC3 &rAccel, const bcs::FVEC3 &rDir, float fDecel, float fGravity,
                                              float fTime, int bSpin) const
    {
        float decel = fDecel * fTime;
        rAccel.x = rDir.x * decel;
        rAccel.z = rDir.z * decel;
        if (bSpin) {
            rAccel.x = m_vSpinAccel.x * fTime + rAccel.x;
            rAccel.y = fTime * (fGravity + m_vSpinAccel.y);
            rAccel.z = m_vSpinAccel.z * fTime + rAccel.z;
        } else {
            rAccel.y = fGravity * fTime;
        }
    }

    // Where the ball is after fFrame frames.
    void CBallVolley::GetVolleyPosAfterFrame(bcs::FVEC3 &rPos, float fFrame, int bSpin) const
    {
        rPos = m_Pos;
        bcs::FVEC3 vel = m_Velocity;
        float gravity = m_fGravity;
        if (fFrame <= m_f360) {
            return;
        }
        float time = fFrame - m_f360;
        float speed = Max(0.0f, bcs::BCS_Vec_GetLenXZ(vel) - m_f324);
        float decel = m_f320;
        bcs::FVEC3 dir = vel;
        NormalizeXZ(dir, dir);
        if (m_f320 * time + speed < 0.0f) {
            float stop = __fabsf(speed / m_f320);
            bcs::FVEC3 accel;
            CalcFlightAccel_(accel, dir, m_f320, gravity, stop, bSpin);
            bcs::FVEC3 move;
            StepFlight(move, vel, accel, stop);
            bcs::FVEC3Add(&rPos, &rPos, &move);
            time -= stop;
            decel = 0.0f;
        }
        bcs::FVEC3 accel;
        CalcFlightAccel_(accel, dir, decel, gravity, time, bSpin);
        bcs::FVEC3 move;
        StepFlight(move, vel, accel, time);
        bcs::FVEC3Add(&rPos, &rPos, &move);
    }

    void CBallVolley::GetPosAfterFrame(bcs::FVEC3 &rPos, float fFrame) const
    {
        GetVolleyPosAfterFrame(rPos, fFrame, 1);
    }

    void CBallVolley::GetVelAfterFrame(bcs::FVEC3 &rVel, float fFrame) const
    {
        GetVolleyVelAfterFrame(rVel, fFrame, 1);
    }

    // The velocity of the ball after fFrame frames.
    void CBallVolley::GetVolleyVelAfterFrame(bcs::FVEC3 &rVel, float fFrame, int bSpin) const
    {
        rVel = m_Velocity;
        bcs::FVEC3 pos = m_Pos;
        bcs::FVEC3 vel = m_Velocity;
        float gravity = m_fGravity;
        if (fFrame <= m_f360) {
            return;
        }
        float time = fFrame - m_f360;
        float speed = Max(0.0f, bcs::BCS_Vec_GetLenXZ(vel) - m_f324);
        float decel = m_f320;
        bcs::FVEC3 dir = vel;
        NormalizeXZ(dir, dir);
        if (m_f320 * time + speed < 0.0f) {
            float stop = __fabsf(speed / m_f320);
            bcs::FVEC3 accel;
            CalcFlightAccel_(accel, dir, m_f320, gravity, stop, bSpin);
            bcs::FVEC3Add(&vel, &vel, &accel);
            time -= stop;
            decel = 0.0f;
        }
        bcs::FVEC3 accel;
        CalcFlightAccel_(accel, dir, decel, gravity, time, bSpin);
        bcs::FVEC3Add(&vel, &vel, &accel);
        rVel = vel;
    }

    long CBallVolley::GetTargetCourtIdx() const
    {
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        return m_vTargetPos.z <= netPos.z;
    }

    int CBallVolley::IsTargetCourt(long nCourt) const
    {
        return nCourt == GetTargetCourtIdx();
    }

    float CBallVolley::GetVolleyCatchableRate() const
    {
        float rate = 1.0f;
        if (m_nSpinType >= 1 && m_nSpinType <= 2 && m_fSpinCount > 0.0f) {
            rate = 0.4f;
        }
        return rate;
    }

    // Whether the ball is where side nSide can catch it (anything above
    // the net can be).
    int CBallVolley::IsVolleyInCatchableArea(long nSide) const
    {
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        if (m_Pos.y >= CStageMan::GetInstance()->GetNetHeightTop()) {
            return 1;
        }
        if (nSide != 0) {
            if (m_Pos.z > netPos.z) {
                return 0;
            }
        } else if (m_Pos.z < netPos.z) {
            return 0;
        }
        return 1;
    }

    long CBallVolley::AttachItemEffect(long nKind)
    {
        CItemBall *pItem = 0;
        if (!(m_nFlag & FLAG_SCORED)) {
            switch (nKind) {
                case 2:
                    pItem = new (0x100) CItemBallVolleyToadstool(this);
                    break;
                case 3:
                    pItem = new (0x100) CItemBallVolleyBomb(this);
                    break;
                case 4:
                    pItem = new (0x100) CItemBallVolleyStar(this);
                    break;
                case 0:
                    pItem = new (0x100) CItemBallVolleyGShell(this);
                    break;
                case 1:
                    pItem = new (0x100) CItemBallVolleyRShell(this);
                    break;
                case 5:
                    pItem = new (0x100) CItemBallVolleyBanana(this);
                    break;
                default:
                    pItem = new (0x100) CItemBallVolleyGShell(this);
                    break;
            }
        }
        if (pItem != 0) {
            m_fChargePower = 0.0f;
            DetachItemEffect();
            LinkItemEffect_(pItem->GetId());
            pItem->DoTrg(CItemBall::TRG_CONSTRUCT);
            if (pItem->GetType() != 4) {
                new CItemBallChangeEffectVolley(CGame::GetTempParent(), GetId());
            }
            return 1;
        }
        return 0;
    }

    // The XZ velocity of a flight from rFrom to rTo: fSpeed0 per frame at
    // the start, fSpeed1 (at least fMinSpeed1) at the end, fMinFrame to
    // fMaxFrame frames (rFrame, rounded up); rDecel the change of speed
    // per frame, rEndSpeed the speed at the end. Returns the distance.
    float CBallVolley::CalcXZVel2(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rVel, float &rDecel,
                                  float &rEndSpeed, float &rFrame, float fSpeed0, float fSpeed1, float fMinFrame,
                                  float fMaxFrame, float fMinSpeed1) const
    {
        rVel.x = 0.0f;
        float speed1 = fSpeed1;
        rVel.z = 0.0f;
        rDecel = 0.0f;
        rEndSpeed = 0.0f;
        rFrame = 0.0f;
        speed1 = Max(fMinSpeed1, speed1);
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &rTo, &rFrom);
        float dist = bcs::BCS_Vec_GetLenXZ(diff);
        if (dist <= 0.0f) {
            return dist;
        }
        float frame = dist / (0.5f * (fSpeed0 + speed1));
        if (fMinFrame > 0.0f) {
            frame = Max(fMinFrame, frame);
        }
        if (fMaxFrame > 0.0f) {
            frame = Min(fMaxFrame, frame);
        }
        float frameUp = ceil(frame);
        rFrame = frameUp;
        float avg = dist / frameUp;
        float speed = (2.0f * avg) / (1.0f + speed1 / fSpeed0);
        float end = 2.0f * avg - speed;
        rDecel = (end - speed) / frameUp;
        rEndSpeed = end;
        NormalizeXZ(rVel, diff);
        rVel.x *= speed;
        rVel.z *= speed;
        return dist;
    }

    // One step of the flight of fRate frames (none while m_f360 counts).
    void CBallVolley::FuncMove_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fRate)
    {
        if (fRate <= m_f360) {
            return;
        }
        float time = fRate - m_f360;
        float speed = Max(0.0f, bcs::BCS_Vec_GetLenXZ(rVel) - m_f324);
        bcs::FVEC3 dir = rVel;
        NormalizeXZ(dir, dir);
        if (m_f320 * time + speed < 0.0f) {
            float stop = __fabsf(speed / m_f320);
            float decel = m_f320 * stop;
            bcs::FVEC3 accel(dir.x * decel, fGravity * stop, dir.z * decel);
            bcs::FVEC3 spin;
            bcs::FVEC3Scale(&spin, &m_vSpinAccel, stop);
            bcs::FVEC3Add(&accel, &accel, &spin);
            MoveFlight(rPos, rVel, accel, stop);
            time -= stop;
            m_f320 = 0.0f;
            m_f324 = 0.0f;
        }
        float decel = m_f320 * time;
        bcs::FVEC3 accel(dir.x * decel, fGravity * time, dir.z * decel);
        bcs::FVEC3 spin;
        bcs::FVEC3Scale(&spin, &m_vSpinAccel, time);
        bcs::FVEC3Add(&accel, &accel, &spin);
        MoveFlight(rPos, rVel, accel, time);
    }

    // The effects of special shot nArts (nKind: 1 and 2 two variants of
    // kind 0x10).
    const CBallVolley::ARTS_EFCT_TBL *CBallVolley::GetArtsEffectTbl_(long nArts, long nKind) const
    {
        const ARTS_EFCT_TBL *pTbl = 0;
        if (nArts < 0x13) {
            pTbl = &s_aArtsEfctTbl[nArts];
            if (nArts == 0x10) {
                switch (nKind) {
                    case 1:
                        pTbl = &s_aArtsEfctTbl10[0];
                        break;
                    case 2:
                        pTbl = &s_aArtsEfctTbl10[1];
                        break;
                }
            }
        } else {
            switch (nArts) {
                case 0x13:
                case 0x15:
                case 0x17:
                    pTbl = &s_aArtsEfctTblEx[0];
                    break;
                case 0x14:
                case 0x16:
                case 0x18:
                    pTbl = &s_aArtsEfctTblEx[1];
                    break;
            }
        }
        return pTbl;
    }

    // The inline members of CBallVolley (only this file sees their bodies;
    // the vtable keeps weak copies).
    inline unsigned long CBallVolley::SIDtoSeID_(long nSe)
    {
        return nSe;
    }

    inline void CBallVolley::SetShotTeamIdx(long nTeam)
    {
        m_nShotTeamIdx = nTeam;
    }

    inline void CBallVolley::SetPassChrIdx(long nChr)
    {
        m_nPassChrIdx = nChr;
    }

    inline void CBallVolley::SetShotChrIdx(long nChr)
    {
        m_nShotChrIdx = nChr;
    }

    inline long CBallVolley::GetTouchNum() const
    {
        return GetVolleyTouchNum(-1);
    }

    inline long CBallVolley::GetShotTeamIdx() const
    {
        return m_nShotTeamIdx;
    }

    inline long CBallVolley::GetPassChrIdx() const
    {
        return m_nPassChrIdx;
    }

    inline long CBallVolley::GetShotChrIdx() const
    {
        return m_nShotChrIdx;
    }

    inline int CBallVolley::IsVolleyArtsAttach() const
    {
        return m_bArtsAttach;
    }

    inline float CBallVolley::GetVolleyPassCounter() const
    {
        return m_fPassCounter;
    }

    inline long CBallVolley::GetVolleyTrueTouchNum(long nTeam) const
    {
        return GetVolleyTouchNum(nTeam);
    }

    inline int CBallVolley::IsVolleyTimingSpike() const
    {
        return IsTimingSpike_();
    }

    inline int CBallVolley::CanebVolleyTouch(long) const
    {
        return 1;
    }

    // -----------------------------------------------------------------------
    // CBallVolleyVsBoss

    CBallVolleyVsBoss::CBallVolleyVsBoss(bcs::CTask *pParent, long nBallNo)
        : CBallVolley(pParent, nBallNo), m_bPickup(0), m_fTint(0.0f)
    {
        m_nVolleyFlag444 |= 7;
    }

    CBallVolleyVsBoss::~CBallVolleyVsBoss()
    {
    }

    int CBallVolleyVsBoss::CanebVolleyTouch(long) const
    {
        return 1;
    }

    float CBallVolleyVsBoss::GetNoCatchCount() const
    {
        if (m_bArtsOver != 0) {
            return 99999.0f;
        }
        return 0.0f;
    }

    long CBallVolleyVsBoss::GetVolleyTouchNum(long nTeam) const
    {
        long n = GetVolleyTrueTouchNum(nTeam);
        return n > 2 ? 2 : (n < 0 ? 0 : n);
    }

    long CBallVolleyVsBoss::GetVolleyTrueTouchNum(long nTeam) const
    {
        long n = 0;
        if (nTeam < 0) {
            if (m_nShotTeamIdx == CGame::g_GameWork.m_nUserTeam) {
                n = m_nTouchNum;
            }
        } else if (m_nShotTeamIdx == nTeam) {
            n = m_nTouchNum;
        }
        return n;
    }

    long CBallVolleyVsBoss::GetVolleyCanbeTouchKind(long nTeam, long)
    {
        long nKind = 3;
        if (m_nShotTeamIdx == nTeam && m_nTouchNum <= 0) {
            nKind = 1;
        }
        return nKind;
    }

    // Every touch of the attacking team tints the ball a little more.
    void CBallVolleyVsBoss::SetTouch_(long nTeam, long nChr, int)
    {
        float rate = 1.0f;
        float num = 2.0f;
        if (num >= rate) {
            rate /= num;
        }
        if (m_nShotTeamIdx == nTeam) {
            if (m_nShotChrIdx != nChr) {
                m_nTouchNum++;
                m_nTrueTouchNum = m_nTouchNum;
                m_fTint += rate;
            }
        } else {
            m_nTrueTouchNum = m_nTouchNum;
            m_nTouchNum = 1;
            m_fTint = rate;
        }
        float t = m_fTint;
        m_fTint = t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = nTeam;
    }

    int CBallVolleyVsBoss::CanbeTouchDown_()
    {
        return 0;
    }

    // Tints the TEV colour 0 of every material towards white by m_fTint.
    void CBallVolleyVsBoss::Disp()
    {
        float t = m_fTint;
        float rate = t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
        if (m_pMdlObj == 0) {
            return;
        }
        nw4r::g3d::ResMdl mdl = m_pMdlObj->GetResMdl();
        unsigned long nMat = mdl.GetResMatNumEntries();
        GXColor color;
        *reinterpret_cast<unsigned long *>(&color) = 0xFFFFFFFF;
        float white = 255.0f;
        for (unsigned long i = 0; i < nMat; i++) {
            bcs::CMdlObj::CopiedMatAccess access(m_pMdlObj, i);
            if (!access.IsValid()) {
                continue;
            }
            nw4r::g3d::ResMatTevColor tev = access.GetResMatTevColor(0);
            if (!tev.IsValid()) {
                continue;
            }
            tev.GXGetTevColor(GX_TEVREG0, &color);
            color.r = rate * ((float)color.r - white) + white;
            color.g = rate * ((float)color.g - white) + white;
            color.b = rate * ((float)color.b - white) + white;
            tev.GXSetTevColor(GX_TEVREG0, color);
            tev.DCStore(false);
        }
    }

    void CBallVolleyVsBoss::ResetParam_()
    {
        CBallVolley::ResetParam_();
        m_bPickup = 0;
        m_fTint = 0.0f;
    }

    void CBallVolleyVsBoss::ResetOnAttach_()
    {
        CBallVolley::ResetOnAttach_();
        m_bPickup = 0;
    }

    void CBallVolleyVsBoss::CollSubCheckTgtPos_()
    {
    }

    int CBallVolleyVsBoss::CollSubCheckNet_(bcs::FVEC3 &, bcs::FVEC3 &)
    {
        return 0;
    }

    void CBallVolleyVsBoss::CollCheckPassHitNet_(bcs::FVEC3 &rOut, const bcs::FVEC3 &, const bcs::FVEC3 &rTo) const
    {
        rOut = rTo;
    }

    // Against a boss every ball on the floor is a point for the boss.
    void CBallVolleyVsBoss::TrgHitFloor_(int)
    {
        if (m_bPickup != 0 || IsScored() || m_bArtsOver != 0) {
            return;
        }
        if (TrgItemEffect_(3) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        long nScore = m_n36C;
        bcs::FVEC3 pos = m_Pos;
        pos.y = 0.0f;
        TrgVolleyArtsOver(0, -1, 1);
        CEffectCtrl::SimpleEffect(0x4E8, pos, 3, 0, 0);
        CGame::TrgVolleyGoalIn(m_nShotChrIdx, m_nShotTeamIdx, m_nShootType, nScore, 0, m_nTouchNum, pos,
                               m_nVolleyFlag444);
        if (CGame::g_GameWork.m_nFlag18 & 0x80) {
            m_nFlag |= 1;
        }
        m_nFlag |= 0x3000;
        UpdateCameraTarget_(0);
        CGame::SetVolleyServeMode(0);
        SetTouch_(-1, -1, 0);
        CGame::SetOffenseTeam(0);
        m_nTouchNum = 0;
        m_bPickup = 1;
        m_fTint = 0.0f;
    }

    void CBallVolleyVsBoss::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush == 0) {
            return;
        }
        if (IsAttach()) {
            pOther->DoBodyPushCallback(pBody, 0);
            return;
        }
        int bWallPush = 0;
        if (m_bPickup != 0 && pOther->GetOwnerKind() == 5) {
            bWallPush = 1;
        }
        if (bWallPush) {
            bcs::FVEC3 from = m_Pos;
            bcs::FVEC3 to;
            bcs::FVEC3 pos;
            bcs::FVEC3 normal;
            float dist = 0.1f * GetFrameRateF();
            bcs::FVEC3 push = *pPush;
            if (push.x * push.x + push.z * push.z >= dist * dist) {
                NormalizeXZ(push, push);
                push.x *= dist;
                push.z *= dist;
            }
            to.x = from.x + push.x;
            to.z = from.z + push.z;
            to.y = from.y;
            float time;
            CCollMan::DetectMapHit(pos, normal, time, from, to, 0.0f, 2, 0);
            pOther->DoBodyPushCallback(pBody, 0);
            SetPos(pos);
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
    }

    // The jump ball against a boss: a toss-up to the first character.
    void CBallVolleyVsBoss::SetJumpBallMode()
    {
        CChrCore *pChr = InitJumpBall_();
        bcs::FVEC3 pos = pChr->GetPos();
        pos.y = 0.0f;
        SetVolleyVsBossTossUpMode(pos, 1, 1);
    }

    // A toss up to rTarget (turned by 10 to 35 degrees, at most 8.5 away)
    // for team nTeam.
    void CBallVolleyVsBoss::SetVolleyVsBossTossUpMode(const bcs::FVEC3 &rTarget, long nTeam, unsigned long)
    {
        Detach_();
        CmnShotInit_();
        SetTouch_(nTeam, -1, 0);
        m_nShootType = 0;
        m_n34C = 0;
        m_n358 = -1;
        EffectStopTail_();
        m_nServeCourt = CGame::GetTargetGoalIdx(0);
        m_nShotChrIdx = 1;
        m_nShotTeamIdx = nTeam;
        SetShotChrNoHit_(m_nShotChrIdx);
        bcs::FVEC3 target = rTarget;
        float angle0 = __fabsf(10.0f);
        float angle1 = __fabsf(35.0f);
        float distMax = 8.5f;
        bcs::FVEC3 ofs;
        bcs::FVEC3Sub(&ofs, &target, &m_Pos);
        float angleMin = angle0;
        float angleMax = angle1;
        if (angle0 > angle1) {
            angleMin = angle1;
            angleMax = angle0;
        }
        if (angleMin < angleMax) {
            angleMin = bcs::BCS_RandRange(angleMin, angleMax);
        }
        if (angleMin > 0.0f) {
            if (bcs::BCS_RandMax(0x100L) & 1) {
                angleMin = -angleMin;
            }
            bcs::BCS_Vec_RotY(ofs, ofs, bcs::ROUND_PI(DegToRad(angleMin)));
            bcs::FVEC3Add(&target, &m_Pos, &ofs);
        }
        if (ofs.x * ofs.x + ofs.z * ofs.z > distMax * distMax) {
            bcs::FVEC3 dir;
            NormalizeXZ(dir, ofs);
            bcs::FVEC3Scale(&target, &dir, distMax);
            bcs::FVEC3Add(&target, &target, &m_Pos);
        }
        target.y = GetBgCollRadius();
        InitFlight_(target);
        float frame = 0.0f;
        m_fPassCounter = 0.0f;
        if (frame <= 0.0f) {
            frame = 70.0f;
        }
        SetPassModeParam_(target, 0.15f, 1.0f, frame, 0.0f);
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_f35C = 8.0f;
        CGame::SetOffenseTeam(0);
        ChangeState(6);
    }

    // -----------------------------------------------------------------------
    // CBallVolleyRhythmReceive

    CBallVolleyRhythmReceive::CBallVolleyRhythmReceive(bcs::CTask *pParent, long nSe, long nSe2, long nType,
                                                       long nColor, int bCom)
        : CBallVolley(pParent, 0)
    {
        m_nVolleyFlag444 |= 3;
        float scale = 1.25f;
        long nModel = 0x24;
        m_nBallType = nType;
        m_nColorType = nColor;
        m_nTouchMask = 0;
        m_nSe = nSe;
        m_nSeOrg = nSe;
        m_nSe2 = nSe2;
        m_bCom = bCom;
        m_fCatchRadius = 0.1f;
        m_nVolleyFlag318 |= 2;
        switch (nType) {
            case 0:
            case 1:
            case 2:
            case 3:
                nModel = 0;
                if (bCom) {
                    nModel = 3;
                }
                m_nTouchMask = 0xF;
                break;
            case 4:
                m_nTouchMask = 0xF;
                scale = 1.4f;
                nModel = 1;
                m_bCom = 0;
                break;
            case 5:
                m_nTouchMask = 0;
                nModel = 2;
                scale = 1.8f;
                m_bCom = 0;
                m_fCatchRadius = 0.8f;
                m_nSe2 = -1;
                break;
            case 6:
                m_nTouchMask = 0;
                nModel = 2;
                scale = 1.8f;
                m_bCom = 0;
                m_fCatchRadius = 0.8f;
                m_nSe2 = -1;
                break;
        }
        m_Rot.x = bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f));
        m_Rot.y = bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f));
        m_Rot.z = bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f));
        m_Rot.w = bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f));
        PSQUATNormalize(&m_Rot, &m_Rot);
        SetScale(scale);
        Create(nModel, 0x40);
        switch (m_nBallType) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                m_nFloorHitSe = 0xDC0;
                break;
            case 5:
                m_nFloorHitSe = 0xDC1;
                break;
            case 6:
                m_nFloorHitSe = 0xDC5;
                break;
        }
    }

    CBallVolleyRhythmReceive::~CBallVolleyRhythmReceive()
    {
    }

    // The colours of the four ball colours (kinds 0 to 3), a rainbow ball
    // (4), a played animation (5 and 6).
    void CBallVolleyRhythmReceive::CreateMdlObj_(nw4r::g3d::ResFile file, nw4r::g3d::ResMdl mdl,
                                                 nw4r::g3d::ResMdl shadowMdl, nw4r::g3d::ResMdl refMdl, long nShadow)
    {
        CBall::CreateMdlObj_(file, mdl, shadowMdl, refMdl, nShadow);
        if (m_pMdlObj == 0) {
            return;
        }
        switch (m_nBallType) {
            case 0:
            case 1:
            case 2:
            case 3:
                m_pMdlObj->SetMatTevColor(GX_TEVREG0, bcs::BCS_Color(s_aRRTevColor[m_nBallType]));
                m_pMdlObj->SetMatTevKColor(GX_KCOLOR0, bcs::BCS_Color(s_aRRKColor[m_nBallType]));
                break;
            case 4: {
                EnableRainbow(1);
                bcs::CAnmObj *pAnm = m_pMdlObj->GetAnmObj(bcs::CMdlObj::ANMOBJTYPE_MATCLR);
                if (pAnm != 0) {
                    pAnm->SetRate(4.0f);
                }
                break;
            }
            case 6:
                m_pMdlObj->SetMatTevColor(GX_TEVREG0, bcs::BCS_Color(0x28, 0x00, 0x64, 0x00));
            case 5:
                if (m_pAnmMotion != 0) {
                    m_pAnmMotion->Play(0, 2, 1.0f, 0.0f, 0.0f);
                }
                break;
        }
    }

    float CBallVolleyRhythmReceive::GetCollCatchRadiusDefault() const
    {
        return m_fCatchRadius;
    }

    int CBallVolleyRhythmReceive::TrgVolleyArtsOver(long nKind, long nChr, unsigned long nArg)
    {
        if (m_bArtsOver == 0) {
            m_bArtsOver = 1;
            float frame = 16.0f;
            if (frame > 0.0f) {
                m_fArtsOverRate = 1.0f / frame;
            } else {
                m_fArtsOverRate = 1.0f;
            }
            switch (m_nBallType) {
                case 4:
                    m_fFadeWait = 12.0f;
                    break;
                case 5:
                    m_fFadeWait = 15.0f;
                    break;
                case 6:
                    m_fFadeWait = 15.0f;
                    break;
                default:
                    m_fFadeWait = 12.0f;
                    break;
            }
        }
        return TrgVolleyArtsOver_(nKind, nChr, nArg);
    }

    int CBallVolleyRhythmReceive::IsVolleyObstacleBallRR() const
    {
        // Kinds 5 and 6.
        return (unsigned long)(m_nBallType - 5) <= 1;
    }

    // A ball of the rhythm receive mission shot at rTarget, fFrame frames
    // of flight, fBeat the music frame it is due on.
    void CBallVolleyRhythmReceive::SetVolleyMissionRRShotMode(const bcs::FVEC3 &rTarget, float fFrame, float fBeat)
    {
        Detach_();
        CmnShotInit_();
        m_nTouchNum = 0;
        m_nShootType = 6;
        m_n34C = 0;
        m_n358 = -1;
        m_nServeCourt = CGame::GetTargetGoalIdx(1);
        m_nShotChrIdx = 1;
        m_nShotTeamIdx = 1;
        SetShotChrNoHit_(1);
        bcs::FVEC3 target = rTarget;
        float frame = fFrame;
        InitFlight_(target);
        float speed = 0.13333334f;
        if (CalcXZVel2(m_Pos, target, m_Velocity, m_f320, m_f324, frame, speed, 0.8f * speed, fFrame, fFrame, 0.0f) <=
            4.0f) {
        }
        float top = 0.0f;
        float minTop = 4.0f;
        if (m_Pos.y > top) {
            top = m_Pos.y + 0.25f;
        }
        if (target.y > top) {
            top = target.y + 0.25f;
        }
        if (minTop < top) {
            minTop = top;
        }
        CalcYVG(m_Pos.y, minTop, target.y, frame, m_Velocity.y, m_fGravity);
        m_fRotSpeed = 0.2617994f;
        if (m_vTargetPos.y > 0.0f) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
        // The shot effect, turned along the flight.
        float velY = m_Velocity.y;
        unsigned long hEffect = CEffectCtrl::CreateEffect(0x47B, 3, 0, 0, 0);
        bcs::FVEC3 vel(m_Velocity.x, velY, m_Velocity.z);
        bcs::FVEC3 rot;
        rot.x = bcs::ROUND_PI(nw4r::math::Atan2Rad(bcs::BCS_Vec_GetLenXZ(vel), velY));
        rot.x = bcs::ROUND_PI(rot.x - 1.5707964f);
        rot.y = 0.0f;
        rot.z = 0.0f;
        bcs::FVEC3 ofs(0.0f, 0.0f, 0.8f);
        bcs::BCS_Vec_RotX(ofs, ofs, rot.x);
        bcs::FVEC3Add(&ofs, &ofs, &m_Pos);
        CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
        if (pCluster != 0) {
            pCluster->SetTranslate(&ofs);
            pCluster->SetRotateRad(&rot);
        }
        m_fBeat = fBeat;
        m_fMarkerDispFrame = floor(fFrame);
        m_fMarkerDispFrame2 = 6.0f;
        switch (m_nBallType) {
            default:
                m_nFloorHitSe = 0xDC0;
                break;
            case 5:
                m_nFloorHitSe = 0xDC1;
                break;
            case 6:
                m_nFloorHitSe = 0xDC5;
                break;
        }
        if (m_nBallType == 6) {
            m_nBounce = 2;
        }
        ChangeState(6);
    }

    void CBallVolleyRhythmReceive::SetVolleyServeTossMode(BALL::VOLLEY::SERVE_TOSS_ARG &rArg)
    {
        SetReceive_(rArg.m_nPlayerNo);
    }

    int CBallVolleyRhythmReceive::SetVolleyServeShotMode(BALL::VOLLEY::SERVE_SHOT_ARG &rArg)
    {
        SetReceive_(rArg.m_nPlayerNo);
        return 0;
    }

    int CBallVolleyRhythmReceive::SetVolleySpikeShotMode(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg)
    {
        SetReceive_(rArg.m_nUnk2C);
        return 0;
    }

    void CBallVolleyRhythmReceive::SetVolleyPassMode(const bcs::FVEC3 &, long, long nChr, long, long, float, long,
                                                     float, unsigned long)
    {
        SetReceive_(nChr);
    }

    void CBallVolleyRhythmReceive::SetVolleyPassMode(const bcs::FVEC3 &, long, long nChr, long, long, long,
                                                     unsigned long)
    {
        SetReceive_(nChr);
    }

    void CBallVolleyRhythmReceive::SetVolleyArtsMode(const bcs::FVEC3 &, long, long nChr, long, long, long, long,
                                                     float, float, float, unsigned long)
    {
        SetReceive_(nChr);
    }

    void CBallVolleyRhythmReceive::SetVolleyShootMode(const bcs::FVEC3 &, long, long nChr, long, long, long,
                                                      unsigned long)
    {
        SetReceive_(nChr);
    }

    void CBallVolleyRhythmReceive::SetVolleyDropMode(const bcs::FVEC3 &, long nChr, long, long nKind, unsigned long)
    {
        TrgVolleyArtsOver(1, nChr, 1);
        SetReceive_(nChr);
    }

    // Character nChr received the ball: the receive sound on the beat (a
    // good one when its colour matches the character's side), the effect,
    // and the ball goes straight up.
    void CBallVolleyRhythmReceive::SetReceive_(long nChr)
    {
        long nSide = nChr >> 1;
        long nResult = 0;
        if (m_nBallType == 5) {
            PlaySePos(m_nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            CGame::StartCameraQuake(0x32, nChr, -1);
            nResult = 3;
        } else if (m_nBallType == 6) {
            PlaySePos(m_nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            CGame::StartCameraQuake(0x32, nChr, -1);
            nResult = 4;
        } else if (m_nSe >= 0) {
            CGameSnd::GetMusicFrame();
            int bGood = 1;
            int bPad = 0;
            if (CChrMan::IsActive() && nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
                CChrCore *pChr = CChrMan::GetChrCore(nChr);
                if (pChr != 0 && pChr->IsEnable() && pChr->IsPadCtrl()) {
                    bPad = 1;
                }
            }
            if (m_nBallType == 4) {
                nResult = 6;
            } else if (m_nBallType != nSide) {
                if (IsVolleyComBallRR()) {
                    if (bPad) {
                        bGood = 1;
                        nResult = 5;
                    } else {
                        bGood = 1;
                        nResult = 0;
                    }
                } else {
                    bGood = 0;
                    nResult = 2;
                }
            }
            long nEffect = 0x47A;
            if (bGood) {
                nEffect = 0x479;
            }
            unsigned long hEffect = CEffectCtrl::CreateEffect(nEffect, 3, 0, 0, 0);
            if (hEffect != 0) {
                CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                if (pCluster != 0) {
                    pCluster->SetTranslate(&m_Pos);
                }
            }
            long nSe2 = m_nSe2;
            new (0x100) BALL_VOLLEY::CBallVolleyRRReceiveEffect(CGame::GetTempParent(), m_nSe, m_fBeat, bGood, nSe2);
        }
        Detach_();
        CmnShotInit_();
        m_nShootType = 0;
        m_n358 = -1;
        m_nShotChrIdx = nChr;
        m_nPassChrIdx = 1;
        m_nShotTeamIdx = 0;
        m_nTouchNum = 2;
        SetShotChrNoHit_(nChr);
        m_nFlag |= 0x40;
        InitFlight_(m_Pos);
        float top = 2.0f + m_Pos.y;
        CalcYVG(m_Pos.y, top, top, 60.0f, m_Velocity.y, m_fGravity);
        m_fRotSpeed = 0.10471976f;
        UpdateCameraTarget_(1);
        CGame::SendMessage(0x5C, 1, nSide, nResult);
        ChangeState(5);
    }

    int CBallVolleyRhythmReceive::IsVolleyComBallRR() const
    {
        return m_bCom;
    }

    void CBallVolleyRhythmReceive::CtorStateAlleyoop()
    {
        AttachTailEffect_();
        m_nFlag |= 0x3000;
    }

    void CBallVolleyRhythmReceive::LoopStateAlleyoop()
    {
        UpdateCountWork_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
    }

    void CBallVolleyRhythmReceive::DtorStateAlleyoop()
    {
    }

    void CBallVolleyRhythmReceive::PlayFloorHitSe(unsigned long nArg)
    {
        if (m_nFloorHitSe >= 0) {
            PlaySePos(m_nFloorHitSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            if (m_nBallType != 6) {
                m_nFloorHitSe = -1;
            }
        } else {
            CBall::PlayFloorHitSe(nArg);
        }
    }

    int CBallVolleyRhythmReceive::CollSubCheckNet_(bcs::FVEC3 &, bcs::FVEC3 &)
    {
        return 0;
    }

    void CBallVolleyRhythmReceive::CollCheckPassHitNet_(bcs::FVEC3 &rOut, const bcs::FVEC3 &,
                                                        const bcs::FVEC3 &rTo) const
    {
        rOut = rTo;
    }

    int CBallVolleyRhythmReceive::CanebVolleyTouch(long nChr) const
    {
        return (m_nTouchMask & (1 << (nChr >> 1))) != 0;
    }

    float CBallVolleyRhythmReceive::GetNoCatchCount() const
    {
        if (m_bArtsOver != 0) {
            return 99999.0f;
        }
        return 0.0f;
    }

    void CBallVolleyRhythmReceive::AttachTailEffect_()
    {
        if (m_bArtsOver != 0) {
            EffectStopTail_();
        } else if (m_n358 >= 0) {
            EffectSetTail_(m_n358);
        }
    }

    // A ball on the floor: the miss, after its last bounce.
    void CBallVolleyRhythmReceive::TrgHitFloor_(int)
    {
        if (IsScored() || m_bArtsOver != 0) {
            return;
        }
        if (TrgItemEffect_(3) & 0x10) {
            m_nVolleyFlag |= 2;
        }
        bcs::FVEC3 pos = m_Pos;
        pos.y = 0.0f;
        if (m_nBallType == 5) {
            m_Velocity.x = 0.0f;
            m_Velocity.y = 0.0f;
            m_Velocity.z = 0.0f;
            m_fRotSpeed = 0.0f;
            m_nBounce = 0;
        }
        m_nBounce--;
        if (m_nBounce <= 0) {
            TrgVolleyArtsOver(0, -1, 1);
        }
        CEffectCtrl::SimpleEffect(0x4E8, pos, 3, 0, 0);
        if (m_nBallType < 5) {
            CGame::SendMessage(0x5C, 1, -1, 1);
        }
        if (m_nBounce <= 0) {
            m_nFlag |= 0x3001;
        }
        UpdateCameraTarget_(0);
    }

    void CBallVolleyRhythmReceive::CmnShotInit_()
    {
        float scale = m_fScale;
        CBallVolley::CmnShotInit_();
        SetScale(scale);
        m_nBounce = 1;
    }
}

// ---------------------------------------------------------------------------
// The tasks the ball starts

msw::BALL_VOLLEY::CBallTouchDownMark::~CBallTouchDownMark()
{
    CEffectCtrl::EraseAttachEffect(m_hEffect);
    m_hEffect = 0;
}

// The mark drifts and turns with the floor flow.
void msw::BALL_VOLLEY::CBallTouchDownMark::Main()
{
    GetFrameRateF();
    bcs::FVEC3 pos = m_Pos;
    CStageMan *pStage = CStageMan::GetInstance();
    if (CCollMan::IsGrdAttrFlow(pStage->GetFloorAttribute(pos, CStageMan::FLOW_TYPE(6)))) {
        m_Rot.y = bcs::ROUND_PI(m_Rot.y + pStage->GetFloorFlowRot(pos, CStageMan::FLOW_TYPE(1)));
        bcs::FVEC3 flow;
        pStage->GetFloorFlow(flow, pos, CStageMan::FLOW_TYPE(1));
        m_Pos.x = pos.x + flow.x;
        m_Pos.z = pos.z + flow.z;
    }
    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(m_hEffect);
    if (pCluster != 0) {
        pCluster->SetTranslate(&m_Pos);
        pCluster->SetRotateRad(&m_Rot);
    }
}

msw::BALL_VOLLEY::CBallTouchDownMdlEffect::~CBallTouchDownMdlEffect()
{
    CEffectCtrl::EraseAttachEffect(m_hEffect);
    m_hEffect = 0;
}

void msw::BALL_VOLLEY::CBallTouchDownMdlEffect::Main()
{
    int bEnd = 1;
    unsigned long hEffect = m_hEffect;
    if (hEffect != 0) {
        CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
        if (pCluster != 0) {
            long nNum = pCluster->GetMdlObjNum();
            for (long i = 0; i < nNum; i++) {
                bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                if (pMdl == 0) {
                    continue;
                }
                {
                    bcs::CAnmObj *pAnm = pMdl->GetAnmObj(bcs::CMdlObj::ANMOBJTYPE_CHR);
                    if (pAnm != 0 && !pAnm->IsLooped() && !pAnm->IsEnd()) {
                        bEnd = 0;
                    }
                }
                {
                    bcs::CAnmObj *pAnm = pMdl->GetAnmObj(bcs::CMdlObj::ANMOBJTYPE_MATCLR);
                    if (pAnm != 0 && !pAnm->IsLooped() && !pAnm->IsEnd()) {
                        bEnd = 0;
                    }
                }
                {
                    bcs::CAnmObj *pAnm = pMdl->GetAnmObj(bcs::CMdlObj::ANMOBJTYPE_3);
                    if (pAnm != 0 && !pAnm->IsLooped() && !pAnm->IsEnd()) {
                        bEnd = 0;
                    }
                }
                {
                    bcs::CAnmObj *pAnm = pMdl->GetAnmObj(bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
                    if (pAnm != 0 && !pAnm->IsLooped() && !pAnm->IsEnd()) {
                        bEnd = 0;
                    }
                }
                {
                    bcs::CAnmObj *pAnm = pMdl->GetAnmObj(bcs::CMdlObj::ANMOBJTYPE_5);
                    if (pAnm != 0 && !pAnm->IsLooped() && !pAnm->IsEnd()) {
                        bEnd = 0;
                    }
                }
            }
        }
    }
    if (bEnd) {
        Delete();
    }
}

msw::BALL_VOLLEY::CBallVolleyRRReceiveEffect::~CBallVolleyRRReceiveEffect()
{
}

void msw::BALL_VOLLEY::CBallVolleyRRReceiveEffect::Main()
{
    CheckPlay_();
    if (m_nSe < 0) {
        Delete();
    }
}

// Plays the receive sound half a frame after its beat (at a random pitch
// for a bad receive), then the second sound.
void msw::BALL_VOLLEY::CBallVolleyRRReceiveEffect::CheckPlay_()
{
    if (m_nSe < 0 || CGameSnd::GetMusicFrame() - m_fBeat < -0.5f) {
        return;
    }
    unsigned long hSe = CGameSnd::Play(m_nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
    if (m_bGood == 0 && hSe != 0) {
        bcs::CSndTask *pSnd = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(hSe));
        if (pSnd != 0) {
            if (bcs::BCS_RandMax(0x100L) < 0x80) {
                pSnd->SetPitch(bcs::BCS_RandRange(0.45f, 0.65f));
            } else {
                pSnd->SetPitch(bcs::BCS_RandRange(2.0f, 3.0f));
            }
        }
    }
    m_nSe = -1;
    if (m_nSe2 >= 0) {
        CGameSnd::Play(m_nSe2, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        m_nSe2 = -1;
    }
}
