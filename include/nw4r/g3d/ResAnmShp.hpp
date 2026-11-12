#ifndef NW4R_G3D_RESANMSHP_HPP_
#define NW4R_G3D_RESANMSHP_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/g3d/ResDic.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp" // AnmPolicy
#include "nw4r/g3d/ResVtx.hpp"

namespace nw4r {
    namespace g3d {

        // Header of a shape animation resource (msw::CEventModel::AnimResFrame,
        // bcs::CAnmShp; DOL). Only the members read so far.
        struct ResAnmShpData {
            unsigned char unk00[0x10];
            long toShpDataDic;       // 0x10, dictionary of the animated shapes
            long toVtxNameArray;     // 0x14, offsets of the vertex array names from the array
            unsigned char unk18[0x0C];
            unsigned short numFrame; // 0x24 (name guessed)
            unsigned short numVtxName; // 0x26
            AnmPolicy policy;        // 0x28
        };

        // The animation of one shape; only the name is read.
        struct ResAnmShpAnmData {
            unsigned long flags; // 0x00
            long name;           // 0x04, offset of the shape's name from this entry

            ResName GetName() const
            {
                return ResName(reinterpret_cast<const ResNameData *>(reinterpret_cast<const char *>(this) + name - 4));
            }
        };

        // The vertex arrays a shape animation blends, looked up by name in
        // the model (bcs::CAnmShp). Size 0x0C.
        struct ShpAnmVtxSet {
            ShpAnmVtxSet() {}

            ResVtxPos resVtxPos; // 0x00
            ResVtxNrm resVtxNrm; // 0x04
            ResVtxClr resVtxClr; // 0x08
        };

        // Result of evaluating a shape animation for one shape: the base
        // shape and up to 32 weighted key shapes. Size 0x218
        // (bcs::CAnmShp::Create allocates them through
        // __construct_new_array with this constructor, which nulls the
        // handles). Member names after nw4r.
        struct ShpAnmResult {
            struct KeyShape {
                ResVtxPos resVtxPos; // 0x00
                ResVtxNrm resVtxNrm; // 0x04
                ResVtxClr resVtxClr; // 0x08
                float weight;        // 0x0C
            };

            ShpAnmResult() {}

            unsigned long flags;           // 0x00
            unsigned long numKeyShape;     // 0x04
            ResVtxPos baseShapeVtxPos;     // 0x08
            ResVtxNrm baseShapeVtxNrm;     // 0x0C
            ResVtxClr baseShapeVtxClr;     // 0x10
            float baseShapeWeight;         // 0x14
            KeyShape keyShape[32];         // 0x18
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
                int GetNumVtxName() const { return ref().numVtxName; }
                AnmPolicy GetAnmPolicy() const { return ref().policy; }

                // The dictionary of the animated shapes (an offset turned
                // into a handle, as nw4r's ofs_to_obj).
                ResDic GetShpDic() const
                {
                    long ofs = ref().toShpDataDic;
                    if (ofs != 0) {
                        return ResDic(reinterpret_cast<unsigned char *>(m_pData) + ofs);
                    }
                    return ResDic(0);
                }

                // Number of animated shapes.
                unsigned long GetNumShpAnm() const { return GetShpDic().GetNumData(); }

                // The animation of the idx-th shape.
                const ResAnmShpAnmData *GetShpAnm(int idx) const
                {
                    return static_cast<const ResAnmShpAnmData *>(GetShpDic()[idx]);
                }

                // The offsets of the vertex array names, each from the start
                // of the array.
                const long *GetVtxNameArray() const
                {
                    long ofs = ref().toVtxNameArray;
                    if (ofs != 0) {
                        return reinterpret_cast<const long *>(reinterpret_cast<unsigned char *>(m_pData) + ofs);
                    }
                    return 0;
                }

                // (DOL) Evaluates the animation of shape id at frame, with
                // the vertex arrays the model has for the animation's names.
                void GetAnmResult(ShpAnmResult *pResult, unsigned long id, float frame, const ShpAnmVtxSet *pVtxSet) const;

            private:
                ResAnmShpData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMSHP_HPP_ */
