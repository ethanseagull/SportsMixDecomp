// msw::CComDrvHockeyCS's thinking: the checks that interrupt the running
// action (ActInterrupt) and the choice of the next one (ThkMain). A file of
// its own: its float constants (0, 0.5) are pooled again after those of
// game/hockey/CComDrvHockeyCS.cpp (name not known).
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Vec.hpp"

namespace msw {

namespace {

    // The party stages with their own computer behaviour.
    enum {
        STAGE_PARTY_85 = 0x55,
        STAGE_PARTY_86 = 0x56,
        STAGE_PARTY_87 = 0x57
    };

    // A rate in percent, kept within 0 to 100.
    inline long ClampRate(long nRate)
    {
        if (nRate > 100) {
            return 100;
        }
        return nRate < 0 ? 0 : nRate;
    }


    // The distance between two points in the XZ plane (name guessed).
    inline float DisXZ(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1)
    {
        bcs::FVEC3 pos0(rPos0.x, 0.0f, rPos0.z);
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        return bcs::FVEC3Dist(&pos0, &pos1);
    }

}

// The computer level rounded to a whole level (name guessed).
static inline long GetComLvRound(const CComDrv *pDrv)
{
    return (long)(0.5f + pDrv->GetComLvF());
}

// Stage 0x56: a burning character runs about.
inline int CComDrvHockeyCS::ThkInterruptStatusFire() {
    if (CGame::g_GameWork.GetStage() != STAGE_PARTY_86) {
        return 0;
    }
    if (IsEntryAct(0x14)) {
        return 1;
    }
    if (m_pChr->GetComLevelWork() != 4) {
        return 0;
    }
    Reset();
    ActCallStatusFire();
    return 1;
}

// Stage 0x55: escapes from where the stage is dangerous.
inline int CComDrvHockeyCS::ThkInterruptEscape() {
    if (CGame::g_GameWork.GetStage() != STAGE_PARTY_85) {
        return 0;
    }
    if (IsEntryAct(0xE)) {
        return 1;
    }
    bcs::FVEC3 target;
    bcs::FVEC3 pos = m_MyPos;
    if (GetStageTargetPosition(target, pos, 0, 0) == 2) {
        m_nUnk1CC = 0;
        m_nUnk1D0 = 0;
        Reset();
        ActCallEscape(target);
        return 1;
    }
    return 0;
}

// A block attack, or else a charged smash.
inline int CComDrvHockeyCS::ThkInterruptAttack() {
    if (ThkInterruptAttack_BlockAttack()) {
        return 1;
    }
    return ThkInterruptAttack_ChargeSmashAttack() != 0;
}

void CComDrvHockeyCS::ActInterrupt() {
    if (ThkInterruptReflectionAttack()) {
        return;
    }

    // Stage 0x56: now and then the character keeps still for a while.
    if (CGame::g_GameWork.GetStage() == STAGE_PARTY_86) {
        if (m_bFireWait) {
            m_fFireWait -= m_pChr->GetFrameStep();
            if (m_fFireWait <= 0.0f) {
                m_fFireWait = 0.0f;
                m_fFireLotTime = 0.0f;
                m_bFireWait = 0;
            }
        } else {
            m_fFireLotTime += m_pChr->GetFrameStep();
            if (m_fFireLotTime >= CComMan::GetDatF(0x4C, GetComLvF())) {
                m_fFireLotTime = 0.0f;
                long nRate = CComMan::GetDatI(0x44, GetComLvF());
                if (LotRate() < nRate) {
                    m_fFireWait = CComMan::GetDatF(0x54, GetComLvF());
                    m_bFireWait = 1;
                } else {
                    m_fFireWait = 0.0f;
                    m_bFireWait = 0;
                }
            }
        }
    }

    if (ThkInterruptStatusFire()) {
        return;
    }
    if (ThkInterruptEruptionWarning()) {
        return;
    }
    if (ThkInterruptEscape()) {
        return;
    }
    if (m_pChr->IsInvincible()) {
        return;
    }
    if (ThkInterruptPriorityAttack()) {
        return;
    }
    if (ThkInterruptAttack()) {
        return;
    }
    if (ThkInterruptIceAttack()) {
        return;
    }
    if (ThkInterruptSearch()) {
        return;
    }
}

int CComDrvHockeyCS::ThkInterruptReflectionAttack() {
    if (CGame::g_GameWork.GetStage() != STAGE_PARTY_87) {
        return 0;
    }
    if (IsEntryAct(0x15)) {
        return 1;
    }
    if (IsEntryAct(0x12)) {
        return 0;
    }
    if (IsEntryAct(0xC)) {
        return 0;
    }
    if (IsEntryAct(0x11)) {
        return 0;
    }
    if (IsEntryAct(0xB)) {
        return 0;
    }

    m_fReflectionTime += m_pChr->GetFrameStep();
    if (m_fReflectionTime < CComMan::GetDatF(0x28B, GetComLvF())) {
        return 0;
    }
    m_fReflectionTime = 0.0f;

    bcs::FVEC3 pos0;
    bcs::FVEC3 pos1;
    long nChr = isRefrectionAttack(pos0, pos1);
    if (nChr < 0) {
        return 0;
    }

    long nRate;
    if (CChrMan::GetChrCore(nChr)->DriverGetType() == 1) {
        nRate = (long)(CComMan::GetDatF(0x2A3, GetComLvF()) * CComMan::GetDatF(0x2C0, GetChrTypeID()));
    } else {
        nRate = (long)(CComMan::GetDatF(0x29B, GetComLvF()) * CComMan::GetDatF(0x2AB, GetChrTypeID()));
    }
    nRate = ClampRate(nRate);
    if (LotRate() >= nRate) {
        return 0;
    }
    Reset();
    ActCallReflectionAttack();
    return 1;
}

int CComDrvHockeyCS::ThkInterruptEruptionWarning() {
    if (CGame::g_GameWork.GetStage() != STAGE_PARTY_86) {
        return 0;
    }
    if (!GetEruptionWarning()) {
        return 0;
    }
    if (IsEntryAct(0x13)) {
        return 1;
    }
    float fRadius = GetEruptionRadius();
    if (fRadius == 0.0f) {
        return 0;
    }

    // The eruption is at the centre of the rink.
    bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
    float fDis = DisXZ(center, m_MyPos);
    if (fDis > fRadius) {
        return 0;
    }
    Reset();
    ActCallEscapeEruption();
    return 1;
}

int CComDrvHockeyCS::ThkInterruptPriorityAttack() {
    if (IsEntryAct(0xC)) {
        return 1;
    }
    if (CComMan::GetDatI(0xD3, GetComLvRound(this)) == 0 && IsEntryAct(0xF)) {
        return 0;
    }
    if (CComMan::GetDatI(0xDB, GetComLvRound(this)) == 0 && IsEntryAct(0x10)) {
        return 0;
    }
    if (CComMan::GetDatI(0xE3, GetComLvRound(this)) == 0 && IsEntryAct(0xB)) {
        return 0;
    }
    if (CComMan::GetDatI(0xEB, GetComLvRound(this)) == 0 && IsEntryAct(0x11)) {
        return 0;
    }
    if (CComMan::GetDatI(0xF3, GetComLvRound(this)) == 0 && IsEntryAct(0x12)) {
        return 0;
    }

    m_fPriorityTime += m_pChr->GetFrameStep();
    if (m_fPriorityTime < CComMan::GetDatF(0xFB, GetComLvF())) {
        return 0;
    }
    m_fPriorityTime = 0.0f;

    long nChr = GetSmashChanceCore(CComMan::GetDatF(0x103, GetComLvF()));
    if (nChr >= 0) {
        long nRate;
        if (CChrMan::GetChrCore(nChr)->DriverGetType() == 1) {
            nRate = (long)(CComMan::GetDatF(0x11B, GetComLvF()) * CComMan::GetDatF(0x138, GetChrTypeID()));
        } else {
            nRate = (long)(CComMan::GetDatF(0x113, GetComLvF()) * CComMan::GetDatF(0x123, GetChrTypeID()));
        }
        nRate = ClampRate(nRate);
        if (LotRate() < nRate) {
            Reset();
            ActCallSmashAttack(nChr);
            return 1;
        }
    }
    return 0;
}

int CComDrvHockeyCS::ThkInterruptAttack_BlockAttack() {
    if (IsEntryAct(0xB)) {
        return 1;
    }
    if (CComMan::GetDatI(0x14D, GetComLvRound(this)) == 0 && IsEntryAct(0xF)) {
        return 0;
    }
    if (CComMan::GetDatI(0x155, GetComLvRound(this)) == 0 && IsEntryAct(0x10)) {
        return 0;
    }
    if (CComMan::GetDatI(0x15D, GetComLvRound(this)) == 0 && IsEntryAct(0xC)) {
        return 0;
    }
    if (CComMan::GetDatI(0x165, GetComLvRound(this)) == 0 && IsEntryAct(0x11)) {
        return 0;
    }
    if (CComMan::GetDatI(0x16D, GetComLvRound(this)) == 0 && IsEntryAct(0x12)) {
        return 0;
    }

    m_fBlockTime += m_pChr->GetFrameStep();
    if (m_fBlockTime < CComMan::GetDatF(0x175, GetComLvF())) {
        return 0;
    }
    m_fBlockTime = 0.0f;

    long nChr = SearchingForTheEenemy(CComMan::GetDatF(0x17D, GetComLvF()), 1, 1, 0xFF00FFFF, 0);
    if (nChr < 0) {
        return 0;
    }

    long nRate;
    if (CChrMan::GetChrCore(nChr)->DriverGetType() == 1) {
        nRate = (long)(CComMan::GetDatF(0x195, GetComLvF()) * CComMan::GetDatF(0x1B2, GetChrTypeID()));
    } else {
        nRate = (long)(CComMan::GetDatF(0x18D, GetComLvF()) * CComMan::GetDatF(0x19D, GetChrTypeID()));
    }
    nRate = ClampRate(nRate);
    if (LotRate() >= nRate) {
        return 0;
    }
    Reset();
    ActCallBlockAttack(nChr);
    return 1;
}

int CComDrvHockeyCS::ThkInterruptAttack_ChargeSmashAttack() {
    if (IsEntryAct(0x11)) {
        return 1;
    }
    if (CComMan::GetDatI(0x1C7, GetComLvRound(this)) == 0 && IsEntryAct(0xF)) {
        return 0;
    }
    if (CComMan::GetDatI(0x1CF, GetComLvRound(this)) == 0 && IsEntryAct(0x10)) {
        return 0;
    }
    if (CComMan::GetDatI(0x1D7, GetComLvRound(this)) == 0 && IsEntryAct(0xC)) {
        return 0;
    }
    if (CComMan::GetDatI(0x1DF, GetComLvRound(this)) == 0 && IsEntryAct(0xB)) {
        return 0;
    }
    if (CComMan::GetDatI(0x1E7, GetComLvRound(this)) == 0 && IsEntryAct(0x12)) {
        return 0;
    }

    m_fChargeSmashTime += m_pChr->GetFrameStep();
    if (m_fChargeSmashTime < CComMan::GetDatF(0x1EF, GetComLvF())) {
        return 0;
    }
    m_fChargeSmashTime = 0.0f;

    long nChr = isChargeSmashAttack();
    if (nChr < 0) {
        return 0;
    }

    long nRate;
    if (CChrMan::GetChrCore(nChr)->DriverGetType() == 1) {
        nRate = (long)(CComMan::GetDatF(0x21F, GetComLvF()) * CComMan::GetDatF(0x23C, GetChrTypeID()));
    } else {
        nRate = (long)(CComMan::GetDatF(0x217, GetComLvF()) * CComMan::GetDatF(0x227, GetChrTypeID()));
    }
    nRate = ClampRate(nRate);
    if (LotRate() >= nRate) {
        return 0;
    }
    Reset();
    ActCallChargeSmashAttack(nChr);
    return 1;
}

int CComDrvHockeyCS::ThkInterruptSearch() {
    if (IsEntryAct(0xB)) {
        return 0;
    }
    if (IsEntryAct(0xC)) {
        return 0;
    }
    if (IsEntryAct(0x11)) {
        return 0;
    }
    if (IsEntryAct(0x12)) {
        return 1;
    }
    if (IsEntryAct(0xA)) {
        return 1;
    }
    if (IsEntryAct(0x10)) {
        return 1;
    }

    m_fSearchTime += m_pChr->GetFrameStep();
    if (m_fSearchTime < CComMan::GetDatF(0x5C, GetComLvF())) {
        return 0;
    }
    m_fSearchTime = 0.0f;

    // The rates of a chase, a wrap-round and an ambush.
    float fLv = GetComLvF();
    long aRate[3];
    aRate[0] = (long)(CComMan::GetDatF(0x7C, fLv) * CComMan::GetDatF(0x94, GetChrTypeID()));
    aRate[1] = (long)(CComMan::GetDatF(0x84, fLv) * CComMan::GetDatF(0xA9, GetChrTypeID()));
    aRate[2] = (long)(CComMan::GetDatF(0x8C, fLv) * CComMan::GetDatF(0xBE, GetChrTypeID()));

    float fDis = CComMan::GetDatF(0x64, GetComLvF());
    long nChr = SearchingForTheEenemy(fDis, 1, 1, 0xFFFF, 0);
    if (nChr < 0) {
        aRate[0] = 0;
    }
    long nEscChr = SearchingForTheEscapeEenemy(fDis, 0);
    if (nEscChr < 0) {
        aRate[1] = 0;
    }
    if (nChr < 0 && nEscChr < 0) {
        return 0;
    }
    if (aRate[0] == 0 && aRate[1] == 0 && aRate[2] == 0) {
        return 0;
    }

    switch (LotProbability(aRate, 3)) {
        case 0:
            Reset();
            ActCallChase(nChr);
            break;
        case 1:
        default:
            Reset();
            ActCallWraps();
            break;
        case 2:
            Reset();
            ActCallAmbushSmashAttack();
            break;
    }
    return 1;
}

int CComDrvHockeyCS::ThkInterruptIceAttack() {
    if (IsEntryAct(0xF)) {
        return 1;
    }
    if (IsEntryAct(0x10)) {
        return 0;
    }
    if (IsEntryAct(0xB)) {
        return 0;
    }
    if (IsEntryAct(0xC)) {
        return 0;
    }
    if (IsEntryAct(0x11)) {
        return 0;
    }
    if (IsEntryAct(0x12)) {
        return 0;
    }

    m_fIceTime += m_pChr->GetFrameStep();
    if (m_fIceTime < CComMan::GetDatF(0x266, GetComLvF())) {
        return 0;
    }
    m_fIceTime = 0.0f;

    short nArg;
    short nCube = isCallIceAttack(nArg);
    if (nCube < 0) {
        return 0;
    }

    long nRate = (long)(CComMan::GetDatF(0x26E, GetComLvF()) * CComMan::GetDatF(0x276, GetChrTypeID()));
    nRate = ClampRate(nRate);
    if (LotRate() >= nRate) {
        return 0;
    }
    m_nUnk1CC = 0;
    m_nUnk1D0 = 0;
    Reset();
    ActCallIceAttack(nCube, nArg);
    return 1;
}

void CComDrvHockeyCS::ThkMain() {
    // An invincible character either wraps round or looks for free space.
    int bDone;
    if (!m_pChr->IsInvincible()) {
        bDone = 0;
    } else {
        float fLv = GetComLvF();
        long aRate[2];
        aRate[0] = CComMan::GetDatI(0x34, fLv);
        aRate[1] = CComMan::GetDatI(0x3C, fLv);
        if (LotProbability(aRate, 2) == 0) {
            ActCallWraps();
        } else {
            ActCallFreeSpace();
        }
        bDone = 1;
    }
    if (!bDone) {
        ActCallFreeSpace();
    }
}

}
