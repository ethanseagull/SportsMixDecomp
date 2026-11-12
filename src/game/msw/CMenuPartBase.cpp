#include "game/msw/CMenuPartBase.hpp"
#include "game/msw/CMenuPartList.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/math/types.h"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"

// One file: msw::CMenuPartBase (the buttons) and msw::CMenuPartList (the
// list of them). Evidence: the two share .sdata2 constants, the
// CMenuPartList vtable comes right before CMenuPartBase's in .data, and
// ~CMenuPartBase and Disp sit among the CMenuPartList functions.

namespace msw {

    CMenuPartBase::CMenuPartBase(long nId)
    {
        m_nBtnKindId = 0xFFFF;
        m_Unk16 = 0xFFFF;
        m_nCurrentBtnKindId = 0xFFFF;
        m_nNextBtnKindId[0] = 0xFFFF;
        m_nNextBtnKindId[1] = 0xFFFF;
        m_nNextBtnKindId[2] = 0xFFFF;
        m_nNextBtnKindId[3] = 0xFFFF;
        m_nNextBtnKindId[4] = 0xFFFF;
        m_Unk24 = 0xFFFF;
        m_nAnim[0] = 0xFF;
        m_nAnim[1] = 0xFF;
        m_nAnim[2] = 0xFF;
        m_nAnim[3] = 0xFF;
        m_nAnim[4] = 0xFF;
        m_nAnim[5] = 0xFF;
        m_nAnim[6] = 0xFF;
        m_nAnim[7] = 0xFF;
        m_nAnim[8] = 0xFF;
        m_nAnim[9] = 0xFF;
        m_nAnim[10] = 0xFF;
        m_nAnim[11] = 0xFF;
        m_nSeCode[0] = -1;
        m_nSeCode[1] = -1;
        m_nSeCode[2] = -1;
        m_pList = 0;
        m_nId = nId;
        m_nState = 0;
        m_nFlags40 = 0;
        m_bUnk40_0002 = 1;
        m_nPadNoBits &= 0xF0;
    }

    void CMenuPartBase::Main()
    {
        if (m_nState == 5) {
            CMenuPaneAccessor *pAccessor = m_pList->m_pPaneAccessor;
            if (pAccessor->IsAnimEnd(m_nCurrentBtnKindId, m_nAnim[4])) {
                if ((m_nFlags40 & 0x0004) && m_pList->m_pPaneAccessor->IsAnimEnd(m_nCurrentBtnKindId, m_nAnim[4])) {
                    if (m_Unk42_0F) {
                        SetButtonIn();
                    } else {
                        SetButtonOut();
                    }
                }
            } else if (m_nFlags40 & 0x0010) {
                SetButtonIn();
            }
        }
    }

    long CMenuPartBase::SetButtonIn()
    {
        if (m_nBtnKindId != 0xFFFF) {
            bcs::G2D::CPaneObj *pPane = m_pList->m_pPaneAccessor->GetPaneObj(m_nBtnKindId);
            if (pPane) {
                if (m_nState == 5 && m_nAnim[5] != 0xFF) {
                    pPane->PlayAnimation(m_nAnim[5], 1.0f, 0.0f, 1, 1, 1);
                } else if (m_nAnim[0] != 0xFF) {
                    pPane->PlayAnimation(m_nAnim[0], 1.0f, 0.0f, 1, 1, 1);
                    if (m_nAnim[1] != 0xFF) {
                        pPane->PushNextAnimation(m_nAnim[1], 1.0f, 0.0f, 1, 1);
                    }
                }
                if (m_nAnim[6] != 0xFF) {
                    pPane->PlayAnimation(m_nAnim[6], 1.0f, 0.0f, 1, 1, 1);
                }
            }
        }
        m_nState = 1;
        return 0;
    }

    long CMenuPartBase::SetButtonOut()
    {
        if (m_nBtnKindId != 0xFFFF) {
            bcs::G2D::CPaneObj *pPane = m_pList->m_pPaneAccessor->GetPaneObj(m_nCurrentBtnKindId);
            if (pPane && m_nAnim[2] != 0xFF) {
                pPane->PlayAnimation(m_nAnim[2], 1.0f, 0.0f, 1, 1, 1);
                if (m_nAnim[3] != 0xFF) {
                    pPane->PushNextAnimation(m_nAnim[3], 1.0f, 0.0f, 1, 1);
                }
                if (m_nAnim[6] != 0xFF) {
                    pPane->PlayAnimation(m_nAnim[6], 1.0f, 0.0f, 1, 1, 1);
                }
            }
        }
        m_nState = 3;
        return 0;
    }

