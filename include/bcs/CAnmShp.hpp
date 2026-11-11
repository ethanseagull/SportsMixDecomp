#ifndef BCS_CANMSHP_HPP_
#define BCS_CANMSHP_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmShp.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    class CMdlObj;

    // Shape animation (DOL class, 0x3C bytes, unnamed in the DOL: class name
    // guessed after the other animation kinds). Constructor fn_8000E870
    // (vtable lbl_80463D88, with the CAnmMatTable part at 0x44 like
    // bcs::CAnmObjMat's derived classes), Create fn_8000EA08. Only what
    // msw::CEventModel calls; the virtuals past 0x40 are taken to be laid
    // out as in bcs::CAnmTexSrt.
    class CAnmShp : public CAnmObjMat {
        public:
            CAnmShp();

            virtual const void *GetResult(void *pResult, unsigned long idx); // 0x50
            virtual int Bind(nw4r::g3d::ResMdl mdl);                          // 0x54
            virtual int ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmShp res); // 0x58
            virtual void Release();                                           // 0x5C

            // Returns 1 on success (arguments as for bcs::CAnmMatClr).
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmShp res, int, long);

        private:
            unsigned char m_Unk28[0x14]; // 0x28
    }; // size 0x3C
}

#endif /* !BCS_CANMSHP_HPP_ */
