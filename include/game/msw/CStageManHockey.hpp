#ifndef MSW_CSTAGEMANHOCKEY_HPP_
#define MSW_CSTAGEMANHOCKEY_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // The stage manager of the hockey game (HockeyF), base of every hockey
    // stage. Size 0x218 (the default allocation in CStageFactoryHockey::Create):
    // CStageMan (0x1FC), then members not mapped yet (BasketF's CStageManBasket
    // has the same size).

    class CStageManHockey : public CStageMan {
        public:
            CStageManHockey(bcs::CTask *pParent);
            virtual ~CStageManHockey();

        private:
            unsigned char m_Unk1FC[0x1C]; // size 0x218
    };
}

#endif /* !MSW_CSTAGEMANHOCKEY_HPP_ */
