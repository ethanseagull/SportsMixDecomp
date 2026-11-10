#ifndef BCS_CMDLOBJ_HPP_
#define BCS_CMDLOBJ_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CScnObj.hpp"

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
            virtual ~CMdlObj();
            // The overrides, named by the vtable of
            // msw::SelectMenu::CCourtBallMdlObj in MenuF. Return types guessed.
            virtual long GetObjType() const;                                                // 0x0C
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

            // -1 if the model has no node of that name.
            long GetNodeID(const char *pName) const;

            // Returns 1 if the animation was attached.
            int AttachAnmObj(CAnmObj *pAnmObj, AnmObjType type);
            // Returns the animation that was in the slot.
            CAnmObj *DetachAnmObj(AnmObjType type);
            CAnmObj *GetAnmObj(AnmObjType type);

            // (names guessed) The modules read these slots directly.
            CAnmObj *GetAnmObjChr() const { return m_pAnmObjChr; }
            CAnmObj *GetAnmObjMatClr() const { return m_pAnmObjMatClr; }
            CAnmObj *GetAnmObjTexSrt() const { return m_pAnmObjTexSrt; }

        private:
            unsigned char m_Unk118[0x34]; // 0x118
            CAnmObj *m_pAnmObjChr;        // 0x14C (names of the five guessed)
            CAnmObj *m_pAnmObjMatClr;     // 0x150
            CAnmObj *m_pAnmObj3;          // 0x154
            CAnmObj *m_pAnmObjTexSrt;     // 0x158
            CAnmObj *m_pAnmObj5;          // 0x15C
            unsigned char m_Unk160[0x4C]; // 0x160

        protected:
            // (name guessed) White after construction; msw::CFgMdl passes it
            // to CStgMdlObjImpl::PatchMaterial_.
            BCS_Color m_BaseColor;        // 0x1AC

        private:
            unsigned char m_Unk1B0[0x18]; // 0x1B0, an object at 0x1B0 and a word at 0x1C4
    };
}

#endif /* !BCS_CMDLOBJ_HPP_ */
