// msw::CChrManBoss: the character manager of the boss game.
#include "game/msw/CChrManBoss.hpp"

#include "bcs/New.hpp"
#include "game/msw/CChrCoreBoss.hpp"
#include "game/msw/CChrDrvPadBoss.hpp"
#include "game/msw/CChrMarkerBoss.hpp"
#include "game/msw/CComDrvBoss.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNpcBehemothBoss.hpp"

namespace msw {

CChrMan *CChrManBoss::CreateInstance(bcs::CTask *pParent)
{
    SetInstance_(new (0x100) CChrManBoss(pParent));
    return GetInstance_();
}

CChrManBoss::~CChrManBoss()
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

void CChrManBoss::Create_()
{
    CreateObj_();
    CreateDriver_();
    CreateMarker_();

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }

    bcs::FVEC3 pos(0.0f, 0.0f, -12.0f);
    bcs::FVEC3 rot(0.0f, 0.0f, 0.0f);
    long nParam = CGame::GetPartyGameParam(0);
    CNpcBehemothBoss *pBoss = new (0x100) CNpcBehemothBoss(this, pos, rot, nParam);
    pBoss->Create();
}

void CChrManBoss::CreateObj_()
{
    m_nChrCoreNum = CheckGenChrCoreNum_();

    for (long i = 0; i < m_nChrCoreNum; i++) {
        long nType = CGame::GetCharType(i);
        long nColor = CGame::GetCharColor(i);
        CChrCore *pChr;

        switch (nType) {
            case 0:  pChr = new (0) CChrCoreBossMario; break;
            case 1:  pChr = new (0) CChrCoreBossLuigi; break;
            case 2:  pChr = new (0) CChrCoreBossPeach; break;
            case 3:  pChr = new (0) CChrCoreBossDaisy; break;
            case 4:  pChr = new (0) CChrCoreBossYoshi; break;
            case 5:  pChr = new (0) CChrCoreBossWario; break;
            case 6:  pChr = new (0) CChrCoreBossWaluigi; break;
            case 7:  pChr = new (0) CChrCoreBossDonkey; break;
            case 8:  pChr = new (0) CChrCoreBossDiddy; break;
            case 9:  pChr = new (0) CChrCoreBossKinopio; break;
            case 10: pChr = new (0) CChrCoreBossKoopa; break;
            case 11: pChr = new (0) CChrCoreBossKoopaJr; break;
            case 12: pChr = new (0) CChrCoreBossMoogle; break;
            case 13: pChr = new (0) CChrCoreBossSaboten; break;
            case 14: pChr = new (0) CChrCoreBossNinja; break;
            case 15: pChr = new (0) CChrCoreBossSiroma; break;
            case 16: pChr = new (0) CChrCoreBossSlime; break;
            case 17: pChr = new (0) CChrCoreBossKuroma; break;
            case 19: pChr = new (0) CChrCoreBossMii00; break;
            case 20: pChr = new (0) CChrCoreBossMii01; break;
            case 21: pChr = new (0) CChrCoreBossMii00; break;
            case 22: pChr = new (0) CChrCoreBossMii01; break;
            case 23: pChr = new (0) CChrCoreBossMii00; break;
            case 24: pChr = new (0) CChrCoreBossMii01; break;
            default: pChr = new (0) CChrCoreBossMario; break;
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

long CChrManBoss::CheckGenChrCoreNum_()
{
    switch (CGame::g_GameWork.m_nUnk06) {
        case 2:
        case 3:
        case 11:
            return 2;
        default:
            return 4;
        case 0:
        case 1:
        case 9:
        case 10:
        case 13:
        case 14:
        case 15:
        case 16:
            return 6;
    }
}

void CChrManBoss::CreateDriver_()
{
    long padId[8];
    long camId[8];

    SetPadIdAssign_(8, padId);
    SetCamIdAssign_(8, camId);
    SetDrvIdAssign_(8, m_nDrvId);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        CChrDrv *pPad = new (0x100) CChrDrvPadBoss;
        CChrDrv *pCom = new (0x100) CComDrvBoss;
        pChr->DriverSetEntry(0, pPad);
        pChr->DriverSetEntry(1, pCom);
        pChr->SetTeamNo(i & 1);
        pChr->SetPadID(padId[i]);
        pChr->SetCamNo(camId[i]);
        if (m_nDrvId[i] == 1) {
            pChr->DriverConnect(0, 0, 0, 0, 0);
        } else {
            pChr->DriverConnect(1, 0, 0, 0, 0);
        }
        if (i < 2) {
            pChr->OnStatusFlag(0x100);
        }
    }
}

void CChrManBoss::CreateMarker_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        if (m_ppChrCore[i]->IsDisable()) {
            continue;
        }

        int bPlayer = 0;
        if (m_ppChrCore[i]->DriverGetType() == 1) {
            bPlayer = 1;
        }
        if (m_ppChrCore[i]->DriverGetEntry(1) != 0) {
            bPlayer = 1;
        }

        if (bPlayer) {
            new (0x100) CChrMarkerBoss(this, i, 2, -1);
        } else {
            new (0x100) CChrMarkerBoss(this, i, 3, -1);
        }
    }
}

void CChrManBoss::AllReset_()
{
}

void CChrManBoss::InitLeader_(int bConnect)
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

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable()) {
            long nDrv = CGame::GetTeamCtrlType(pChr->GetTeamNo()) == 1 ? 1 : m_nDrvId[i] == 3;
            pChr->DriverConnect(nDrv, 0, 0, 0, 0);
        }
    }
}

void CChrManBoss::DispFunc()
{
}

CChrCore *CChrManBoss::Util_SearchPassTo_(long nChr, float)
{
    if (m_nChrOrderNum <= 0) {
        return 0;
    }
    return GetChrCore_((nChr + 2) % m_nChrOrderNum);
}

CChrCore *CChrManBoss::Util_SearchPassmeetTo_(long nChr, float fRate, bcs::FVEC3 &rPos)
{
    CChrCore *pChr = Util_SearchPassTo_(nChr, fRate);
    if (pChr != 0 && pChr->IsEnable()) {
        pChr->GetPos(rPos);
    }
    return pChr;
}

CChrCore *CChrManBoss::Util_SearchArtsPassTo_(long nChr)
{
    return Util_SearchPassTo_(nChr, 0.0f);
}

int CChrManBoss::Util_GetSubChrTargetPos_(long, bcs::FVEC3 &rPos)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    return 1;
}

int CChrManBoss::CanbeTurnOver_()
{
    int bResult = CChrMan::CanbeTurnOver_();

    if (m_ppChrCore == 0) {
        return bResult;
    }

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        // (state 0x27 is not named yet)
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetState() == 0x27) {
            return 1;
        }
    }
    return bResult;
}

} // namespace msw
