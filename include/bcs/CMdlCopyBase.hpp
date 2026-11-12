#ifndef BCS_CMDLCOPYBASE_HPP_
#define BCS_CMDLCOPYBASE_HPP_

#include "bcs/CMdlObj.hpp"

namespace bcs {

    // Base of the models that copy the pose of another one (bcs::CMdlRef,
    // bcs::CMdlClone, bcs::CMdlCopy). A DOL class, size 0x1D8, vtable
    // lbl_80467C98. Defined in the DOL's CMdlObj file (destructor
    // fn_8005DFB4, SetLink, ObjProc fn_8005E4DC, CopyNodeMtx fn_8005E79C).
    class CMdlCopyBase : public CMdlObj {
        public:
            // Inline: every derived constructor clears the four words
            // itself and stores only its own vtable.
            CMdlCopyBase() : m_nLinkHandle(0), m_nLinkFlag(0), m_nNodeNum(0), m_pNodeMap(0) {}
            virtual ~CMdlCopyBase();

            virtual void ObjProc(unsigned long, unsigned long, void *); // 0x1C
            // (name guessed) Copies the node matrices of pSrc, the model
            // linked to, into this one's (slot 0x118).
            virtual void CopyNodeMtx(const CMdlObj *pSrc);

            // (meaning of the second argument not known)
            void SetLink(const CMdlObj *pMdl, unsigned long);

        protected:
            // (name guessed) Builds the model from mdl and links it to pMdl
            // when there is one; 0 when the model could not be built.
            // Inline in the CMdlCopy and CMdlRef constructors.
            int CreateLink(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, unsigned long nArg, unsigned long nLinkFlag, long nHeap)
            {
                if (!CMdlObj::Create(mdl, nArg, nHeap)) {
                    return 0;
                }
                if (pMdl != 0) {
                    SetLink(pMdl, nLinkFlag);
                }
                return 1;
            }

            // (names guessed) The handle of the model linked to (its
            // CScnObj handle at 0xF8), SetLink's second argument (flag bits:
            // BasketF's ninja copies set 0x40000 when they start to vanish),
            // and per node of this model the node of the linked one it
            // copies (bit 15 set: none), allocated with new[].
            unsigned long m_nLinkHandle;  // 0x1C8
            unsigned long m_nLinkFlag;    // 0x1CC
            long m_nNodeNum;              // 0x1D0
            unsigned short *m_pNodeMap;   // 0x1D4
    };
}

#endif /* !BCS_CMDLCOPYBASE_HPP_ */
