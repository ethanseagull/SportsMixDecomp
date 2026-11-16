#ifndef CCOCKPITRESULTPARTYVS_HPP_
#define CCOCKPITRESULTPARTYVS_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"
#include "nw4r/math/types.h"

namespace msw {

    // Size 0xAC (the last member ends there; no allocation site seen). The
    // party game's versus result screen, layout "prty_result_vs.brlyt".
    // Lives in the DOL (game/msw/CCockpitResultPartyVS.cpp). Built like
    // CCockpitResultPartyTeam: CCockpitLayoutBase at 0, CCockpitFrameOutBase
    // at 0x60 (vptr at 0x68), own members from 0x6C. Names other than
    // SetSubTitleText and SetDelayResultButton are guesses.
    class CCockpitResultPartyVS : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CCockpitResultPartyVS(long nId, long nType, float fSortZ);
            virtual ~CCockpitResultPartyVS();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            void SetSubTitleText(const wchar_t *pText);
            // The score of player nPlayer (0 to 3); hidden when negative.
            // Clamped to what the text box can show.
            void SetPlayerScore(long nPlayer, long nScore);
            // The time of player nPlayer (0 to 3) in frames, as minutes,
            // seconds and hundredths; hidden when negative.
            void SetPlayerTime(long nPlayer, float fFrame);
            // Puts the face of character nChr on player nPlayer's icon (0
            // to 3) and shows its "1P".."4P" or "COM" label. 0, or a
            // negative error.
            long SetPlayerIcon(long nPlayer, long nChr);
            // Shows crown nCrown (0 to 3) glowing, or hides it.
            void SetCrown(long nCrown, int bDisp);
            // Puts a copy of the place mark nRank (0 to 3, 4: first with
            // the gold mark) on player nPlayer's line and plays its bar
            // animation.
            void SetPlayerRank(long nPlayer, long nRank);
            // Moves player nPlayer's line to where line nLine was at Set.
            long SetPlayerLinePosition(long nPlayer, long nLine);
            void SetDelayResultButton();

        private:
            void StateControl();

        protected:
            long m_nType;          // 0x6C, the constructor argument
            long m_Unk70;          // 0x70

            // 0x74, 0x78: the largest number the score and the minutes
            // text boxes have room for (9...9).
            long m_nScoreMax;
            long m_nMinuteMax;

            // 0x7C: the positions of the four player lines at Set.
            nw4r::math::VEC3 m_LinePos[4];
    };
}

#endif /* !CCOCKPITRESULTPARTYVS_HPP_ */
