#include "game/msw/CCokcpitMenuDataList.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CMenuPadInfo.hpp"

namespace msw {

    inline CCokcpitMenuDataList::CData::CData()
    {
        CMenuPadInfo &rPadInfo = CMenuPadInfo::s_Instance;

        m_aPadState[0] = rPadInfo.GetPadState(0);
        m_aPadState[1] = rPadInfo.GetPadState(1);
        m_aPadState[2] = rPadInfo.GetPadState(2);
        m_aPadState[3] = rPadInfo.GetPadState(3);

        CCockpit *pCockpit = CCockpit::GetTask();
        if (pCockpit != 0) {
            m_nUnk70 = pCockpit->GetUnk70();
            m_nResult = CCockpit::GetResult();
        }
    }

    CCokcpitMenuDataList::CData::~CData()
    {
        CMenuPadInfo &rPadInfo = CMenuPadInfo::s_Instance;

        rPadInfo.SetPadState(0, m_aPadState[0]);
        rPadInfo.SetPadState(1, m_aPadState[1]);
        rPadInfo.SetPadState(2, m_aPadState[2]);
        rPadInfo.SetPadState(3, m_aPadState[3]);

        CCockpit *pCockpit = CCockpit::GetTask();
        if (pCockpit != 0) {
            pCockpit->SetResults(m_nResult);
            pCockpit->SetUnk70(m_nUnk70);
        }
    }

    CCokcpitMenuDataList *CCokcpitMenuDataList::GetInstance()
    {
        return &s_Instance;
    }

    CCokcpitMenuDataList::~CCokcpitMenuDataList()
    {
        DeleteAll();
    }

    void CCokcpitMenuDataList::Initialize()
    {
        CData *pPrev;
        CData *pData = static_cast<CData *>(nw4r::ut::List_GetPrev(&m_List, 0));
        while (pData != 0) {
            pPrev = static_cast<CData *>(nw4r::ut::List_GetPrev(&m_List, pData));
            nw4r::ut::List_Remove(&m_List, pData);
            delete pData;
            pData = pPrev;
        }
        nw4r::ut::List_Init(&m_List, 0);
    }

    // The same loop as Initialize's. Both declare pPrev before the loop and
    // call List_GetPrev themselves; through DeleteAll (as the destructor
    // has it) or with pPrev declared inside the loop, the registers of the
    // two pointers come out swapped.
    void CCokcpitMenuDataList::Finalize()
    {
        CData *pPrev;
        CData *pData = static_cast<CData *>(nw4r::ut::List_GetPrev(&m_List, 0));
        while (pData != 0) {
            pPrev = static_cast<CData *>(nw4r::ut::List_GetPrev(&m_List, pData));
            nw4r::ut::List_Remove(&m_List, pData);
            delete pData;
            pData = pPrev;
        }
    }

    void CCokcpitMenuDataList::Push()
    {
        CData *pData = new CData;
        if (pData != 0) {
            nw4r::ut::List_Append(&m_List, pData);
        }
    }

    void CCokcpitMenuDataList::Pop()
    {
        CData *pData = GetPrev(0);
        if (pData != 0) {
            nw4r::ut::List_Remove(&m_List, pData);
            delete pData;
        }
    }

    CCokcpitMenuDataList CCokcpitMenuDataList::s_Instance;
}
