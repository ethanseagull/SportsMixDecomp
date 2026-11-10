#ifndef MSW_CRADARICON2DBASKET_HPP_
#define MSW_CRADARICON2DBASKET_HPP_

#include "game/msw/CRadarIcon2D.hpp"

namespace msw {

    // The radar icons of the basket game. Size 0x7C: adds no data to
    // CRadarIcon2D.
    class CRadarIcon2DBasket : public CRadarIcon2D {
        public:
            CRadarIcon2DBasket(long nId);
            virtual ~CRadarIcon2DBasket();
            virtual long SendEvent(long nEvent, void *pParam);

            virtual void Initialize();
            virtual void CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                         const char *pBaseName, const char *pPictureName);
            virtual void CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                        const char *pBaseName, const char *pPictureName);
            virtual void Finalize();
    };
}

#endif /* !MSW_CRADARICON2DBASKET_HPP_ */
