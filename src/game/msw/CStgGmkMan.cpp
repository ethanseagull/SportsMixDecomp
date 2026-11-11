#include "game/msw/CStgGmkMan.hpp"

namespace msw {

    // Messages of SendMessage (values from CStgGmkMan::SendMessage; names
    // guessed from the CStgGmk hooks they call).
    enum {
        STG_MSG_ENTRANCE_DEMO_START = 3,
        STG_MSG_GAME_INIT = 15,
        STG_MSG_GAME_START = 16,
        STG_MSG_GAME_END = 17,
        STG_MSG_PERIOD_INIT = 18,
        STG_MSG_TURN_INIT = 19
    };

    CStgGmkMan *CStgGmkMan::m_pInstance;

    CStgGmkMan::~CStgGmkMan() {}

    CStgGmkMan *CStgGmkMan::GetInstance()
    {
        return m_pInstance;
    }

    void CStgGmkMan::CreateInstance()
    {
        if (m_pInstance == 0) {
            m_pInstance = new CStgGmkMan;
        }
    }

    void CStgGmkMan::DestroyInstance()
    {
        if (m_pInstance != 0) {
            delete m_pInstance;
        }
        m_pInstance = 0;
    }

    void CStgGmkMan::Regist(CStgGmk::CNode *pNode)
    {
        pNode->Link(m_List.GetEnd());
    }

    inline CStgGmk *CStgGmkMan::Find_(bcs::CListNodeBase *pNode, int type)
    {
        if (type != -1) {
            while (pNode != m_List.GetEnd()) {
                if (type == static_cast<CStgGmk::CNode *>(pNode)->GetGmk()->GetType()) {
                    break;
                }
                bcs::CListNodeBase::Step(pNode);
            }
        }
        if (pNode == m_List.GetEnd()) {
            return 0;
        }
        return static_cast<CStgGmk::CNode *>(pNode)->GetGmk();
    }

    inline const CStgGmk *CStgGmkMan::Find_(const bcs::CListNodeBase *pNode, int type) const
    {
        if (type != -1) {
            while (pNode != m_List.GetEnd()) {
                if (type == static_cast<const CStgGmk::CNode *>(pNode)->GetGmk()->GetType()) {
                    break;
                }
                bcs::CListNodeBase::Step(pNode);
            }
        }
        if (pNode == m_List.GetEnd()) {
            return 0;
        }
        return static_cast<const CStgGmk::CNode *>(pNode)->GetGmk();
    }

    CStgGmk *CStgGmkMan::GetFirst(int type)
    {
        return Find_(m_List.CListBase::GetFirst(), type);
    }

    const CStgGmk *CStgGmkMan::GetFirst(int type) const
    {
        const bcs::CListNodeBase *pNode = m_List.CListBase::GetFirst();
        return Find_(pNode, type);
    }

    CStgGmk *CStgGmkMan::GetNext(CStgGmk *pGmk, int type)
    {
        bcs::CListNodeBase *pNode = &pGmk->GetNode();
        bcs::CListNodeBase::Step(pNode);
        return Find_(pNode, type);
    }

    const CStgGmk *CStgGmkMan::GetNext(const CStgGmk *pGmk, int type) const
    {
        const bcs::CListNodeBase *pNode = &pGmk->GetNode();
        bcs::CListNodeBase::Step(pNode);
        return Find_(pNode, type);
    }

    // Each case tests the first gimmick before its loop as well (the
    // original has the extra test; a plain for loop leaves it out).
    void CStgGmkMan::SendMessage(long msg, long, long, long)
    {
        CStgGmk *pGmk;

        switch (msg) {
            case STG_MSG_ENTRANCE_DEMO_START:
                pGmk = GetFirst(-1);
                if (pGmk != 0) {
                    while (pGmk != 0) {
                        pGmk->OnEntranceDemoStart();
                        pGmk = GetNext(pGmk, -1);
                    }
                }
                break;
            case STG_MSG_GAME_INIT:
                pGmk = GetFirst(-1);
                if (pGmk != 0) {
                    while (pGmk != 0) {
                        pGmk->OnGameInit();
                        pGmk = GetNext(pGmk, -1);
                    }
                }
                break;
            case STG_MSG_GAME_START:
                pGmk = GetFirst(-1);
                if (pGmk != 0) {
                    while (pGmk != 0) {
                        pGmk->OnGameStart();
                        pGmk = GetNext(pGmk, -1);
                    }
                }
                break;
            case STG_MSG_GAME_END:
                pGmk = GetFirst(-1);
                if (pGmk != 0) {
                    while (pGmk != 0) {
                        pGmk->OnGameEnd();
                        pGmk = GetNext(pGmk, -1);
                    }
                }
                break;
            case STG_MSG_PERIOD_INIT:
                pGmk = GetFirst(-1);
                if (pGmk != 0) {
                    while (pGmk != 0) {
                        pGmk->OnPeriodInit();
                        pGmk = GetNext(pGmk, -1);
                    }
                }
                break;
            case STG_MSG_TURN_INIT:
                pGmk = GetFirst(-1);
                if (pGmk != 0) {
                    while (pGmk != 0) {
                        pGmk->OnTurnInit();
                        pGmk = GetNext(pGmk, -1);
                    }
                }
                break;
        }
    }
}
