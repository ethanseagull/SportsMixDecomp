#ifndef BCS_CMDLOBJ_HPP_
#define BCS_CMDLOBJ_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CScnObj.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    class CAnmObj;

    // A model in the scene. A DOL class, size 0x1C8 (the constructor writes
    // 0x1C4; bcs::CMultViewAlphaMdlObj's members start at 0x1C8). Only what
    // the modules use so far is declared.
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
            virtual void UpdateFrame(float);           // 0x10C
            virtual void SetLightSelector(long);       // 0x110 (return type guessed)
            virtual void ResetLightSelector();         // 0x114 (return type guessed)

            // Builds the model from mdl. (meaning of the last two arguments
            // not known; msw::CTournamentChip passes 0 and 1)
            void Create(nw4r::g3d::ResMdl mdl, unsigned long, long);
            // Sets the depth test of every material. (parameter names
            // guessed from GXSetZMode)
            void MatSetZModeAll(unsigned char bCompare, _GXCompare func, unsigned char bUpdate);
            // The number of nodes of the model's resource.
            unsigned long GetResNodeNum() const;

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
