#ifndef NW4R_G3D_RESANMTEXPAT_HPP_
#define NW4R_G3D_RESANMTEXPAT_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp" // AnmPolicy

namespace nw4r {
    namespace g3d {

        // Header of a texture pattern animation resource; only the members
        // bcs::CAnmTexPat reads.
        struct ResAnmTexPatData {
            unsigned char m_Unk00[0x10];  // 0x00
            long toTexPatDataDic;         // 0x10, dictionary of the animated materials
            unsigned char m_Unk14[0x1C];  // 0x14
            unsigned short numFrame;      // 0x30
            unsigned short numMaterial;   // 0x32
            unsigned char m_Unk34[0x04];  // 0x34
            AnmPolicy policy;             // 0x38
        };

        // The animation of one material; only the name is read.
        struct ResAnmTexPatMatData {
            long name; // 0x00, offset of the material's name from this entry

            // The name's characters are at name; its length is the word
            // before them.
            ResName GetName() const
            {
                return ResName(reinterpret_cast<const ResNameData *>(reinterpret_cast<const char *>(this) + name - 4));
            }
        };

        // Handle to a texture or a palette of a resource file; only
        // default-constructed here. Minimal declarations written for this
        // project.
        struct ResTexData;
        struct ResPlttData;

        class ResTex {
            public:
                explicit ResTex(void *p = 0) : m_pData(static_cast<ResTexData *>(p)) {}

            private:
                ResTexData *m_pData; // 0x00
        };

        class ResPltt {
            public:
                explicit ResPltt(void *p = 0) : m_pData(static_cast<ResPlttData *>(p)) {}

            private:
                ResPlttData *m_pData; // 0x00
        };

        // Result of evaluating a texture pattern animation for one material:
        // a texture and a palette for each of the eight texture maps. Size
        // 0x44 (bcs::CAnmTexPat::Create allocates an array of them through
        // __construct_new_array with this constructor, which nulls the
        // handles). Member names after nw4r.
        struct TexPatAnmResult {
            TexPatAnmResult() {}

            unsigned char bTexExist;   // 0x00, one bit per texture map
            unsigned char bPlttExist;  // 0x01
            ResTex tex[8];             // 0x04
            ResPltt pltt[8];           // 0x24
        };

        // Handle to a texture pattern animation resource: one pointer, null
        // when the resource was not found. Minimal declaration written for
        // this project; the member and function names are guessed. The check
        // in ref, its file and line are the ones in the DOL's inline copies.
        class ResAnmTexPat {
            public:
                explicit ResAnmTexPat(ResAnmTexPatData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResAnmTexPat"; }

                ResAnmTexPatData &ref() const
                {
                    if (!IsValid()) {
                        nw4r::db::Panic("g3d_resanmtexpat_ac.h", 92, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
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
                    long ofs = ref().toTexPatDataDic;
                    if (ofs != 0) {
                        return ResDic(reinterpret_cast<unsigned char *>(m_pData) + ofs);
                    }
                    return ResDic(0);
                }

                // The animation of the idx-th material.
                const ResAnmTexPatMatData *GetMatAnm(int idx) const
                {
                    return static_cast<const ResAnmTexPatMatData *>(GetMatDic()[idx]);
                }

                // (DOL) Evaluates the animation of material id at frame.
                void GetAnmResult(TexPatAnmResult *pResult, unsigned long id, float frame) const;

            private:
                ResAnmTexPatData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMTEXPAT_HPP_ */
