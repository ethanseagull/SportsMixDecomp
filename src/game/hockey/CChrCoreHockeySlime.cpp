// msw::CChrCoreHockeySlime: the Slime in the hockey game. His special shot
// calls three slimes (the squad): their models first come out with him
// (CC_CMN::SLIME::CcSquadFactory), then he throws them as
// CC16_HOCKEY::CcSlimeSquadHockey, which circle a point in front of the
// goal before they chase the players of the other team. The effects of the
// trick are tinted with the Slime's colour (SLIME_COL_SET).
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_CMN/SLIME/CcSlimeSquad.hpp"
#include "game/msw/CC_CMN/SLIME/CcSquadFactory.hpp"
#include "game/msw/CC_PARAM_HOCKEY.hpp"
#include "game/msw/CChrOptTask.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"

namespace msw {
    namespace CC16_HOCKEY {

        // The colours of the trick's effects for one Slime colour: the two
        // colours of the point light and the two konstant colours of the
        // effect model.
        struct SLIME_COL_SET {
            bcs::BCS_Color m_LightColor0;
            bcs::BCS_Color m_LightColor1;
            bcs::BCS_Color m_KColor0;
            bcs::BCS_Color m_KColor1;
        };

        // The squad models' factory of the hockey game: plays a motion and
        // an effect on the three models.
        class CcSquadFactoryHockey : public CC_CMN::SLIME::CcSquadFactory {
            public:
                CcSquadFactoryHockey(bcs::CTask *pParent, CChrCore *pChr, const float *pfSpeed)
                    : CcSquadFactory(pParent, pChr, pfSpeed)
                {
                }
                // (implicit destructor: the original emits it after the
                // base's inline Main and Disp)
                virtual void AnimPlay(long nEffSlot, int bLoop, float fSpeed);
        };

        // One slime of the shot in the hockey game: its motions, effects
        // and speeds, and which way it circles.
        class CcSlimeSquadHockey : public CC_CMN::SLIME::CcSlimeSquad {
            public:
                CcSlimeSquadHockey(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nIndex,
                                   long nTeam, long nPlayer, nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot);
                virtual ~CcSlimeSquadHockey();
                virtual void CtorStateTouchDown();
                virtual void CtorStateDead();
                virtual short GetMotID_(long nMot) const;
                virtual float GetRotateRadius_() const;
                virtual float GetRotateRotSpeed_() const;
                virtual float GetRotateSpeed_() const;
                virtual float GetChaseSpeed_() const;
                virtual float GetSearchTargetDist_() const;
                virtual float GetGiveupChaseDist_() const;

            private:
                int m_bReverse; // 0x118: circles the other way
        };
    }
}

namespace {

    // The speed of each squad model's motion while they come out.
    const float s_SquadSpeed[3] = { 15.0f, 31.0f, 48.0f };

    // Each squad model's motion and the effect on it (CcSquadFactoryHockey).
    struct SQUAD_ANIM {
        short nMot;
        short nEff;
    };
    const SQUAD_ANIM s_SquadAnim[3] = {
        { 0x1, 0xCB },
        { 0x7, 0xCC },
        { 0xD, 0xCD },
    };

    // Each slime's motions (CcSlimeSquad::GetMotID_).
    const short s_SquadMotId[3][4] = {
        { 0x2, 0x3, 0x4, 0x5 },
        { 0x8, 0x9, 0xA, 0xB },
        { 0xE, 0xF, 0x10, 0x11 },
    };

    // Each slime's effects: while it flies, when it lands and when it is
    // gone; negative for none.
    struct SQUAD_EFFECT {
        short nFly;
        short nTouchDown;
        short nDead;
    };
    const SQUAD_EFFECT s_SquadEffect[3] = {
        { 0xCE, 0xD2, 0x63C },
        { 0xCF, 0xD3, 0x63D },
        { 0xD0, 0xD4, 0x63E },
    };

