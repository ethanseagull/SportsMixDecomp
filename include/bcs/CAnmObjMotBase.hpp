#ifndef BCS_CANMOBJMOTBASE_HPP_
#define BCS_CANMOBJMOTBASE_HPP_

#include "bcs/CAnmObj.hpp"
#include "bcs/New.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "nw4r/math/types.h"
#include <stdlib.h>

namespace bcs {

    // Base of the skeletal animations bcs::CAnmMotion and
    // bcs::CAnmBlendMotion (named by the mangled names of their members and
    // of msw::CC_DATA). Size 0x18: the derived classes' members start
    // there (CAnmBlendMotion::Create stores at 0x18).
    class CAnmObjMotBase : public CAnmObj {
        public:
            // Per-node settings of a model's motion (contents not mapped).
            struct NODE_EX_INFO;
            // A trigger in a motion (contents not mapped).
            struct MOT_TRG;
            // (names guessed) The triggers of one motion, and the motion
            // extra data file (mot_ex_info.bin) holding them.
            struct MOT_TRG_LIST;
            struct MOT_EX_INFO;

            // m_pNodeFlag values (names guessed): the low 14 bits are the
            // node's index in the animation.
            enum {
                NODE_UNBOUND = 0x8000, // not animated
                NODE_REST = 0x4000     // (meaning guessed) uses the rest pose
            };

            CAnmObjMotBase() : m_pNodeFlagBuf(0), m_pNodeFlag(0), m_nNodeNum(0), m_nPartMask(0) {}
            virtual ~CAnmObjMotBase() {}

            // 0x44 to 0xAC: the motion interface, overridden by
            // bcs::CAnmMotion, bcs::CAnmBlendMotion and the 0x50-byte
            // single-motion class whose vtable is at 0x80464018 (vtables
            // read from the DOL; names guessed from msw::CChrCore's Anim*
            // wrappers, which call them on a CAnmBlendMotion). nMask
            // selects the parts of the body (bit 0 the first, bit 1 the
            // second, 3 both); nLayer one of the blended motions of a part.
            // The VFuncXX names are kept where callers already use them.
            //
            // 0x44 The result of node nNode (pResult may be filled and
            // returned, or an internal buffer returned).
            virtual const nw4r::g3d::ChrAnmResult *GetResult(nw4r::g3d::ChrAnmResult *pResult, unsigned long nNode);
            virtual int VFunc48();                                        // 0x48 sets up the per-node flags, returns 1
            virtual void VFunc4C();                                       // 0x4C marks every node 0x8000
            virtual void SetAnmFrame(unsigned long nMask, float fFrame);  // 0x50 (msw::CChrCore::AnimSetFrame)
            virtual float GetAnmFrame(unsigned long nMask) const;         // 0x54 (AnimGetFrame)
            // 0x58 (AnimGetFrameNum) The length in frames. MenuF's
            // msw::SelectMenu::CBgMdl divides by it.
            virtual float VFunc58(long nMask) const;
            virtual void SetAnmRate(unsigned long nMask, float fRate);    // 0x5C (AnimSetSpeed)
            virtual float GetAnmRate(unsigned long nMask) const;          // 0x60 (AnimGetSpeed)
            // 0x64 (AnimGetMotID) The motion number playing in a layer, -1
            // if none; the caller compares it with 2.
            virtual long VFunc64(long nMask, unsigned long nLayer) const;
            virtual float GetBlendWeight(unsigned long nMask, unsigned long nLayer) const;  // 0x68
            virtual long GetNodeMotID(unsigned long nNode, unsigned long nLayer) const;     // 0x6C, 0x64 for the node's part
            // 0x70 The name of the motion playing in a layer (the ResAnmChr's
            // name), 0 if none.
            virtual const char *GetAnmName(unsigned long nMask, unsigned long nLayer) const;
            // 0x74 The entry of the motion playing in a layer in the table at
            // 0x18 of the motion extra data (mot_ex_info.bin; its mirrored
            // entry when the motion plays mirrored), 0 if none.
            virtual const void *GetMotExtInfo(unsigned long nMask, unsigned long nLayer) const;
            virtual const char *GetNodeAnmName(unsigned long nNode, unsigned long nLayer) const;  // 0x78, 0x70 for the node's part
            virtual unsigned long GetPartNum() const;                    // 0x7C (inline in the derived classes)
            virtual unsigned long GetLayerNum() const;                   // 0x80 (inline in the derived classes)
            virtual unsigned long GetResAnmNum() const;                  // 0x84 the number of motions
            // 0x88, 0x8C (AnimGetResFrameNum, IsAnimResLoopType): the
            // length in frames of motion nAnim of the file, and whether it
            // loops.
            virtual float VFunc88(long nAnim) const;
            virtual int VFunc8C(long nAnim) const;
            virtual const void *GetResMotExtInfo(long nAnim) const;     // 0x90, as 0x74 for a motion number
            virtual const char *GetResAnmName(long nAnim) const;        // 0x94
            // 0x98 (tested for nonzero by the callers) Whether the motion
            // of a layer plays mirrored (a negative motion number).
            virtual int VFunc98(long nMask, unsigned long nLayer) const;
            virtual int VFunc9C(long nMask, unsigned long nLayer) const;  // 0x9C
            // 0xA0, 0xA4 (msw::CChrCore::IsAnimInter): whether the part is
            // blending from the previous motion, and the blend frames.
            virtual int VFuncA0(long nMask) const;
            virtual float VFuncA4(long nMask) const;
            virtual void VFuncA8(int bLoop);                             // 0xA8 sets or clears the loop bit
            virtual const NODE_EX_INFO *GetNodeExInfo() const;           // 0xAC

