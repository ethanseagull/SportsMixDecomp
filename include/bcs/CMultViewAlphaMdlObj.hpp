#ifndef BCS_CMULTVIEWALPHAMDLOBJ_HPP_
#define BCS_CMULTVIEWALPHAMDLOBJ_HPP_

#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    // A model whose transparency is set per view. A DOL class, size 0x1FC.
    // Only what the modules use so far is declared.
    class CMultViewAlphaMdlObj : public CMdlObj {
        public:
            CMultViewAlphaMdlObj();
            virtual ~CMultViewAlphaMdlObj();
            virtual void VFunc118(); // 0x118 (name, parameters and return type not known)

            // (parameter meanings not known; the callers pass 0 and 0)
            void Create(nw4r::g3d::ResMdl mdl, unsigned long, long);

        private:
            unsigned char m_Unk1C8[0x34]; // 0x1C8
    };
}

#endif /* !BCS_CMULTVIEWALPHAMDLOBJ_HPP_ */