    long CMenuPartBase::SetButtonPush()
    {
        if (m_nBtnKindId != 0xFFFF) {
            bcs::G2D::CPaneObj *pPane = m_pList->m_pPaneAccessor->GetPaneObj(m_nBtnKindId);
            if (pPane) {
                if (m_nAnim[4] != 0xFF) {
                    pPane->PlayAnimation(m_nAnim[4], 1.0f, 0.0f, 1, 1, 1);
                    if (m_nAnim[5] != 0xFF) {
                        pPane->PushNextAnimation(m_nAnim[5], 1.0f, 0.0f, 1, 1);
                    }
                    if (m_nAnim[6] != 0xFF) {
                        pPane->PlayAnimation(m_nAnim[6], 1.0f, 0.0f, 1, 1, 1);
                    }
                    m_nState = 5;
                }
                return m_nId;
            }
        }
        return -1;
    }

    long CMenuPartBase::SetButtonPushCancel()
    {
        SetButtonOut();
        return 0;
    }

    long CMenuPartBase::SetButtonMultiIn(long)
    {
        return 0;
    }

    long CMenuPartBase::SetButtonMultiOut(long)
    {
        return 0;
    }

    long CMenuPartBase::ResetButton()
    {
        InitButtonAnim();
        ClearPadNoBits();
        return 0;
    }

    long CMenuPartBase::InitButtonAnim()
    {
        if (m_nBtnKindId != 0xFFFF) {
            if (m_nAnim[3] != 0xFF) {
                m_pList->m_pPaneAccessor->PlayAnimation(m_nBtnKindId, m_nAnim[3], 1.0f, 0.0f, 1, 1);
            }
            if (m_nAnim[6] != 0xFF) {
                m_pList->m_pPaneAccessor->PlayAnimation(m_nBtnKindId, m_nAnim[6], 1.0f, 0.0f, 1, 1);
            }
            if (m_nAnim[11] != 0xFF) {
                m_pList->m_pPaneAccessor->PlayAnimation(m_nBtnKindId, m_nAnim[11], 1.0f, 0.0f, 1, 1);
            }
            m_nState = 3;
        }
        return 0;
    }

    long CMenuPartBase::SetButtonAnimWait(long nAnim, int bReset)
    {
        if (m_nBtnKindId != 0xFFFF) {
            m_pList->m_pPaneAccessor->PlayAnimation(m_nBtnKindId, nAnim, 1.0f, 0.0f, 1, bReset);
            m_nState = 7;
        }
        return 0;
    }

    long CMenuPartBase::SetCursorAnimByPadNoBits(unsigned long nBits)
    {
        unsigned long nPadNoBits = nBits & 0xF;
        if (m_nBtnKindId != 0xFFFF) {
            if ((m_nFlags40 & 0x0001) == 1 || nPadNoBits == 0) {
                if (m_nAnim[11] != 0xFF) {
                    m_pList->m_pPaneAccessor->PlayAnimation(m_nBtnKindId, m_nAnim[11], 1.0f, 0.0f, 1, 1);
                }
            } else {
                // The cursor animation of the first pad whose bit is set.
                for (long i = 0; nPadNoBits != 0; i++) {
                    if (nPadNoBits & (1 << i)) {
                        if (m_nAnim[7 + i] != 0xFF) {
                            m_pList->m_pPaneAccessor->PlayAnimation(m_nBtnKindId, m_nAnim[7 + i], 1.0f, 0.0f, 1, 1);
                        }
                        break;
                    }
                }
            }
        }
        return 0;
    }

    long CMenuPartBase::SetButtonAnim(long nAnim, int bReset, long nNextAnim)
    {
        if (m_nBtnKindId != 0xFFFF) {
            bcs::G2D::CPaneObj *pPane = m_pList->m_pPaneAccessor->GetPaneObj(m_nBtnKindId);
            if (pPane) {
                if (nAnim >= 0) {
                    pPane->PlayAnimation(nAnim, 1.0f, 0.0f, 1, bReset, 1);
                }
                if (nNextAnim >= 0) {
                    pPane->PushNextAnimation(nNextAnim, 1.0f, 0.0f, 1, 1);
                }
            }
        }
        return 0;
    }

    void CMenuPartBase::ClearPadNoBits()
    {
        SetCursorAnimByPadNoBits(0);
        m_nPadNoBits = 0;
    }

