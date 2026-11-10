#ifndef MSW_CSTAGENETVOLLEY_HPP_
#define MSW_CSTAGENETVOLLEY_HPP_

#include "game/msw/CStageNet.hpp"

namespace msw {

    // The net of the volley court (VolleyF). Size 0x6C. Only what
    // CStageManVolley needs is declared.
    class CStageNetVolley : public CStageNet {
        public:
            CStageNetVolley(bcs::CTask *pParent);
            virtual ~CStageNetVolley();
            virtual void Main();

        private:
            long m_Unk68; // 0x68, 1 after construction
    };
}

#endif /* !MSW_CSTAGENETVOLLEY_HPP_ */
