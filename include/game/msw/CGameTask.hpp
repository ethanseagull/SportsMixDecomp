#ifndef MSW_CGAMETASK_HPP_
#define MSW_CGAMETASK_HPP_

#include "bcs/CUnivTask.hpp"

namespace msw {

    // A task of the game modes. Nothing of it is in the DOL: every module
    // has a weak copy of its destructor (__dt__Q23msw9CGameTaskFv, which
    // tests `this` twice before calling bcs::CTask's, one test per inline
    // level) and of its vtable (__vt__Q23msw9CGameTask: the destructor,
    // CTask::Main, CTask::Disp, CUnivTask::GetFrameRateF), so it adds no
    // virtual function to bcs::CUnivTask and both its constructor and its
    // destructor are inline. bcs::CUnivTaskEx has the same shape and may be
    // this class under a guessed name.
    class CGameTask : public bcs::CUnivTask {
        public:
            CGameTask(bcs::CTask *pParent, short priority) : CUnivTask(pParent, priority) {}
            virtual ~CGameTask() {}
    };
}

#endif /* !MSW_CGAMETASK_HPP_ */
