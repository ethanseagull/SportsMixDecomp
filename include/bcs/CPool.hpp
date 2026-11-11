#ifndef BCS_CPOOL_HPP_
#define BCS_CPOOL_HPP_

#include "bcs/CListNodeBase.hpp"

namespace bcs {

    namespace pool {
        // Locking policy of a pool that is used from one thread only.
        class NoLock;
    }

    // (member names guessed) A node of a bcs::CPool: a list node (the pool's
    // free or used list) with the id its handles compare at 0x10. The id
    // holds the node's index in the low 16 bits and a serial number in the
    // 12 bits above, bumped (skipping 0) each time the node is released so
    // that old handles stop matching. The parts are read and written with
    // masks (the original loads the word once for the value and once for
    // the masked store; bit-fields load it once). A DOL class; OnCreate and OnRelease
    // are its empty inline hooks (emitted weak by the modules). Layout and
    // inline code from DodgeF CStageDodge11 (the pools of
    // msw::STAGE11::CBgFirePillarCluster).
    class CPoolNodeBase : public CListNodeBase {
        public:
            CPoolNodeBase()
            {
                m_nHandleId = 0;
                SetSerial(1);
            }

            unsigned long GetHandleId() const { return m_nHandleId; }

            // (names guessed) The parts of the id: the index (bits 0-15)
            // and the serial number (bits 16-27).
            void SetIndex(unsigned long index) { m_nHandleId = (index & 0xFFFF) | (m_nHandleId & ~0xFFFF); }
            unsigned long GetSerial() const { return (m_nHandleId >> 16) & 0xFFF; }
            void SetSerial(unsigned long serial)
            {
                m_nHandleId = ((serial << 16) & 0x0FFF0000) | (m_nHandleId & ~0x0FFF0000);
            }

            // (name guessed) A new serial number, never 0: old handles no
            // longer refer to the node.
            void IncSerial()
            {
                SetSerial(GetSerial() + 1);
                if (GetSerial() == 0) {
                    SetSerial(GetSerial() + 1);
                }
            }

            // Called when the pool hands the node out and takes it back.
            virtual void OnCreate() {}
            virtual void OnRelease() {}

            unsigned long m_nHandleId; // 0x10
    };

    // (guessed) A pool node holding a T at 0x14. The second parameter's
    // meaning is not known (always 0).
    template <typename T, int N>
    class CPoolNode : public CPoolNodeBase {
        public:
            virtual ~CPoolNode() {}

            T m_Data; // 0x14
    };

    // A pool of TNode objects that hold a T each, size 0x5C: the vptr, the
    // node array and its size, the list of free nodes and the list of nodes
    // in use. Known from the mangled names of functions that take a Handle
    // by reference, the inline Handle code in BasketF CStageBasket13 and
    // CPool<unsigned long, CPoolNode<unsigned long, 0>, NoLock>::Handle::IsValid,
    // and the pools of DodgeF CStageDodge11 (constructor, destructor, Create
    // and GetHandle).
    //
    // TNode must have the handle's id at 0x10 (m_nHandleId) and the T at
    // 0x14 (m_Data): both pools seen read them there (nodes of 0x18 and 0x2C
    // bytes).
    template <typename T, typename TNode, typename TLock>
    class CPool {
        public:
            // Refers to one node of the pool. Size 8. The low 16 bits of
            // the id are the node's index; the node is still the one
            // referred to while its own id equals the handle's.
            class Handle {
                public:
                    // BasketF has both as weak functions (0x10 and 0x40
                    // bytes) for the CStageMdl pool.
                    Handle() : m_pPool(0), m_nId(0) {}
                    Handle(CPool *pPool, unsigned long id) : m_pPool(pPool), m_nId(id) {}
                    ~Handle() {}

                    // (name guessed) The handle's node, or null when the
                    // pool has no nodes or the node has been reused.
                    TNode *GetNode() const
                    {
                        TNode *pNode = m_pPool->m_pNodes + (m_nId & 0xFFFF);
                        if (m_pPool->m_pNodes == 0 || m_nId != pNode->m_nHandleId) {
                            pNode = 0;
                        }
                        return pNode;
                    }

                    // (name guessed) The held value, or null.
                    T *Get() const
                    {
                        TNode *pNode = GetNode();
                        if (pNode == 0) {
                            return 0;
                        }
                        return &pNode->m_Data;
                    }