            // (added for bcs::CAnmBlendMotion, which reads the flags of its
            // layers) The flags of node nNode.
            unsigned short GetNodeFlag(unsigned long nNode) const { return m_pNodeFlag[nNode]; }

        protected:
            // (name guessed) Allocates one flag per node of mdl. Inline in
            // bcs::CAnmMotion::Create and bcs::CAnmChrMotion::Create (the
            // model handle is copied to the stack for it).
            void CreateNodeFlag(nw4r::g3d::ResMdl mdl, long nHeap)
            {
                m_nNodeNum = mdl.GetResNodeNumEntries();
                m_pNodeFlag = m_pNodeFlagBuf = new (nHeap | 0x100) unsigned short[m_nNodeNum];
            }

            // (name guessed) Marks every node unbound. Inline in VFunc48 of
            // bcs::CAnmChrMotion; VFunc4C does the same and sets the bound
            // flag.
            void ClearNodeFlag()
            {
                for (unsigned long i = 0; i < m_nNodeNum; i++) {
                    m_pNodeFlag[i] = NODE_UNBOUND;
                }
            }

            // (name guessed) Frees the node flags. Inline in the Release
            // functions of all three motion classes.
            void ReleaseNodeFlag()
            {
                if (m_pNodeFlagBuf != 0) {
                    delete[] m_pNodeFlagBuf;
                }
                m_pNodeFlagBuf = 0;
                m_pNodeFlag = 0;
                m_nPartMask = 0;
                m_nNodeNum = 0;
                OffAnmFlag(ANMFLAG_4);
            }

            unsigned short *m_pNodeFlagBuf; // 0x0C what Create allocated (m_pNodeFlag may point into a shared buffer)
            unsigned short *m_pNodeFlag;    // 0x10 one per node (0x8000: not animated)
            unsigned short m_nNodeNum;      // 0x14
            unsigned short m_nPartMask;     // 0x16 the parts this object plays (3: all)
    };

    // (DOL fn_80010024 and fn_800103E4, defined near the end of the
    // animation file; names and parameter meanings guessed.) The result of
    // a node computed from the model's current node matrices, and the
    // weighted blend of two results (bFlag: bit 3 of the animation's flag
    // word).
    void CalcChrAnmResultFromMtx(nw4r::g3d::ResNode node, nw4r::g3d::ChrAnmResult *pResult,
                                 const nw4r::math::MTX34 *pNodeMtx);
    void BlendChrAnmResult(nw4r::g3d::ChrAnmResult *pOut, const nw4r::g3d::ChrAnmResult *pResult,
                           const nw4r::g3d::ChrAnmResult *pPrev, int bFlag, float fWeight, float fPrevWeight);

    // (field names guessed) The settings of one node of a model's motion;
    // the motion takes an array of them. BasketF's goal model
    // (game/basket/CGoalMdlBasket.cpp) gives tables of node, a kind (0 to
    // 3) and a flag that is 0 for the root and 1 for the others.
    struct CAnmObjMotBase::NODE_EX_INFO {
        // The DOL's motion code (game/bcs/CAnmMotion.cpp) shows what they
        // are: the node whose result a mirrored motion uses for this one,
        // how the result is mirrored (0 to 8, see
        // CAnmChrMotion::MirrorResult), the parts the node belongs to
        // (tested against m_nPartMask), and for kind 8 the axes to negate.
        unsigned short nNode;   // 0x00 mirror partner
        unsigned char nKind;    // 0x02 mirror kind
        unsigned char nFlag;    // 0x03 part mask
        unsigned short nUnk04;  // 0x04, 0 in every table seen
    };

