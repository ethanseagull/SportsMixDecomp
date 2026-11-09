#ifndef NW4R_G3D_RESANMCHR_HPP_
#define NW4R_G3D_RESANMCHR_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace g3d {

        // Only the member the modules read.
        struct ResAnmChrData {
            unsigned char unk00[0x20];
            unsigned short numFrame; // 0x20 (name guessed)
        };

        // Handle to a skeletal animation resource: one pointer, null when
        // the resource was not found. Minimal declaration written for this
        // project. GetClassName and ref are named by the string objects the
        // modules emit for them ("@STRING@GetClassName__Q34nw4r3g3d9ResAnmChrFv",
        // "@STRING@ref__Q34nw4r3g3d9ResAnmChrCFv"); the other names are
        // guessed.
        class ResAnmChr {
            public:
                explicit ResAnmChr(ResAnmChrData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResAnmChr"; }

                // The check, its file name and its line are the ones in the
                // original's inline copy (MenuF, msw::CMenuMdlTableSet::PlayImpl).
                ResAnmChrData &ref() const
                {
                    if (!IsValid()) {
                        nw4r::db::Panic("g3d_resanmchr_ac.h", 39, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                int GetNumFrame() const { return ref().numFrame; }

            private:
                ResAnmChrData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMCHR_HPP_ */
