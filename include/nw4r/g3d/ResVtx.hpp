#ifndef NW4R_G3D_RESVTX_HPP_
#define NW4R_G3D_RESVTX_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace g3d {

        // Header of a vertex array of a model resource (positions, normals
        // or colours); only the id is read so far.
        struct ResVtxPosData {
            unsigned long size;    // 0x00
            long toResMdlData;     // 0x04
            long toVtxPosArray;    // 0x08
            long name;             // 0x0C
            unsigned long id;      // 0x10, the array's index in the model
        };
        struct ResVtxNrmData;
        struct ResVtxClrData;

        // Handles to the vertex arrays of a model resource: one pointer,
        // null when the array was not found. Minimal declarations written
        // for this project; the check, its file and line are the ones in the
        // DOL's inline copy (bcs::CAnmShp's Bind).
        class ResVtxPos {
            public:
                // The default constructor makes NW4R's alignment check (it
                // folds away; the line is a guess, the strings are in the
                // DOL's animation file .data, one set per class). It keeps
                // the key shape array of nw4r::g3d::ShpAnmResult built by a
                // plain store loop as in the original.
                ResVtxPos() : m_pData(0)
                {
                    ResVtxPosData *p = 0;
                    if ((unsigned long)p & 0x3) {
                        db::Panic("g3d_resvtx_ac.h", 39, "NW4R:Failed assertion !((u32)p & 0x3)");
                    }
                }
                explicit ResVtxPos(ResVtxPosData *pData) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResVtxPos"; }

                ResVtxPosData &ref() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resvtx_ac.h", 39, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                unsigned long GetID() const { return ref().id; }

            private:
                ResVtxPosData *m_pData; // 0x00
        };

        class ResVtxNrm {
            public:
                // The default constructor makes NW4R's alignment check (it
                // folds away; the line is a guess, the strings are in the
                // DOL's animation file .data, one set per class). It keeps
                // the key shape array of nw4r::g3d::ShpAnmResult built by a
                // plain store loop as in the original.
                ResVtxNrm() : m_pData(0)
                {
                    ResVtxNrmData *p = 0;
                    if ((unsigned long)p & 0x3) {
                        db::Panic("g3d_resvtx_ac.h", 39, "NW4R:Failed assertion !((u32)p & 0x3)");
                    }
                }
                explicit ResVtxNrm(ResVtxNrmData *pData) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                ResVtxNrmData *m_pData; // 0x00
        };

        class ResVtxClr {
            public:
                // The default constructor makes NW4R's alignment check (it
                // folds away; the line is a guess, the strings are in the
                // DOL's animation file .data, one set per class). It keeps
                // the key shape array of nw4r::g3d::ShpAnmResult built by a
                // plain store loop as in the original.
                ResVtxClr() : m_pData(0)
                {
                    ResVtxClrData *p = 0;
                    if ((unsigned long)p & 0x3) {
                        db::Panic("g3d_resvtx_ac.h", 39, "NW4R:Failed assertion !((u32)p & 0x3)");
                    }
                }
                explicit ResVtxClr(ResVtxClrData *pData) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                ResVtxClrData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESVTX_HPP_ */