    // (field names guessed) A trigger in a motion, as msw::CChrCore's
    // CallbackMotTrg_ reads it: the kind of trigger at 0x04 (it switches on
    // it) and two parameters at 0x06 and 0x08.
    struct CAnmObjMotBase::MOT_TRG {
        float m_fFrame;         // 0x00 the frame it fires at
        unsigned char m_nType;  // 0x04
        unsigned char m_nPartMask; // 0x05 fires only for these parts (tested against m_nPartMask)
        short m_nParam0;        // 0x06
        short m_nParam1;        // 0x08
        // 0x0A-0x0F: not known. The size, 0x10, is from BasketF's
        // CChrCoreBasketNinja, which clears a whole trigger on the stack.
        unsigned char m_Unk0A[0x10 - 0xA];
    };

    // (names guessed) The triggers of one motion: m_nNum MOT_TRGs at
    // m_nOffset from this header, sorted by frame. The offset is from the
    // file's start in the file and made relative by MOT_EX_INFO::Relocate.
    struct CAnmObjMotBase::MOT_TRG_LIST {
        unsigned long m_nNum; // 0x00
        long m_nOffset;       // 0x04

        bool IsEmpty() const
        {
            if (m_nNum != 0 && m_nOffset != 0) {
                return false;
            }
            return true;
        }

        const MOT_TRG *GetTrg() const
        {
            if (m_nOffset == 0) {
                return 0;
            }
            return reinterpret_cast<const MOT_TRG *>(m_nOffset + reinterpret_cast<const unsigned char *>(this));
        }
    };

    // (names guessed) The external data "mot_ex_info.bin" of a motion file,
    // read by bcs::CAnmMotion and bcs::CAnmBlendMotion. The offsets are
    // from the start of this header.
    struct CAnmObjMotBase::MOT_EX_INFO {
        enum {
            FLAG_RELOCATED = 2
        };

        unsigned char m_Unk00[0x8];
        unsigned long m_nFlag;   // 0x08
        unsigned long m_nAnmNum; // 0x0C the number of motions
        long m_nOfsAnmNo;        // 0x10 the ResAnmChr index of each motion number (unsigned short)
        long m_nOfsTrg;          // 0x14 one MOT_TRG_LIST per motion
        long m_nOfsExt;          // 0x18 two offsets per motion: normal and mirrored (meaning not known)

        // Makes the trigger lists' offsets relative to the lists, once.
        void Relocate()
        {
            if (!(m_nFlag & FLAG_RELOCATED)) {
                if (m_nOfsTrg != 0) {
                    MOT_TRG_LIST *pList = reinterpret_cast<MOT_TRG_LIST *>(m_nOfsTrg + reinterpret_cast<unsigned char *>(this));
                    for (unsigned long i = 0; i < m_nAnmNum; i++, pList++) {
                        if (pList->IsEmpty()) {
                            pList->m_nNum = 0;
                            pList->m_nOffset = 0;
                        } else {
                            pList->m_nOffset -= reinterpret_cast<unsigned char *>(pList) - reinterpret_cast<unsigned char *>(this);
                        }
                    }
                }
                m_nFlag |= FLAG_RELOCATED;
            }
        }

        // The ResAnmChr index of motion nAnim (negative: mirrored).
        unsigned long GetResAnmIndex(long nAnim) const
        {
            return reinterpret_cast<const unsigned short *>(m_nOfsAnmNo + reinterpret_cast<const unsigned char *>(this))[abs(nAnim)];
        }

        const MOT_TRG_LIST *GetTrgList(long nAnim) const
        {
            long idx = abs(nAnim);
            if (m_nOfsTrg == 0) {
                return 0;
            }
            const MOT_TRG_LIST *pList = reinterpret_cast<const MOT_TRG_LIST *>(m_nOfsTrg + reinterpret_cast<const unsigned char *>(this)) + idx;
            if (pList->IsEmpty()) {
                return 0;
            }
            return pList;
        }

        const void *GetExt(long nAnim) const
        {
            long idx = abs(nAnim);
            if (m_nOfsExt == 0) {
                return 0;
            }
            const long *pOfs = reinterpret_cast<const long *>(m_nOfsExt + reinterpret_cast<const unsigned char *>(this)) + idx * 2;
            if (nAnim < 0) {
                return OfsToPtr(pOfs[1]);
            }
            return OfsToPtr(pOfs[0]);
        }

        // The data at offset ofs of the file, 0 for offset 0.
        const void *OfsToPtr(long ofs) const
        {
            if (ofs == 0) {
                return 0;
            }
            return ofs + reinterpret_cast<const unsigned char *>(this);
        }
    };
}

#endif /* !BCS_CANMOBJMOTBASE_HPP_ */
