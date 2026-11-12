#ifndef CEVENTMDLOBJ_HPP_
#define CEVENTMDLOBJ_HPP_

#include "bcs/CMdlObj.hpp"

namespace msw {

    // The model CEventModel makes from its model file (0x1CC bytes, vtable
    // lbl_8046F0F0; nothing names it: class and member names guessed). It
    // can be shown through the camera of a view of its own.
    class CEventMdlObj : public bcs::CMdlObj {
        public:
            CEventMdlObj();
            virtual ~CEventMdlObj();
            virtual void ObjProc(unsigned long nTask, unsigned long nArg, void *pArg);

            // (name guessed) Inline in msw::CEvDemoMdlObj::SetCameraView:
            // shows the model through the camera of view nView.
            void SetView(long nView)
            {
                m_bOwnView = 1;
                m_nView = nView;
            }

        private:
            short m_nView;    // 0x1C8: the view whose camera shows the model
            short m_bOwnView; // 0x1CA: m_nView is used
    }; // size 0x1CC
}

#endif /* !CEVENTMDLOBJ_HPP_ */
