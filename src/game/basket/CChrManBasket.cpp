// msw::CChrManBasket: the character manager of the basketball game, and
// msw::CChrManBasketHungryBossPackun, the one of its party game against the
// Hungry Boss Packun.
#include "game/msw/CChrManBasket.hpp"

#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/New.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManBasket.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrDrvPadBasket.hpp"
#include "game/msw/CChrMarkerBasket.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CComDrvBasketGame.hpp"
#include "game/msw/CComDrvBasketHB.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameGuideManBasket.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/triangular.h"

namespace msw {

    // The character manager of the party game against the Hungry Boss
    // Packun (game type 2), size 0xEC: four teams of one (two of three in a
    // tournament), all with pads or the script-driven computer. Its
    // functions are all inline: the original emits them as weak functions
    // after CChrManBasket's, in reverse order of these declarations (the
    // destructor last), and builds the drivers' table before any other
    // constant of the file.
    class CChrManBasketHungryBossPackun : public CChrManBasket {
        public:
            CChrManBasketHungryBossPackun(bcs::CTask *pParent) : CChrManBasket(pParent)
            {
                m_nUnkE8 = 0;
                m_nChrCoreNum = CGame::IsTournament() ? 6 : 4;
            }

            virtual void Create_()
            {
                CreateObj_();
                CreateDriver_();
                CreateMarker_();
                SetTeamLeader(0, 0, 0);
            }

            virtual void CreateObj_() { CChrManBasket::CreateObj_(); }

            virtual void CreateDriver_()
            {
                long nNum = m_nChrCoreNum;
                long nTeamNum = CGame::IsTournament() ? 2 : 4;
                long *pDrvId = (long *)__alloca(nNum * sizeof(long));
                long *pCamId = (long *)__alloca(nNum * sizeof(long));

                SetDrvIdAssign_(nNum, pDrvId);
                SetCamIdAssign_(nNum, pCamId);

                long nPad = 0;

                for (long i = 0; i < nNum; i++) {
                    CChrCore *pChr = m_ppChrCore[i];
                    if (pChr->IsDisable()) {
                        continue;
                    }

                    // The pads are numbered in order; a computer gets pad 0.
                    CChrDrv *pDrv;
                    long nPadId;
                    if (pDrvId[i] == 1) {
                        pDrv = new (0) CChrDrvPadBasket;
                        nPadId = nPad++;
                    } else {
                        nPadId = 0;
                        pDrv = new (0) CComDrvBasketHB;
                    }
                    pChr->DriverSetEntry(0, pDrv);
                    pChr->SetTeamNo(i & (nTeamNum - 1));
                    pChr->SetPadID(nPadId);
                    pChr->SetCamNo(pCamId[i]);
                    pChr->DriverConnect(0, 0, 0, 0, 0);
                    pChr->OnStatusFlag(0x100);
                }
            }

            // On stage 6 (not in a tournament) the first two players are
            // pads, the rest computers.
            virtual void SetDrvIdAssign_(long nNum, long *pId)
            {
                if (CGame::g_GameWork.GetUnk06() == 6 && !CGame::IsTournament()) {
                    const long drvId[] = { 1, 1, 3, 3, 3, 3, 3, 3 };
                    static const long MAX = 8;
                    for (long i = 0; i < nNum; i++) {
                        pId[i] = drvId[i];
                    }
                } else {
                    CChrMan::SetDrvIdAssign_(nNum, pId);
                }
            }

            virtual void CreateMarker_()
            {
                long nTeamNum = CGame::IsTournament() ? 2 : 4;

                for (long i = 0; i < m_nChrCoreNum; i++) {
                    if (m_ppChrCore[i]->IsDisable()) {
                        continue;
                    }
                    if (m_ppChrCore[i]->DriverGetType() == 1) {
                        CChrMarkerBasketFactory::CreateMarker(this, i, 2, i & (nTeamNum - 1));
                    } else {
                        CChrMarkerBasketFactory::CreateMarker(this, i, 3, i & (nTeamNum - 1));
                    }
                }
            }

            virtual void AllReset_() { CChrManBasket::AllReset_(); }

            // State 1 (time over): every character learns whether its team
            // won (the teams with the best score, unless all scores are
            // equal).
            virtual void AllSetState_(long nState, long nArg1, long nArg2)
            {
                if (nState == 1) {
                    unsigned long nWinBit = 0;
                    int bDraw = 1;
                    long nScore = CGame::GetTeamScore(0);
                    for (long i = 0; i < CGame::g_GameWork.m_nTeamNum; i++) {
                        if (nScore != CGame::GetTeamScore(i)) {
                            bDraw = 0;
                            break;
                        }
                    }
                    if (!bDraw) {
                        long nMax = 0;
                        for (long i = 0; i < CGame::g_GameWork.m_nTeamNum; i++) {
                            long n = CGame::GetTeamScore(i);
                            if (n >= nMax) {
                                nWinBit = n == nMax ? nWinBit | (1 << i) : 1 << i;
                                nMax = n;
                            }
                        }
                    }
                    for (long i = 0; i < GetChrCoreNum(); i++) {
                        CChrCore *pChr = m_ppChrCore[i];
                        if (pChr != 0 && !pChr->IsDisable()) {
                            long nTeam = pChr->GetTeamNo();
                            pChr->MethdTimeOver((nWinBit & (1 << nTeam)) ? nTeam : -1);
                        }
                    }
                } else {
                    CChrMan::AllSetState_(nState, nArg1, nArg2);
                }
            }

            virtual void InitLeader_(int bConnect)
            {
                if (m_ppChrCore == 0) {
                    return;
                }

                for (long i = 0; i < 4; i++) {
                    SetTeamLeader(i, i, 0);
                }

                if (!bConnect) {
                    return;
                }

                for (long i = 0; i < m_nChrCoreNum; i++) {
                    CChrCore *pChr = m_ppChrCore[i];
                    if (!pChr->IsDisable()) {
                        pChr->DriverConnect(0, 0, 0, 0, 0);
                    }
                }
            }

