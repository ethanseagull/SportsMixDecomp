#ifndef BCS_CLISTNODEBASE_HPP_
#define BCS_CLISTNODEBASE_HPP_

namespace bcs {

    class CListBase;

    // A node of the engine's linked lists. A DOL class; GetBegin, Link and
    // Unlink are its only named functions. Layout read from GetBegin (the
    // list at 0x08) and from the loops of msw::CStageMan::SetDispByTag and
    // HockeyF's CStageHockey01::SwitchAudienceMdl_ (the next node at 0x00,
    // the node's data from 0x10). Member names and the word at 0x04 (most
    // likely the previous node) are guesses.
    class CListNodeBase {
        public:
            // DOL. The first node of the list this node is in, or null.
            CListNodeBase *GetBegin();
            // DOL. (parameter meaning guessed)
            void Link(CListNodeBase *pNode);
            // DOL.
            void Unlink();

            // (guessed) The step of the engine's inline list loops: the next
            // node, and after a node with no next one, GetBegin() of that
            // null pointer, as the DOL's loops do. The loops stop at the
            // list's end node first, so that branch is never taken.
            static void Step(CListNodeBase *&rpNode)
            {
                rpNode = rpNode->m_pNext;
                if (rpNode == 0) {
                    rpNode = rpNode->GetBegin();
                }
            }

            // (name guessed) The next node, or null after the last one.
            CListNodeBase *GetNext() const { return m_pNext; }

        private:
            CListNodeBase *m_pNext; // 0x00
            CListNodeBase *m_pPrev; // 0x04 (guessed)
            CListBase *m_pList;     // 0x08

        public:
            // The vptr is at 0x0C: the data members are declared before the
            // first virtual function. HockeyF's CStageHockey07 calls the
            // fourth virtual function of msw::CNpcBossPackun::CMudBomb
            // through the slot after these (vtable of
            // CNpcBossPackunHockeyBase::CMudBombHockey: the node's part holds
            // the destructor, PreLink, PostLink, PreUnlink, PostUnlink). The
            // modules emit all five as weak functions; the four hooks are
            // empty (4 bytes each), the destructor's body (0x60 bytes) is not
            // written yet.
            virtual ~CListNodeBase();
            virtual void PreLink() {}
            virtual void PostLink() {}
            virtual void PreUnlink() {}
            virtual void PostUnlink() {}
    };

    // (name and layout guessed) The head of a list as the loops above read
    // it: the first node at 0x00, the end node at 0x10.
    class CListBase {
        public:
            CListNodeBase *GetFirst() const { return m_pFirst; }
            CListNodeBase *GetEnd() { return &m_End; }

        private:
            CListNodeBase *m_pFirst; // 0x00
            unsigned char m_Unk04[0xC];
            CListNodeBase m_End;     // 0x10
    };

    // (guessed) A list of T, a class with CListNodeBase as a base; the
    // nodes are converted to T and back. The DOL names instances of it
    // (bcs::CList<bcs::CPoolNode<...>>). The two steps are what HockeyF's
    // CStageHockey07 inlines to walk the mud bombs: the first T, or null
    // for an empty list; the T after pObj (the first one for null), or null
    // after the last one.
    template <class T>
    class CList : public CListBase {
        public:
            T *GetFirst()
            {
                CListNodeBase *pNode = CListBase::GetFirst();
                if (pNode != GetEnd()) {
                    return static_cast<T *>(pNode);
                }
                return 0;
            }

            T *GetNext(T *pObj)
            {
                if (pObj == 0) {
                    return GetFirst();
                }
                CListNodeBase *pNext = pObj->GetNext();
                bool bValid = pNext != 0 && pNext != static_cast<T *>(GetEnd());
                return bValid ? static_cast<T *>(pNext) : 0;
            }
    };
}

#endif /* !BCS_CLISTNODEBASE_HPP_ */
