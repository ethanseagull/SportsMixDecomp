#ifndef BCS_CMDLOBJ_HPP_
#define BCS_CMDLOBJ_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CScnObj.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

namespace nw4r {
    namespace g3d {
        struct ChrAnmResult;

        // (added for DOL game/bcs/CMdlObj/CopiedMatAccess.cpp; they belong
        // in nw4r/g3d/ResMat.hpp and a ResTev.hpp, move them there when
        // those are next edited) The size of one material's copy of each
        // block, as bcs::CMdlObj keeps them; contents not used.
        struct ResTexObjData { unsigned char data[0x104]; };
        struct ResTlutObjData { unsigned char data[0x64]; };
        struct ResTexSrtData { unsigned char data[0x248]; };
        struct ResMatMiscData { unsigned char data[0xC]; };
        struct ResPixDL { unsigned char data[0x20]; };
        struct ResTevColorDL { unsigned char data[0x80]; };
        struct ResIndMtxAndScaleDL { unsigned char data[0x40]; };
        struct ResTexCoordGenDL { unsigned char data[0xA0]; };
        struct ResTevData { unsigned char data[0x200]; };

        // Handles to a material's pixel, texture coordinate and TEV display
        // lists. Minimal declarations written for this project; the
        // alignment checks, files and lines are the ones in the DOL's inline
        // copies (bcs::CMdlObj::CopiedMatAccess's constructors).
        class ResMatPix {
            public:
                explicit ResMatPix(void *pData = 0) : m_pData(pData)
                {
                    if ((unsigned long)pData & 0x1F) {
                        db::Panic("g3d_resmat_ac.h", 340, "NW4R:Failed assertion !((u32)p & 0x1f)");
                    }
                }

                bool IsValid() const { return m_pData != 0; }

            private:
                void *m_pData; // 0x00
        };

        class ResMatTexCoordGen {
            public:
                explicit ResMatTexCoordGen(void *pData = 0) : m_pData(pData)
                {
                    if ((unsigned long)pData & 0x1F) {
                        db::Panic("g3d_resmat_ac.h", 513, "NW4R:Failed assertion !((u32)p & 0x1f)");
                    }
                }

                bool IsValid() const { return m_pData != 0; }

            private:
                void *m_pData; // 0x00
        };

        class ResTev {
            public:
                explicit ResTev(void *pData = 0) : m_pData(pData)
                {
                    if ((unsigned long)pData & 0x1F) {
                        db::Panic("g3d_restev_ac.h", 37, "NW4R:Failed assertion !((u32)p & 0x1f)");
                    }
                }

                bool IsValid() const { return m_pData != 0; }

            private:
                void *m_pData; // 0x00
        };
    }
}

namespace bcs {

    class CAnmObj;

    // A model in the scene. A DOL class, size 0x1C8 (the constructor writes
    // 0x1C4; msw::SelectMenu::CCourtBallMdlObj's and bcs::CMultViewAlphaMdlObj's
    // members start at 0x1C8). Only what the modules use so far is declared.
    class CMdlObj : public CScnObj {
        public:
            // Selects one of the five animation slots. The values are the
            // ones DetachAnmObj switches on (1 is not handled). The names
            // are guessed from the class created for the slot.
            enum AnmObjType {
                ANMOBJTYPE_CHR = 0,    // 0x14C
                ANMOBJTYPE_MATCLR = 2, // 0x150, bcs::CAnmMatClr
                ANMOBJTYPE_3 = 3,      // 0x154
                ANMOBJTYPE_TEXSRT = 4, // 0x158, bcs::CAnmTexSrt
                ANMOBJTYPE_5 = 5       // 0x15C
            };

