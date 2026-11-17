// msw::CChangeFieldDemo: the dodgeball change-field demo (CEventDemoDodge's
// demo 6). Characters walk out of the field to the outside, or back in, one
// change after the other, each change with a telop naming the characters.
#include <stdio.h>
#include "game/msw/CChangeFieldDemo.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CCameraAnm.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitDodge.hpp"
#include "game/msw/CEvNodeData.hpp"
#include "game/msw/CEventChrCore.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CEventDemoDodge.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMove.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CNameMesg.hpp"

// The motion table of the characters, then the sequences the timers play:
// the throw-in, the catch, and a short one. (names guessed)
static const short s_MotTable[8] = { 1, 0x10, 0x11, 0x74, 0x75, 0x76, 0x77, 0 };
static const msw::CEventModel::MOT_DATA s_MotThrow[3] = {
    { 5, 0, 8.0f, 0.0f },
    { 6, 2, 8.0f, 0.0f },
    { 0, 0, 0.0f, 0.0f },
};
static const msw::CEventModel::MOT_DATA s_MotCatch[3] = {
    { 3, 0, 8.0f, 0.0f },
    { 4, 2, 8.0f, 0.0f },
    { 0, 0, 0.0f, 0.0f },
};
static const msw::CEventModel::MOT_DATA s_MotStand[2] = {
    { 4, 2, 8.0f, 0.0f },
    { 0, 0, 0.0f, 0.0f },
};

namespace msw {

    // Weak in this file (in TourF they are ordinary functions of
    // game/tour/CTournamentRouteMan.cpp).
    inline void CEventMoveAction::SetFrame(float fFrame)
    {
        m_fFrame = fFrame;
    }

