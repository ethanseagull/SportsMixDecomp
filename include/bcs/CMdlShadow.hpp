#ifndef BCS_CMDLSHADOW_HPP_
#define BCS_CMDLSHADOW_HPP_

#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    // A model drawn as the shadow of another. A DOL class, size 0x204.
    // Only what the modules use so far is declared.
    class CMdlShadow : public CMdlObj {
        public:
            CMdlShadow();
            virtual ~CMdlShadow();

            // (meanings of the last three arguments not known)
            void Create(nw4r::g3d::ResMdl mdl, long, unsigned long, long);
            // Follows the pose of pMdl.
            void SetLink(const CMdlObj *pMdl);

        private:
            unsigned char m_Unk1C8[0x3C]; // 0x1C8
    };
}

#endif /* !BCS_CMDLSHADOW_HPP_ */
