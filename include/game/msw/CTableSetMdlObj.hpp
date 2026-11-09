#ifndef MSW_CTABLESETMDLOBJ_HPP_
#define MSW_CTABLESETMDLOBJ_HPP_

#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"

namespace bcs {
    class CMdlObj;
    struct FVEC3;
}

namespace msw {

    // A model driven by msw::CMenuMdlTableSet. Size 0x2C, vptr at 0x28.
    // msw::SelectMenu::CCourtTableSetObj derives from it.
    class CTableSetMdlObj {
        public:
            CTableSetMdlObj(bcs::CMdlObj *pMdl);

            void Finalize();
            int IsAnmEnd();
            int IsAnmLooped();
            void SetVisible(int bVisible);
            void GetNodePosition(const char *pName, bcs::FVEC3 &rPos);
            void Play(nw4r::g3d::ResAnmClr res, float);
            void Play(nw4r::g3d::ResAnmTexSrt res, float);

        private:
            bcs::CMdlObj *m_pMdl;         // 0x00 (name guessed)
            unsigned char m_Unk04[0x20];  // 0x04
            int m_bOwnMdl;                // 0x24, Finalize releases the model if set (name guessed)

        public:
            // Declared after the data: the vptr is at 0x28.
            virtual ~CTableSetMdlObj();
    };
}

#endif /* !MSW_CTABLESETMDLOBJ_HPP_ */
