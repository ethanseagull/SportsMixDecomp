#ifndef MSW_CRADARICON2DVOLLEY_HPP_
#define MSW_CRADARICON2DVOLLEY_HPP_

#include "game/msw/CRadarIcon2D.hpp"

namespace msw {

    // The radar icons of the volley game. Size 0x7C: adds no data to
    // CRadarIcon2D.
    class CRadarIcon2DVolley : public CRadarIcon2D {
        public:
            CRadarIcon2DVolley(long nId);
            virtual ~CRadarIcon2DVolley();
            virtual long SendEvent(long nEvent, void *pParam);

            virtual void Initialize();
            virtual void CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                         const char *pBaseName, const char *pPictureName);
            virtual void CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                        const char *pBaseName, const char *pPictureName);
            virtual void Finalize();
            // The volley radar shows no ball icon (inline: emitted at the end
            // of game/volley/CCockpitVolleyMarkerIcon2D.cpp).
            virtual void DispBallIcon(long, const bcs::FVEC3 &, float, float) {}
    };
}

#endif /* !MSW_CRADARICON2DVOLLEY_HPP_ */
