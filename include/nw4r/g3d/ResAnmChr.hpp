#ifndef NW4R_G3D_RESANMCHR_HPP_
#define NW4R_G3D_RESANMCHR_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/math/types.h"

namespace nw4r {
    namespace g3d {

        // Only the member the modules read.
        struct ResAnmChrData {
            unsigned char unk00[0x10];
            long toChrDataDic;       // 0x10: offset of the node dictionary (name guessed)
            unsigned char unk14[0xC];
            unsigned short numFrame; // 0x20 (name guessed)
        };

        // The result of one node of a skeletal animation at one frame:
        // flags, scale, rotation and the rotation-translation matrix.
        // 0x4C bytes. Minimal declaration written for this project after
        // the layout NW4R decompilations give it; only GetTranslate is used
        // (TourF's Stage33).
        struct ChrAnmResult {
            unsigned long flags; // 0x00
            math::VEC3 s;        // 0x04
            math::VEC3 rawR;     // 0x10
            float rt[3][4];      // 0x1C

            void GetTranslate(math::VEC3 *pTrans) const;
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

                // (name guessed) The dictionary of the animated nodes, by
                // node name; inline in msw::CEvNodeData.
                ResDic GetChrDic() const
                {
                    ResAnmChrData &r = ref();
                    if (r.toChrDataDic != 0) {
                        return ResDic(reinterpret_cast<unsigned char *>(&r) + r.toChrDataDic);
                    }
                    return ResDic(0);
                }

                // Node nNode of the animation at frame fFrame (DOL).
                void GetAnmResult(ChrAnmResult *pResult, unsigned long nNode, float fFrame) const;

            private:
                ResAnmChrData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMCHR_HPP_ */
