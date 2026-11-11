#ifndef MSW_CSTAGENETHOCKEY_HPP_
#define MSW_CSTAGENETHOCKEY_HPP_

#include "game/msw/CStageNet.hpp"

namespace msw {

    // The net of the hockey court (VolleyF). Size 0x6C. Only what
    // CStageManHockey needs is declared.
    class CStageNetHockey : public CStageNet {
        public:
            CStageNetHockey(bcs::CTask *pParent);
            virtual ~CStageNetHockey();
            virtual void Main();

        private:
            long m_Unk68; // 0x68, 1 after construction
    };
}

#endif /* !MSW_CSTAGENETHOCKEY_HPP_ */