    CMenuPartList::CMenuPartList()
    {
        nw4r::ut::List_Init(&m_List, 0);
        m_fSeWait = 1.0f;
        m_fVibWait = 1.0f;
    }

    CMenuPartList::~CMenuPartList()
    {
        CMenuPartBase *pPart = (CMenuPartBase *)nw4r::ut::List_GetLast(&m_List);
        while (pPart) {
            CMenuPartBase *pPrev = (CMenuPartBase *)nw4r::ut::List_GetPrev(&m_List, pPart);
            nw4r::ut::List_Remove(&m_List, pPart);
            delete pPart;
            pPart = pPrev;
        }
    }

    void CMenuPartList::Add(CMenuPartBase *pPart)
    {
        nw4r::ut::List_Append(&m_List, pPart);
    }

    void CMenuPartList::StateCheck()
    {
        for (CMenuPartBase *pPart = (CMenuPartBase *)nw4r::ut::List_GetFirst(&m_List); pPart;
             pPart = (CMenuPartBase *)nw4r::ut::List_GetNext(&m_List, pPart)) {
            pPart->Main();
        }
    }

    CMenuPartBase *CMenuPartList::FindObject(long nId)
    {
        if (nId < 0) {
            return 0;
        }
        for (CMenuPartBase *pPart = (CMenuPartBase *)nw4r::ut::List_GetFirst(&m_List); pPart;
             pPart = (CMenuPartBase *)nw4r::ut::List_GetNext(&m_List, pPart)) {
            if (nId == pPart->m_nId) {
                return pPart;
            }
        }
        return 0;
    }

    // m_nNextBtnKindId[0] is the part's own kind id, [1..4] the kind ids the
    // stick moves to (by the input bits 1, 4, 8, 2).
    CMenuPartBase *CMenuPartList::FindObjectByCurrentBtnKindId(unsigned short nBtnKindId)
    {
        for (CMenuPartBase *pPart = (CMenuPartBase *)nw4r::ut::List_GetFirst(&m_List); pPart;
             pPart = (CMenuPartBase *)nw4r::ut::List_GetNext(&m_List, pPart)) {
            if (nBtnKindId == pPart->m_nNextBtnKindId[0]) {
                return pPart;
            }
        }
        return 0;
    }

    long CMenuPartList::GetIndexByCurrentBtnKindId(unsigned short nBtnKindId)
    {
        CMenuPartBase *pPart = FindObjectByCurrentBtnKindId(nBtnKindId);
        return pPart ? pPart->m_nId : -1;
    }

    long CMenuPartList::CheckHitButton(bcs::FVEC3 *pPos)
    {
        nw4r::math::VEC2 pos(pPos->x, pPos->y);
        for (CMenuPartBase *pPart = (CMenuPartBase *)nw4r::ut::List_GetFirst(&m_List); pPart;
             pPart = (CMenuPartBase *)nw4r::ut::List_GetNext(&m_List, pPart)) {
            if (!(pPart->m_nFlags40 & 0x0001) && pPart->m_Unk16 != 0xFFFF) {
                nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(pPart->m_Unk16);
                if (pPane && bcs::G2D::IsHitPane(pPane, pos)) {
                    return pPart->m_nNextBtnKindId[0];
                }
            }
        }
        return 0;
    }

    long CMenuPartList::CheckStckMoveButton(long nPad, unsigned short nBtnKindId, unsigned long nInput)
    {
        long nNext = nBtnKindId;
        for (long i = 0; i <= 100; i++) {
            nNext = SearchStickMove_(nPad, nNext, nInput, nBtnKindId, 0);
            CMenuPartBase *pPart = FindObjectByCurrentBtnKindId(nNext);
            if (pPart && !(pPart->m_nFlags40 & 0x0001)) {
                return nNext;
            }
        }
        return nBtnKindId;
    }

