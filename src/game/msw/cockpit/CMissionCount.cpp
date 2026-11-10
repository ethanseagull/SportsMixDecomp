// The cockpit's small parts, one file in the DOL: the special-shot gauge and
// its input prompt, the clock, the mission counter, the 3D coin and the pane
// effect set. One translation unit because they share the float constants
// (0.0f and 1.0f) and .data holds the jump table of GetPadCtrlChrCore, the
// effect set's member pointer and the three vtables in one block. Named after
// its largest class.
#include "game/msw/cockpit/CSPGauge.hpp"
#include "game/msw/cockpit/CSPGaugeInputDisp.hpp"
#include "game/msw/cockpit/CTimeCount.hpp"
#include "game/msw/cockpit/CMissionCount.hpp"
#include "game/msw/cockpit/C3DCoin.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"

#include "bcs/CTaskMan.hpp"
#include "bcs/G2D/CLytManager.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "wchar.h"

namespace {
    // The text box of a pane object, null when it is something else.
    inline bcs::G2D::CTextBoxDL *GetTextBox(bcs::G2D::CPaneObj *pObj)
    {
        return nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pObj->GetPane());
    }

    // (name guessed) Whether pad nPad is held with a nunchaku.
    inline int IsNunchakuPad(long nPad)
    {
        switch (msw::CGame::GetPadDefaultType(nPad)) {
            case 1:
                return 1;
            case 0:
                return 0;
            case -1:
                return msw::CMenuPad::s_Instance.IsNunchakuStyle(nPad) != 0;
            default:
                return 0;
        }
    }

    // The effect CMissionCount starts on its effect pane after a count.
    const long EFFECT_COUNT_UP = 0x45D;
}

namespace msw {
    namespace cockpit {

        // (name guessed; the DOL's fn_801BBF24) The first character of team
        // nTeam a pad drives, among the players the game mode fields: an
        // enabled, pad-driven infielder that is neither a dodgeball
        // outfielder nor a hockey keeper. Null when there is none.
        CChrCore *GetPadCtrlChrCore(long nTeam)
        {
            long nMax;

            switch (CGame::g_GameWork.m_nUnk06) {
                case 2:
                case 3:
                case 11:
                    nMax = 2;
                    break;
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
                    nMax = 6;
                    break;
                default:
                    nMax = 4;
                    break;
            }

            long nNum = CChrMan::GetChrCoreNum();
            CChrCore *pResult = 0;

            for (long i = 0; i < nNum && i < nMax; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr == 0 || pChr->IsDisable() || nTeam != pChr->GetTeamNo() || !pChr->IsPadCtrl() ||
                    !pChr->IsDodgeInfield() || CGame::IsCharEx_DodgeOutfielder(i) ||
                    CGame::IsCharEx_HockeyKeeper(i)) {
                    continue;
                }
                pResult = pChr;
                break;
            }
            return pResult;
        }

        // CSPGauge

        void CSPGauge::Initialize()
        {
            m_nPane = 0xFFFF;
            m_nUnk02 = 0xFFFF;
            m_nAnim = 0xFF;
            m_nMaxAnim = 0xFF;
            m_fFrameMax = 10.0f;
            m_fRate = 0.0f;
        }

        void CSPGauge::Finalize()
        {
        }

        void CSPGauge::SetParam(bcs::G2D::CPaneCtrl *pPaneCtrl, unsigned short nPane, unsigned short nUnk02,
                                unsigned short nSubPane, unsigned char nAnim, unsigned char nMaxAnim,
                                unsigned char nSubAnim, unsigned char nSubMaxAnim)
        {
            if (pPaneCtrl == 0) {
                return;
            }

            m_nPane = nPane;
            m_nUnk02 = nUnk02;
            m_nSubPane = nSubPane;
            m_nAnim = nAnim;
            m_nMaxAnim = nMaxAnim;
            m_nSubAnim = nSubAnim;
            m_nSubMaxAnim = nSubMaxAnim;

            float fFrameMax = pPaneCtrl->GetAnimFrameMax(nAnim);
            if (fFrameMax > 0.0f) {
                m_fFrameMax = fFrameMax;
            }
        }

