#ifndef NW4R_G3D_RESMAT_HPP_
#define NW4R_G3D_RESMAT_HPP_

#include "nw4r/db/Panic.hpp"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"
#include "revolution/gx/GXTexture.h"

struct _GXTexObj;

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

        // Handle to the texture objects of a material: one pointer.
        // Minimal declaration written for this project (MenuF
        // msw::SelectMenu::CCourtChr::GetResTexObj returns one in r3).
        // A material's TEV colour block (added for VolleyF
        // game/volley/CBallTargetMarker.cpp; only what it uses).
        class ResMatTevColor {
            public:
                explicit ResMatTevColor(void *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                // DOL. The getters return false when the material does not
                // set that register (added for nw4r::g3d::ApplyClrAnmResult,
                // which compiles into the DOL's animation file).
                bool GXGetTevColor(_GXTevRegID reg, _GXColor *pColor) const;
                bool GXGetTevKColor(_GXTevKColorID reg, _GXColor *pColor) const;
                void GXSetTevColor(_GXTevRegID reg, _GXColor color);
                void GXSetTevKColor(_GXTevKColorID reg, _GXColor color);
                // Flushes the block from the data cache (bSync: wait for it).
                void DCStore(bool bSync);

            private:
                void *m_pData; // 0x00
        };

        // Contents of a material's lighting channel block (added for
        // nw4r::g3d::ApplyClrAnmResult; only the colours are used). Member
        // names after nw4r.
        struct ResChanData {
            unsigned long flag;   // 0x00
            _GXColor matColor;    // 0x04
            _GXColor ambColor;    // 0x08
            unsigned long paramChan;  // 0x0C
            unsigned long paramAlpha; // 0x10
        };
        struct ResMatChanData {
            ResChanData chan[2];  // 0x00
        };

        // Handle to a material's lighting channel block. Minimal declaration
        // written for this project; the check, its file and line are the
        // ones in the DOL's inline copies (nw4r::g3d::ApplyClrAnmResult).
        class ResMatChan {
            public:
                explicit ResMatChan(void *pData = 0) : m_pData(static_cast<ResMatChanData *>(pData)) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResMatChan"; }

                ResMatChanData &ref() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resmat_ac.h", 465, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

            private:
                ResMatChanData *m_pData; // 0x00
        };

        // Contents of a material's GenMode block (added for DOL
        // game/bcs/CMdlRef.cpp; only the cull mode is used).
        struct ResGenModeData {
            unsigned char nTexGens;  // 0x00
            unsigned char nChans;    // 0x01
            unsigned char nTevs;     // 0x02
            unsigned char nInds;     // 0x03
            _GXCullMode cullMode;    // 0x04
        };

        // Handle to a material's GenMode block. Minimal declaration written
        // for this project; the check, its file and line are the ones in the
        // DOL's inline copy (bcs::CMdlRef's constructor).
        class ResGenMode {
            public:
                explicit ResGenMode(void *pData = 0) : m_pData(static_cast<ResGenModeData *>(pData)) {}

                bool IsValid() const { return m_pData != 0; }

                // GX_CULL_ALL without a block.
                _GXCullMode GXGetCullMode() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resmat_ac.h", 223, "NW4R:Failed assertion IsValid()");
                    }
                    return IsValid() ? m_pData->cullMode : GX_CULL_ALL;
                }
                // (DOL fn_8040C620, unnamed)
                void GXSetCullMode(_GXCullMode mode);

            private:
                ResGenModeData *m_pData; // 0x00
        };

        class ResTexObj {
            public:
                // With NW4R's alignment check (added for MenuF
                // game/menu/CBgDisplay.cpp: its constructor loop over a
                // ResTexObj array has the shape the check gives, and it and
                // CCourtChr.cpp carry the check's strings as
                // @STRING@__ct__Q34nw4r3g3d9ResTexObjFPv objects). Every
                // use so far passes a constant, so the call is folded away
                // and its line is not known; 0 stands in for it.
                explicit ResTexObj(void *pData = 0) : m_pData(pData)
                {
                    if ((unsigned long)pData & 0x3) {
                        db::Panic("g3d_resmat_ac.h", 0, "NW4R:Failed assertion !((u32)p & 0x3)");
                    }
                }

                bool IsValid() const { return m_pData != 0; }

                // DOL. The texture object bound to slot mapId (added for MenuF
                // game/menu/CCharaName.cpp; not const in the mangled name).
                _GXTexObj *GetTexObj(_GXTexMapID mapId);

            private:
                void *m_pData; // 0x00
        };

        // Handles to a material's texture matrix block and its indirect
        // matrix block (added for the DOL's bcs::ApplyTexSrtAnmResult; only
        // passed on). Minimal declarations written for this project.
        class ResTexSrt {
            public:
                explicit ResTexSrt(void *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                void *m_pData; // 0x00
        };

        class ResMatIndMtxAndScale {
            public:
                explicit ResMatIndMtxAndScale(void *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                void *m_pData; // 0x00
        };

        // Handle to the palette objects of a material (added for
        // nw4r::g3d::ApplyTexPatAnmResult). Minimal declaration written for
        // this project.
        class ResTlutObj {
            public:
                explicit ResTlutObj(void *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                // DOL (fn_8040C3A0, unnamed; name after ResTexObj's). The
                // palette object of slot tlut.
                _GXTlutObj *GetTlut(_GXTlut tlut);

            private:
                void *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESMAT_HPP_ */
