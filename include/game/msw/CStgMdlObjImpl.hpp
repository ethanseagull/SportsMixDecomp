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
            void PatchMaterial_(nw4r::g3d::ResMdl mdl, bcs::BCS_Color &rColor);

        private:
            float m_aUnk04[7];      // 0x04
            unsigned long m_Unk20;  // 0x20
            unsigned long m_Unk24;  // 0x24
    };
}

#endif /* !MSW_CSTGMDLOBJIMPL_HPP_ */