    // By the Slime's colour (CChrCore::m_nColor, 0 to 2).
    msw::CC16_HOCKEY::SLIME_COL_SET s_SlimeColSet[3] = {
        {
            bcs::BCS_Color(0x00, 0x46, 0xBE, 0xFF),
            bcs::BCS_Color(0x32, 0xAF, 0xFF, 0xFF),
            bcs::BCS_Color(0x32, 0xFF, 0xFF, 0xFF),
            bcs::BCS_Color(0x00, 0x46, 0xFF, 0xFF),
        },
        {
            bcs::BCS_Color(0xFF, 0x00, 0x00, 0xFF),
            bcs::BCS_Color(0xFF, 0xA5, 0x3C, 0xFF),
            bcs::BCS_Color(0xFF, 0x78, 0x32, 0xFF),
            bcs::BCS_Color(0xE6, 0x5F, 0x00, 0xFF),
        },
        {
            bcs::BCS_Color(0x5F, 0x5F, 0x5F, 0xFF),
            bcs::BCS_Color(0x91, 0x91, 0x91, 0xFF),
            bcs::BCS_Color(0xC8, 0xC8, 0xC8, 0xFF),
            bcs::BCS_Color(0x69, 0x69, 0x69, 0xFF),
        },
    };

    // (guessed helper) The colour set of colour nColor, clamped to 0 to 2.
    inline const msw::CC16_HOCKEY::SLIME_COL_SET &GetColSet(long nColor)
    {
        long nIndex;
        if (nColor > 2) {
            nIndex = 2;
        } else {
            nIndex = nColor < 0 ? 0 : nColor;
        }
        return s_SlimeColSet[nIndex];
    }

    // (guessed helper, as in CChrCoreHockeyDonkey.cpp) The larger of two
    // values.
    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    // (guessed helper) An angle brought into [0, 2 pi].
    inline float WrapRad(float fRad)
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

// ---- CChrCoreHockeySlime ----------------------------------------------

msw::CChrCoreHockeySlime::CChrCoreHockeySlime()
{
    m_hSquadFactory = 0;
}

msw::CChrCoreHockeySlime::~CChrCoreHockeySlime()
{
}

void msw::CChrCoreHockeySlime::Reset_()
{
    if (m_hSquadFactory != 0) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSquadFactory);
        if (pTask != 0 && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    CChrCoreHockey::Reset_();
}

void msw::CChrCoreHockeySlime::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    // The squad models go with the jump.
    if (m_hSquadFactory != 0) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSquadFactory);
        if (pTask != 0 && !pTask->IsDelete()) {
            pTask->Delete();
        }
    }
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xCA, 3);
    SE_PlayArtsJumpSE(0);
    const CC16_HOCKEY::SLIME_COL_SET &rColor = GetColSet(m_nColor);
    Eff_SetPointLightColor(0x13, rColor.m_LightColor0, rColor.m_LightColor1);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR0, rColor.m_KColor0);
    Eff_SetMdlKColorReg(0x13, GX_KCOLOR1, rColor.m_KColor1);
}

// The trick's motion is the two throwing motions in a row.
float msw::CChrCoreHockeySlime::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep != 0) {
        return AnimGetResFrameNum(0x9D) + AnimGetResFrameNum(0x9E);
    }
    return CChrCoreHockey::SubStateArtsShotGetTrickMotFrame_(pWork, nStep);
}

// The jump rises from where he stands to the height in the parameters (or
// stays level when he is higher already) over the trick's frames.
void msw::CChrCoreHockeySlime::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        pWork->m_nJumpType = 0;
        return;
    }

    float fHeight = GetCcParam_()->m_Unk1BC;
    float fLand = GetCcParam_()->m_Unk1C0;
    float fPosY = GetPosY();
    if (fHeight < fPosY) {
        fHeight = fPosY;
    }

    float fFrame = Max(pWork->m_fTrickMotFrameNum - fLand - 2.0f, 1.0f);
    if (fHeight > 0.0f) {
        Util_SetJumpParam(fFrame, fPosY, fPosY, fHeight);
        m_fJumpUnk224 = 0.0f;
        m_fJumpUnk228 = 0.0f;
        pWork->m_nJumpType = 2;
        return;
    }
    pWork->m_nJumpType = 0;
}

// He does not move during the trick.
void msw::CChrCoreHockeySlime::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        CChrCoreHockey::SubStateArtsShotSetTrickMoveParam_(pWork, nStep);
        return;
    }
    SubStateArtsShotClearMoveParamWork_(pWork->m_Move);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fMoveFrame = 0.0f;
}

