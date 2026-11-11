// msw::CChrCoreDodgeDiddy: Diddy Kong in the dodgeball game. His special
// shot jumps twice; during the second jump he throws a banana boomerang
// (CC08_DODGE::CcBoomerangDodge) from the hand of his option model, then
// plays a last motion before the shot.
//
// CC08_DODGE::CcBoomerangDodge: the boomerang. The base class
// (msw::CC_CMN::DIDDY::CcBoomerang, in main.dol) does the work; this class
// gives the trail effect and the tuning values. (Top-level namespace, unlike
// the msw::CC08_BASKET class.)
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcBoomerang.hpp"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace CC08_DODGE {

    class CcBoomerangDodge : public msw::CC_CMN::DIDDY::CcBoomerang {
        public:
            CcBoomerangDodge(bcs::CTask *pParent, long nPlayer, long nTeam)
                : CcBoomerang(pParent, nPlayer, nTeam)
            {
            }
            virtual ~CcBoomerangDodge() {}

            virtual void Create(nw4r::g3d::ResFile file, const msw::CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY);

        protected:
            virtual void SetParam_();
    };
}

namespace {

    // (guessed helper) An angle brought into [0, 2 pi).
    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    // Stand-in for an unknown user of -1, which the original keeps in
    // .rodata after the throw's constants but nothing reads (likely stripped
    // code). See notes/decisions.md, "Unreferenced data kept by dead
    // references".
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }
}

void CC08_DODGE::CcBoomerangDodge::Create(nw4r::g3d::ResFile file, const msw::CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                          const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY)
{
    CcBoomerang::Create(file, pInfo, rPos, rRot, fRotY);
    SetTailEffect_(0x56C);
}

// (values as in the original; their meaning is not known)
void CC08_DODGE::CcBoomerangDodge::SetParam_()
{
    m_afParam[0] = 0.18333334f;
    m_afParam[1] = 5.0f;
    m_afParam[2] = 2.5f;
    m_afParam[3] = 16.0f;
    m_afParam[4] = 0.029088821f;
    m_afParam[5] = 0.07853982f;
    m_afParam[6] = 0.029088821f;
    m_afParam[7] = 16.0f;
    m_afParam[8] = 1.0f;
    m_afParam[9] = 0.1f;
    m_afParam[10] = 1.0f;
    m_afParam[11] = 30.0f;
    m_fUnkCC = 180.0f;
}

msw::CChrCoreDodgeDiddy::CChrCoreDodgeDiddy()
{
}

msw::CChrCoreDodgeDiddy::~CChrCoreDodgeDiddy()
{
}

// The first jump goes up to 2.5, the second to the height in the
// parameters (each at least where Diddy is), and both stay there.
void msw::CChrCoreDodgeDiddy::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 2.5f;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    } else {
        float fHeight = GetCcParam_()->m_Unk560;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    }
}

void msw::CChrCoreDodgeDiddy::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
}

// Both jumps move as the first one does.
void msw::CChrCoreDodgeDiddy::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long)
{
    CChrCoreDodge::SubStateArtsShotSetTrickMoveParam_(pWork, 0);
}

// Step 0 waits for the end of the first jump's motion and starts the
// second jump (motion 0x7B); step 1 throws the boomerang at frame 5 of it
// (or at its end); step 2 plays motion 0x7C at its end; ready once that one
// is over.
int msw::CChrCoreDodgeDiddy::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x7B, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x86, 3);
                PlaySePos(0x436, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_fTrickFrame = AnimGetFrameNum(3);
                SubStateArtsShotSetTrickJumpParam_(pWork, 1);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 5.0f) {
                SubStateArtsShotFireBullet_(pWork, 0);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x7C, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x87, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                pWork->m_nTrickStep++;
                bReady = 1;
            }
            break;
        default:
            bReady = 1;
            break;
    }
    return bReady;
}

// The option model (the boomerang in his hand) fades in with the trick.
void msw::CChrCoreDodgeDiddy::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x85, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 25.0f, 1);
}

void msw::CChrCoreDodgeDiddy::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
}

void msw::CChrCoreDodgeDiddy::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
    Eff_Stop(0x15);
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x16, 0x88, 3);
}

// The boomerang leaves from a node of the option model (Diddy's position
// when there is none), turned half round, and flies off 70 degrees to the
// right of his yaw. The option model goes with the throw.
void msw::CChrCoreDodgeDiddy::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    // The node of the option model the boomerang leaves from. (The original
    // keeps this one-word object in .rodata; its type is a guess.)
    static const long _SP_BULLET_NODE[] = { 1 };

    const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo =
        CC_DATA::GetArtsOptionMdl_CreateInfo(GetChrIdS(), CGame::g_GameWork.GetGameMode());
    if (!pInfo) {
        return;
    }
    nw4r::g3d::ResFile file = GetChrOptResFile();
    if (!file.IsValid()) {
        return;
    }

    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 rot(GetRot());
    float fRotY = WrapRad(GetRotY() + bcs::DEG_TO_RAD(-70.0f));
    rot.y = WrapRad(3.1415927f + rot.y);
    UnusedReferences(fRotY);

    bcs::CMdlObj *pMdl = GetArtsOptMdl();
    if (pMdl) {
        bcs::FMTX34 mtx;
        pMdl->GetNodeWorldMtx(_SP_BULLET_NODE[0], mtx);
        pos = bcs::FVEC3(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
    }

    CC08_DODGE::CcBoomerangDodge *pBanana =
        new (0x100) CC08_DODGE::CcBoomerangDodge(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pBanana) {
        pBanana->Create(file, pInfo, pos, rot, fRotY);
    }
    ArtsOptDestroy();
    PlaySePos(0x438, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}