        void CSPGauge::SetGaugeAnim(CMenuPaneAccessor *pAccessor, float fRate)
        {
            if (m_nPane == 0xFFFF) {
                return;
            }

            float fFrame = fRate;
            if (fRate > 1.0f) {
                fFrame = 1.0f;
            } else if (fRate < 0.0f) {
                fFrame = 0.0f;
            }
            fFrame *= m_fFrameMax;
            m_fRate = fRate;
            pAccessor->PlayAnimation(m_nPane, m_nAnim, 0.0f, fFrame, 1, 1);
        }

        void CSPGauge::SetGaugeMaxAnim(CMenuPaneAccessor *pAccessor, int bMax)
        {
            if (bMax) {
                pAccessor->PlayAnimation(m_nPane, m_nMaxAnim, 1.0f, 0.0f, 1, 1);
                pAccessor->PlayAnimation(m_nSubPane, m_nSubMaxAnim, 1.0f, 0.0f, 0, 1);
            } else {
                SetGaugeAnim(pAccessor, m_fRate);
                pAccessor->PlayAnimation(m_nSubPane, m_nSubAnim, 1.0f, 0.0f, 0, 1);
            }
        }

        // CSPGaugeInputDisp

        void CSPGaugeInputDisp::Initialize()
        {
            m_nPane = 0;
            m_nAnim = 0xFF;
        }

        void CSPGaugeInputDisp::Finalize()
        {
        }

        void CSPGaugeInputDisp::SetParam(unsigned char nTeam, unsigned short nPane, unsigned char nAnim,
                                         unsigned short nNunchakuPane, unsigned short nRemotePane)
        {
            m_nTeam = nTeam;
            m_nPane = nPane;
            m_nAnim = nAnim;
            m_nNunchakuPane = nNunchakuPane;
            m_nRemotePane = nRemotePane;
        }

        void CSPGaugeInputDisp::SetDispAnim(CMenuPaneAccessor *pAccessor, int bDisp)
        {
            long nPane = m_nPane;
            CChrCore *pChr = GetPadCtrlChrCore(CGame::GetTeamColorIdx(m_nTeam));

            if (bDisp && pChr != 0 && !pChr->IsDisable() && pChr->IsPadCtrl()) {
                bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(nPane);
                if (pObj != 0) {
                    pObj->GetPane()->SetVisible(true);
                    pObj->PlayAnimation(m_nAnim, 1.0f, 0.0f, 1, 1, 1);
                }

                int bNunchaku;
                int bRemote;
                if (IsNunchakuPad(pChr->GetPadNo())) {
                    bNunchaku = 1;
                    bRemote = 0;
                } else {
                    bNunchaku = 0;
                    bRemote = 1;
                }
                pAccessor->SetPaneDisp(m_nNunchakuPane, bNunchaku);
                pAccessor->SetPaneDisp(m_nRemotePane, bRemote);
            } else {
                pAccessor->SetPaneDisp(nPane, 0);
            }
        }

        // CTimeCount

        void CTimeCount::Initialize()
        {
            m_nUnk00 = -1;
        }

        void CTimeCount::Finalize()
        {
        }

        long CTimeCount::GetSec(float fFrame)
        {
            long nFrame = (long)fFrame;
            long nSec = nFrame / 60;
            if (nFrame % 60 / 6 > 0) {
                nSec++;
            }
            return nSec;
        }