        protected:
            long m_nUnkE8; // 0xE8, cleared by the constructor only
    };
}

// Set by the file's static initialiser and used by nothing in the file
// (see game/basket/CBallManBasket.cpp, which has the same angle).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (names guessed) An angle brought into [0, 2 pi), and into (-pi, pi].
    inline float RoundRad(float fRad)
    {
        while (fRad > PI2) {
            fRad -= PI2;
        }
        while (fRad < 0.0f) {
            fRad += PI2;
        }
        return fRad;
    }

    inline float RoundRadPI(float fRad)
    {
        fRad = RoundRad(fRad);
        if (fRad > PI) {
            fRad -= PI2;
        }
        return fRad;
    }

    // rOut = r1 - r2 and rOut = r1 + r2 (stand-ins for vector operators of
    // the original, as in game/hockey/CChrManHockey.cpp).
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Sub(&rOut, &r1, &r2);
    }
    inline void VecAdd(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Add(&rOut, &r1, &r2);
    }

    // (guessed helpers, as in game/basket/CComManBasket.cpp) rPos1 - rPos2,
    // and the length of rVec in the XZ plane.
    inline bcs::FVEC3 Diff(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 diff;
        return *bcs::FVEC3Sub(&diff, &rPos1, &rPos2);
    }

    inline float LenXZ(const bcs::FVEC3 &rVec)
    {
        bcs::FVEC3 xz(rVec.x, 0.0f, rVec.z);
        return bcs::FVEC3Len(&xz);
    }

    // The value clamped to [fMin, fMax].
    inline float Clamp(float f, float fMin, float fMax)
    {
        return f > fMax ? fMax : (f < fMin ? fMin : f);
    }

    // The flag of the word at 0xEC of a character that is in pass-check
    // mode (CChrManBasket::CheckPassMode_ ends it).
    const unsigned long PASS_CHECK_MODE = 0x10000000;
}

namespace msw {

CChrManBasket::~CChrManBasket()
{
    if (m_ppChrCore != 0) {
        for (long i = 0; i < m_nChrCoreNum; i++) {
            if (m_ppChrCore[i] != 0) {
                delete m_ppChrCore[i];
            }
            m_ppChrCore[i] = 0;
        }
    }
}

CChrMan *CChrManBasket::CreateInstance(bcs::CTask *pParent)
{
    CChrMan *pMan;

    if (CGame::g_GameWork.GetGameType() == 2) {
        pMan = new CChrManBasketHungryBossPackun(pParent);
    } else {
        pMan = new CChrManBasket(pParent);
    }
    CGameGuideManBasket::Create(pMan);
    SetInstance_(pMan);
    return GetInstance_();
}

void CChrManBasket::Create_()
{
    CreateObj_();
    CreateDriver_();
    CreateMarker_();

    switch (CGame::g_GameWork.m_nUnk06) {
        case 0:
        case 1:
        case 9:
        case 10:
        case 13:
        case 14:
        case 15:
        case 16:
        case 18:
        case 19:
        case 21:
        case 24:
        case 25:
            m_nTeamChrNum = 3;
            break;
        case 2:
        case 3:
        case 11:
            m_nTeamChrNum = 1;
            break;
        default:
            m_nTeamChrNum = 2;
            break;
    }

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }

    // The ghosts of stage 6, one per team.
    if (CGame::g_GameWork.GetStage() == 6) {
        for (long i = 0; i < 2; i++) {
            long nType = CGameBasket::GetTeamGhostType(i);
            GetInstance()->CreateGhost_(nType, i);
        }
    }
}

void CChrManBasket::CreateObj_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        long nType = CGame::GetCharType(i);
        long nColor = CGame::GetCharColor(i);
        CChrCore *pChr;

        switch (nType) {
            case 0:  pChr = new (0) CChrCoreBasketMario; break;
            case 1:  pChr = new (0) CChrCoreBasketLuigi; break;
            case 2:  pChr = new (0) CChrCoreBasketPeach; break;
            case 3:  pChr = new (0) CChrCoreBasketDaisy; break;
            case 4:  pChr = new (0) CChrCoreBasketYoshi; break;
            case 5:  pChr = new (0) CChrCoreBasketWario; break;
            case 6:  pChr = new (0) CChrCoreBasketWaluigi; break;
            case 7:  pChr = new (0) CChrCoreBasketDonkey; break;
            case 8:  pChr = new (0) CChrCoreBasketDiddy; break;
            case 9:  pChr = new (0) CChrCoreBasketKinopio; break;
            case 10: pChr = new (0) CChrCoreBasketKoopa; break;
            case 11: pChr = new (0) CChrCoreBasketKoopaJr; break;
            case 12: pChr = new (0) CChrCoreBasketMoogle; break;
            case 13: pChr = new (0) CChrCoreBasketSaboten; break;
            case 14: pChr = new (0) CChrCoreBasketNinja; break;
            case 15: pChr = new (0) CChrCoreBasketSiroma; break;
            case 16: pChr = new (0) CChrCoreBasketSlime; break;
            case 17: pChr = new (0) CChrCoreBasketKuroma; break;
            case 19: pChr = new (0) CChrCoreBasketMii00; break;
            case 20: pChr = new (0) CChrCoreBasketMii01; break;
            case 21: pChr = new (0) CChrCoreBasketMii00; break;
            case 22: pChr = new (0) CChrCoreBasketMii01; break;
            case 23: pChr = new (0) CChrCoreBasketMii00; break;
            case 24: pChr = new (0) CChrCoreBasketMii01; break;
            default: pChr = new (0) CChrCoreBasket; break;
        }
        m_ppChrCore[i] = pChr;

        // A negative type is an empty slot: the character stays out of
        // the game.
        if (nType < 0) {
            pChr->SetDisable();
        } else {
            pChr->Create(m_pChrTask, i, nType, nColor);
        }
    }
}