            CMdlObj();
            // (DOL) The model built from mdl at once, as Create does.
            CMdlObj(nw4r::g3d::ResMdl mdl, unsigned long, long);
            virtual ~CMdlObj();
            // (added for game/msw/CCockpitChrIcon2D.cpp; return type and the
            // meaning of the second argument guessed) A scene matrix of the
            // model with its position.
            void GetScnMtxPos(FMTX34 *pMtx, unsigned long nUnk, CScnObj::ScnObjMtxType type) const;
            // The overrides, named by the vtable of
            // msw::SelectMenu::CCourtBallMdlObj in MenuF. Return types guessed.
            // Inline: 4 for a model (weak copies in VolleyF
            // game/volley/CStageNetVolley.cpp, MenuF game/menu/CBgMdl.cpp).
            virtual long GetObjType() const { return 4; }                                   // 0x0C
            virtual void ObjProc(unsigned long, unsigned long, void *);                     // 0x1C
            virtual void CalcBoundingVolume();                                              // 0x2C
            virtual void GetNodeLocalMtx(unsigned long, FMTX34 &) const;                    // 0x34
            virtual const FMTX34 *GetNodeLocalMtxPtr(unsigned long) const;                  // 0x38
            virtual void GetNodeWorldMtx(unsigned long, FMTX34 &) const;                    // 0x3C
            virtual const FMTX34 *GetNodeWorldMtxPtr(unsigned long) const;                  // 0x40
            virtual void SetNodeLocalMtx(unsigned long, const FMTX34 &);                    // 0x44
            virtual void SetNodeWorldMtx(unsigned long, const FMTX34 &);                    // 0x48
            virtual void SetAlphaMode(AlphaMode);                                           // 0x54
            virtual void SetExBlendMode(int);                                               // 0x64
            virtual void ExEffectDraw(int, unsigned long, void *);                          // 0x6C
            virtual void DrawJointObjOpa(void *);                                           // 0x70
            virtual void DrawJointObjXluExOpa(void *);                                      // 0x74
            virtual void DrawJointObjXluExXlu(void *);                                      // 0x78
            virtual void SetMatTevColor(_GXTevRegID, BCS_Color);                            // 0x7C
            virtual void SetMatTevColor(const char *, _GXTevRegID, BCS_Color);              // 0x80
            virtual void SetMatTevColorRGB(_GXTevRegID, BCS_Color);                         // 0x84
            virtual void SetMatTevColorRGB(const char *, _GXTevRegID, BCS_Color);           // 0x88
            virtual void SetMatTevColorA(_GXTevRegID, unsigned char);                       // 0x8C
            virtual void SetMatTevColorA(const char *, _GXTevRegID, unsigned char);         // 0x90
            virtual void RestoreMatTevColor(_GXTevRegID);                                   // 0x94
            virtual void RestoreMatTevColor(const char *, _GXTevRegID);                     // 0x98
            virtual void SetMatTevKColor(_GXTevKColorID, BCS_Color);                        // 0x9C
            virtual void SetMatTevKColor(const char *, _GXTevKColorID, BCS_Color);          // 0xA0
            virtual void SetMatTevKColorRGB(_GXTevKColorID, BCS_Color);                     // 0xA4
            virtual void SetMatTevKColorRGB(const char *, _GXTevKColorID, BCS_Color);       // 0xA8
            virtual void SetMatTevKColorA(_GXTevKColorID, unsigned char);                   // 0xAC
            virtual void SetMatTevKColorA(const char *, _GXTevKColorID, unsigned char);     // 0xB0
            virtual void RestoreMatTevKColor(_GXTevKColorID);                               // 0xB4
            virtual void RestoreMatTevKColor(const char *, _GXTevKColorID);                 // 0xB8
            virtual void SetMatPixDstAlpha(unsigned char, unsigned char);                   // 0xBC
            virtual void SetMatPixDstAlpha(const char *, unsigned char, unsigned char);     // 0xC0
            virtual void SetMatFogIdx(long);                                                // 0xC4
            virtual void SetMatFogIdx(const char *, long);                                  // 0xC8
            virtual void SetMatChanMatColor(_GXChannelID, BCS_Color);                       // 0xCC
            virtual void SetMatChanMatColor(const char *, _GXChannelID, BCS_Color);         // 0xD0
            virtual void SetMatChanMatColorRGB(_GXChannelID, BCS_Color);                    // 0xD4
            virtual void SetMatChanMatColorRGB(const char *, _GXChannelID, BCS_Color);      // 0xD8
            virtual void SetMatChanMatColorA(_GXChannelID, unsigned char);                  // 0xDC
            virtual void SetMatChanMatColorA(const char *, _GXChannelID, unsigned char);    // 0xE0
            virtual void RestoreMatChanMatColor(_GXChannelID);                              // 0xE4
            virtual void RestoreMatChanMatColor(const char *, _GXChannelID);                // 0xE8
            virtual void SetMatChanAmbColor(_GXChannelID, BCS_Color);                       // 0xEC
            virtual void SetMatChanAmbColor(const char *, _GXChannelID, BCS_Color);         // 0xF0
            virtual void SetMatChanAmbColorRGB(_GXChannelID, BCS_Color);                    // 0xF4
            virtual void SetMatChanAmbColorRGB(const char *, _GXChannelID, BCS_Color);      // 0xF8
            virtual void SetMatChanAmbColorA(_GXChannelID, unsigned char);                  // 0xFC
            virtual void SetMatChanAmbColorA(const char *, _GXChannelID, unsigned char);    // 0x100
            virtual void RestoreMatChanAmbColor(_GXChannelID);                              // 0x104
            virtual void RestoreMatChanAmbColor(const char *, _GXChannelID);                // 0x108
            virtual void UpdateFrame(float);                                                // 0x10C
            virtual void SetLightSelector(long);                                            // 0x110
            virtual void ResetLightSelector();                                              // 0x114