        long CTimeCount::SetTimeValue(CMenuPaneAccessor *pAccessor, float fFrame, int nUnused0, long nAnim,
                                      long nUnused1, long nMinPane, long nSecPane, int nMode)
        {
            if (pAccessor == 0) {
                return -1;
            }

            long nFrame = (long)fFrame;
            long nSec = nFrame / 60 % 60;
            long nMin = nFrame / 3600;
            // Rounded up from the tenth of a second.
            if (nFrame % 60 / 6 > 0) {
                nSec++;
                if (nSec >= 60) {
                    nSec -= 60;
                    nMin++;
                }
            }

            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(nMinPane);
            if (pObj != 0) {
                bcs::G2D::CTextBoxDL *pTextBox = GetTextBox(pObj);
                if (pTextBox != 0) {
                    wchar_t text[] = L"x";
                    long nLen = wcslen(text) + 1;
                    swprintf(text, nLen, L"%1d", nMin);
                    if (nMode == 1 || wcsncmp(text, pTextBox->GetStringBuffer(), nLen) != 0) {
                        pTextBox->AllocStringBuffer(nLen);
                        pTextBox->SetString(text, 0);
                        pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
                    }
                }
            }

            bcs::G2D::CPaneObj *pSecObj = pAccessor->GetPaneObj(nSecPane);
            if (pSecObj != 0) {
                bcs::G2D::CTextBoxDL *pTextBox = GetTextBox(pSecObj);
                if (pTextBox != 0) {
                    wchar_t text[] = L":xx";
                    long nLen = wcslen(text) + 1;
                    swprintf(text, nLen, L":%02d", nSec);
                    if (nMode == 1 || wcsncmp(text, pTextBox->GetStringBuffer(), nLen) != 0) {
                        pTextBox->AllocStringBuffer(nLen);
                        pTextBox->SetString(text, 0);
                        pSecObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
                    }
                }
            }
            return 0;
        }

        long CTimeCount::SetCountDown(CMenuPaneAccessor *pAccessor, long nCount, long nPane, long nTextPane,
                                      long nAnim)
        {
            if (pAccessor == 0) {
                return -1;
            }

            if ((unsigned long)nCount <= 9) {
                pAccessor->SetTextBoxString(nTextPane, nCount, 0);
                bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(nPane);
                if (pObj != 0) {
                    pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
                    pObj->GetPane()->SetVisible(true);
                }
            } else {
                pAccessor->SetPaneDisp(nPane, 0);
            }
            return 0;
        }

        // CMissionCount

        CMissionCount::CMissionCount()
        {
            nw4r::ut::List_Init(&m_List, 0);
            m_nPane = 0;
            m_nNumPane = 0;
            m_nNextPane = 0;
            m_nMaxPane = 0;
            m_nAnim05 = 0;
            m_nRollAnim = 0;
            m_nRollEndAnim = 0;
            m_nWaitAnim = 0;
            m_nInAnim = 0;
            m_nOutAnim = 0;
            m_nEffectPane = 0;
        }

        CMissionCount::~CMissionCount()
        {
            CountUp *pCountUp = static_cast<CountUp *>(nw4r::ut::List_GetPrev(&m_List, 0));
            while (pCountUp != 0) {
                nw4r::ut::List_Remove(&m_List, pCountUp);
                delete pCountUp;
                pCountUp = static_cast<CountUp *>(nw4r::ut::List_GetPrev(&m_List, 0));
            }
        }

        void CMissionCount::SetParameter(unsigned char nPane, unsigned char nNumPane, unsigned char nNextPane,
                                         unsigned char nMaxPane, unsigned char nEffectPane, unsigned char nAnim05,
                                         unsigned char nRollAnim, unsigned char nRollEndAnim,
                                         unsigned char nWaitAnim, unsigned char nInAnim, unsigned char nOutAnim)
        {
            m_nPane = nPane;
            m_nNumPane = nNumPane;
            m_nNextPane = nNextPane;
            m_nMaxPane = nMaxPane;
            m_nAnim05 = nAnim05;
            m_nRollAnim = nRollAnim;
            m_nRollEndAnim = nRollEndAnim;
            m_nWaitAnim = nWaitAnim;
            m_nInAnim = nInAnim;
            m_nOutAnim = nOutAnim;
            m_nEffectPane = nEffectPane;
        }