// Motion 0x9D brings the squad out, the slimes are thrown at its shot frame
// and motion 0x9E follows; the shot is ready when that is over (and, after
// a jump, once he has landed).
int msw::CChrCoreHockeySlime::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;
    long nJump = SubStateArtsShotDoJumpMove_(pWork);

    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                SubStateArtsShotSetSetbackFrame_(pWork, 1);
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                SubStateArtsShotDoGetBallShotTrg_(pWork, 16.0f);

                CC16_HOCKEY::CcSquadFactoryHockey *pFactory =
                    new (0x100) CC16_HOCKEY::CcSquadFactoryHockey(CGame::GetTempParent(), this, s_SquadSpeed);
                if (pFactory != 0) {
                    m_hSquadFactory = pFactory->GetId();
                }
                if (m_hSquadFactory != 0) {
                    CC_CMN::SLIME::CcSquadFactory *pTask =
                        static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
                    if (pTask != 0 && !pTask->IsDelete()) {
                        pTask->AnimPlay(0, 0, 0.0f);
                    }
                }
                PlaySePos(0x487, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                SubStateArtsShotSetTrickJumpParam_(pWork, 1);
                SubStateArtsShotSetTrickMoveParam_(pWork, 1);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (!IsAnimEnd(3) && !(AnimGetFrame(3) >= pWork->m_fShotFrame)) {
                break;
            }
            SubStateArtsShotFireBullet_(pWork, 0);
            pWork->m_nTrickStep++;
            // fall through
        case 2:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9E, 0, 0.0f, 0.0f);
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                pWork->m_nTrickStep++;
                if (pWork->m_nJumpType == 2) {
                    if (nJump >= 2) {
                        bReady = 1;
                    }
                } else {
                    bReady = 1;
                }
            }
            break;
        default:
            if (pWork->m_nJumpType == 2) {
                if (nJump >= 2) {
                    bReady = 1;
                }
            } else {
                bReady = 1;
            }
            break;
    }
    return bReady;
}

void msw::CChrCoreHockeySlime::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xD1, 3);
    const CC16_HOCKEY::SLIME_COL_SET &rColor = GetColSet(m_nColor);
    Eff_SetPointLightColor(0x14, rColor.m_LightColor0, rColor.m_LightColor1);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR0, rColor.m_KColor0);
    Eff_SetMdlKColorReg(0x14, GX_KCOLOR1, rColor.m_KColor1);
}

