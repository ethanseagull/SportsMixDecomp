// msw::CChrManVolley: the character manager of the volleyball game, and
// CHR_MAN_VOLLEY::CChrManVolleyRhythmReceive, the one of its rhythm receive
// party game.
#include "game/msw/CChrManVolley.hpp"

#include "bcs/New.hpp"
#include "game/msw/CChrCoreVolley.hpp"
#include "game/msw/CChrDrvPadVolley.hpp"
#include "game/msw/CChrMarkerVolley.hpp"
#include "game/msw/CChrTargetMarkerVolley.hpp"
#include "game/msw/CComDrvVolleyGame.hpp"
#include "game/msw/CComDrvVolleyRR.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameGuideMan.hpp"
#include "game/msw/CNpcGhostVolley.hpp"

namespace msw {

CChrMan *CChrManVolley::CreateInstance(bcs::CTask *pParent)
{
    CChrMan *pMan;
    if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
        pMan = new (0x100) CHR_MAN_VOLLEY::CChrManVolleyRhythmReceive(pParent);
    } else {
        pMan = new (0x100) CChrManVolley(pParent);
    }
    SetInstance_(pMan);
    return GetInstance_();
}

CChrManVolley::~CChrManVolley()
{
    DeleteChrCore_();
}

void CChrManVolley::Create_()
{
    CreateObj_();
    CreateDriver_();
    CreateMarker_();

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }

    // The ghost of stage 6 (not in the party game).
    if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_NORMAL && CGame::g_GameWork.GetStage() == 6) {
        CNpcGhostVolley *pGhost = new (0x100) CNpcGhostVolley(this, 5);
        if (pGhost != 0) {
            pGhost->Create();
        }
    }
}

void CChrManVolley::CreateObj_()
{
    m_nChrCoreNum = CheckGenChrCoreNum_();

    for (long i = 0; i < m_nChrCoreNum; i++) {
        long nType = CGame::GetCharType(i);
        long nColor = CGame::GetCharColor(i);
        CChrCore *pChr;

        switch (nType) {
            case 0:  pChr = new (0) CChrCoreVolleyMario; break;
            case 1:  pChr = new (0) CChrCoreVolleyLuigi; break;
            case 2:  pChr = new (0) CChrCoreVolleyPeach; break;
            case 3:  pChr = new (0) CChrCoreVolleyDaisy; break;
            case 4:  pChr = new (0) CChrCoreVolleyYoshi; break;
            case 5:  pChr = new (0) CChrCoreVolleyWario; break;
            case 6:  pChr = new (0) CChrCoreVolleyWaluigi; break;
            case 7:  pChr = new (0) CChrCoreVolleyDonkey; break;
            case 8:  pChr = new (0) CChrCoreVolleyDiddy; break;
            case 9:  pChr = new (0) CChrCoreVolleyKinopio; break;
            case 10: pChr = new (0) CChrCoreVolleyKoopa; break;
            case 11: pChr = new (0) CChrCoreVolleyKoopaJr; break;
            case 12: pChr = new (0) CChrCoreVolleyMoogle; break;
            case 13: pChr = new (0) CChrCoreVolleySaboten; break;
            case 14: pChr = new (0) CChrCoreVolleyNinja; break;
            case 15: pChr = new (0) CChrCoreVolleySiroma; break;
            case 16: pChr = new (0) CChrCoreVolleySlime; break;
            case 17: pChr = new (0) CChrCoreVolleyKuroma; break;
            case 19: pChr = new (0) CChrCoreVolleyMii00; break;
            case 20: pChr = new (0) CChrCoreVolleyMii01; break;
            case 21: pChr = new (0) CChrCoreVolleyMii00; break;
            case 22: pChr = new (0) CChrCoreVolleyMii01; break;
            case 23: pChr = new (0) CChrCoreVolleyMii00; break;
            case 24: pChr = new (0) CChrCoreVolleyMii01; break;
            default: pChr = new (0) CChrCoreVolley; break;
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

long CChrManVolley::CheckGenChrCoreNum_()
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
        case 18:
        case 19:
        case 21:
        case 24:
        case 25:
            return 6;
    }
}

void CChrManVolley::CreateDriver_()
{
    long padId[6];
    long camId[6];

    SetPadIdAssign_(6, padId);
    SetCamIdAssign_(6, camId);
    SetDrvIdAssign_(6, m_nDrvId);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        CChrDrv *pPad = new (0x100) CChrDrvPadVolley;
        CChrDrv *pCom = new (0x100) CComDrvVolleyGame;
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

void CChrManVolley::CreateMarker_()
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
            new (0x100) CChrMarkerVolley(this, i, 2, -1);
            new (0x100) CChrMarkerVolleyJump(this, i, 8, -1);
        }
        new (0x100) CChrTargetMarkerVolley(this, i, bPlayer);
    }
}

