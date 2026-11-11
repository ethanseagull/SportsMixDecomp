#ifndef NW4R_G3D_RESMDL_HPP_
#define NW4R_G3D_RESMDL_HPP_

#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/g3d/ResMat.hpp"
#include "nw4r/g3d/ResNode.hpp"

namespace nw4r {
    namespace g3d {

        struct ResMdlData;
        class ResFile;

        // Handle to a model resource: one pointer, null when the resource
        // was not found. Minimal declaration written for this project.
        class ResMdl {
            public:
                // The default constructor makes NW4R's alignment check like
                // ResFile's (it folds away; the line number is a guess, the
                // strings are in the DOL's msw::CStgGmk_WaterPillar .data).
                ResMdl() : m_pData(0)
                {
                    ResMdlData *p = 0;
                    if ((unsigned long)p & 0x1F) {
                        db::Panic("g3d_resmdl_ac.h", 60, "NW4R:Failed assertion !((u32)p & 0x1f)");
                    }
                }
                explicit ResMdl(ResMdlData *pData) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                // (DOL) The material of that name; an invalid handle if
                // there is none.
                ResMat GetResMat(ResName name) const;
                // (DOL) The node of that name; an invalid handle if there is
                // none.
                ResNode GetResNode(const char *pName) const;
                // (DOL) The number of materials.
                unsigned long GetResMatNumEntries() const;
                // (DOL) The file the model is in.
                ResFile GetParent();

            private:
                ResMdlData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESMDL_HPP_ */
