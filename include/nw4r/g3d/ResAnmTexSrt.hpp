#ifndef NW4R_G3D_RESANMTEXSRT_HPP_
#define NW4R_G3D_RESANMTEXSRT_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/g3d/ResMat.hpp"

namespace nw4r {
    namespace g3d {

        // What an animation does at its end. (values after nw4r)
        enum AnmPolicy {
            ANM_POLICY_ONETIME = 0,
            ANM_POLICY_LOOP = 1
        };

        // Header of a texture matrix animation resource; only the members
        // bcs::CAnmTexSrt reads.
        struct ResAnmTexSrtData {
            unsigned char m_Unk00[0x10];  // 0x00
            long toTexSrtDataDic;         // 0x10, dictionary of the animated materials
            unsigned char m_Unk14[0x0C];  // 0x14
            unsigned short numFrame;      // 0x20
            unsigned short numMaterial;   // 0x22
            unsigned long texMtxMode;     // 0x24
            AnmPolicy policy;             // 0x28
        };

        // The animation of one material; only the name is read.
        struct ResAnmTexSrtMatData {
            long name; // 0x00, offset of the material's name from this entry

            // The name's characters are at name; its length is the word
            // before them.
            ResName GetName() const
            {
                return ResName(reinterpret_cast<const ResNameData *>(reinterpret_cast<const char *>(this) + name - 4));
            }
        };

        // Result of evaluating a texture matrix animation for one material.
        // Size 0xE8 (bcs::CAnmTexSrt::Create allocates an array of them
        // through __construct_new_array). Only the flags are mapped (0 = no
        // texture matrix animated). Minimal declaration written for this
        // project.
        struct TexSrtAnmResult {
            TexSrtAnmResult() {}

            unsigned long flags;       // 0x00
            unsigned char m_Data[0xE4]; // 0x04
        };

        // Handle to a texture matrix animation resource: one pointer, null
        // when the resource was not found. Minimal declaration written for
        // this project; the member and function names are guessed. The check
        // in ref, its file and line are the ones in the DOL's inline copies.
        class ResAnmTexSrt {
            public:
                explicit ResAnmTexSrt(ResAnmTexSrtData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResAnmTexSrt"; }

                ResAnmTexSrtData &ref() const
                {
                    if (!IsValid()) {
                        nw4r::db::Panic("g3d_resanmtexsrt_ac.h", 38, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
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
                    long ofs = ref().toTexSrtDataDic;
                    if (ofs != 0) {
                        return ResDic(reinterpret_cast<unsigned char *>(m_pData) + ofs);
                    }
                    return ResDic(0);
                }

                // The animation of the idx-th material.
                const ResAnmTexSrtMatData *GetMatAnm(int idx) const
                {
                    return static_cast<const ResAnmTexSrtMatData *>(GetMatDic()[idx]);
                }

                // (DOL) Evaluates the animation of material id at frame.
                void GetAnmResult(TexSrtAnmResult *pResult, unsigned long id, float frame) const;

            private:
                ResAnmTexSrtData *m_pData; // 0x00
        };

        // (NW4R, in the DOL) Puts the animated texture matrices of pResult
        // into a material (added for bcs::ApplyTexSrtAnmResult).
        void ApplyTexSrtAnmResult(ResTexSrt srt, ResMatIndMtxAndScale ind, const TexSrtAnmResult *pResult);
    }
}

#endif /* !NW4R_G3D_RESANMTEXSRT_HPP_ */
