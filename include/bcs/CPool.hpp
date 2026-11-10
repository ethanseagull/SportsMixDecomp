#ifndef BCS_CPOOL_HPP_
#define BCS_CPOOL_HPP_

namespace bcs {

    namespace pool {
        // Locking policy of a pool that is used from one thread only.
        class NoLock;
    }

    // A pool of TNode objects that hold a T each. Known so far from the
    // mangled names of functions that take a Handle by reference and from
    // the inline Handle code in BasketF CStageBasket13 and
    // CPool<unsigned long, CPoolNode<unsigned long, 0>, NoLock>::Handle::IsValid.
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

                    // Out of line in BasketF for the CPoolNode<unsigned long, 0>
                    // pool, inline in CStageBasket13.
                    bool IsValid() const { return m_pPool != 0 && GetNode() != 0; }

                private:
                    CPool *m_pPool;      // 0x00
                    unsigned long m_nId; // 0x04
            };

        private:
            unsigned char m_Unk00[0x4]; // 0x00, the vptr (the pool has a vtable)
            TNode *m_pNodes;            // 0x04

            friend class Handle;
    };
}

#endif /* !BCS_CPOOL_HPP_ */