    long CMenuPartList::SearchStickMove_(long nPad, unsigned short nBtnKindId, unsigned long nInput,
                                                   unsigned short nStartBtnKindId, long nDepth)
    {
        if (nDepth >= 16) {
            return nStartBtnKindId;
        }
        CMenuPartBase *pPart = FindObjectByCurrentBtnKindId(nBtnKindId);
        if (!pPart) {
            return nBtnKindId;
        }
        unsigned short nNext = 0;
        if (nInput & 1) {
            nNext = pPart->m_nNextBtnKindId[1];
        }
        if (nNext == 0 && (nInput & 4)) {
            nNext = pPart->m_nNextBtnKindId[2];
        }
        if (nNext == 0 && (nInput & 8)) {
            nNext = pPart->m_nNextBtnKindId[3];
        }
        if (nNext == 0 && (nInput & 2)) {
            nNext = pPart->m_nNextBtnKindId[4];
        }
        if (nNext == 0) {
            return nBtnKindId;
        }
        if (!FindObjectByCurrentBtnKindId(nNext)) {
            return nBtnKindId;
        }
        if (nNext == nStartBtnKindId) {
            return nNext;
        }
        if (CanStopCursor_(nPad, nNext)) {
            return nNext;
        }
        return SearchStickMove_(nPad, nNext, nInput, nStartBtnKindId, nDepth + 1);
    }

    unsigned short CMenuPartList::StickMovementJudgmentCursor(long nPad, unsigned short nBtnKindId, unsigned long nInput)
    {
        return JudgeStickMove_(nPad, nBtnKindId, nInput, nBtnKindId, 0);
    }

    unsigned short CMenuPartList::JudgeStickMove_(long nPad, long nBtnKindId, unsigned long nInput,
                                                  long nStartBtnKindId, long nDepth)
    {
        if (nDepth >= 16) {
            return nStartBtnKindId;
        }
        CMenuPartBase *pPart = FindObjectByCurrentBtnKindId(nBtnKindId);
        if (!pPart) {
            return nBtnKindId;
        }
        unsigned short nNext = 0;
        if (nInput & 1) {
            nNext = pPart->m_nNextBtnKindId[1];
        }
        if (nNext == 0 && (nInput & 4)) {
            nNext = pPart->m_nNextBtnKindId[2];
        }
        if (nNext == 0 && (nInput & 8)) {
            nNext = pPart->m_nNextBtnKindId[3];
        }
        if (nNext == 0 && (nInput & 2)) {
            nNext = pPart->m_nNextBtnKindId[4];
        }
        if (nNext == 0) {
            return nBtnKindId;
        }
        return nNext;
    }

    long CMenuPartList::CanStopCursor_(long nPad, long nBtnKindId)
    {
        CMenuPartBase *pPart = FindObjectByCurrentBtnKindId(nBtnKindId);
        if (!pPart) {
            return nBtnKindId;
        }
        if (pPart->m_nFlags40 & 0x0010) {
            return 1;
        }
        long nState = pPart->m_nState;
        if (CMenuPartBase::IsStatePush(nState) || !(pPart->m_nFlags40 & 0x0002)) {
            return 0;
        }
        // Not while the cursor of another pad is on it, nor unless it is out.
        for (long i = 0; i < 4; i++) {
            if (i != nPad) {
                if (pPart->m_nPadNoBits & (1 << i)) {
                    return 0;
                }
                if (!CMenuPartBase::IsStateOut(nState)) {
                    return 0;
                }
            }
        }
        return 1;
    }

    // (guesses) Word fill and copy loops.
    static inline void FillZero(unsigned long *pDst, unsigned long nNum)
    {
        for (long i = 0; i < nNum; i++) {
            pDst[i] = 0;
        }
    }

    static inline void Copy(unsigned long *pDst, const unsigned long *pSrc, unsigned long nNum)
    {
        for (long i = 0; i < nNum; i++) {
            pDst[i] = pSrc[i];
        }
    }

    static inline float ClampWait(float fWait)
    {
        if (fWait < 0.0f) {
            fWait = 0.0f;
        }
        return fWait;
    }