            // Builds the model from mdl. (meaning of the last two arguments
            // not known; msw::CTournamentChip passes 0 and 1) Returns 0 on
            // failure (bcs::CMdlShadow::Create tests it with cmpwi).
            int Create(nw4r::g3d::ResMdl mdl, unsigned long, long);
            // Sets the depth test of every material. (parameter names
            // guessed from GXSetZMode)
            void MatSetZModeAll(unsigned char bCompare, _GXCompare func, unsigned char bUpdate);
            // (fn_800575C4, name guessed after its neighbours
            // MatSetZModeAll and MatSetDstAlphaAll) Sets the blend mode of
            // every material (ResMatPix::GXSetBlendMode).
            void MatSetBlendModeAll(_GXBlendMode mode, _GXBlendFactor src, _GXBlendFactor dst, _GXLogicOp op);
            // (fn_8005773C, name guessed) Calls the virtual SetMatFogIdx(long);
            // msw::CChrMarker::SetFogIdx goes through it.
            void SetFogIdx(long nIdx);
            // The number of nodes of the model's resource.
            unsigned long GetResNodeNum() const;
            // (DOL; return type guessed) Recomputes the world matrices of
            // every node; msw::CItemBall passes null.
            void CalcWorldMatrixAll(const FMTX34 *pMtx);

            // (DOL) The resource node of that name; an invalid handle if
            // there is none.
            nw4r::g3d::ResNode GetResNode(const char *pName) const;

            // (name guessed) Inline in MenuF's msw::CChrMenuObj::SetSilhouette:
            // the two colours are passed by value and stored byte by byte.
            // The base colour comes first: the silhouette colour's copy is
            // made first, at the lower stack slot.
            void SetSilhouetteColor(BCS_Color base, BCS_Color silhouette)
            {
                m_SilhouetteColor = silhouette;
                m_BaseColor = base;
            }
            // (names guessed) The two one at a time, as MenuF's
            // msw::SelectMenu::CCupChr::CreateChrMenuObj sets them: each
            // colour is copied and stored before the next is read.
            void SetSilhouetteColor(BCS_Color silhouette) { m_SilhouetteColor = silhouette; }
            void SetBaseColor(BCS_Color base) { m_BaseColor = base; }
            // (names guessed) msw::CChrOptTask copies both from the model it
            // follows (DOL game/msw/CChrOptMdlObj.cpp).
            const BCS_Color &GetSilhouetteColor() const { return m_SilhouetteColor; }
            const BCS_Color &GetBaseColor() const { return m_BaseColor; }

            // -1 if the model has no node of that name.
            long GetNodeID(const char *pName) const;

            // Returns 1 if the animation was attached.
            int AttachAnmObj(CAnmObj *pAnmObj, AnmObjType type);
            // Returns the animation that was in the slot.
            CAnmObj *DetachAnmObj(AnmObjType type);
            // (DOL fn_8005A790, unnamed) Detaches pAnmObj from whichever slot
            // holds it; returns 0 when no slot does. (return meaning guessed)
            int DetachAnmObj(CAnmObj *pAnmObj);
            // (DOL fn_80058E40, fn_800590F0, fn_800593A8, unnamed; names
            // guessed) What ObjProc does for tasks 4 (view matrix), 6 and 7
            // (the opaque and the translucent draw).
            void ProcCalcView(unsigned long nArg, void *pArg);
            void ProcDrawOpa(unsigned long nArg, void *pArg);
            void ProcDrawXlu(unsigned long nArg, void *pArg);
            CAnmObj *GetAnmObj(AnmObjType type);
            // (DOL; meaning guessed from the name and BasketF's
            // CChrCoreBasketYoshi, which negates the motion id when it is
            // set) Not zero while animation slot nSlot plays mirrored.
            int IsAnmFlipPlay(unsigned long nSlot, long) const;