void CChrManBasket::CreateDriver_()
{
    long nNum = m_nChrCoreNum;
    long *pPadId = (long *)__alloca(nNum * sizeof(long));
    long *pCamId = (long *)__alloca(nNum * sizeof(long));
    long *pDrvId = (long *)__alloca(nNum * sizeof(long));

    SetPadIdAssign_(nNum, pPadId);
    SetCamIdAssign_(nNum, pCamId);
    SetDrvIdAssign_(nNum, pDrvId);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        CChrDrv *pPad = new (0x100) CChrDrvPadBasket;
        CChrDrv *pCom = new (0x100) CComDrvBasketGame;
        pChr->DriverSetEntry(0, pPad);
        pChr->DriverSetEntry(1, pCom);
        pChr->SetTeamNo(i & 1);
        pChr->SetPadID(pPadId[i]);
        pChr->SetCamNo(pCamId[i]);
        if (pDrvId[i] == 1) {
            pChr->DriverConnect(0, 0, 0, 0, 0);
        } else {
            pChr->DriverConnect(1, 0, 0, 0, 0);
        }
        if (i < 2) {
            pChr->OnStatusFlag(0x100);
        }
    }
}

void CChrManBasket::CreateMarker_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        if (m_ppChrCore[i]->IsDisable()) {
            continue;
        }

        long nTeam = m_ppChrCore[i]->GetTeamNo();
        if (CGame::IsPlayTypeCanChangeLeader(nTeam)) {
            CChrMarkerBasketFactory::CreateMarker(this, i, 0, -1);
            if (IsNeedSubComChrMarker_(nTeam)) {
                CChrMarkerBasketFactory::CreateMarker(this, i, 1, -1);
            }
        } else if (m_ppChrCore[i]->DriverGetType() == 1) {
            CChrMarkerBasketFactory::CreateMarker(this, i, 2, -1);
        } else {
            CChrMarkerBasketFactory::CreateMarker(this, i, 3, -1);
        }
    }
}

void CChrManBasket::AllReset_()
{
    m_nFlagAC = 0;
    CGame::BgSetFadeIn(0.0f, 0.0f);
    ResetClockOver_();
    m_pArtsChr = 0;
    for (long i = 0; i < 4; i++) {
        m_fPassDisableFrame[i] = 0.0f;
        m_fChgInputDisableFrame[i] = 0.0f;
    }
    for (long i = 0; i < 4; i++) {
        m_fPrevLeaderFrame[i] = 0.0f;
        m_pPrevLeader[i] = 0;
    }
    CGameGuideManBasket::RemoveGuideTelop();
}

void CChrManBasket::InitLeader_(int bConnect)
{
    if (m_ppChrCore == 0) {
        return;
    }

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }

    if (!bConnect) {
        return;
    }

    long nNum = m_nChrCoreNum;
    long *pDrvId = (long *)__alloca(nNum * sizeof(long));
    SetDrvIdAssign_(nNum, pDrvId);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable()) {
            long nDrv = CGame::GetTeamCtrlType(pChr->GetTeamNo()) == 1 ? 1 : pDrvId[i] == 3;
            pChr->DriverConnect(nDrv, 0, 0, 0, 0);
        }
    }
}

void CChrManBasket::CheckPassMode_(CChrCore *pChr)
{
    if (!(pChr->GetFlagEC() & PASS_CHECK_MODE)) {
        return;
    }

    if (pChr->GetStatusFlag() & 1) {
        pChr->OffFlagEC(PASS_CHECK_MODE);
        return;
    }

    CChrCore *pTo = pChr->GetPassCheckChr();
    if (pTo != 0) {
        CBall *pBall = pChr->GetPassToBall();
        if (pBall != 0 && (static_cast<CBallBasket *>(pBall)->GetBasketFlag() & 0x200000)) {
            pChr->OffFlagEC(PASS_CHECK_MODE);
            return;
        }

        // Whether the pad may switch to the receiver.
        int bChange = CGame::IsPlayTypeCanChangeLeader(pChr->GetTeamNo());
        if (!static_cast<CChrCoreBasket *>(pChr)->IsAutoChrChangeToPassCatch()) {
            bChange = 0;
        }

        if (pTo->GetStatusFlag() & 1) {
            if (!bChange) {
                pChr->OffFlagEC(PASS_CHECK_MODE);
                return;
            }
            pChr->OffFlagEC(PASS_CHECK_MODE);
            if (pChr->DriverSendMessage((CChrDrv::MSG_ID)0x27, 0, 0, 0, 0) == 0 &&
                pChr->MethdChgLeader(pTo->GetPlayerNo(), 0, 0)) {
                pChr->OffFlagEC(PASS_CHECK_MODE);
            }
        } else {
            if ((CGame::g_GameWork.m_nFlag18 & 1) || CGame::g_GameWork.m_nUserTeam != pChr->GetTeamNo()) {
                pChr->OffFlagEC(PASS_CHECK_MODE);
                return;
            }
            if (IsChgCharacterPassEnable(pChr->GetTeamNo())) {
                if (!bChange) {
                    pChr->OffFlagEC(PASS_CHECK_MODE);
                    return;
                }
                if (pChr->DriverSendMessage((CChrDrv::MSG_ID)0x27, 0, 0, 0, 0) == 0 &&
                    pChr->MethdChgLeader(pTo->GetPlayerNo(), 0, 0)) {
                    pChr->OffFlagEC(PASS_CHECK_MODE);
                }
            }
        }
    } else {
        pChr->OffFlagEC(PASS_CHECK_MODE);
    }
}

