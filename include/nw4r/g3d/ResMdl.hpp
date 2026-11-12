#ifndef NW4R_G3D_RESMDL_HPP_
#define NW4R_G3D_RESMDL_HPP_

#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/g3d/ResMat.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "nw4r/g3d/ResVtx.hpp"

namespace nw4r {
    namespace g3d {

        // Only the member read so far.
        struct ResMdlData {
            unsigned char unk00[0x48];
            long name; // 0x48: offset of the model's name from the start
        };
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

                // The check, its file name and line are the ones in the
                // DOL's inline copy (msw::CEffectCluster::FindEffect).
                static const char *GetClassName() { return "ResMdl"; }
                ResMdlData &ref() const
                {
                    if (!IsValid()) {
                        nw4r::db::Panic("g3d_resmdl_ac.h", 120, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }
                // The model's name, null when it has none.
                const char *GetName() const
                {
                    // (as nw4r's ofs_to_ptr: the base pointer is taken
                    // before the offset is tested; the original's branch
                    // layout needs this order)
                    const char *p = reinterpret_cast<const char *>(&ref());
                    long ofs = reinterpret_cast<const ResMdlData *>(p)->name;
                    if (ofs != 0) {
                        return p + ofs;
                    }
                    return 0;
                }

                // (DOL) The material of that name; an invalid handle if
                // there is none.
                ResMat GetResMat(ResName name) const;
                // (DOL; added for VolleyF game/volley/CBallTargetMarker.cpp)
                ResMat GetResMat(int index) const;
                // (DOL; added for game/msw/CStgMdl.cpp)
                ResMat GetResMat(unsigned long index) const;
                // (DOL) The node of that name; an invalid handle if there is
                // none.
                ResNode GetResNode(const char *pName) const;
                // (DOL; added for bcs::CAnmMotion) By index, and the number
                // of nodes.
                ResNode GetResNode(unsigned long idx) const;
                // (DOL; added for game/msw/CStgMdl.cpp)
                ResNode GetResNode(int idx) const;
                unsigned long GetResNodeNumEntries() const;
                // (DOL) The number of materials.
                unsigned long GetResMatNumEntries() const;
                // (DOL) The file the model is in.
                ResFile GetParent();
                // (DOL; added for bcs::CAnmShp) The vertex arrays of that
                // name, invalid handles if there are none, and the number of
                // position arrays.
                ResVtxPos GetResVtxPos(ResName name) const;
                ResVtxNrm GetResVtxNrm(ResName name) const;
                ResVtxClr GetResVtxClr(ResName name) const;
                unsigned long GetResVtxPosNumEntries() const;

            private:
                ResMdlData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESMDL_HPP_ */
