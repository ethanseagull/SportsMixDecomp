#ifndef MSW_CCOMMANHOCKEYCS_HPP_
#define MSW_CCOMMANHOCKEYCS_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CComManHockey.hpp"

namespace msw {

    class CChrCore;

    // (purpose guessed from the names) Helpers of the computer players on
    // the hockey crush skate party stages (85 to 87; HockeyF,
    // game/hockey/CComManHockeyCS.cpp). All static; whether the class
    // derives from CComManHockey is a guess. Positions are on the court,
    // whose centre is the origin.
    class CComManHockeyCS : public CComManHockey {
        public:
            // Where to stand: on the line from the centre through rPos, at
            // rPos's distance plus fAdd (not less than 0).
            static bcs::FVEC3 GetTargetPos_StageCommon(bcs::FVEC3 pos, float fAdd);
            // Stage 86: towards rPos, turned 30 degrees towards rDir when
            // rDir is more than 15 degrees off, at rPos's distance (plus
            // fAdd up to 8 when fAdd > 0) and no closer than fMin + fMin2.
            static bcs::FVEC3 GetTargetPos_Stage86(bcs::FVEC3 dir, bcs::FVEC3 pos, float fMin, float fAdd,
                                                   float fMin2);
            // Stage 87: the same, with the distance kept between fMin and
            // fMax.
            static bcs::FVEC3 GetTargetPos_Stage87(bcs::FVEC3 dir, bcs::FVEC3 pos, float fMin, float fMax);
            // The player number of another active character that pChr can
            // reach (CComManHockey::IsExistTargetCore), or -1.
            static long IsExistTargetInArea(const CChrCore *pChr, float, float, float);
    };
}

#endif /* !MSW_CCOMMANHOCKEYCS_HPP_ */
