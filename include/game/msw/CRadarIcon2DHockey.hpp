#ifndef MSW_CRADARICON2DHOCKEY_HPP_
#define MSW_CRADARICON2DHOCKEY_HPP_

#include "game/msw/CRadarIcon2D.hpp"

namespace msw {

    // The radar icons of the hockey game (HockeyF). Size 0x7C: adds no data
    // to CRadarIcon2D. (Its vtable has one more virtual after Finalize,
    // taking (bcs::FVEC3 &, float, float); not declared yet.)
    class CRadarIcon2DHockey : public CRadarIcon2D {
        public:
            CRadarIcon2DHockey(long nId);
            virtual ~CRadarIcon2DHockey();
            virtual long SendEvent(long nEvent, void *pParam);

            virtual void Initialize();
            virtual void CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                         const char *pBaseName, const char *pPictureName);
            virtual void CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                        const char *pBaseName, const char *pPictureName);
            virtual void Finalize();
    };
}

#endif /* !MSW_CRADARICON2DHOCKEY_HPP_ */