            // (names guessed) The modules read these slots directly.
            // (name guessed) Inline in bcs::CAnmTexSrt::Create.
            nw4r::g3d::ResMdl GetResMdl() const { return m_ResMdl; }
            // (names guessed; added for DOL game/bcs/CMdlRef.cpp) The node
            // matrices and their attribute words.
            nw4r::math::MTX34 *GetNodeMtxArray() const { return m_pNodeMtx; }
            unsigned long *GetNodeMtxAttribArray() const { return m_pNodeMtxAttrib; }
            CAnmObj *GetAnmObjChr() const { return m_pAnmObjChr; }
            // (names guessed; added for game/msw/CStgMdl.cpp) The skeletal
            // animation's result per node (null without one), and
            // fn_80056DA8, which CStgMdl::ResetAnm calls.
            nw4r::g3d::ChrAnmResult *GetChrAnmResult() const { return m_pChrAnmResult; }
            void ResetAnm();
            CAnmObj *GetAnmObjMatClr() const { return m_pAnmObjMatClr; }
            CAnmObj *GetAnmObjTexSrt() const { return m_pAnmObjTexSrt; }
            // (names guessed) The texture pattern and the shape slots.
            CAnmObj *GetAnmObjTexPat() const { return m_pAnmObj3; }
            CAnmObj *GetAnmObjShp() const { return m_pAnmObj5; }

            // (DOL game/bcs/CMdlObj/CopiedMatAccess.cpp) Access to the copy
            // of material nMat that a model keeps of each block of its
            // material resource, like nw4r's ScnMdl::CopiedMatAccess. Size
            // 0x34. Every handle is null and m_pMdl is null when the model
            // has no such material. Each getter returns its handle (null
            // without a model or without that copy); bMarkDirty sets that
            // block's bit in the material's dirty word so the model sends
            // the copy to the GPU again.
            class CopiedMatAccess {
                public:
                    CopiedMatAccess(CMdlObj *pMdl, unsigned long nMat);
                    // (DOL fn_8005AFD4, named here) The material of that name.
                    CopiedMatAccess(const char *pMatName, CMdlObj *pMdl);

                    bool IsValid() const { return m_pMdl != 0; }
                    nw4r::g3d::ResTexObj GetResTexObj(int bMarkDirty);    // bit 0x01
                    // (DOL fn_8005B548, unnamed; name guessed, added for
                    // game/msw/CTrophyObj.cpp) The copy of the texture
                    // objects when the model has one, else the resource's
                    // own block; null without a model.
                    nw4r::g3d::ResTexObj GetResTexObjEx();
                    // (DOL fn_8005B3E0, named here after nw4r)
                    nw4r::g3d::ResTexSrt GetResTexSrt(bool bMarkDirty);   // bit 0x04
                    // (DOL fn_8005B428, named here after nw4r)
                    nw4r::g3d::ResGenMode GetResGenMode(bool bMarkDirty); // bit 0x10
                    // (DOL; added for game/msw/CStgMdl.cpp, name after nw4r)
                    nw4r::g3d::ResMatMisc GetResMatMisc(bool bMarkDirty); // bit 0x20
                    // (DOL fn_8005B4B8, named here after nw4r)
                    nw4r::g3d::ResMatPix GetResMatPix(bool bMarkDirty);   // bit 0x80
                    // (DOL; added for VolleyF game/volley/CBallTargetMarker.cpp)
                    nw4r::g3d::ResMatTevColor GetResMatTevColor(int bMarkDirty);
                    // (added for DOL game/bcs/CMdlRef.cpp) GetResGenMode(false):
                    // the original's CMdlRef, in the same file, inlines it.
                    nw4r::g3d::ResGenMode GetResGenMode()
                    {
                        if (m_pMdl != 0 && m_GenMode.IsValid()) {
                            return m_GenMode;
                        }
                        return nw4r::g3d::ResGenMode(0);
                    }