void CChrManBasket::ChrCoreMainCall_()
{
    for (long i = 0; i < GetChrCoreNum(); i++) {
        CChrCore *pChr = GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable()) {
            static_cast<CChrCoreBasket *>(pChr)->m_nBasketFlag &= ~2;
            pChr->OffFlagF4(0x100000);
        }
    }

    CChrMan::ChrCoreMainCall_();

    // The ball's marker is hidden while a character shows its one-on-one
    // marker.
    if (!(GetInstance()->m_nFlagAC & 2)) {
        CChrCore *pHold = Util_SearchHoldBallChr(0);
        if (pHold != 0) {
            int bMarker = 1;
            for (long i = 0; i < GetChrCoreNum(); i++) {
                CChrCore *pChr = GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && pChr->IsDisp1On1Maker()) {
                    bMarker = 0;
                    break;
                }
            }
            CBall *pBall = pHold->GetAttachBall();
            if (pBall != 0) {
                pBall->SetDisableMarker(!bMarker);
            }
        }
    }
}

void CChrManBasket::MainFunc_()
{
    float fRate = GetFrameRateF();
    float fZero = 0.0f;

    for (long i = 0; i < 4; i++) {
        if (m_fPrevLeaderFrame[i] > fZero) {
            m_fPrevLeaderFrame[i] -= fRate;
            if (m_fPrevLeaderFrame[i] <= fZero) {
                m_fPrevLeaderFrame[i] = fZero;
            }
        }
        if (m_fPassDisableFrame[i] > fZero) {
            m_fPassDisableFrame[i] -= fRate;
            if (m_fPassDisableFrame[i] <= fZero) {
                m_fPassDisableFrame[i] = fZero;
            }
        }
        if (m_fChgInputDisableFrame[i] > fZero) {
            m_fChgInputDisableFrame[i] -= fRate;
            if (m_fChgInputDisableFrame[i] <= fZero) {
                m_fChgInputDisableFrame[i] = fZero;
            }
        }
    }

    // The end of the arts event of a special shot.
    if ((m_nFlagAC & 4) && m_pArtsChr != 0) {
        switch (m_pArtsChr->GetArtsShootType()) {
            case 10:
            case 11:
                if (m_pArtsChr->GetState() != 0x27) {
                    m_nFlagAC &= ~6;
                    m_pArtsChr = 0;
                    CGame::BgSetFadeIn(0.0f, 30.0f);
                    CGame::SendMessage(0xE, 0, 0, 0);
                }
                break;
            case 9:
                if (!m_pArtsChr->IsHoldBall() || m_pArtsChr->GetState() != 0x27) {
                    CBall *pBall = CBallMan::GetBallPtr(0);
                    if (pBall == 0 || !pBall->IsShootMode()) {
                        m_nFlagAC &= ~6;
                        m_pArtsChr = 0;
                        CGame::BgSetFadeIn(0.0f, 30.0f);
                        CGame::SendMessage(0xE, 0, 0, 0);
                    }
                }
                break;
        }
    }

    // The rim lets go of a dunk.
    if (m_nFlagAC & 0x40) {
        m_fDunkReleaseFrame -= GetFrameRateF();
        if (m_fDunkReleaseFrame <= 0.0f) {
            CGoalObj *pGoal = CStageMan::GetInstance()->GetGoal(m_nDunkGoal);
            if (pGoal != 0) {
                pGoal->DunkRelease();
            }
            m_nFlagAC &= ~0x40;
        }
    }
}

CChrCore *CChrManBasket::Util_SearchChgLeader_(long, long, CChrCore *)
{
    return 0;
}

CChrCore *CChrManBasket::Util_SearchNearOwnGoal_(long nTeam, int nArg, CChrCore *pExclude)
{
    return Util_SearchNearPos_(CGame::GetGoalPos(1 - CGame::GetGoalIdx(nTeam)), nTeam, nArg, pExclude);
}

CChrCore *CChrManBasket::Util_SearchPassTo_(long nChr, float fRotY)
{
    CChrCore *pChr;
    float fMin = 12.566371f;
    CChrCore *pRet = 0;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();

    // The team mate nearest to the direction fRotY.
    for (long i = 0; i < m_nChrCoreNum; i++) {
        pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() &&
            !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000)) {
            float fDiff = __fabsf(RoundRadPI(pMe->GetRotY_ToChr(pChr) - fRotY));
            if (fDiff < fMin) {
                fMin = fDiff;
                pRet = pChr;
            }
        }
    }
    return pRet;
}

CChrCore *CChrManBasket::Util_SearchPassmeetTo_(long nChr, float fRotY, bcs::FVEC3 &rPos)
{
    // Three ranges (15, 35 and 45 degrees) that are not used; their
    // constants are in the original's .rodata all the same, at this place.
    // The pass goes straight at a receiver within 15 degrees of the
    // passer's yaw.
    float fRangeNear = 0.2617994f;
    float fRangeMid = 0.61086524f;
    float fRangeFar = 0.7853982f;

    CChrCore *pRet = 0;
    float fMin = 999.0f;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();
    bcs::FVEC3 myPos;
    pMe->GetCompPos(myPos);

    // The team mate nearest to the direction fRotY.
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() &&
            !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000)) {
            float fDiff = __fabsf(RoundRadPI(pMe->GetRotY_ToChr(pChr) - fRotY));
            if (fDiff < fMin) {
                fMin = fDiff;
                pRet = pChr;
            }
        }
    }

    if (pRet != 0) {
        if (pRet->GetState() == 0x28) {
            // (state 0x28) The pass goes where the receiver is.
            rPos = pRet->GetPos();
        } else {
            bcs::FVEC3 pos;
            bcs::FVEC3 vec;
            float fTime;
            float fDiff = RoundRadPI(pMe->GetRotY() - pMe->GetRotY_ToChr(pRet));
            float fAbs = __fabsf(fDiff);
            if (fAbs <= 0.2617994f) {
                // Nearly ahead: straight at the receiver.
                pRet->GetCompPos(pos);
                VecSub(vec, pos, myPos);
                pRet->GetCompPos(rPos);
            } else {
                // Otherwise ahead of it, 30 degrees off the passer's yaw at
                // most, as far away as the receiver.
                float fOfs = fAbs >= 0.5235988f ? fAbs - 0.5235988f : 0.5235988f - fAbs;
                if (fDiff > 0.0f) {
                    fOfs = -fOfs;
                }
                pRet->GetCompPos(pos);
                VecSub(vec, pos, myPos);
                float fLen = LenXZ(vec);
                bcs::BCS_VecZ_RotY(rPos, fLen, RoundRad(fOfs + pMe->GetRotY()));
                VecAdd(rPos, rPos, myPos);
            }

            // Over a jumping receiver the pass goes higher.
            float fHeight = 0.0f;
            if (pRet->GetStatusFlag() & 0x8000) {
                fHeight = Clamp(pRet->GetUnk1AC() - 1.0f, fHeight, 2.0f);
            } else if ((pRet->GetStatusFlag() & 1) == 1) {
                fHeight = Clamp(1.1f * GetFrameRateF() * 0.033333335f + fHeight, 0.0f, 1.1f);
            }
            if (CCollMan::DetectMapHit(pos, vec, fTime, myPos, rPos, 1.2f + fHeight, 1, 0x1000000)) {
                rPos.x = pos.x;
                rPos.y = pos.y;
                rPos.z = pos.z;
            }
        }
    }
    return pRet;
}

