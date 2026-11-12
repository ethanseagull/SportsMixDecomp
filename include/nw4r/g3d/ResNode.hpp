#ifndef NW4R_G3D_RESNODE_HPP_
#define NW4R_G3D_RESNODE_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/g3d/ResDic.hpp"

namespace nw4r {
    namespace g3d {

        struct ChrAnmResult;

        // Contents of a node of a model resource; only the id is used.
        struct ResNodeData {
            unsigned char m_Unk00[0x08]; // 0x00
            long name;                   // 0x08 offset of the name's characters (added for DOL bcs::CAnmChrMotion)
            unsigned long id;            // 0x0C, the node's index
            unsigned long mtxId;         // 0x10, the node's matrix index (added for DOL bcs::CalcChrAnmResultFromMtx)
            unsigned char m_Unk14[0x48]; // 0x14
            long toParentNode;           // 0x5C offset of the parent node from this one (added for DOL bcs::CalcChrAnmResultFromMtx)
            long toChildNode;            // 0x60
            long toNextSibling;          // 0x64 (added for DOL game/msw/CStgMdl.cpp)
        };

        // Handle to a node of a model resource: one pointer, null when the
        // node was not found. Minimal declaration written for this project.
        class ResNode {
            public:
                explicit ResNode(ResNodeData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                // The node's index, with the check the DOL's inline copies
                // make (g3d_resnode_ac.h, line 56; the strings are the ones
                // in the modules).
                unsigned long GetID() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resnode_ac.h", 56, "NW4R:Failed assertion IsValid()");
                    }
                    return IsValid() ? m_pData->id : 0;
                }

                // (added for DOL bcs::CAnmChrMotion) The check and its line
                // (44) are the DOL's inline copies'.
                static const char *GetClassName() { return "ResNode"; }
                ResNodeData &ref() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resnode_ac.h", 44, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                // The node's name, as a dictionary key (the length word
                // before the characters).
                ResName GetResName() const
                {
                    const char *p = reinterpret_cast<const char *>(&ref());
                    long ofs = reinterpret_cast<const ResNodeData *>(p)->name;
                    if (ofs != 0) {
                        return ResName(reinterpret_cast<const ResNameData *>(p + ofs - 4));
                    }
                    return ResName(0);
                }

                // (added for DOL bcs::CalcChrAnmResultFromMtx) The node's
                // matrix index, with the check the DOL's inline copies make
                // (line 83).
                unsigned long GetMtxID() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resnode_ac.h", 83, "NW4R:Failed assertion IsValid()");
                    }
                    return IsValid() ? m_pData->mtxId : 0;
                }

                // (added for DOL bcs::CalcChrAnmResultFromMtx) The parent
                // node, with NW4R's alignment check on the pointer (line 44
                // and strings as in the DOL's animation file).
                ResNode GetParentNode() const
                {
                    long ofs = ref().toParentNode;
                    if (ofs != 0) {
                        ResNodeData *p = reinterpret_cast<ResNodeData *>(reinterpret_cast<unsigned char *>(m_pData) + ofs);
                        if (reinterpret_cast<unsigned long>(p) & 0x3) {
                            db::Panic("g3d_resnode_ac.h", 44, "NW4R:Failed assertion !((u32)p & 0x3)");
                        }
                        return ResNode(p);
                    }
                    return ResNode(0);
                }

                // (added for DOL game/msw/CStgMdl.cpp) The next node of the
                // same parent; same shape and checks as GetParentNode.
                ResNode GetNextSibling() const
                {
                    long ofs = ref().toNextSibling;
                    if (ofs != 0) {
                        ResNodeData *p = reinterpret_cast<ResNodeData *>(reinterpret_cast<unsigned char *>(m_pData) + ofs);
                        if (reinterpret_cast<unsigned long>(p) & 0x3) {
                            db::Panic("g3d_resnode_ac.h", 44, "NW4R:Failed assertion !((u32)p & 0x3)");
                        }
                        return ResNode(p);
                    }
                    return ResNode(0);
                }

                // (DOL; added for game/msw/CStgMdl.cpp) Set the node's rest
                // pose.
                void SetScale(float x, float y, float z);
                void SetRotate(float x, float y, float z);
                void SetTranslate(float x, float y, float z);

                // (DOL) The node's rest pose as an animation result.
                void CalcChrAnmResult(ChrAnmResult *pResult) const;

            private:
                ResNodeData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESNODE_HPP_ */