    long CMenuPartList::CheckButtonAnim(const unsigned short *pPushButtonId, unsigned long *pPadBits)
    {
        if (!pPushButtonId) {
            return -1;
        }
        unsigned long nNum = nw4r::ut::List_GetSize(&m_List);
        CMenuPadInfo *pPadInfo = &CMenuPadInfo::s_Instance;
        CMenuPad *pMenuPad = CMenuPad::GetInstance();
        // Per part, the bits of the players whose cursor is on it.
        unsigned long *aPadBits = (unsigned long *)__alloca(nNum * sizeof(unsigned long));
        FillZero(aPadBits, nNum);
        for (long nPad = 0; nPad < 4; nPad++) {
            long nIndex = GetIndexByCurrentBtnKindId(pPushButtonId[nPad]);
            if (nIndex >= 0) {
                aPadBits[nIndex] |= 1 << pPadInfo->GetPlayerNo(nPad);
            }
        }

        for (unsigned long i = 0; i < nNum; i++) {
            CMenuPartBase *pPart = FindObject(i);
            if (!(pPart->m_nFlags40 & 0x0001) && !pPart->IsPush()) {
                if (aPadBits[i] == 0 && !pPart->IsOut()) {
                    pPart->SetButtonOut();
                    pPart->SetCursorAnimByPadNoBits(0);
                }
                if (pPart->m_nPadNoBits != aPadBits[i]) {
                    pPart->SetButtonMultiOut(pPart->m_nPadNoBits & ~aPadBits[i]);
                    pPart->SetCursorAnimByPadNoBits(aPadBits[i]);
                }
            }
        }

        for (unsigned long i = 0; i < nNum; i++) {
            CMenuPartBase *pPart = FindObject(i);
            if (!(pPart->m_nFlags40 & 0x0001) && !pPart->IsPush()) {
                if (aPadBits[i] != 0) {
                    for (long nPad = 0; nPad < 4; nPad++) {
                        unsigned long nBit = 1 << nPad;
                        if (pMenuPad->IsConnect(nPad) && (nBit & aPadBits[i])
                            && !(nBit & pPart->m_nPadNoBits) && pPart->m_nNextBtnKindId[0] != 0xFFFF) {
                            if (!(m_fSeWait > 0.0f)) {
                                menu::CSndEff::s_Instance.Play(pPart->m_nSeCode[0], 1);
                            }
                            if (pMenuPad->IsInputPointerMode(nPad) && !(m_fVibWait > 0.0f)) {
                                menu::CVibEff::s_Instance.Play(pPart->m_nSeCode[2], 1, nBit & aPadBits[i]);
                            }
                        }
                    }
                    if (!pPart->IsIn()) {
                        pPart->SetButtonIn();
                        pPart->SetCursorAnimByPadNoBits(aPadBits[i]);
                    }
                    unsigned long nOld = pPart->m_nPadNoBits;
                    if (nOld != aPadBits[i]) {
                        pPart->SetButtonMultiIn(aPadBits[i] & ~nOld);
                        if (nOld & aPadBits[i]) {
                            pPart->SetCursorAnimByPadNoBits(nOld & aPadBits[i]);
                        }
                    }
                }
                pPart->m_nPadNoBits = aPadBits[i];
            }
        }

        if (pPadBits) {
            Copy(pPadBits, aPadBits, nNum);
        }

        float fRate = bcs::CMainApp::GetAppFrameRateF();
        m_fSeWait = ClampWait(m_fSeWait - fRate);
        m_fVibWait = ClampWait(m_fVibWait - fRate);
        return 0;
    }

    long CMenuPartList::SetPushButtonAnim(unsigned short nBtnKindId, long nPad)
    {
        CMenuPartBase *pPart = FindObjectByCurrentBtnKindId(nBtnKindId);
        if (!pPart) {
            return -1;
        }
        if (pPart->m_nFlags40 & 0x0001) {
            return -2;
        }
        if (!pPart->IsPush()) {
            pPart->SetButtonPush();
            pPart->m_nPadNoBits = 1 << CMenuPadInfo::s_Instance.GetPlayerNo(nPad);
            pPart->SetCursorAnimByPadNoBits(pPart->m_nPadNoBits);
            menu::CSndEff::s_Instance.Play(pPart->m_nSeCode[1], 1);
        }
        return pPart->m_nId;
    }

    long CMenuPartList::CancelPushButtonAnim(unsigned short nBtnKindId, long)
    {
        CMenuPartBase *pPart = FindObjectByCurrentBtnKindId(nBtnKindId);
        if (!pPart) {
            return -1;
        }
        if (pPart->m_nFlags40 & 0x0001) {
            return -2;
        }
        if (pPart->IsPush()) {
            pPart->SetButtonPushCancel();
        }
        return pPart->m_nId;
    }

    int CMenuPartList::IsPushButtonAnimEnd()
    {
        unsigned long nNum = nw4r::ut::List_GetSize(&m_List);
        for (unsigned long i = 0; i < nNum; i++) {
            CMenuPartBase *pPart = FindObject(i);
            if (pPart->IsPush()) {
                bcs::G2D::CPaneObj *pPane = m_pPaneAccessor->GetPaneObj(pPart->m_nCurrentBtnKindId);
                if (!bcs::G2D::IsAnimEndRecursive(pPane, pPart->m_nAnim[4])) {
                    return 0;
                }
            }
        }
        return 1;
    }

}
