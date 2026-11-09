#ifndef MSW_CTABLESETMDLOBJ_HPP_
#define MSW_CTABLESETMDLOBJ_HPP_

#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"

namespace bcs {
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

            // The functions below have no copy of their own in MenuF. They
            // are the repeated shapes inside msw::CMenuMdlTableSet::MainLoop
            // and PlayImpl; names and parameter order are guessed.

            bcs::CAnmMotion *GetAnmMotion() const
            {
                return static_cast<bcs::CAnmMotion *>(m_pMdl->GetAnmObjChr());
            }

            // Like IsAnmEnd, for the animation's virtual function at 0x3C.
            int IsAnmVFunc3C()
            {
                bcs::CAnmObj *pAnmObj = m_pMdl->GetAnmObjChr();
                if (pAnmObj == 0) {
                    return 1;
                }
                return pAnmObj->VFunc3C();
            }

            void StopAnm()
            {
                bcs::CAnmMotion *pAnm = GetAnmMotion();
                if (pAnm != 0) {
                    pAnm->Stop();
                }
            }

            void Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float frame)
            {
                if (res.IsValid()) {
                    bcs::CAnmMotion *pAnm = GetAnmMotion();
                    if (pAnm != 0) {
                        pAnm->Play(res, flag, 1.0f, 0.0f, 0.0f);
                        pAnm->SetRate(rate);
                        pAnm->SetFrame(frame);
                    }
                }
            }

            void Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float blend, float frame)
            {
                if (res.IsValid()) {
                    bcs::CAnmMotion *pAnm = GetAnmMotion();
                    if (pAnm != 0) {
                        pAnm->Play(res, flag, rate, blend, frame);
                        pAnm->SetRate(rate);
                        pAnm->SetFrame(frame);
                    }
                }
            }

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
