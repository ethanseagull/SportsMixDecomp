#ifndef CCOCKPITRESULTPARTYTEAM_HPP_
#define CCOCKPITRESULTPARTYTEAM_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x80 (CCockpit::CreateObjImpl allocates it). The party game's
    // team (co-op) result screen, layout "prty_result_coop.brlyt". Lives in
    // the DOL (game/msw/CCockpitResultPartyTeam.cpp). CCockpitLayoutBase is
    // at 0, CCockpitFrameOutBase at 0x60 (vptr at 0x68, destructor thunks
    // adjust by 0x1C and 0x60), own members from 0x6C. Names other than
    // SetSubTitleText, SetRankText and SetDelayResultButton are guesses
    // (the DOL has no symbols for them; the virtuals follow the vtable
    // slots of CCockpitLayoutBase).
    class CCockpitResultPartyTeam : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            // CCockpit passes 0 for nType.
            CCockpitResultPartyTeam(long nId, long nType, float fSortZ);
            virtual ~CCockpitResultPartyTeam();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            void SetSubTitleText(const wchar_t *pText);
            void SetRankText(const wchar_t *pText);
            // The team's total; hidden when negative. Clamped to what the
            // text box can show.
            void SetTotalScore(long nScore);
            // The score of player nPlayer (0 to 3); hidden when negative.
            void SetPlayerScore(long nPlayer, long nScore);
            // Puts the face of character nChr on player nPlayer's icon (0
            // to 3) and shows its "1P".."4P" or "COM" label. 0, or a
            // negative error.
            long SetPlayerIcon(long nPlayer, long nChr);
            // Count nCount (0 to 2) of the results; hidden when negative.
            void SetCount(long nCount, long nValue);
            // Plays the "new record" animation, or hides it.
            void SetNewRecord(int bNewRecord);
            void SetDelayResultButton();

        private:
            void StateControl();

        protected:
            long m_nType;          // 0x6C, the constructor argument

        public:
            // 0x70 (name guessed): 1 to 4 picks a "PlayStyle" text for the
            // rank line; anything else shows none.
            long m_nPlayStyle;

        protected:
            // 0x74, 0x78, 0x7C: the largest number the total, count and
            // player score text boxes have room for (9...9).
            long m_nTotalScoreMax;
            long m_nCountMax;
            long m_nPlayerScoreMax;
    };
}

#endif /* !CCOCKPITRESULTPARTYTEAM_HPP_ */