    inline void CEventMoveAction::UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame)
    {
        m_fFrame += fFrame;
    }

    inline void CChangeFieldDemo::_PlayMotion(long nChr, const CEventModel::MOT_DATA *pData)
    {
        if (nChr != -1 && pData != 0) {
            CEventModel *pChr = m_ppChr[nChr];
            if (pChr != 0 && (m_nChrMask & (1 << nChr))) {
                pChr->SetLinkAction(0, 0);
                m_ppChr[nChr]->AnimPlay(pData, 0, 0.0f, -1);
            }
        }
    }

    // A change of team nTeam going out (coming in) when it has characters.
    inline unsigned short CChangeFieldDemo::_AddOut(long nTeam, char *pField)
    {
        unsigned short mask = _SetOut(m_nFieldNum, nTeam, pField);
        if (mask) {
            m_nFieldNum++;
        }
        return mask;
    }

    inline unsigned short CChangeFieldDemo::_AddIn(long nTeam, char *pField)
    {
        unsigned short mask = _SetIn(m_nFieldNum, nTeam, pField);
        if (mask) {
            m_nFieldNum++;
        }
        return mask;
    }

    CChangeFieldDemo::CChangeFieldDemo(bcs::CTask *pParent, char *pField, long nCount) : CEventScene(8, pParent, 0)
    {
        unsigned long mask = 0;
        m_nFieldNum = 0;
        m_bButton = 0;
        m_nSkipBit = 0;
        CNameMesg::Initialize();
        if (CGame::g_GameWork.GetStage() == 5) {
            m_bStage5 = 1;
        } else {
            m_bStage5 = 0;
        }

        for (long i = 0; i < FIELD_MAX; i++) {
            FIELD_WORK &rField = m_Field[i];
            rField.nTeam = -1;
            rField.nOut = -1;
            rField.nPlayer[0] = -1;
            rField.nPlayer[1] = -1;
            rField.nChr[0] = -1;
            rField.nChr[1] = -1;
            rField.nChr[2] = -1;
            rField.nChr[3] = -1;
            m_nCutIdx[i] = 0;
        }

        m_nChrNum = CChrMan::GetChrCoreNum();
        long nOut = 0;
        long nIn = 0;
        for (long i = 0; i < m_nChrNum; i++) {
            if (pField[i] == 1) {
                nOut++;
            }
            if (pField[i] == 0) {
                nIn++;
            }
        }

        if (nOut > 0) {
            if (nIn > 0) {
                mask = _AddOut(0, pField);
                mask |= _AddOut(1, pField);
                mask |= _AddIn(0, pField);
                mask |= _AddIn(1, pField);
            } else {
                mask = _AddOut(0, pField);
                mask |= _AddOut(1, pField);
            }
        } else if (nIn > 0) {
            mask = _AddIn(0, pField);
            mask |= _AddIn(1, pField);
        }
        m_nChrMask |= mask;
    }

    CChangeFieldDemo::~CChangeFieldDemo()
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            pDemo->RemoveTelopAll();
        }
        EvDemoDodge::ResetChangeFieldDemo();
        CCockpit::ClearResult(-1);
        CNameMesg::Remove();
    }

    nw4r::g3d::ResFile CChangeFieldDemo::GetRes()
    {
        return CLoadData::GetRes(0x60);
    }

    // The characters of team nTeam that come in take slots 0 and 1 (their
    // players are named in the telop); the team's other enabled infield
    // characters that stay take slots 1 on, and an outfielder of the other
    // team slot 3.
    unsigned long CChangeFieldDemo::_SetIn(long nIdx, long nTeam, char *pField)
    {
        unsigned long mask = 0;
        long n = 0;
        for (long i = 0; i < m_nChrNum; i++) {
            if (pField[i] == 0) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && pChr->IsEnable() && nTeam == pChr->GetTeamNo()) {
                    switch (n) {
                        case 0:
                            m_Field[nIdx].nPlayer[0] = pChr->GetPlayerNo();
                            m_Field[nIdx].nChr[n] = i;
                            break;
                        case 1:
                            m_Field[nIdx].nPlayer[1] = pChr->GetPlayerNo();
                            m_Field[nIdx].nChr[n] = i;
                            break;
                    }
                    mask |= 1 << i;
                    n++;
                }
            }
        }

        if (n > 0) {
            long k = 1;
            for (long i = 0; i < m_nChrNum; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pField[i] == -1 && pChr != 0 && pChr->IsEnable()) {
                    if (nTeam == pChr->GetTeamNo() && pChr->IsDodgeInfield()) {
                        m_Field[nIdx].nChr[k] = i;
                        mask |= 1 << i;
                        k++;
                    }
                    if (nTeam != pChr->GetTeamNo() && CGame::IsCharEx_DodgeOutfielder(i)) {
                        m_Field[nIdx].nChr[3] = i;
                        mask |= 1 << i;
                    }
                }
            }
            m_Field[nIdx].nTeam = nTeam;
            m_Field[nIdx].nOut = 0;
        }
        return mask;
    }

    // As _SetIn for the characters that go out; the team's other enabled
    // infield characters take slots 2 on.
    unsigned long CChangeFieldDemo::_SetOut(long nIdx, long nTeam, char *pField)
    {
        unsigned long mask = 0;
        long n = 0;
        for (long i = 0; i < m_nChrNum; i++) {
            if (pField[i] == 1) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && pChr->IsEnable() && nTeam == pChr->GetTeamNo()) {
                    switch (n) {
                        case 0:
                            m_Field[nIdx].nPlayer[0] = pChr->GetPlayerNo();
                            m_Field[nIdx].nChr[n] = i;
                            break;
                        case 1:
                            m_Field[nIdx].nPlayer[1] = pChr->GetPlayerNo();
                            m_Field[nIdx].nChr[n] = i;
                            break;
                    }
                    mask |= 1 << i;
                    n++;
                }
            }
        }

        if (n > 0) {
            long k = 2;
            for (long i = 0; i < m_nChrNum; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pField[i] == -1 && pChr != 0 && pChr->IsEnable() && nTeam == pChr->GetTeamNo() &&
                    pChr->IsDodgeInfield()) {
                    m_Field[nIdx].nChr[k] = i;
                    mask |= 1 << i;
                    k++;
                }
            }
            m_Field[nIdx].nTeam = nTeam;
            m_Field[nIdx].nOut = 1;
        }
        return mask;
    }

    // Stops the game, makes the cuts (one per change, of the camera
    // animation for going out or coming in) and a model on a node path for
    // every character.
    int CChangeFieldDemo::EventFirst()
    {
        CEventDemoBase::GetInstance();
        CChrMan::SetChrMarkerDisp(1);
        CBallMan::AllReset();
        CBallMan::AllSleep();
        CreateTimer(0x40);

        long num = m_pCamera->GetSceneNum();
        if (num > 0) {
            if (m_pCut != 0) {
                delete[] m_pCut;
            }
            m_pCut = 0;
            m_pCut = new CUT_WORK[num];
            const char *pNoData = "NoData";
            for (long i = 0; i < num; i++) {
                CUT_WORK &rCut = m_pCut[i];
                rCut.pName = pNoData;
                rCut.nUnk04 = -1;
                rCut.nUnk06 = -1;
                rCut.fUnk1C = 0.0f;
                rCut.fUnk20 = 1.0f;
                rCut.nUnk08 = 0;
                rCut.nUnk0A = 0;
                rCut.fUnk10 = 1.0f / 30.0f;
                rCut.fUnk18 = 1.0f / 30.0f;
                rCut.fUnk14 = 40.0f;
                rCut.nUnk0C = -1;
            }

            float frame = 0.0f;
            for (long i = 0; i < m_nFieldNum; i++) {
                float start = frame;
                m_nCutIdx[i] = m_nCutNum;
                if (m_Field[i].nOut != 0) {
                    frame += AddCut("dod_change_OUT", i, 0, 10.0f);
                } else {
                    frame += AddCut("dod_change_IN", i, 0, 10.0f);
                }
                m_Field[i].fStart = start;
                m_Field[i].fEnd = frame + -1.0f;
            }

            if (m_nCutNum > 0) {
                CUT_WORK &rFirst = m_pCut[0];
                rFirst.nUnk08 = 0x7E;
                rFirst.nUnk0A = 2;
                for (long i = 0; i < m_nChrNum; i++) {
                    long color = CGame::GetCharColor(i);
                    long type = CGame::GetCharType(i);
                    if (type != -1) {
                        m_ppChr[i] = new CEventChrCore(type, i, s_MotTable);
                        if (m_ppChr[i]->IsObj()) {
                            m_ppChr[i]->Create();
                            m_ppChr[i]->SetDisp(0);
                            new CEventMove(m_ppChr[i], m_pNodeData, 1);
                            new CEvMoveActFieldDemo(m_ppChr[i]);
                            if (color >= 4) {
                                m_nChrMask &= ~(1 << i);
                            }
                        } else {
                            if (m_ppChr[i] != 0) {
                                delete m_ppChr[i];
                            }
                            m_ppChr[i] = 0;
                            m_nChrMask &= ~(1 << i);
                        }
                    } else {
                        m_nChrMask &= ~(1 << i);
                    }
                }
            }
        }
        return 1;
    }

    void CChangeFieldDemo::EventStart()
    {
        AddTimer(8, 0.0f, 0, 0);
    }

    // Puts character nIdx of change nField on its path, walking it on its
    // own, and plays motion nMot.
    inline void CChangeFieldDemo::_SetMoveWalk(long nField, long nIdx, long nMot)
    {
        long nChr = _SetMoveCommon(nField, nIdx);
        if (nChr != -1) {
            m_ppChr[nChr]->SetLinkAction(0, 0);
            m_ppChr[nChr]->AnimPlay(nMot, 2, 0.0f, 0.0f, 1);
        }
    }

    // Plays cut rCut: the change it shows (nUnk04), the field turned for
    // the team, the characters put on their paths and the timers of their
    // motions and voices.
    long CChangeFieldDemo::EventPlay(CUT_WORK &rCut)
    {
        if (m_nUnk9D >= m_nCutNum) {
            if (m_nUnk9F) {
                _StartFadeOut();
            }
            CEventDemo::m_Work.bCancel = 1;
            ResetTimer();
            return 7;
        }
        if (IsFadeOutOnly(rCut.nUnk08)) {
            return StartFadeOut(rCut, rCut.nUnk08);
        }

        long left = m_nFieldNum - m_nUnk9D;
        if (left >= 3) {
            m_nSkipBit = 4;
        } else if (left >= 2) {
            m_nSkipBit = 2;
        } else if (left >= 1) {
            AddTimerRel(9, rCut.fUnk1C - 30.0f, 0, 0);
            m_nSkipBit = 1;
        }

        FIELD_WORK &rField = m_Field[rCut.nUnk04];
        // The field is turned round for the team defending goal 0.
        if (CGame::GetGuardGoalIdx(rField.nTeam)) {
            AddOffset(bcs::FVEC3(0.0f, 0.0f, 0.0f), bcs::FVEC3(0.0f, 0.0f, 0.0f), 1);
        } else {
            AddOffset(bcs::FVEC3(0.0f, 0.0f, 0.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
        }

        CEventDemoBase *pDemo = CEventDemoBase::GetInstance();
        m_fUnk3C = 0.0f;
        m_bButton = 0;
        pDemo->RemoveTelop(0x26);
        CCockpit::ClearResult(-1);
        CNameMesg::RemoveWindow();
        SetDispChr(0);

        if (rCut.pName != 0) {
            m_pNodeData->SetResAnm(CEvNodeData::_GetResAnm(m_resFile, rCut.pName));
            switch (rField.nOut) {
                case 0: {
                    long nChr = _SetMoveCommon(rCut.nUnk04, 0);
                    if (nChr != -1) {
                        m_ppChr[nChr]->AnimPlay(0L, 2, 0.0f, 0.0f, 1);
                    }
                    _SetMoveWalk(rCut.nUnk04, 1, 0);
                    _SetMoveWalk(rCut.nUnk04, 2, 0);
                    _SetMoveWalk(rCut.nUnk04, 3, 6);
                    AddTimerRel(0xB, 90.0f, rCut.nUnk04, 0);
                    AddTimerRel(0xD, 90.0f, rCut.nUnk04, 0);
                    AddTimerRel(0xA, 0.0f, rCut.nUnk04, 1);
                    AddTimerRel(0xD, 0.0f, rCut.nUnk04, 1);
                    AddTimerRel(0xA, 30.0f, rCut.nUnk04, 2);
                    AddTimerRel(0xD, 30.0f, rCut.nUnk04, 2);
                    AddTimerRel(rCut.nUnk04, 0.0f, 0, 0);
                    break;
                }
                case 1:
                    _SetMoveWalk(rCut.nUnk04, 0, 0);
                    _SetMoveWalk(rCut.nUnk04, 1, 0);
                    _SetMoveWalk(rCut.nUnk04, 2, 0);
                    _SetMoveWalk(rCut.nUnk04, 3, 0);
                    AddTimerRel(0xC, 0.0f, rCut.nUnk04, 0);
                    AddTimerRel(0xE, 0.0f, rCut.nUnk04, 0);
                    AddTimerRel(0xC, 0.0f, rCut.nUnk04, 1);
                    AddTimerRel(0xE, 30.0f, rCut.nUnk04, 1);
                    AddTimerRel(0xC, 0.0f, rCut.nUnk04, 2);
                    AddTimerRel(0xE, 90.0f, rCut.nUnk04, 2);
                    AddTimerRel(0xC, 0.0f, rCut.nUnk04, 3);
                    AddTimerRel(0xE, 120.0f, rCut.nUnk04, 3);
                    AddTimerRel(rCut.nUnk04, 0.0f, 0, 0);
                    break;
            }
        }
        return StartFadeOut(rCut, rCut.nUnk08);
    }

    // Timers 0 to 3 show the telop of change nId, 4 to 7 take it away; 8
    // starts the camera, 9 ends the last change; 10 to 14 play motions and
    // voices on character nIdx of change nField. Others go to the demo.
    void CChangeFieldDemo::EventTimer(short nId, short nField, unsigned long nIdx)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetInstance();
        if (nId >= 8) {
            switch (nId) {
                case 8:
                    m_pCamera->Resume();
                    break;
                case 9:
                    m_nUnk9E = 7;
                    break;
                case 13: {
                    long nChr = m_Field[nField].nChr[nIdx];
                    if (nChr != -1) {
                        CEventModel *pChr = m_ppChr[nChr];
                        if (pChr != 0 && (m_nChrMask & (1 << nChr))) {
                            static_cast<CEventChrCore *>(pChr)->PlayVoice(0x65);
                        }
                    }
                    break;
                }
                case 14: {
                    long nChr = m_Field[nField].nChr[nIdx];
                    if (nChr != -1) {
                        CEventModel *pChr = m_ppChr[nChr];
                        if (pChr != 0 && (m_nChrMask & (1 << nChr))) {
                            static_cast<CEventChrCore *>(pChr)->PlayVoice(0x66);
                        }
                    }
                    break;
                }
                case 11:
                    _PlayMotion(m_Field[nField].nChr[nIdx], s_MotCatch);
                    break;
                case 10:
                    _PlayMotion(m_Field[nField].nChr[nIdx], s_MotStand);
                    break;
                case 12:
                    _PlayMotion(m_Field[nField].nChr[nIdx], s_MotThrow);
                    break;
                default:
                    pDemo->TimerEvent(nId, nField, nIdx);
                    break;
            }
        } else if (nId >= 4) {
            pDemo->RemoveTelop(0x26);
            CNameMesg::MenuOutWindow();
        } else {
            FIELD_WORK &rField = m_Field[nId];
            const wchar_t *pName0 = 0;
            const wchar_t *pName1 = 0;
            if (rField.nPlayer[0] != -1) {
                long type = CGame::GetCharType(rField.nPlayer[0]);
                pName0 = CNameMesg::GetCharName(type, CGame::GetCharColor(rField.nPlayer[0]), 0);
                if (type >= 0x13) {
                    pName0 = CGame::GetCharName(rField.nPlayer[0]);
                }
            }
            if (rField.nPlayer[1] != -1) {
                long type = CGame::GetCharType(rField.nPlayer[1]);
                pName1 = CNameMesg::GetCharName(type, CGame::GetCharColor(rField.nPlayer[1]), 0);
                if (type >= 0x13) {
                    pName1 = CGame::GetCharName(rField.nPlayer[1]);
                }
            }
            pDemo->CreateTelop(0x26, 0);
            CCockpitDodge::SetTelopInOut(0x26, rField.nOut, rField.nTeam, pName0, pName1);
        }
    }

    void CChangeFieldDemo::EventTimerReset(short nId, short, unsigned long)
    {
    }

    void CChangeFieldDemo::EventFlags(unsigned long, unsigned long)
    {
    }

    void CChangeFieldDemo::CheckEventSkip()
    {
        if (CGame::DemoSkipInputCheck(-1) >= 0 || CCockpit::GetResult() == 1000) {
            m_nSkip |= m_nSkipBit;
        }
    }

    int CChangeFieldDemo::IsEventSkip()
    {
        if (_IsPlayFade() == 0) {
            if (!m_bButton) {
                CNameMesg::CreateButtonWindow("Btn_A_Skip");
                m_bButton = 1;
            }
            if (CEventDemo::m_Work.bCancel != 0) {
                m_nSkip |= 1;
            }
            if (m_nSkip != m_nSkipNone) {
                return 1;
            }
        }
        return 0;
    }

    // Skip bit 1 (or the last cut) ends the demo; bits 2 to 4 jump to the
    // next cut, played faster.
    long CChangeFieldDemo::EventCansel(CUT_WORK &rCut, long nRet)
    {
        m_nSkipNone |= m_nSkip;
        CNameMesg::MenuOutWindow();
        if ((m_nSkip & 1) || m_nUnk9D + 1 >= m_nCutNum) {
            if (m_nUnk9F) {
                _StartFadeOut();
            }
            CEventDemo::m_Work.bCancel = 1;
            ResetTimer();
            return 5;
        }
        if (m_nSkip & 0xE) {
            ResetTimer();
            m_nUnk9D++;
            CUT_WORK &rNext = m_pCut[m_nUnk9D];
            rNext.nUnk08 = 0;
            rNext.fUnk10 = 1.0f / 30.0f;
            rNext.fUnk18 = 1.0f / 30.0f;
            rNext.fUnk14 = 40.0f;
            return EventPlay(rNext);
        }
        return nRet;
    }

    long CChangeFieldDemo::_SetMoveCommon(long nField, long nIdx)
    {
        long nChr = m_Field[nField].nChr[nIdx];
        if (m_ppChr[nChr] != 0 && (m_nChrMask & (1 << nChr))) {
            char name[0x40];
            sprintf(name, "player%02d", nIdx + 1);
            long id = m_pNodeData->GetAnmId(name);
            if (id == -1) {
                return -1;
            }
            bcs::FVEC3 pos;
            bcs::FVEC3 rot;
            bcs::FVEC3 scale;
            m_pNodeData->GetResultEx(pos, rot, scale, 0.0f, id, m_nNodeFlip);
            bcs::BCS_Vec_RotY(pos, pos, m_fNodeRotY);
            bcs::FVEC3Add(&pos, &pos, &m_vNodeOffset);
            bcs::FVEC3Add(&rot, &rot, &m_vNodeOffsetRot);
            if (m_ppChr[nChr]->SetNode(id, 0.0f) == 0) {
                return -1;
            }
            m_ppChr[nChr]->SetPosition(pos);
            m_ppChr[nChr]->SetRotation(rot);
            m_ppChr[nChr]->SetScale(scale);
            m_ppChr[nChr]->SetDisp(1);
            return nChr;
        }
        return -1;
    }

    CEvMoveActFieldDemo::CEvMoveActFieldDemo(CEventModel *pModel) : CEventMoveAction(pModel, 1)
    {
    }

    CEvMoveActFieldDemo::~CEvMoveActFieldDemo()
    {
    }

    // Runs, walks or stands by the distance moved on the floor this frame.
    void CEvMoveActFieldDemo::UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame)
    {
        if (m_pModel->IsDisp()) {
            float fInv = 1.0f / fFrame;
            bcs::FVEC3 move(pos.x, 0.0f, pos.z);
            float speed = fInv * bcs::FVEC3Len(&move);
            if (speed > 0.07f) {
                AnimPlay(2, 2, 4.0f, 0.0f, 1.0f);
            } else if (speed > 0.02f) {
                AnimPlay(1, 2, 4.0f, 0.0f, 1.0f);
            } else {
                float rate = speed / 0.02f;
                if (rate < 0.001f) {
                    AnimPlay(0, 2, 4.0f, 0.0f, 1.0f);
                } else {
                    m_pModel->SetAnimSpeed(rate > 1.0f ? 1.0f : (rate < 0.001f ? 0.001f : rate));
                }
            }
        }
    }

    void EvDemoDodge::CreateChgFieldDemo(bcs::CTask *pParent, char *pField, long nCount)
    {
        if (CEventDemo::IsEnable() == 0) {
            CEventDemo::m_Work.bSkip = 1;
            return;
        }
        new CChangeFieldDemo(pParent, pField, nCount);
    }
}
