#ifndef NW4R_G3D_RESMAT_HPP_
#define NW4R_G3D_RESMAT_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace g3d {

        // Contents of a material of a model resource; only the id is used.
        struct ResMatData {
            unsigned long size;        // 0x00
            long toResMdlData;         // 0x04
            long name;                 // 0x08
            unsigned long id;          // 0x0C, the material's index
        };

        // Handle to a material of a model resource: one pointer, null when
        // the material was not found. Minimal declaration written for this
        // project; the check, its file and line are the ones in the DOL's
        // inline copies (bcs::CAnmTexSrt::Bind).
        class ResMat {
            public:
                explicit ResMat(ResMatData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResMat"; }

                ResMatData &ref() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resmat_ac.h", 621, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                unsigned long GetID() const { return ref().id; }

            private:
                ResMatData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESMAT_HPP_ */
