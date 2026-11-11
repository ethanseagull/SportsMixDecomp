#ifndef NW4R_G3D_RESNODE_HPP_
#define NW4R_G3D_RESNODE_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace g3d {

        // Contents of a node of a model resource; only the id is used.
        struct ResNodeData {
            unsigned char m_Unk00[0x0C]; // 0x00
            unsigned long id;            // 0x0C, the node's index
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

            private:
                ResNodeData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESNODE_HPP_ */
