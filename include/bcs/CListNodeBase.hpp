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
    //
    // The constructor and destructor are inline: the modules' stage 11
    // (DodgeF CStageDodge11) inline both into their list and pool
    // constructors and destructors, and emit the destructor weak (0x60
    // bytes).
    class CListNodeBase {
        public:
            CListNodeBase(CListBase *pList = 0) : m_pNext(0), m_pPrev(0), m_pList(pList) {}

            // DOL. The first node of the list this node is in, or null.
            CListNodeBase *GetBegin();
            // DOL (fn_8004BFA4, the same body; msw::CStgGmkMan's const
            // getters call it).
            const CListNodeBase *GetBegin() const;
            // DOL. Links this node in before pNode, into pNode's list.
            void Link(CListNodeBase *pNode);
            // DOL. Takes the node out of its list; returns the node that
            // followed it (the list loops of the pools and
            // CListBase::Clear walk on with it).
            CListNodeBase *Unlink();

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

            // The same step on a const node (calls the const GetBegin).
            static void Step(const CListNodeBase *&rpNode)
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

            friend class CListBase;

        public:
            // The vptr is at 0x0C: the data members are declared before the
            // first virtual function. HockeyF's CStageHockey07 calls the
            // fourth virtual function of msw::CNpcBossPackun::CMudBomb
            // through the slot after these (vtable of
            // CNpcBossPackunHockeyBase::CMudBombHockey: the node's part holds
            // the destructor, PreLink, PostLink, PreUnlink, PostUnlink). The
            // modules emit all five as weak functions; the four hooks are
            // empty (4 bytes each).
            virtual ~CListNodeBase() { Unlink(); }
            virtual void PreLink() {}
            virtual void PostLink() {}
            virtual void PreUnlink() {}
            virtual void PostUnlink() {}
    };

    // (member names guessed) The head of a list, 0x28 bytes: a head node
    // whose next node is the first one, the end node the loops stop at, the
    // number of nodes, and the vptr at 0x24 (vtable __vt__Q23bcs9CListBase:
    // the destructor and Clear). Layout and the inline constructor,
    // destructor and Clear from DodgeF CStageDodge11, whose pools hold two
    // lists each; CListBase::Clear is emitted weak (DodgeF CChrCore).
    class CListBase {
        public:
            CListBase() : m_End(this)
            {
                m_Head.Link(&m_End);
                m_nCount = 0;
            }

            CListNodeBase *GetFirst() const { return m_Head.m_pNext; }
            CListNodeBase *GetEnd() { return &m_End; }
            const CListNodeBase *GetEnd() const { return &m_End; }
            // (name guessed) The number of nodes.
            long GetCount() const { return m_nCount; }

        private:
            CListNodeBase m_Head; // 0x00
            CListNodeBase m_End;  // 0x10
            long m_nCount;        // 0x20

        public:
            virtual ~CListBase() { Clear(); }
            // Takes every node out of the list.
            virtual void Clear()
            {
                CListNodeBase *pNode = m_Head.m_pNext;
                while (pNode != &m_End) {
                    pNode = pNode->Unlink();
                }
            }
    };

    // (guessed) A list of T, a class with CListNodeBase as a base; the
    // nodes are converted to T and back. The DOL names instances of it
    // (bcs::CList<bcs::CPoolNode<...>>, each with its own vtable). The two
    // steps are what HockeyF's CStageHockey07 inlines to walk the mud bombs:
    // the first T, or null for an empty list; the T after pObj (the first
    // one for null), or null after the last one.
    template <class T>
    class CList : public CListBase {
        public:
            virtual ~CList() {}

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

            // (name guessed) Takes the first node out of the list.
            void PopFront()
            {
                T *pFirst = GetFirst();
                if (pFirst != 0) {
                    pFirst->Unlink();
                }
            }
    };
}

#endif /* !BCS_CLISTNODEBASE_HPP_ */
