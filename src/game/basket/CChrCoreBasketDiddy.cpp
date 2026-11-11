// msw::CChrCoreBasketDiddy: Diddy Kong in the basketball game. His special
// shot jumps twice; at the top of the second jump he throws a banana
// boomerang (CC08_BASKET::CcBoomerangBasket) from the hand of his option
// model.
//
// msw::CC08_BASKET::CcBoomerangBasket: the boomerang. The base class
// (msw::CC_CMN::DIDDY::CcBoomerang, in main.dol) does the work; this class
// gives the trail effect and the tuning values.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcBoomerang.hpp"

namespace msw {
    namespace CC08_BASKET {

        class CcBoomerangBasket : public CC_CMN::DIDDY::CcBoomerang {
            public:
                CcBoomerangBasket(bcs::CTask *pParent, long nPlayer, long nTeam)
                    : CcBoomerang(pParent, nPlayer, nTeam)
                {
                }
                virtual ~CcBoomerangBasket() {}

                virtual void Create(nw4r::g3d::ResFile file, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                    const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY);

            protected:
                virtual void SetParam_();
        };
    }
}

namespace {

    // (guessed helpers) An angle brought into [0, 2 pi); degrees to radians.
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

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    // Stand-in for an unknown user of 0.5 and -1, which the original keeps
    // in .rodata after the throw's constants but nothing reads (likely
    // stripped code). See notes/decisions.md, "Unreferenced data kept by
    // dead references".
    inline void UnusedReferences(float f)
    {
        if (f == 0.5f) {
        }
        if (f == -1.0f) {
        }
    }

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = DegToRad(70.0f);
}

void msw::CC08_BASKET::CcBoomerangBasket::Create(nw4r::g3d::ResFile file, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                                 const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY)
{
    CcBoomerang::Create(file, pInfo, rPos, rRot, fRotY);
    SetTailEffect_(0x560);
}

// (values as in the original; their meaning is not known)
void msw::CC08_BASKET::CcBoomerangBasket::SetParam_()
{
    m_afParam[0] = 0.16666667f;
    m_afParam[1] = 5.0f;
    m_afParam[2] = 3.0f;
    m_afParam[3] = 16.0f;
    m_afParam[4] = 0.029088821f;
    m_afParam[5] = 0.07853982f;
    m_afParam[6] = 0.029088821f;
    m_afParam[7] = 16.0f;
    m_afParam[8] = 1.0f;
    m_afParam[9] = 0.1f;
    m_afParam[10] = 1.2f;
    m_afParam[11] = 70.0f;
    m_fUnkCC = 180.0f;
}

msw::CChrCoreBasketDiddy::CChrCoreBasketDiddy()
{
    m_hBanana = 0;
}

msw::CChrCoreBasketDiddy::~CChrCoreBasketDiddy()
{
    if (m_hBanana) {
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hBanana);
        if (pTask) {
            pTask->Delete();
        }
        m_hBanana = 0;
    }
}

// Both jumps go up to their height and stay there: the first to 3, the
// second to the height in the parameters (at least where Diddy is).
void msw::CChrCoreBasketDiddy::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 3.0f;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    } else {
        float fHeight = GetCcParam2_()->m_fArtsJumpHeight;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    }
}

void msw::CChrCoreBasketDiddy::DtorStateAirTrick()
{
    CChrCoreBasket::DtorStateAirTrick();
}

// Both jumps move as the first one does.
void msw::CChrCoreBasketDiddy::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    CChrCoreBasket::SubStateArtsShotSetTrickMoveParam_(pWork, 0);
}

// Step 0 waits for the end of the first jump's motion and starts the
// second jump; step 1 throws the boomerang at frame 5 of its motion (or at
// its end); ready once that motion is over.
int msw::CChrCoreBasketDiddy::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)1, (m_nStatusFlag >> 2) & 1), 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x8B, 3);
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
void msw::CChrCoreBasketDiddy::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x8A, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 25.0f, 1);
}

void msw::CChrCoreBasketDiddy::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x8C, 3);
}

void msw::CChrCoreBasketDiddy::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    Eff_Stop(0x15);
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x16, 0x8D, 3);
}

void msw::CChrCoreBasketDiddy::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    ThrowBanana_();
    m_hBanana = 0;
    ArtsOptDestroy();
}

// The boomerang leaves from node 1 of the option model (Diddy's position
// when there is none), turned half round, and flies off 80 degrees to the
// right of his yaw. The option model goes with the throw.
void msw::CChrCoreBasketDiddy::ThrowBanana_()
{
    if (m_hBanana) {
        return;
    }
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
    float fRotY = WrapRad(GetRotY() + DegToRad(-80.0f));
    rot.y = WrapRad(rot.y + 3.1415927f);
    UnusedReferences(fRotY);

    bcs::CMdlObj *pMdl = GetArtsOptMdl();
    if (pMdl) {
        bcs::FMTX34 mtx;
        pMdl->GetNodeWorldMtx(1, mtx);
        pos = bcs::FVEC3(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
    }

    CC08_BASKET::CcBoomerangBasket *pBanana =
        new (0x100) CC08_BASKET::CcBoomerangBasket(CGame::GetTempParent(), GetPlayerNo(), GetTeamNo());
    if (pBanana) {
        pBanana->Create(file, pInfo, pos, rot, fRotY);
    }
    ArtsOptDestroy();
    PlaySePos(0x438, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}
