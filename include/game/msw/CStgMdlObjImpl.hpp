#ifndef MSW_CSTGMDLOBJIMPL_HPP_
#define MSW_CSTGMDLOBJIMPL_HPP_

#include "bcs/BCS_Color.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace msw {

    // A second base of stage models (msw::CFgMdl). A DOL class, size 0x28,
    // vptr at 0x00. Its members are read and written by CFgMdl's accessors
    // (names of those guessed there).
    class CStgMdlObjImpl {
        public:
            virtual ~CStgMdlObjImpl();

        protected:
            // Changes the materials of `mdl` for the stage, with `rColor`.
            // (reads only its arguments)
            void PatchMaterial_(nw4r::g3d::ResMdl mdl, bcs::BCS_Color &rColor);

        protected:
            // (names guessed after CFgMdl's accessors, which are inline:
            // game/msw/CStgGmk_Case.cpp in the DOL emits the setters)
            float m_fAlphaTestNearRange; // 0x04
            float m_fAlphaTestFarRange;  // 0x08
            float m_fFadeDir;            // 0x0C
            float m_fFadeAngle0;         // 0x10
            float m_fFadeAngle1;         // 0x14
            float m_fUnk18;              // 0x18
            float m_fUnk1C;              // 0x1C
            unsigned long m_nFadeEnable; // 0x20

        private:
            unsigned long m_Unk24;  // 0x24
    };
}

#endif /* !MSW_CSTGMDLOBJIMPL_HPP_ */