        int CMissionCount::IsCountUpAnimPlay(CMenuPaneAccessor *pAccessor)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_nNextPane);
            if (pObj == 0) {
                return 0;
            }
            bcs::G2D::CPaneObj *pParent = pAccessor->GetPaneObj(pObj->GetPane()->GetParent());
            if (pParent->IsAnimPlay(m_nRollAnim) || pParent->IsAnimPlay(m_nRollEndAnim)) {
                return 1;
            }
            return 0;
        }

        void CMissionCount::MainLoop(CMenuPaneAccessor *pAccessor)
        {
            if (IsCountUpAnimPlay(pAccessor)) {
                return;
            }

            CountUp *pCountUp = static_cast<CountUp *>(nw4r::ut::List_GetNext(&m_List, 0));
            if (pCountUp != 0) {
                StartCountUp(pAccessor, pCountUp->m_nFrom, pCountUp->m_nTo);
                nw4r::ut::List_Remove(&m_List, pCountUp);
                delete pCountUp;
            }
        }

        void CMissionCount::SetDisp(CMenuPaneAccessor *pAccessor, int bDisp)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_nPane);
            if (pObj == 0) {
                return;
            }

            long nStop;
            long nPlay;
            if (bDisp) {
                nStop = m_nOutAnim;
                nPlay = m_nInAnim;
            } else {
                nStop = m_nInAnim;
                nPlay = m_nOutAnim;
            }
            pObj->StopAnimation(nStop, 1);
            pObj->PlayAnimation(nPlay, 1.0f, 0.0f, 1, 0, 1);
        }

        void CMissionCount::SetMove(CMenuPaneAccessor *pAccessor, long nAnim)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_nPane);
            if (pObj == 0) {
                return;
            }

            pObj->StopAnimation(m_nAnim05, 1);
            pObj->StopAnimation(m_nRollAnim, 1);
            pObj->StopAnimation(m_nRollEndAnim, 1);
            pObj->StopAnimation(m_nWaitAnim, 1);
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 0, 1);
        }

        void CMissionCount::SetNum(CMenuPaneAccessor *pAccessor, long nNum, long nMax)
        {
            pAccessor->SetTextBoxString(m_nNumPane, nNum, 0);
            pAccessor->SetTextBoxString(m_nNextPane, nNum, 0);
            pAccessor->SetTextBoxString(m_nMaxPane, nMax, 0);

            long nAnim = m_nWaitAnim;
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_nNumPane);
            if (pObj != 0) {
                bcs::G2D::CPaneObj *pParent = pAccessor->GetPaneObj(pObj->GetPane()->GetParent());
                if (pParent != 0) {
                    pParent->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 1, 1);
                }
            }
            pObj = pAccessor->GetPaneObj(m_nNextPane);
            if (pObj != 0) {
                bcs::G2D::CPaneObj *pParent = pAccessor->GetPaneObj(pObj->GetPane()->GetParent());
                if (pParent != 0) {
                    pParent->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 1, 1);
                }
            }
        }

        void CMissionCount::SetCountUpNum(CMenuPaneAccessor *pAccessor, long nFrom, long nTo)
        {
            if (!IsCountUpAnimPlay(pAccessor)) {
                StartCountUp(pAccessor, nFrom, nTo);
            } else {
                CountUp *pCountUp = new CountUp;
                pCountUp->m_nFrom = nFrom;
                pCountUp->m_nTo = nTo;
                nw4r::ut::List_Append(&m_List, pCountUp);
            }
        }

        void CMissionCount::StartCountUp(CMenuPaneAccessor *pAccessor, long nFrom, long nTo)
        {
            pAccessor->SetTextBoxString(m_nNumPane, nFrom, 0);
            pAccessor->SetTextBoxString(m_nNextPane, nTo, 0);

            // The roll, its end and the wait animation, in a row, on the
            // parents of both count boxes.
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_nNumPane);
            if (pObj != 0) {
                bcs::G2D::CPaneObj *pParent = pAccessor->GetPaneObj(pObj->GetPane()->GetParent());
                if (pParent != 0) {
                    pParent->PlayAnimation(m_nRollAnim, 1.0f, 0.0f, 0, 1, 1);
                    pParent->PushNextAnimation(m_nRollEndAnim, 1.0f, 0.0f, 0, 1);
                    pParent->PushNextAnimation(m_nWaitAnim, 1.0f, 0.0f, 0, 1);
                }
            }
            pObj = pAccessor->GetPaneObj(m_nNextPane);
            if (pObj != 0) {
                bcs::G2D::CPaneObj *pParent = pAccessor->GetPaneObj(pObj->GetPane()->GetParent());
                if (pParent != 0) {
                    pParent->PlayAnimation(m_nRollAnim, 1.0f, 0.0f, 0, 1, 1);
                    pParent->PushNextAnimation(m_nRollEndAnim, 1.0f, 0.0f, 0, 1);
                    pParent->PushNextAnimation(m_nWaitAnim, 1.0f, 0.0f, 0, 1);
                }
            }

            pObj = pAccessor->GetPaneObj(m_nEffectPane);
            if (pObj != 0) {
                CEffectCtrl::CreateAttachEffect(EFFECT_COUNT_UP, pObj, 0, -1, 0, 0, 0, 4, 0, 0);
            }
        }

        void CMissionCount::SetActiveColor(CMenuPaneAccessor *pAccessor, int bActive)
        {
        }

        // C3DCoin

        C3DCoin::C3DCoin()
        {
            m_nTeam = 0;
            m_hTask = 0;
            m_fAdjustScaleW = 1.0f;
            m_fAdjustScaleH = 1.0f;
        }

        C3DCoin::~C3DCoin()
        {
            Finalize();
        }

        void C3DCoin::Initialize(long nTeam, bcs::CTask *pParent)
        {
            m_nTeam = nTeam;
            m_bDisp = 1;

            bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
            m_hTask = (new C3DCoinTask(pParent, pos, m_nTeam))->GetId();

            bcs::G2D::CLytManager *pLytManager = bcs::G2D::CLytManager::GetInstance();
            if (pLytManager != 0) {
                bcs::FVEC3 scale;
                pLytManager->GetLocationAdjustScale(&scale, pLytManager->GetAdjustMode());
                m_fAdjustScaleW = scale.x;
                m_fAdjustScaleH = pLytManager->GetLocationAdjustScaleH(pLytManager->GetAdjustMode());
            }
        }

        void C3DCoin::Finalize()
        {
            C3DCoinTask *pTask = Get3DCoinTask();
            if (pTask != 0) {
                pTask->Delete();
                m_hTask = 0;
            }
        }

        void C3DCoin::ActReset()
        {
            C3DCoinTask *pTask = Get3DCoinTask();
            if (pTask != 0) {
                pTask->ActReset();
            }
        }

        void C3DCoin::ActPopup()
        {
            C3DCoinTask *pTask = Get3DCoinTask();
            if (pTask != 0) {
                pTask->ActPopup();
            }
        }

        void C3DCoin::ActDrop()
        {
            C3DCoinTask *pTask = Get3DCoinTask();
            if (pTask != 0) {
                pTask->ActDrop();
            }
        }

        void C3DCoin::ActUse(long nNum)
        {
            C3DCoinTask *pTask = Get3DCoinTask();
            if (pTask != 0) {
                pTask->ActUse(nNum);
            }
        }

        void C3DCoin::EnableDisp(int bDisp)
        {
            m_bDisp = bDisp;
            C3DCoinTask *pTask = Get3DCoinTask();
            if (pTask != 0) {
                pTask->SetDisp(bDisp);
            }
        }

        void C3DCoin::AttachPane(nw4r::lyt::Pane *pPane)
        {
            if (m_hTask != 0 && pPane != 0) {
                C3DCoinTask *pTask = Get3DCoinTask();
                if (pTask == 0) {
                    return;
                }

                // The pane's position: the sum of its and its parents'
                // translations, the x of location-adjusted ones scaled.
                bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                int bVisible = 1;
                int bAdjust = 0;
                // The manager is looked up but not used.
                bcs::G2D::CLytManager::GetInstance();

                for (; pPane != 0; pPane = pPane->GetParent()) {
                    if (!pPane->IsVisible()) {
                        bVisible = 0;
                        break;
                    }
                    if (pPane->IsLocationAdjust()) {
                        bAdjust = 1;
                    }
                    const nw4r::math::VEC3 &rTrans = pPane->GetTranslate();
                    bcs::FVEC3 trans;
                    trans.x = rTrans.x;
                    trans.y = rTrans.y;
                    trans.z = rTrans.z;
                    if (bAdjust) {
                        trans.x = trans.x * m_fAdjustScaleW / m_fAdjustScaleH;
                    }
                    pos.x += trans.x;
                    pos.y += trans.y;
                }

                pTask->SetDisp(bVisible && m_bDisp);
                pTask->SetPos(pos);
                pTask->CalcMtx();
            }
        }

        C3DCoinTask *C3DCoin::Get3DCoinTask()
        {
            return static_cast<C3DCoinTask *>(bcs::CTaskMan::GetTask(m_hTask));
        }

        // CEffectSet

        CEffectSet::CEffectSet()
        {
            Initialize();
        }

        CEffectSet::~CEffectSet()
        {
            Finalize();
        }

        void CEffectSet::Initialize()
        {
            for (long i = 0; i < GROUP_NUM; i++) {
                for (long j = 0; j < SLOT_NUM; j++) {
                    m_hEffect[i][j] = 0;
                }
            }
        }

        void CEffectSet::Finalize()
        {
            for (long i = 0; i < GROUP_NUM; i++) {
                for (long j = 0; j < SLOT_NUM; j++) {
                    if (m_hEffect[i][j] != 0) {
                        CEffectCtrl::EraseAttachEffect(m_hEffect[i][j]);
                        m_hEffect[i][j] = 0;
                    }
                }
            }
        }

        void CEffectSet::SetEffect(long nGroup, long nIdx, long nEffect, bcs::G2D::CPaneObj *pObj)
        {
            if (pObj != 0 && nGroup < GROUP_NUM && nIdx < SLOT_NUM) {
                if (nIdx < 0) {
                    unsigned long hEffect =
                        CEffectCtrl::CreateAttachEffect(nEffect, pObj, 0, -1, 0, 0, 0, 4, 0, 0);
                    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
                    if (pCluster != 0) {
                        pCluster->Set2D(1);
                    }
                } else if (m_hEffect[nGroup][nIdx] == 0) {
                    m_hEffect[nGroup][nIdx] =
                        CEffectCtrl::CreateAttachEffect(nEffect, pObj, 0, -1, 0, 0, 0, 4, 0, 0);
                }
            }
        }

        void CEffectSet::RemoveEffect(long nGroup, long nIdx)
        {
            if (nGroup < GROUP_NUM && nIdx < SLOT_NUM) {
                CEffectCtrl::StopAttachEffect(m_hEffect[nGroup][nIdx]);
                m_hEffect[nGroup][nIdx] = 0;
            }
        }

        void CEffectSet::SetDispFlag(int bDisp)
        {
            for (long i = 0; i < GROUP_NUM; i++) {
                for (long j = 0; j < SLOT_NUM; j++) {
                    unsigned long hEffect = m_hEffect[i][j];
                    CEffectCluster *pCluster = CEffectCtrl::GetInstance()->GetObject(hEffect);
                    if (pCluster != 0) {
                        pCluster->CallEffectFunc(&bcs::EF::CEffect::SetDispFlag, bDisp);
                    }
                }
            }
        }
    }
}
