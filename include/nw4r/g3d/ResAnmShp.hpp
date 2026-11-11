#ifndef NW4R_G3D_RESANMSHP_HPP_
#define NW4R_G3D_RESANMSHP_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace g3d {

        // Only the member read so far (msw::CEventModel::AnimResFrame, DOL).
        struct ResAnmShpData {
            unsigned char unk00[0x24];
            unsigned short numFrame; // 0x24 (name guessed)
        };

        // Handle to a shape animation resource: one pointer, null when the
        // resource was not found. Minimal declaration written for this
        // project; the check, its file name and line are the ones in the
        // original's inline copy in msw::CEventModel::AnimResFrame.
        class ResAnmShp {
            public:
                explicit ResAnmShp(ResAnmShpData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResAnmShp"; }

                ResAnmShpData &ref() const
                {
                    if (!IsValid()) {
                        nw4r::db::Panic("g3d_resanmshp_ac.h", 166, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                int GetNumFrame() const { return ref().numFrame; }

            private:
                ResAnmShpData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMSHP_HPP_ */