int CChrManBasket::IsBadConditon(const CChrCore *pChr)
{
    if (pChr == 0) {
        return 1;
    }
    if (pChr->IsDisable()) {
        return 1;
    }
    if (pChr->GetStatusFlag() & 0x10000) {
        return 1;
    }
    if (pChr->GetState() == 6) {
        return 1;
    }
    if (pChr->GetComLevelWork() == 4) {
        return 1;
    }
    return (unsigned long)pChr->GetComLevelWork() > 1;
}

void CChrManBasket::Util_SendPassRequest_(long nTeam, const CChrCore *pChr)
{
    long nChrTeam = pChr->GetTeamNo();
    long nPlayer = pChr->GetPlayerNo();

    CChrCore *pHold = Util_SearchHoldBallChr(nTeam);
    if (pHold != 0) {
        if (nChrTeam == pHold->GetTeamNo()) {
            pHold->DriverSendMessage((CChrDrv::MSG_ID)0x52, nPlayer, 0, 0, 0);
        }
    } else {
        CNpcBase *pNpc = Util_SearchHoldBallNpc(nTeam);
        if (pNpc != 0) {
            long nType = pNpc->GetNpcType();
            if (nType >= 3 && nType <= 5) {
                NpcSendMessage(nType, 3, nPlayer, 0, 0);
            }
        }
    }
}

float CChrManBasket::GetRotY_Pos1ToPos2XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
{
    bcs::FVEC3 dir;
    dir.x = rPos2.x - rPos1.x;
    dir.z = rPos2.z - rPos1.z;
    return RoundRad(nw4r::math::Atan2Rad(dir.x, dir.z));
}

int CChrManBasket::InRectAndRad_(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2, const bcs::FVEC3 &rPos3,
                                 float fA, float fB, float fRange) const
{
    if (!bcs::BCS_Util_IsPointInFaceXZ(rPos3, rPos1, rPos2, fB, fA)) {
        return 0;
    }
    float fRot2 = GetRotY_Pos1ToPos2XZ(rPos1, rPos2);
    float fRot3 = GetRotY_Pos1ToPos2XZ(rPos1, rPos3);
    return !(__fabsf(RoundRadPI(fRot3 - fRot2)) > fRange);
}

void CChrManBasket::CheckCameraSlowTrickMove_(float fRate, const CChrCore *pTarget, CChrCoreBasket *pChr)
{
    if (pChr->CheckStateWorkTrickMoveSlow(fRate, pTarget)) {
        CGameBasket::GetInstance()->PlayExceptionalCameraSlow_(5);
    }
}

int CChrManBasket::In3PoinArea_(long nGoal, const bcs::FVEC3 &rPos, float fMargin) const
{
    fMargin += CGameBasket::GetCourt3PointSize();
    return CStageMan::GetInstance()->Is3PointArea(nGoal, rPos, fMargin);
}

void CChrManBasket::AllStop_(CChrCore::STOP_MODE mode, CChrCore::STOP_MODE)
{
    CChrMan::AllStop_(mode, mode);
}

void CChrManBasket::AllSendMsg_(CChrDrv::MSG_ID id, long nArg1, long nArg2, unsigned long nArg3,
                                unsigned long nArg4)
{
    switch (id) {
        case 0x13:
            if (m_nFlagAC & 8) {
                GetInstance()->ResetClockOver_();
                m_nFlagAC &= ~8;
            }
            if (CGame::g_GameWork.GetStage() == 6) {
                NpcSendMessage(-1, 0, 0, 0, 0);
            }
            break;
        case 0x18:
            for (long i = 0; i < GetChrCoreNum(); i++) {
                CChrCore *pChr = GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable()) {
                    static_cast<CChrCoreBasket *>(pChr)->DispOffCombinationIcon();
                }
            }
            break;
        case 0xE:
            if (CGame::g_GameWork.GetStage() == 6) {
                NpcSendMessage(-1, 2, nArg1, nArg2, 0);
            }
            break;
        case 7:
            m_nFlagAC |= 8;
            break;
    }
    CChrMan::AllSendMsg_(id, nArg1, nArg2, nArg3, nArg4);
}

void CChrManBasket::AllSetState_(long nState, long nArg1, long nArg2)
{
    CChrMan::AllSetState_(nState, nArg1, nArg2);
}

