// msw::CComDrvDodgeGame, the computer driver of the dodgeball game: the
// off-ball runs (to the charge spot, forward, back, sideways, to a
// character) and the wait for a catch in the infield. The rest of the class
// is in game/dodge/CComDrvDodgeGame.cpp and the files after it.
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CComManDodgeGame.hpp"

namespace msw {

namespace {
    // The work of one action, as these actions use it (names guessed; the
    // rest belongs to CComDrv's common actions).
    struct OFF_RUN_WORK {
        unsigned char m_Unk00[0x04];
        long m_nStep;           // 0x04, the step the action is at
        unsigned char m_Unk08[0x1C - 0x08];
        bcs::FVEC3 m_Pos;       // 0x1C, where to run to
        long m_nParam;          // 0x28, the action's argument
        float m_fParam;         // 0x2C, the action's second argument
    };

    // Character states (CChrCore's state numbers; meanings not known).
    enum {
        CHR_STATE_5 = 5,
        CHR_STATE_34 = 0x22
    };

    // The actions of this file, by number.
    enum {
        ACT_OFF_RUN_CHARGE = 0x1D,
        ACT_OFF_RUN_FRONT = 0x1E,
        ACT_OFF_RUN_BACK = 0x1F,
        ACT_OFF_RUN_SIDE = 0x20,
        ACT_OFF_RUN_CHR = 0x21,
        ACT_IN_FIELD_CATCH_WAIT = 0x22
    };
}

void CComDrvDodgeGame::ActCallOffRunCharge()
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(EntryAct(ACT_OFF_RUN_CHARGE));
    pWork->m_nParam = -1;
}

// Runs to the charge spot holding the ball, then shoots or passes there.
// m_nParam is what to do there, drawn once near the centre line: 0 shoot as
// soon as the ball is charged, 3 and 7 pass, 4 and 8 neither, otherwise
// shoot.
void CComDrvDodgeGame::ActLoopOffRunCharge()
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(GetCurrentActWork());

    if (!m_pChr->IsHoldBall()) {
        ReturnAct(1);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            SetOffStanceMode(2);
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            ActCallLookTarget();
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetOffRunChargePos(pWork->m_Pos);
            pWork->m_nStep++;
            break;
        case 3: {
            float fZ = CComMan::GetZtoCenter(m_ChrPos);
            if (fZ < 1.5f && pWork->m_nParam < 0) {
                pWork->m_nParam = LotActChargeAfterOffRunCharge();
            }
            if (pWork->m_nParam == 0) {
                if (CComManDodgeGame::IsBallChargeMax(m_pChr)) {
                    MethdShoot();
                    ReturnAct(1);
                    break;
                }
                if (fZ < 0.8f) {
                    ReturnAct(0);
                    break;
                }
            }
            if (m_pChr->GetState() == CHR_STATE_5) {
                if (pWork->m_nParam >= 5 && pWork->m_nParam <= 8) {
                    ActCallWait(RandRange(7.0f, 10.0f));
                    pWork->m_nStep++;
                } else {
                    ActCallWait(RandRange(10.0f, 20.0f));
                    pWork->m_nStep += 2;
                }
            } else if (MethdRunEx(pWork->m_Pos, 0.5f, 1.0f, 600.0f, 0xF, 10.0f)) {
                ReturnAct(0);
            }
            break;
        }
        case 4:
            if (MethdTrickMove()) {
                ActCallWait(RandRange(6.0f, 10.0f));
                pWork->m_nStep++;
            }
            break;
        case 5: {
            long nAct = pWork->m_nParam;
            if (nAct == 3 || nAct == 7) {
                CChrCore *pTarget = GetPassTargetChrCore();
                if (pTarget && MethdPass(pTarget)) {
                    pWork->m_nStep++;
                    break;
                }
            } else if (nAct != 4 && nAct != 8) {
                if (nAct == 2 || nAct == 6) {
                    m_nShootKind = 1;
                } else {
                    m_nShootKind = 0;
                }
                if (MethdShoot()) {
                    pWork->m_nStep++;
                    break;
                }
            }
            if (m_pChr->GetState() != CHR_STATE_5 && m_pChr->GetState() != CHR_STATE_34) {
                pWork->m_nStep++;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvDodgeGame::ActCallOffRunFront()
{
    EntryAct(ACT_OFF_RUN_FRONT);
}

void CComDrvDodgeGame::ActLoopOffRunFront()
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            ActCallLookTarget();
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetOffRunFrontPos(pWork->m_Pos);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Pos, 0.5f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvDodgeGame::ActCallOffRunBack()
{
    EntryAct(ACT_OFF_RUN_BACK);
}

void CComDrvDodgeGame::ActLoopOffRunBack()
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            ActCallLookTarget();
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetOffRunBackPos(pWork->m_Pos);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Pos, 0.5f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvDodgeGame::ActCallOffRunSide()
{
    EntryAct(ACT_OFF_RUN_SIDE);
}

void CComDrvDodgeGame::ActLoopOffRunSide()
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            ActCallLookTarget();
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetOffRunSidePos(pWork->m_Pos);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Pos, 0.5f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvDodgeGame::ActCallOffRunChr(long nChr, float fSpeed)
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(EntryAct(ACT_OFF_RUN_CHR));
    pWork->m_nParam = nChr;
    pWork->m_fParam = fSpeed;
}

// Runs to character m_nParam (as far as it was when the run started).
void CComDrvDodgeGame::ActLoopOffRunChr()
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(GetCurrentActWork());

    if (pWork->m_nParam < 0) {
        ReturnAct(0);
        return;
    }

    bcs::FVEC3 pos = CComMan::GetChrPos(pWork->m_nParam);
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            ActCallLookTarget(pos);
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            pWork->m_Pos = pos;
            pWork->m_nStep++;
            break;
        case 3:
            if (MethdRunEx(pWork->m_Pos, 2.0f, 1.0f, pWork->m_fParam, 7, 10.0f)) {
                pWork->m_nStep++;
            }
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvDodgeGame::ActCallInFieldCatchWait()
{
    EntryAct(ACT_IN_FIELD_CATCH_WAIT);
}

// Faces the target and waits a while, as long as the level's parameter
// says: a fixed time when a human plays on the team, otherwise a random
// one between half the parameter and the parameter.
void CComDrvDodgeGame::ActLoopInFieldCatchWait()
{
    OFF_RUN_WORK *pWork = static_cast<OFF_RUN_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
        case 0:
            ActCallLookTarget();
            pWork->m_nStep++;
            break;
        case 1: {
            float fFrame;
            if (IsExistPadDrv()) {
                fFrame = CComMan::GetDatF(0x296, GetComLvF());
            } else {
                float fMax = CComMan::GetDatF(0x29E, GetComLvF());
                fFrame = RandRangeF(0.5f * fMax, fMax);
            }
            ActCallWait(fFrame);
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

}
