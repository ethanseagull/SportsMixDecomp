#ifndef BCS_CMDLOBJ_HPP_
#define BCS_CMDLOBJ_HPP_

#include "bcs/CScnObj.hpp"

namespace bcs {

    class CAnmObj;

    // A model in the scene. A DOL class; its members reach at least 0x1C8
    // (the constructor writes 0x1C4). Only what the modules use so far is
    // declared, so the class must not be created from a module yet.
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
    };
}

#endif /* !BCS_CMDLOBJ_HPP_ */
