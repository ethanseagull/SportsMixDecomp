#ifndef MSW_CSTAGESCROLL_HPP_
#define MSW_CSTAGESCROLL_HPP_

#include "bcs/CPool.hpp"
#include "game/msw/CStageMdl.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    class CArn;
    class CStageMan;

    // (names guessed) The scrolling background of a stage, a DOL class held
    // as a member (HockeyF's CStageHockey05 at 0x218). Size 0x87C, with the
    // vptr at 0x878 after the data; the constructor and destructor are its
    // only named functions. The constructor stores the stage at 0x00 and
    // builds 13 entries of 0x24 bytes at 0x0C, two lists at 0x1E0 and 0x208
    // and 16 entries of 0x60 bytes at 0x270; 0x870 and 0x874 are the scene
    // objects whose X and Y angles GetRotX and GetRotY return.
    //
    // Vtable at 0x80474908. The slots before the destructor are not named
    // in the DOL; they are placeholders that keep the known ones in place.
    class CStageScroll {
        public:
            // (names guessed from here to m_aMdlPool) A node of a model
            // pool, 0x1C bytes: a reference count and the model, which the
            // node deletes (vtable at 0x804748E0).
            class CMdlNode : public bcs::CPoolNodeBase {
                public:
                    CMdlNode() : m_nRef(0), m_pMdl(0) {}
                    virtual ~CMdlNode()
                    {
                        if (m_pMdl != 0) {
                            delete m_pMdl;
                        }
                        m_pMdl = 0;
                    }

                    long m_nRef;        // 0x14
                    CStageMdl *m_pMdl;  // 0x18
            };

            // A pool of model nodes whose node array is allocated with new[]
            // and kept at 0x5C, 0x60 bytes (vtable at 0x80474A04; the base
            // pool's at 0x80474A10).
            class CMdlPool : public bcs::CPool<CStageMdl *, CMdlNode, bcs::pool::NoLock> {
                public:
                    CMdlPool() : m_pNodeBuf(0) {}
                    virtual ~CMdlPool()
                    {
                        Term();
                        if (m_pNodeBuf != 0) {
                            delete[] m_pNodeBuf;
                        }
                        m_pNodeBuf = 0;
                    }

                    CMdlNode *Find(unsigned long id) const { return GetNode(id); }
                    void Free(unsigned long id) { Release(id); }

                private:
                    CMdlNode *m_pNodeBuf; // 0x5C
            };

            // A counted reference to a node of a CMdlPool, 8 bytes: the
            // node is given back to the pool when its last reference goes.
            class CMdlRef {
                public:
                    CMdlRef() : m_pPool(0), m_nId(0) {}
                    ~CMdlRef()
                    {
                        CMdlPool *pPool = m_pPool;
                        if (pPool != 0) {
                            unsigned long id = m_nId;
                            CMdlNode *pNode = pPool->Find(id);
                            if (pNode != 0 && --pNode->m_nRef <= 0) {
                                pPool->Free(id);
                            }
                        }
                    }

                private:
                    CMdlPool *m_pPool;    // 0x00
                    unsigned long m_nId;  // 0x04
            };

            // What a scroll node carries, 0x14 bytes; the first 0xC bytes
            // are not set by the constructor.
            class CScrollMdl {
                public:
                    unsigned char m_Unk00[0xC]; // 0x00
                    CMdlRef m_Ref;              // 0x0C
            };

            // One of the 13 scroll nodes at 0x0C, 0x24 bytes (vtable at
            // 0x804749E8); the constructor links them all into the list at
            // 0x208.
            class CScrollNode : public bcs::CListNodeBase {
                public:
                    CScrollNode() {}
                    virtual ~CScrollNode() {}

                    CScrollMdl m_Mdl; // 0x10
            };

            enum {
                SCROLL_NODE_NUM = 13,
                MDL_POOL_NUM = 16
            };

            CStageScroll(CStageMan *pStage);

        private:
            CStageMan *m_pStage;                            // 0x00
            unsigned char m_Unk04;                          // 0x04
            long m_Unk08;                                   // 0x08, 1 on stage 0x19
            CScrollNode m_aNode[SCROLL_NODE_NUM];           // 0x0C
            bcs::CList<CScrollNode> m_List;                 // 0x1E0
            bcs::CList<CScrollNode> m_FreeList;             // 0x208
            float m_Unk230;                                 // 0x230
            long m_Unk234;                                  // 0x234
            long m_Unk238;                                  // 0x238
            float m_Unk23C;                                 // 0x23C
            float m_Unk240;                                 // 0x240
            float m_Unk244;                                 // 0x244
            float m_Unk248;                                 // 0x248
            unsigned char m_Unk24C[0x10];                   // 0x24C
            float m_Unk25C;                                 // 0x25C
            float m_Unk260;                                 // 0x260
            float m_Unk264;                                 // 0x264
            float m_Unk268;                                 // 0x268
            float m_Unk26C;                                 // 0x26C
            CMdlPool m_aMdlPool[MDL_POOL_NUM];              // 0x270
            bcs::CScnObj *m_pScnObj870;                     // 0x870
            bcs::CScnObj *m_pScnObj874;                     // 0x874

        public:
            virtual void Unk08();
            virtual void Unk0C();
            virtual void Unk10();
            virtual void Unk14();
            virtual void Unk18();
            virtual void Unk1C();
            virtual void Unk20();
            virtual void Unk24();
            virtual void Unk28();
            virtual void Unk2C();
            virtual void Unk30();
            virtual void Unk34();
            virtual void Unk38();
            virtual void Unk3C();
            virtual void Unk40();
            virtual void Unk44();
            virtual void Unk48();
            virtual void Unk4C();
            virtual void Unk50();
            virtual void Unk54();
            virtual void Unk58();
            virtual void Unk5C();
            virtual void Unk60();
            virtual void Unk64();
            virtual void Unk68();
            virtual void Unk6C();
            virtual void Unk70();
            virtual void Unk74();
            virtual void Unk78();
            virtual void Unk7C();
            virtual void Unk80();
            virtual void Unk84();
            virtual void Unk88();
            virtual ~CStageScroll();                                                    // 0x8C
            // 0x90, fn_8016EB5C: one frame of the scroll.
            virtual void Main(float frame);
            virtual void Unk94();
            // 0x98, fn_8016C304: builds scroll entry no from a stage model.
            virtual void CreateMdl(long no, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            // 0x9C, fn_8016C914
            virtual void OnSceneEntry();
            // 0xA0, fn_8016DF98: the X angle of the object at 0x870 (0 without one).
            virtual float GetRotX() const;
            // 0xA4, fn_8016DFB4: the Y angle of the object at 0x874.
            virtual float GetRotY() const;
    };
}

#endif /* !MSW_CSTAGESCROLL_HPP_ */
