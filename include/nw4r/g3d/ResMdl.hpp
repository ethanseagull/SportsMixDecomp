#ifndef NW4R_G3D_RESMDL_HPP_
#define NW4R_G3D_RESMDL_HPP_

#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/g3d/ResMat.hpp"

namespace nw4r {
    namespace g3d {

        struct ResMdlData;
        class ResFile;

        // Handle to a model resource: one pointer, null when the resource
        // was not found. Minimal declaration written for this project.
        class ResMdl {
            public:
                explicit ResMdl(ResMdlData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                // (DOL) The material of that name; an invalid handle if
                // there is none.
                ResMat GetResMat(ResName name) const;
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