                    // Out of line in the modules for the
                    // CPoolNode<unsigned long, 0> pool (its address is taken
                    // in msw::STAGE11::CBgFirePillarCluster::PushBack),
                    // inline in CStageBasket13.
                    bool IsValid() const { return m_pPool != 0 && m_pPool->GetNode(m_nId) != 0; }

                private:
                    CPool *m_pPool;      // 0x00
                    unsigned long m_nId; // 0x04
            };

            CPool() : m_pNodes(0), m_nNodeNum(0) {}
            virtual ~CPool() { ReleaseAll(); }

            // Takes a free node and returns a handle to it (a handle with
            // id 0, which matches no node, when none is free).
            //
            // Create and GetHandle are not inline. The original calls both
            // (Create from PushBack and from DodgeF's CChrCoreDodge, GetHandle
            // from Create) wherever they are used; the stage 11 files are
            // compiled with `flags=noauto` (-inline noauto) so that ours do
            // too, see notes/toolchain.md.
            Handle Create();
            // A handle to the node with this id.
            Handle GetHandle(unsigned long id);

        protected:
            // (name guessed) The node with this id, or null.
            TNode *GetNode(unsigned long id) const
            {
                TNode *pNode = m_pNodes + (id & 0xFFFF);
                if (m_pNodes == 0 || id != pNode->m_nHandleId) {
                    pNode = 0;
                }
                return pNode;
            }

            // (name guessed) Gives the node with this id back to the free
            // list.
            void Release(unsigned long id)
            {
                TNode *pNode = GetNode(id);
                if (pNode != 0) {
                    pNode->Unlink();
                    pNode->OnRelease();
                    pNode->Link(m_FreeList.GetEnd());
                    pNode->IncSerial();
                }
            }

            // (name guessed) Gives every node in use back to the free list.
            void ReleaseAll()
            {
                TNode *pNode = static_cast<TNode *>(m_UsedList.CListBase::GetFirst());
                while (pNode != m_UsedList.GetEnd()) {
                    pNode->IncSerial();
                    TNode *pNext = static_cast<TNode *>(pNode->Unlink());
                    pNode->OnRelease();
                    pNode->Link(m_FreeList.GetEnd());
                    pNode = pNext;
                }
            }

            // (name guessed) Makes the pool use the nNum nodes at pNodes,
            // all of them free.
            void Init(TNode *pNodes, long nNum)
            {
                ReleaseAll();
                m_pNodes = pNodes;
                m_nNodeNum = nNum;
                for (long i = 0; i < m_nNodeNum; i++) {
                    m_pNodes[i].SetIndex(i);
                    m_pNodes[i].Link(m_FreeList.GetEnd());
                }
            }

            // (name guessed) Takes the nodes back and forgets them.
            void Term()
            {
                ReleaseAll();
                m_nNodeNum = 0;
                m_pNodes = 0;
                m_FreeList.Clear();
                m_UsedList.Clear();
            }

            TNode *m_pNodes;         // 0x04
            long m_nNodeNum;         // 0x08
            CList<TNode> m_FreeList; // 0x0C
            CList<TNode> m_UsedList; // 0x34

            friend class Handle;
    };

    template <typename T, typename TNode, typename TLock>
    typename CPool<T, TNode, TLock>::Handle CPool<T, TNode, TLock>::Create()
    {
        unsigned long id = 0;
        TNode *pNode = m_FreeList.Front();
        if (pNode != 0) {
            m_FreeList.PopFront();
            pNode->Link(m_UsedList.GetEnd());
            id = pNode->m_nHandleId;
            pNode->OnCreate();
        }
        return GetHandle(id);
    }

    template <typename T, typename TNode, typename TLock>
    typename CPool<T, TNode, TLock>::Handle CPool<T, TNode, TLock>::GetHandle(unsigned long id)
    {
        return Handle(this, id);
    }

    // (guessed) A CPool with its N nodes inside it, size 0x5C + N * the
    // node size.
    template <typename T, int N, typename TNode>
    class CFixedPool : public CPool<T, TNode, pool::NoLock> {
        public:
            CFixedPool() { this->Init(m_aNodes, N); }
            virtual ~CFixedPool() { this->Term(); }

        private:
            TNode m_aNodes[N]; // 0x5C
    };
}

#endif /* !BCS_CPOOL_HPP_ */