void CChrManBasket::StartScoreView_(long nTeam)
{
    CChrMan::StartScoreView_(nTeam);

    // On stage 5 the characters are brought back onto the court.
    if (CGame::g_GameWork.GetStage() == 5) {
        static const float fOffset = 1.5f;
        float fLeft;
        float fRight;
        float fFront;
        float fBack;

        CStageMan::GetInstance()->GetCourtRect(&fLeft, &fRight, &fFront, &fBack);
        fLeft += fOffset;
        fRight -= fOffset;
        fFront += fOffset;
        fBack -= fOffset;

        for (long i = 0; i < GetChrCoreNum(); i++) {
            CChrCore *pChr = GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }

            if (pChr->GetState() == 0x58 && pChr->FsmqGetNextState() == 0xD) {
                pChr->FsmqNext();
            }

            bcs::FVEC3 pos(pChr->GetPos());
            pos.x = Clamp(pos.x, fLeft, fRight);
            pos.z = Clamp(pos.z, fFront, fBack);
            if (pos.y < 0.0f) {
                pos.y = 0.0f;
            }
            pChr->SetPos(pos);
            pChr->RecalcAllMatrix();
        }
    }
}

int CChrManBasket::CanbeTurnOver_()
{
    if (m_ppChrCore == 0) {
        return 1;
    }

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        switch (pChr->GetState()) {
            case 0x23:
            case 0x25:
                if (!(CGame::g_GameWork.m_nFlag18 & 0x80) && !(CGame::g_GameWork.m_nFlag18 & 0x40) &&
                    !(CGame::g_GameWork.m_nFlag18 & 1)) {
                    return 0;
                }
                break;
            case 0x27:
                if (!(CGame::g_GameWork.m_nFlag18 & 0x80) && !(CGame::g_GameWork.m_nFlag18 & 0x40) &&
                    !(CGame::g_GameWork.m_nFlag18 & 1)) {
                    return 0;
                }
                break;
        }
    }
    return CBallMan::CanbeTurnOver() != 0;
}

void CChrManBasket::SendMessage_(long nMsg, long nArg1, long nArg2, long nArg3)
{
    if (nMsg == 0xB || nMsg == 0x36) {
        CGameGuideManBasket::RemoveGuideTelop();
    }
    CChrMan::SendMessage_(nMsg, nArg1, nArg2, nArg3);
}

void CChrManBasket::OnChgCharacterPassDisableFrame_(const CChrCore *pChr)
{
    float fFrame = 18.0f;
    if (pChr->DriverGetType() == 1 && pChr != 0 &&
        (static_cast<const CChrCoreBasket *>(pChr)->m_nBasketFlag & 0x800)) {
        fFrame = 9999.9f;
    }
    m_fPassDisableFrame[pChr->GetTeamNo()] = fFrame;
}

void CChrManBasket::OnChgCharacterInputDiableFrame_(long nTeam)
{
    m_fChgInputDisableFrame[nTeam] = 20.0f;
}

void CChrManBasket::DispPlayerIcon_(const CChrCore *pChr)
{
    bcs::FVEC3 pos;
    bcs::FVEC3 ofs(0.0f, 1.5f, 0.0f);
    bcs::FVEC3Sum(pos, pChr->GetPos(), ofs);

    const CChrCore *pPrev = GetInstance()->m_pPrevLeader[pChr->GetTeamNo()];
    if (pPrev != 0) {
        pos = pPrev->GetPos();
    }

    if (GetInstance()->IsPadTypePlayer_(pChr) && CGameBasket::GetInstance()->IsNowMainLoopEnable_()) {
        bcs::FVEC3 iconOfs(0.0f, 1.5f, 0.0f);
        CCockpit::SetChrHeadIcon(pChr->GetCtrlPlayerNo(), pChr->GetPlayerNo(), &iconOfs, pChr->GetCtrlPlayerNo(),
                                 &pos, 0);
    }
}

void CChrManBasket::DispPlayerIconSetCore_(const CChrCore *pChr)
{
    bcs::FVEC3 pos;
    bcs::FVEC3 ofs(0.0f, 1.5f, 0.0f);
    bcs::FVEC3Sum(pos, pChr->GetPos(), ofs);

    if (GetInstance()->IsPadTypePlayer_(pChr) && CGameBasket::GetInstance()->IsNowMainLoopEnable_()) {
        bcs::FVEC3 iconOfs(0.0f, 1.5f, 0.0f);
        CCockpit::SetChrHeadIcon(pChr->GetCtrlPlayerNo(), pChr->GetPlayerNo(), &iconOfs, pChr->GetCtrlPlayerNo(),
                                 &pos, 0);
    }
}

void CChrManBasket::AllDispPlayerIcon_()
{
    for (long i = 0; i < GetChrCoreNum(); i++) {
        CChrCore *pChr = GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable()) {
            DispPlayerIconSetCore_(pChr);
        }
    }
}

void CChrManBasket::ClearPlayerIcon_(const CChrCore *pChr, int bArg)
{
    if (GetInstance()->IsPadTypePlayer_(pChr)) {
        CCockpit::ClearChrHeadIcon(pChr->GetCtrlPlayerNo(), pChr->GetPlayerNo(), bArg);
    }
}

void CChrManBasket::AllClearPlayerIcon_(int bArg)
{
    for (long i = 0; i < GetChrCoreNum(); i++) {
        ClearPlayerIcon_(GetChrCore(i), bArg);
    }
}

int CChrManBasket::IsPadTypePlayer_(const CChrCore *pChr) const
{
    return pChr->DriverGetType() == 1;
}

void CChrManBasket::SetClockOverTime_()
{
    CGame::SetShotTime(CGame::GetMaxShotTime());
    if (CGame::GetMaxShotTime() < 1.0f) {
        m_nFlagAC |= 0x10;
    }
}

void CChrManBasket::ResetClockOver_()
{
    SetClockOverTime_();
    m_nFlagAC &= ~1;
}

long CChrManBasket::GetClockTimeSeconds_() const
{
    return (float)ceil(CGame::g_GameWork.m_fShotTime / 60.0f);
}

float CChrManBasket::GetClockTime_() const
{
    return CGame::g_GameWork.m_fShotTime;
}

