#ifndef MSW_CSTGGMKMAN_HPP_
#define MSW_CSTGGMKMAN_HPP_

#include "bcs/CListNodeBase.hpp"
#include "game/msw/CStgGmk.hpp"

namespace msw {

    // The list of every stage gimmick (msw::CStgGmk), a singleton. A DOL
    // class, size 0x2C (CreateInstance allocates it): the list at 0x00, the
    // vptr at 0x28 after it (vtable lbl_80474E50: the destructor and
    // SendMessage). Each gimmick's constructor registers its node.
    // CStageMan creates the instance, destroys it and forwards its
    // SendMessage calls to it. Names marked "guessed" are not in the DOL's
    // symbols.
    class CStgGmkMan {
        public:
            typedef bcs::CList<CStgGmk::CNode> CGmkList;

            static CStgGmkMan *GetInstance();
            // (names guessed) Called by CStageMan's constructor and
            // destructor.
            static void CreateInstance();
            static void DestroyInstance();

            // (name guessed) Adds a gimmick's node at the end of the list.
            void Regist(CStgGmk::CNode *pNode);

            // The first gimmick of the given type (-1: of any type) and the
            // one after pGmk; null when there is none.
            CStgGmk *GetFirst(int type);
            const CStgGmk *GetFirst(int type) const;
            CStgGmk *GetNext(CStgGmk *pGmk, int type);
            const CStgGmk *GetNext(const CStgGmk *pGmk, int type) const;

        private:
            // (names guessed) The walk shared by the getters: the gimmick of
            // pNode or of the first node after it whose type matches.
            CStgGmk *Find_(bcs::CListNodeBase *pNode, int type);
            const CStgGmk *Find_(const bcs::CListNodeBase *pNode, int type) const;

            static CStgGmkMan *m_pInstance;

            CGmkList m_List; // 0x00, 0x28 bytes

        public:
            virtual ~CStgGmkMan();
            // Passes a stage message on to every gimmick (CStageMan's
            // SendMessage calls it with its own four arguments). Only the
            // messages that start a phase of the game are handled.
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
    };
}

#endif /* !MSW_CSTGGMKMAN_HPP_ */
