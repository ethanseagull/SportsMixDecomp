#ifndef MSW_CRADARICON2DBOSS_HPP_
#define MSW_CRADARICON2DBOSS_HPP_

#include "game/msw/CRadarIcon2D.hpp"

namespace msw {

    // The radar icons of the boss game. Size 0x7C: adds no data to
    // CRadarIcon2D.
    class CRadarIcon2DBoss : public CRadarIcon2D {
        public:
            CRadarIcon2DBoss(long nId);
            virtual ~CRadarIcon2DBoss();
            virtual long SendEvent(long nEvent, void *pParam);

            virtual void Initialize();
            virtual void CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                         const char *pBaseName, const char *pPictureName);
            virtual void CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                        const char *pBaseName, const char *pPictureName);
            virtual void Finalize();
    };
}

#endif /* !MSW_CRADARICON2DBOSS_HPP_ */
