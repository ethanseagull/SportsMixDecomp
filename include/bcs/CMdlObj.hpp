#ifndef BCS_CMDLOBJ_HPP_
#define BCS_CMDLOBJ_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CScnObj.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

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
            CAnmObj *GetAnmObjChr() const { return m_pAnmObjChr; }
            CAnmObj *GetAnmObjMatClr() const { return m_pAnmObjMatClr; }
            CAnmObj *GetAnmObjTexSrt() const { return m_pAnmObjTexSrt; }
            // (names guessed) The texture pattern and the shape slots.
            CAnmObj *GetAnmObjTexPat() const { return m_pAnmObj3; }
            CAnmObj *GetAnmObjShp() const { return m_pAnmObj5; }

            // (DOL) Access to the copied material nMat of a model. Size
            // about 0x34 (layout.py: fields up to 0x30); only the first word
            // is read from outside, null when there is no such material.
            class CopiedMatAccess {
                public:
                    CopiedMatAccess(CMdlObj *pMdl, unsigned long nMat);

                    bool IsValid() const { return m_pData != 0; }
                    nw4r::g3d::ResTexObj GetResTexObj(int nIndex);
                    // (DOL; added for VolleyF game/volley/CBallTargetMarker.cpp)
                    nw4r::g3d::ResMatTevColor GetResMatTevColor(int nIndex);

                private:
                    void *m_pData;                 // 0x00
                    unsigned char m_Unk04[0x30];   // 0x04
            };

        private:
            nw4r::g3d::ResMdl m_ResMdl;   // 0x118 (name guessed)
            unsigned char m_Unk11C[0x30]; // 0x11C
            CAnmObj *m_pAnmObjChr;        // 0x14C (names of the five guessed)
            CAnmObj *m_pAnmObjMatClr;     // 0x150
            CAnmObj *m_pAnmObj3;          // 0x154
            CAnmObj *m_pAnmObjTexSrt;     // 0x158
            CAnmObj *m_pAnmObj5;          // 0x15C
            unsigned char m_Unk160[0x48]; // 0x160

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