void CChrManBasket::UpDateShotClockOver_(float)
{
    if (m_nFlagAC & 0x10) {
        return;
    }

    if (GetInstance()->m_nFlagAC & 2) {
        GetInstance()->ResetClockOver_();
        return;
    }

    if (!(m_nFlagAC & 1)) {
        return;
    }

    // The clock stops while the ball is shot, and while its holder is in
    // state 0x24.
    int bCount = 1;
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall != 0) {
        bCount = !pBall->IsShootMode();
        CChrCore *pHold = Util_SearchHoldBallChr(0);
        if (pHold != 0 && pHold->GetState() == 0x24) {
            bCount = 0;
        }
    }
    if (bCount) {
        float fTime = GetClockTime_();
        fTime -= GetFrameRateF();
        if (fTime <= 0.0f) {
            fTime = 0.0f;
        }
        CGame::SetShotTime(fTime);
    }
}

void CChrManBasket::StartAirTrick_(CChrCore *pChr)
{
    m_pArtsChr = static_cast<CChrCoreBasket *>(pChr);
    m_nFlagAC |= 6;
    CGame::TrgArtsEvent(pChr->GetPlayerNo());
    CGame::SendMessage(0xD, 0, 0, 0);
}

float CChrManBasket::GetArtsCurveValue_(long nType) const
{
    switch (nType) {
        case 3:
            return 3.5f;
        case 4:
            return 3.7f;
        default:
            return 0.0f;
    }
}

int CChrManBasket::IsCreateBossPackun_() const
{
    return NpcSearch(2, 0) != 0;
}

CNpcBossPackun *CChrManBasket::GetBossPackun_(long nIdx) const
{
    if (NpcSearch(2, 0) == 0) {
        return 0;
    }

    CNpcBase *pNpc = NpcSearch(2, 0);
    while (pNpc != 0) {
        if (pNpc != 0 && static_cast<CNpcBossPackun *>(pNpc)->GetGoalIdx() == nIdx) {
            return static_cast<CNpcBossPackun *>(pNpc);
        }
        pNpc = NpcSearch(2, pNpc);
    }
    return 0;
}

float CChrManBasket::GetRemainderMouthOpenFrame_(long nIdx)
{
    CNpcBossPackun *pBoss = GetInstance()->GetBossPackun_(nIdx);
    if (pBoss != 0) {
        return pBoss->GetRemainderMouthOpenFrame();
    }
    return 0.0f;
}

} // namespace msw

namespace {
    // The distances from the goal of a near, middle and far just shot
    // (GetJustShotTimingMin_ and Max_ copy the table to the stack).
    struct JUST_SHOT_DIST {
        float m_fDist[3];
    };
    const JUST_SHOT_DIST s_JustShotDist = { { 0.0f, 5.5f, 11.0f } };
}

namespace msw {

long CChrManBasket::GetJustShotTimingMin_(const CChrCoreBasket *pChr) const
{
    const bcs::FVEC3 &rPos = pChr->GetPos();
    const bcs::FVEC3 &rGoal = pChr->GetTargetGoalPos();

    if (CGame::g_GameWork.GetGameType() == 2) {
        const CBall *pBall = pChr->GetAttachBall();
        if (pBall == 0) {
            return 0;
        }
        return CBallManBasket::GetInstance()->GetJustShotTimingMin_(rPos, rGoal, pBall);
    }

    // The frames by distance: near, middle and far, between the distances
    // of fDist.
    long frm[3];
    frm[0] = pChr->GetJustShotMinStartFrm();
    frm[1] = pChr->GetJustShotMidStartFrm();
    frm[2] = pChr->GetJustShotMaxStartFrm();
    long n = 5.5f <= LenXZ(Diff(rGoal, rPos));
    JUST_SHOT_DIST dist = s_JustShotDist;
    const float *fDist = dist.m_fDist;
    float fFrm0 = frm[n];
    float fFrm1 = frm[n + 1];
    float fDist0 = fDist[n];
    float fDist1 = fDist[n + 1];
    float fLen = Clamp(LenXZ(Diff(rGoal, rPos)), fDist0, fDist1);
    float fRate = Clamp((fLen - fDist0) / (fDist1 - fDist0), 0.0f, 1.0f);
    return fFrm0 + (fFrm1 - fFrm0) * fRate;
}

long CChrManBasket::GetJustShotTimingMax_(const CChrCoreBasket *pChr) const
{
    const bcs::FVEC3 &rPos = pChr->GetPos();
    const bcs::FVEC3 &rGoal = pChr->GetTargetGoalPos();

    if (CGame::g_GameWork.GetGameType() == 2) {
        const CBall *pBall = pChr->GetAttachBall();
        if (pBall == 0) {
            return 0;
        }
        return CBallManBasket::GetInstance()->GetJustShotTimingMax_(rPos, rGoal, pBall);
    }

    long frm[3];
    frm[0] = pChr->GetJustShotMinEndFrm();
    frm[1] = pChr->GetJustShotMidEndFrm();
    frm[2] = pChr->GetJustShotMaxEndFrm();
    long n = 5.5f <= LenXZ(Diff(rGoal, rPos));
    JUST_SHOT_DIST dist = s_JustShotDist;
    const float *fDist = dist.m_fDist;
    float fFrm0 = frm[n];
    float fFrm1 = frm[n + 1];
    float fDist0 = fDist[n];
    float fDist1 = fDist[n + 1];
    float fLen = Clamp(LenXZ(Diff(rGoal, rPos)), fDist0, fDist1);
    float fRate = Clamp((fLen - fDist0) / (fDist1 - fDist0), 0.0f, 1.0f);
    return fFrm0 + (fFrm1 - fFrm0) * fRate;
}

int CChrManBasket::PlayCombination_(const CChrCore *pChr)
{
    int bPlay = 0;
    long nTeam = pChr->GetTeamNo();
    int bWasOn = (static_cast<const CChrCoreBasket *>(pChr)->m_nBasketFlag >> 11) & 1;

    for (long i = 0; i < GetChrCoreNum(); i++) {
        CChrCore *pMate = GetChrCore(i);
        if (pMate != 0 && nTeam == pMate->GetTeamNo() &&
            pMate->DriverSendMessage((CChrDrv::MSG_ID)0x23, 0, 0, 0, 0) && pMate->MethdCombination()) {
            bPlay = 1;
        }
    }

    // The sound when the combination has just been switched on.
    if (!bWasOn && (static_cast<const CChrCoreBasket *>(pChr)->m_nBasketFlag & 0x800)) {
        const_cast<CChrCoreBasket *>(static_cast<const CChrCoreBasket *>(pChr))->PlayCombinationSe();
    }
    return bPlay;
}

int CChrManBasket::IsScoreBookCountChr_(const CChrCore *pChr) const
{
    if (pChr == 0) {
        return 0;
    }
    if (pChr->IsDisable()) {
        return 0;
    }
    return pChr->IsScoreBookCountChr_();
}

int CChrManBasket::GetRingCSetPos_(long nGoal, bcs::FVEC3 &rPos) const
{
    rPos = CGame::GetGoalPos(nGoal);

    CGoalObj *pGoal = CStageMan::GetInstance()->GetGoal(nGoal);
    if (pGoal == 0) {
        return 0;
    }
    bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(pGoal->GetMdlHandle()));
    if (pMdl == 0) {
        return 0;
    }

