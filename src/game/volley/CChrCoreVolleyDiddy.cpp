// msw::CChrCoreVolleyDiddy: Diddy Kong in the volleyball game. His air
// trick shows his arts model (a boomerang in hand) and throws a boomerang
// (CC08_VOLLEY::CcBoomerangVolley) from it.
//
// msw::CC08_VOLLEY::CcBoomerangVolley: the boomerang. The base class
// (msw::CC_CMN::DIDDY::CcBoomerang, in main.dol) does the work; this class
// gives the tuning values and the trailing effect.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcBoomerang.hpp"

namespace msw {
    namespace CC08_VOLLEY {

        class CcBoomerangVolley : public CC_CMN::DIDDY::CcBoomerang {
            public:
                CcBoomerangVolley(bcs::CTask *pParent, long nPlayer, long nTeam)
                    : CcBoomerang(pParent, nPlayer, nTeam)
                {
                }
                virtual ~CcBoomerangVolley() {}

                virtual void Create(nw4r::g3d::ResFile res, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                    const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY);

            protected:
                virtual void SetParam_();
        };
    }
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

    // The original .rodata ends with a -1 that nothing left in the file
    // reads, most likely used by code the link stripped. This dead
    // comparison, made from the last function, keeps it in place. See
    // notes/decisions.md, "Unreferenced data kept by dead references".
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }
}

msw::CChrCoreVolleyDiddy::CChrCoreVolleyDiddy()
{
}

msw::CChrCoreVolleyDiddy::~CChrCoreVolleyDiddy()
{
}

float msw::CChrCoreVolleyDiddy::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x77) + (AnimGetResFrameNum(0x75) + AnimGetResFrameNum(0x76));
}

float msw::CChrCoreVolleyDiddy::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *)
{
    return AnimGetResFrameNum(0x75);
}

// The boomerang model appears in his hand.
void msw::CChrCoreVolleyDiddy::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x84, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 16.0f, 1);
}

// Step 0 waits for the trick motion and plays the throw (0x76); in step 1
// the boomerang leaves at frame 7 of it; step 2 waits for the throw to end
// and plays 0x77, step 3 for that to end.
int msw::CChrCoreVolleyDiddy::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateAirTrickDoJumpMove_(pWork);
    SubStateAirTrickDoBallAdsorb_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x76, 0, 4.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x85, 3);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                PlaySePos(0x436, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (GetArtsOpt() && AnimGetFrame(3) >= 7.0f) {
                SubStateArtsShotFireBullet_(pWork, 0);
                pWork->m_nTrickStep++;
            }
            // fall through
        case 2:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x77, 0, 4.0f, 0.0f);
                Eff_CreateAttachAuto(0x15, 0x86, 3);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
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

void msw::CChrCoreVolleyDiddy::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x16, 0x87, 3);
}

void msw::CChrCoreVolleyDiddy::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    GetAttachBall(); // (the original asks for the ball and uses nothing)
    CChrCoreVolley::SubStateAirTrickShootBall_(pWork);
    ArtsOptFadeOut(0.0f, 0.0f, 0);
}

void msw::CChrCoreVolleyDiddy::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

// The boomerang leaves the model in his hand (or his feet, without one),
// flying out 70 degrees to his right, and the model goes.
void msw::CChrCoreVolleyDiddy::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo =
        CC_DATA::GetArtsOptionMdl_CreateInfo(GetChrIdS(), CGame::g_GameWork.GetGameMode());
    if (!pInfo) {
        return;
    }
    nw4r::g3d::ResFile res = GetChrOptResFile();
    if (!res.IsValid()) {
        return;
    }

    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 rot(GetRot());
    float fRotY = WrapRad(GetRotY() + -1.2217305f); // -70 degrees
    rot.y = WrapRad(rot.y + 3.1415927f);

    bcs::CMdlObj *pMdl = GetArtsOptMdl();
    if (pMdl) {
        bcs::FMTX34 mtx;
        pMdl->GetNodeWorldMtx(1, mtx);
        bcs::FVEC3 hand(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        pos = hand;
    }

    CC08_VOLLEY::CcBoomerangVolley *pBoomerang =
        new (0x100) CC08_VOLLEY::CcBoomerangVolley(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pBoomerang) {
        pBoomerang->Create(res, pInfo, pos, rot, fRotY);
    }
    ArtsOptDestroy();
    PlaySePos(0x438, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CC08_VOLLEY::CcBoomerangVolley::Create(nw4r::g3d::ResFile res, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                                 const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY)
{
    CcBoomerang::Create(res, pInfo, rPos, rRot, fRotY);
    SetTailEffect_(0x5B0);
}

// (values' meanings not known)
void msw::CC08_VOLLEY::CcBoomerangVolley::SetParam_()
{
    m_afParam[0] = 0.23333333f;
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
    UnusedReferences(m_fUnkCC);
}
