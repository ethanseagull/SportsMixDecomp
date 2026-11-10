#ifndef MSW_CSTAGEHOCKEY01_HPP_
#define MSW_CSTAGEHOCKEY01_HPP_

#include "game/msw/CStageManHockey.hpp"

namespace msw {

    // Hockey stage 01 (HockeyF), made by CStageFactoryHockey. Size 0x224.

    class CStageHockey01 : public CStageManHockey {
        public:
            CStageHockey01(bcs::CTask *pParent);
            virtual ~CStageHockey01();

        private:
            unsigned char m_Unk218[0xC]; // size 0x224
    };
}

#endif /* !MSW_CSTAGEHOCKEY01_HPP_ */
