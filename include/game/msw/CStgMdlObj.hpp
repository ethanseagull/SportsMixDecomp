#ifndef MSW_CSTGMDLOBJ_HPP_
#define MSW_CSTGMDLOBJ_HPP_

#include "bcs/CMdlObj.hpp"
#include "game/msw/CStgMdlObjImpl.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace msw {

    // (name guessed) A stage model built on a plain bcs::CMdlObj, with
    // CStgMdlObjImpl at 0x1C8: the shape msw::CStgMdl::CGenericGimmickCreator
    // gives the model it creates (it patches the materials through the
    // second base at 0x1C8, as CFgMdl::Create does through 0x1FC). The DOL
    // vtable lbl_80475110 is probably this class's; nothing of it is
    // declared yet but the inline Create.
    class CStgMdlObj : public bcs::CMdlObj, public CStgMdlObjImpl {
        public:
            // (guessed, after CFgMdl::Create)
            void Create(nw4r::g3d::ResMdl mdl, unsigned long flag, long n)
            {
                bcs::CMdlObj::Create(mdl, flag, n);
                PatchMaterial_(mdl, m_BaseColor);
            }
    };
}

#endif /* !MSW_CSTGMDLOBJ_HPP_ */