void CChrManVolley::AllReset_()
{
}

void CChrManVolley::InitLeader_(int bConnect)
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

void CChrManVolley::StartArtsEvent_(long nChr)
{
    if (nChr < 0 || nChr >= m_nChrCoreNum) {
        return;
    }

    CChrCore *pChr = m_ppChrCore[nChr];
    if (pChr != 0 && pChr->IsEnable()) {
        CGame::SetOffenseTeam(pChr->GetTeamNo());
    }
    CChrMan::StartArtsEvent_(nChr);
}

void CChrManVolley::MainFunc_()
{
    GameGuideDispCheck_();
}

void CChrManVolley::GameGuideDispCheck_()
{
    if (!CGameGuideMan::IsDispEnable()) {
        return;
    }

    // The guide telop: the first pad-driven character that asks for one.
    long nTelop = -1;
    long nTelopChr = -1;
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr != 0 && !pChr->IsDisable() && pChr->IsPadCtrl()) {
            long nType = pChr->CheckGuideTelopDisp();
            if (nType >= 0 && nTelopChr < 0) {
                nTelopChr = i;
                nTelop = nType;
            }
        }
    }

    if (nTelop >= 0 && nTelopChr >= 0) {
        bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
        float fTime = 60.0f;
        float fArg0 = -1.0f;
        float fArg1 = -1.0f;
        float fArg2 = -1.0f;

        switch (nTelop) {
            case 10:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                fArg0 = 0.0f;
                fArg1 = 10.0f;
                fArg2 = 10.0f;
                break;
            case 11:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                fArg0 = 0.0f;
                fArg1 = 10.0f;
                fArg2 = 10.0f;
                break;
            case 12:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                fArg0 = 0.0f;
                break;
            case 13:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                fArg0 = 0.0f;
                break;
            case 14:
                fArg0 = 0.0f;
                break;
            case 15:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                fArg0 = 0.0f;
                break;
            case 16:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                fArg0 = 0.0f;
                break;
            case 17:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                fArg0 = 0.0f;
                fArg1 = 20.0f;
                fArg2 = 10.0f;
                break;
        }
        CGameGuideMan::StartGuideTelop(nTelop, m_ppChrCore[nTelopChr], fTime, &ofs, fArg0, fArg1, fArg2);
    } else {
        CGameGuideMan::EndGuideTelop(-1);
    }

    // The line message, chosen the same way.
    long nMsg = -1;
    long nMsgChr = -1;
    CChrCore *pMsgChr = 0;
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr != 0 && !pChr->IsDisable() && pChr->IsPadCtrl()) {
            long nType = pChr->CheckVolleyLineMsgDisp();
            if (nType >= 0 && nMsgChr < 0) {
                nMsgChr = i;
                pMsgChr = pChr;
                nMsg = nType;
            }
        }
    }

    if (nMsg >= 0 && nMsgChr >= 0) {
        CGameGuideMan::StartLineMsg(nMsg, pMsgChr, 1, 0.0f);
    } else {
        CGameGuideMan::EndLineMsg(0);
    }
}

void CChrManVolley::DispFunc()
{
}

CChrCore *CChrManVolley::Util_SearchPassTo_(long nChr, float)
{
    if (m_nChrOrderNum <= 0) {
        return 0;
    }
    return GetChrCore_((nChr + 2) % m_nChrOrderNum);
}

