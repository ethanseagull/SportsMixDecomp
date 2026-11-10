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
            // Builds the shadow of pMdl at once. (meanings of the last three
            // arguments not known; msw::CItemBall passes 0x20, 0, 0)
            CMdlShadow(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, long, unsigned long, long);
            virtual ~CMdlShadow();

            // (meanings of the last three arguments not known)
            void Create(nw4r::g3d::ResMdl mdl, long, unsigned long, long);
            // Follows the pose of pMdl.
            void SetLink(const CMdlObj *pMdl);
            // (fn_80060C04, unnamed in the DOL; name and meaning guessed)
            // Sets the byte at 0x1CC; msw::CDropCoin clears it for a coin
            // that rises out of the ground.
            void SetEnable(bool bEnable);

        private:
            unsigned char m_Unk1C8[0x3C]; // 0x1C8
    };
}

#endif /* !BCS_CMDLSHADOW_HPP_ */