                private:
                    CMdlObj *m_pMdl;                                 // 0x00
                    unsigned long m_nMat;                            // 0x04
                    nw4r::g3d::ResTexObj m_TexObj;                   // 0x08
                    nw4r::g3d::ResTlutObj m_TlutObj;                 // 0x0C
                    nw4r::g3d::ResTexSrt m_TexSrt;                   // 0x10
                    nw4r::g3d::ResMatChan m_Chan;                    // 0x14
                    nw4r::g3d::ResGenMode m_GenMode;                 // 0x18
                    nw4r::g3d::ResMatMisc m_MatMisc;                 // 0x1C
                    nw4r::g3d::ResMatPix m_Pix;                      // 0x20
                    nw4r::g3d::ResMatTevColor m_TevColor;            // 0x24
                    nw4r::g3d::ResMatIndMtxAndScale m_IndMtxAndScale; // 0x28
                    nw4r::g3d::ResMatTexCoordGen m_TexCoordGen;      // 0x2C
                    nw4r::g3d::ResTev m_Tev;                         // 0x30
            };

        private:
            nw4r::g3d::ResMdl m_ResMdl;   // 0x118 (name guessed)

        protected:
            // (names guessed; added for DOL game/bcs/CMdlRef.cpp, whose
            // CopyNodeMtx copies them from the linked model) One matrix and
            // one attribute word per node, and the envelope byte code passed
            // to nw4r::g3d::CalcSkinning (null without one).
            nw4r::math::MTX34 *m_pNodeMtx;      // 0x11C
            unsigned long *m_pNodeMtxAttrib;    // 0x120
            unsigned char m_Unk124[0xC];        // 0x124
            // (name guessed; added for game/msw/CStgMdl.cpp) One result of
            // the skeletal animation per node, or null.
            nw4r::g3d::ChrAnmResult *m_pChrAnmResult; // 0x130
            unsigned char m_Unk134[0xC];        // 0x134
            const unsigned char *m_pEnvByteCode; // 0x140
            unsigned char m_Unk144[0x8];        // 0x144

        private:
            CAnmObj *m_pAnmObjChr;        // 0x14C (names of the five guessed)
            CAnmObj *m_pAnmObjMatClr;     // 0x150
            CAnmObj *m_pAnmObj3;          // 0x154
            CAnmObj *m_pAnmObjTexSrt;     // 0x158
            CAnmObj *m_pAnmObj5;          // 0x15C
            // (names after nw4r's ScnMdl; added for DOL
            // game/bcs/CMdlObj/CopiedMatAccess.cpp) One dirty word per
            // material, then the model's copies of each material block, one
            // element per material (null when the block is not copied).
            unsigned long *m_pMatBufDirtyFlag;                      // 0x160
            unsigned char m_Unk164[0xC];                            // 0x164
            nw4r::g3d::ResTexObjData *m_pTexObjBuf;                 // 0x170
            nw4r::g3d::ResTlutObjData *m_pTlutObjBuf;               // 0x174
            nw4r::g3d::ResTexSrtData *m_pTexSrtBuf;                 // 0x178
            nw4r::g3d::ResMatChanData *m_pChanBuf;                  // 0x17C
            nw4r::g3d::ResGenModeData *m_pGenModeBuf;               // 0x180
            nw4r::g3d::ResMatMiscData *m_pMatMiscBuf;               // 0x184
            nw4r::g3d::ResPixDL *m_pPixBuf;                         // 0x188
            nw4r::g3d::ResTevColorDL *m_pTevColorBuf;               // 0x18C
            nw4r::g3d::ResIndMtxAndScaleDL *m_pIndMtxAndScaleBuf;   // 0x190
            nw4r::g3d::ResTexCoordGenDL *m_pTexCoordGenBuf;         // 0x194
            nw4r::g3d::ResTevData *m_pTevBuf;                       // 0x198
            unsigned char m_Unk19C[0xC];                            // 0x19C

        protected:
            // (name guessed) White after construction, like m_BaseColor;
            // msw::CChrMenuObj::SetSilhouette (MenuF) sets the two together.
            BCS_Color m_SilhouetteColor;  // 0x1A8
            // (name guessed) White after construction; msw::CFgMdl passes it
            // to CStgMdlObjImpl::PatchMaterial_.
            BCS_Color m_BaseColor;        // 0x1AC

        private:
            unsigned char m_Unk1B0[0x18]; // 0x1B0, an object at 0x1B0 and a word at 0x1C4
    };
}

#endif /* !BCS_CMDLOBJ_HPP_ */