    bcs::FMTX34 mtx;
    pMdl->GetScnMtxPos(&mtx, pGoal->GetRingNodeIdx(), (bcs::CScnObj::ScnObjMtxType)1);
    bcs::FVEC3 pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
    rPos = pos;
    return 1;
}

long CChrManBasket::GetDir4ToBattle_(float fRotY) const
{
    // Half the width of the quarters to the sides (degrees).
    static const float fRang = 75.0f;
    float fDeg = (180.0f / 3.1415927f) * RoundRad(fRotY);

    if (fDeg >= 90.0f - fRang && fDeg <= 90.0f + fRang) {
        return 3;
    }
    if (fDeg > 90.0f + fRang && fDeg <= 270.0f - fRang) {
        return 2;
    }
    if (fDeg > 270.0f - fRang && fDeg <= 270.0f + fRang) {
        return 1;
    }
    return 0;
}

int CChrManBasket::IsAllComTeam_(long nTeam) const
{
    return CGame::GetTeamCtrlType(nTeam) == 1;
}

void CChrManBasket::SetDunkRelease_(long nGoal, float fFrame)
{
    m_fDunkReleaseFrame = fFrame;
    m_nFlagAC |= 0x40;
    m_nDunkGoal = nGoal;
}

void CChrManBasket::Util_SearchPassTo_(CChrCore **ppChr, CNpcBase **ppNpc, long nChr, float fRotY,
                                       bcs::FVEC3 &rPos)
{
    *ppChr = 0;
    *ppNpc = 0;

    float fMin = 12.566371f;
    float fMinDiff;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();
    bcs::FVEC3 myPos;
    pMe->GetCompPos(myPos);

    // The team mate nearest to the direction fRotY.
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() &&
            !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000)) {
            float fDiff = RoundRadPI(pMe->GetRotY_ToChr(pChr) - fRotY);
            float fAbs = __fabsf(fDiff);
            if (fAbs < fMin) {
                fMinDiff = fDiff;
                *ppChr = pChr;
                fMin = fAbs;
            }
        }
    }

    // On the ghost stage the team's ghost takes passes too.
    if (CGame::g_GameWork.GetStage() == 6) {
        CNpcBase *pGhost = NpcSearch(CGameBasket::GetTeamGhostType(pMe->GetTeamNo()), 0);
        if (pGhost != 0 && pGhost->CanbeCatch()) {
            float fDiff = RoundRadPI(pMe->GetRotY_ToPosXZ(pGhost->GetPos()) - fRotY);
            float fAbs = __fabsf(fDiff);
            if (fAbs < fMin) {
                *ppChr = 0;
                fMinDiff = fDiff;
                fMin = fAbs;
                *ppNpc = pGhost;
            }
        }
    }

    if (*ppChr == 0 && *ppNpc == 0) {
        return;
    }

    bcs::FVEC3 pos;
    bcs::FVEC3 vec;
    float fTime;
    if (*ppChr != 0) {
        (*ppChr)->GetCompPos(pos);
    } else if (*ppNpc != 0) {
        pos = (*ppNpc)->GetPos();
    }

    if (fMin <= 0.2617994f) {
        // Nearly ahead: straight at the receiver.
        VecSub(vec, pos, myPos);
        rPos = pos;
    } else {
        // Otherwise ahead of it, 30 degrees off the passer's yaw at most.
        float fOfs = fMin >= 0.5235988f ? fMin - 0.5235988f : 0.5235988f - fMin;
        if (fMinDiff > 0.0f) {
            fOfs = -fOfs;
        }
        VecSub(vec, pos, myPos);
        float fLen = LenXZ(vec);
        bcs::BCS_VecZ_RotY(rPos, fLen, RoundRad(fOfs + pMe->GetRotY()));
        VecAdd(rPos, rPos, myPos);
    }

    float fHeight = 0.0f;
    if (CCollMan::DetectMapHit(pos, vec, fTime, myPos, rPos, 1.2f + fHeight, 1, 0x1000000)) {
        rPos.x = pos.x;
        rPos.y = pos.y;
        rPos.z = pos.z;
    }
}

void CChrManBasket::SetPrevLeader_(long nTeam, const CChrCore *pChr)
{
    m_fPrevLeaderFrame[nTeam] = 0.0f;
    if (pChr != 0 && CGame::IsPlayTypeCanChangeLeader(pChr->GetTeamNo()) && m_nTeamChrNum > 2) {
        m_fPrevLeaderFrame[nTeam] = 40.0f;
    }
    m_pPrevLeader[nTeam] = pChr;
}

}