CChrCore *CChrManVolley::Util_SearchPassmeetTo_(long nChr, float fRate, bcs::FVEC3 &rPos)
{
    CChrCore *pChr = Util_SearchPassTo_(nChr, fRate);
    if (pChr != 0 && pChr->IsEnable()) {
        pChr->GetPos(rPos);
    }
    return pChr;
}

CChrCore *CChrManVolley::Util_SearchArtsPassTo_(long nChr)
{
    return Util_SearchPassTo_(nChr, 0.0f);
}

int CChrManVolley::Util_GetSubChrTargetPos_(long, bcs::FVEC3 &rPos)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    return 1;
}

int CChrManVolley::CanbeTurnOver_()
{
    int bResult = CChrMan::CanbeTurnOver_();

    if (m_ppChrCore == 0) {
        return bResult;
    }

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        // (state 0x27 is not named yet)
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetState() == 0x27) {
            return CGame::IsVolleyScored();
        }
    }
    return bResult;
}

namespace CHR_MAN_VOLLEY {

CChrManVolleyRhythmReceive::~CChrManVolleyRhythmReceive()
{
}

void CChrManVolleyRhythmReceive::Create_()
{
    CreateObj_();
    CreateDriver_();
    CreateMarker_();
    SetTeamLeader(0, 0, 0);
}

long CChrManVolleyRhythmReceive::CheckGenChrCoreNum_()
{
    return 8;
}

void CChrManVolleyRhythmReceive::CreateDriver_()
{
    long drvId[8];

    SetDrvIdAssign_(8, drvId);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        CChrDrv *pDrv;
        if (drvId[i] == 3) {
            pDrv = new (0) CComDrvVolleyRR;
        } else {
            pDrv = new (0) CChrDrvPadVolleyRhythmReceive;
        }
        pChr->DriverSetEntry(0, pDrv);
        pChr->SetTeamNo(0);
        // Two characters share each pad.
        pChr->SetPadID(i >> 1);
        pChr->SetCamNo(0);
        pChr->DriverConnect(0, 0, 0, 0, 0);
        pChr->OnStatusFlag(0x100);
    }
}

void CChrManVolleyRhythmReceive::SetDrvIdAssign_(long nNum, long *pId)
{
    // The number of pad-driven characters; the rest are computers.
    long nPad = 0;
    switch (CGame::g_GameWork.m_nUnk06) {
        case 1:
        case 3:
        case 5:
        case 6:
        case 18:
            nPad = 2;
            break;
        case 7:
        case 9:
        case 15:
        case 16:
        case 17:
        case 20:
        case 21:
            nPad = 3;
            break;
        case 8:
        case 10:
        case 14:
        case 19:
        case 22:
        case 23:
        case 24:
        case 25:
            nPad = 4;
            break;
        default:
        case 0:
        case 2:
        case 4:
            nPad = 1;
            break;
        case 11:
        case 12:
        case 13:
            break;
    }

    for (long i = 0; i < nNum; i++) {
        pId[i] = 3;
    }
    // Every second character, from the first, is a player's.
    for (long i = 0; i < nPad && i < nNum / 2; i++) {
        pId[i * 2] = 1;
    }
}

void CChrManVolleyRhythmReceive::CreateMarker_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        if (m_ppChrCore[i]->IsDisable()) {
            continue;
        }

        if (m_ppChrCore[i]->DriverGetType() == 1) {
            new (0x100) CChrMarkerVolley(this, i, 2, -1);
        } else {
            new (0x100) CChrMarkerVolley(this, i, 3, -1);
        }
    }
}

void CChrManVolleyRhythmReceive::AllReset_()
{
}

void CChrManVolleyRhythmReceive::InitLeader_(int bConnect)
{
    CChrManVolley::InitLeader_(bConnect);
}

int CChrManVolleyRhythmReceive::CanbeTurnOver_()
{
    int bResult = CChrMan::CanbeTurnOver_();

    if (m_ppChrCore == 0) {
        return bResult;
    }

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetState() == 0x27) {
            bResult = CGame::IsVolleyScored();
            break;
        }
    }
    return bResult;
}

void CChrManVolleyRhythmReceive::DispFunc()
{
}

} // namespace CHR_MAN_VOLLEY

} // namespace msw
