#ifndef NW4R_G3D_RESANMCLR_HPP_
#define NW4R_G3D_RESANMCLR_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp" // AnmPolicy
#include "nw4r/g3d/ResMat.hpp"

namespace nw4r {
    namespace g3d {

        // Header of a colour animation resource (msw::CEventModel::AnimResFrame,
        // bcs::CAnmMatClr; DOL). Only the members read so far.
        struct ResAnmClrData {
            unsigned char unk00[0x10];
            long toClrDataDic;       // 0x10, dictionary of the animated materials
            unsigned char unk14[0x0C];
            unsigned short numFrame; // 0x20 (name guessed)
            unsigned short numMaterial; // 0x22
            AnmPolicy policy;        // 0x24
        };

        // The animation of one material; only the name is read.
        struct ResAnmClrMatData {
            long name; // 0x00, offset of the material's name from this entry

            ResName GetName() const
            {
                return ResName(reinterpret_cast<const ResNameData *>(reinterpret_cast<const char *>(this) + name - 4));
            }
        };

        // The colour result of one material. Size 0x5C
        // (bcs::CAnmMatClr::Create allocates them). Member names after
        // nw4r: one bit per animated colour register in bRgbaExist, each
        // colour with the mask of the bits it replaces.
        struct ClrAnmResult {
            enum {
                CLA_MAX = 11
            };

            unsigned long bRgbaExist;          // 0x00
            unsigned long rgba[CLA_MAX];       // 0x04
            unsigned long rgbaMask[CLA_MAX];   // 0x30
        };

        // (DOL; written in the DOL's animation file, bcs::CMdlObj calls it)
        // Puts the animated colours of pResult into a material's lighting
        // channels and TEV colour registers, keeping the bits each mask
        // leaves alone.
        void ApplyClrAnmResult(ResMatChan chan, ResMatTevColor tev, const ClrAnmResult *pResult);

        // Handle to a colour animation resource: one pointer, null when the
        // resource was not found. Minimal declaration written for this
        // project; the member and function names are guessed.
        class ResAnmClr {
            public:
                // (guessed) The default constructor holds a check that folds
                // away (a null pointer tested for alignment), like ResFile's.
                // It has no effect, but it keeps a member array of handles
                // (BossF CBallBoss::m_aPowerAnmClr) built by a plain store
                // loop as in the original instead of a peeled, unrolled one.
                // The original has no assertion strings for this class, so
                // the check here makes no call. Stand-in for whatever the
                // original constructor really held.
                ResAnmClr() : m_pData(0)
                {
                    ResAnmClrData *p = 0;
                    if ((unsigned long)p & 0x3) {
                        m_pData = 0;
                    }
                }
                explicit ResAnmClr(ResAnmClrData *pData) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResAnmClr"; }

                // File name and line from the original's inline copy in
                // msw::CEventModel::AnimResFrame (DOL).
                ResAnmClrData &ref() const
                {
                    if (!IsValid()) {
                        nw4r::db::Panic("g3d_resanmclr_ac.h", 39, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                int GetNumFrame() const { return ref().numFrame; }
                int GetNumMaterial() const { return ref().numMaterial; }
                AnmPolicy GetAnmPolicy() const { return ref().policy; }

                // The dictionary of the animated materials (an offset turned
                // into a handle, as nw4r's ofs_to_obj).
                ResDic GetMatDic() const
                {
                    long ofs = ref().toClrDataDic;
                    if (ofs != 0) {
                        return ResDic(reinterpret_cast<unsigned char *>(m_pData) + ofs);
                    }
                    return ResDic(0);
                }

                // The animation of the idx-th material.
                const ResAnmClrMatData *GetMatAnm(int idx) const
                {
                    return static_cast<const ResAnmClrMatData *>(GetMatDic()[idx]);
                }

                // (DOL) Evaluates the animation of material id at frame.
                void GetAnmResult(ClrAnmResult *pResult, unsigned long id, float frame) const;

            private:
                ResAnmClrData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMCLR_HPP_ */