// Throws the three slimes. They circle the corners of a triangle (turned at
// random) six units in front of the goal he aims at, each starting at
// another angle.
void msw::CChrCoreHockeySlime::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    nw4r::g3d::ResFile res = GetChrOptResFile();
    if (!res.IsValid()) {
        return;
    }
    nw4r::g3d::ResFile mot = GetChrOptMotResFile();

    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 rot(GetRot());
    bcs::FVEC3 goal(GetTargetGoalPos());
    float fGoalRotY = GetTargetGoalYRot();

    float fStartRot = 40.0f;
    float fSide = 4.0f;
    // The distance from the centre of a triangle of side fSide to its
    // corners (the sine of 60 degrees).
    float fCorner = fSide / nw4r::math::SinFIdx(42.666668f);
    float fRadius = 0.5f * fCorner;
    float fPhase = WrapRad(bcs::BCS_RandMax(6.2831855f));

    bcs::FVEC3 center;
    bcs::BCS_VecZ_RotY(center, 2.0f + fSide, fGoalRotY);
    center.x += goal.x;
    center.z += goal.z;

    bcs::FVEC3 aCorner[3];
    bcs::FVEC3 *pCorner = aCorner;
    long nDeg = 0;
    for (long i = 0; i < 3; i++) {
        float fOffset = 0.017453292f * (float)nDeg;
        bcs::BCS_VecZ_RotY(*pCorner, fRadius, WrapRad(fPhase + fOffset));
        pCorner->x += center.x;
        pCorner->z += center.z;
        pCorner->y = 0.0f;
        nDeg += 120;
        pCorner++;
    }

    pCorner = aCorner;
    for (long i = 0; i < 3; i++) {
        if (m_hSquadFactory != 0) {
            CC_CMN::SLIME::CcSquadFactory *pFactory =
                static_cast<CC_CMN::SLIME::CcSquadFactory *>(bcs::CTaskMan::GetTask(m_hSquadFactory));
            if (pFactory != 0 && !pFactory->IsDelete()) {
                pFactory->KillChild(i);
            }
        }
        CC16_HOCKEY::CcSlimeSquadHockey *pSquad = new (0x100) CC16_HOCKEY::CcSlimeSquadHockey(
            CGame::GetTempParent(), pos, rot, i, GetTeamNo(), GetPlayerNo(), res, mot);
        if (pSquad != 0) {
            pSquad->SetTargetOrbit(*pCorner, 2.0f + GetPosY(), fStartRot);
            fStartRot += 30.0f;
        }
        pCorner++;
    }
    PlaySePos(0x4EA, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

// ---- CC16_HOCKEY::CcSquadFactoryHockey ----------------------------------

// Plays each squad model's motion (looped or not, at fSpeed) and puts its
// effect in effect slot nEffSlot.
void msw::CC16_HOCKEY::CcSquadFactoryHockey::AnimPlay(long nEffSlot, int bLoop, float fSpeed)
{
    const SQUAD_ANIM *pAnim = s_SquadAnim;
    for (long i = 0; i < SQUAD_NUM; i++) {
        if (m_aTaskId[i] != 0) {
            CChrOptTask *pTask = static_cast<CChrOptTask *>(bcs::CTaskMan::GetTask(m_aTaskId[i]));
            if (pTask != 0 && !pTask->IsDelete()) {
                if (pAnim->nMot >= 0) {
                    pTask->AnimPlay(pAnim->nMot, bLoop, fSpeed, 0.0f);
                }
                if (pAnim->nEff >= 0) {
                    pTask->Eff_CreateAttachAuto(nEffSlot, pAnim->nEff, 3);
                }
            }
        }
        pAnim++;
    }
}

// ---- CC16_HOCKEY::CcSlimeSquadHockey ------------------------------------

inline msw::CC16_HOCKEY::CcSlimeSquadHockey::CcSlimeSquadHockey(bcs::CTask *pParent, const bcs::FVEC3 &rPos,
                                                               const bcs::FVEC3 &rRot, long nIndex, long nTeam,
                                                               long nPlayer, nw4r::g3d::ResFile res,
                                                               nw4r::g3d::ResFile mot)
    : CcSlimeSquad(pParent, rPos, rRot, nIndex, nTeam, nPlayer)
{
    m_bReverse = bcs::BCS_RandMax(1L);
    Create(res, mot);
    m_Body.SetHitEffectFrameAll(0.0f);
    short nEff = s_SquadEffect[m_nIndex].nFly;
    if (nEff >= 0) {
        m_hEffect = CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
    }
}

msw::CC16_HOCKEY::CcSlimeSquadHockey::~CcSlimeSquadHockey()
{
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
}

short msw::CC16_HOCKEY::CcSlimeSquadHockey::GetMotID_(long nMot) const
{
    return s_SquadMotId[m_nIndex][nMot];
}

void msw::CC16_HOCKEY::CcSlimeSquadHockey::CtorStateTouchDown()
{
    CcSlimeSquad::CtorStateTouchDown();
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
    if (m_pMdl != 0) {
        short nEff = s_SquadEffect[m_nIndex].nTouchDown;
        if (nEff >= 0) {
            CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, 3);
        }
    }
}

// A last effect where the slime's item node was.
void msw::CC16_HOCKEY::CcSlimeSquadHockey::CtorStateDead()
{
    short nEff = s_SquadEffect[m_nIndex].nDead;
    if (nEff >= 0 && m_pMdl != 0) {
        long nNode = m_pMdl->GetNodeID("SP_item_00");
        bcs::FMTX34 mtx;
        m_pMdl->GetScnMtxPos(&mtx, nNode > 0 ? nNode : 0, static_cast<bcs::CScnObj::ScnObjMtxType>(1));
        bcs::FVEC3 pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        CEffectCtrl::SimpleEffect(nEff, pos, 3, 0, 0);
    }
    CcSlimeSquad::CtorStateDead();
}

float msw::CC16_HOCKEY::CcSlimeSquadHockey::GetRotateRadius_() const
{
    return 3.5f;
}

float msw::CC16_HOCKEY::CcSlimeSquadHockey::GetRotateSpeed_() const
{
    return 4.0f * (1.0f / 60.0f);
}

float msw::CC16_HOCKEY::CcSlimeSquadHockey::GetChaseSpeed_() const
{
    return 3.0f * (1.0f / 60.0f);
}

float msw::CC16_HOCKEY::CcSlimeSquadHockey::GetSearchTargetDist_() const
{
    return 0.1f;
}

float msw::CC16_HOCKEY::CcSlimeSquadHockey::GetGiveupChaseDist_() const
{
    return 0.1f;
}

// Two degrees a frame, either way.
float msw::CC16_HOCKEY::CcSlimeSquadHockey::GetRotateRotSpeed_() const
{
    if (m_bReverse != 0) {
        return -0.034906585f;
    }
    return 0.034906585f;
}
