#ifndef CCOCKPITBASKETHARAPEKO_HPP_
#define CCOCKPITBASKETHARAPEKO_HPP_

#include "game/msw/CCockpitBasketLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CTimeCount.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // Size 0xB4. The cockpit of the basket "harapeko" (hungry) game (layout
    // BA_harapeko): each player's score with a face icon and a "1P"/"COM"
    // label, the score that pops up over a player who scores, a crown over
    // the leader, the time, and the fruit ball scores
    // (game/basket/CCockpitBasketHarapeko.cpp). CCockpitBasketLayoutBase is
    // at 0, own members from 0x60. Member names are guesses.
    class CCockpitBasketHarapeko : public CCockpitBasketLayoutBase {
        public:
            // fSortZ is not used: the sort depth is set to 0.
            CCockpitBasketHarapeko(long nId, float fSortZ);
            virtual ~CCockpitBasketHarapeko();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // The score panes slide in and out with the cockpit's score part.
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            // Shows nScore on player nPlayer's counter; bAnim plays the
            // count-up animation (bBig picks the larger one). Returns the
            // score shown (clamped to 0 to 999 with the animation).
            long SetScore(long nPlayer, long nScore, int bAnim, int bBig);
            // Shows the time fTime and counts the last seconds down. Returns 0.
            long SetTime(float fTime, int nArg, int bArg);
            // Pops nPoint up over world position rPos for player nPlayer
            // (three at a time per player). Returns 0.
            long SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint);
            // Clears player nPlayer's pop-up scores. Returns 0.
            long ClearPointUpText(long nPlayer);
            // Shows nScore (with a sign when not negative) next to fruit
            // ball nBall. Returns 0.
            long SetBallScore(long nBall, long nScore);
            // Shows or hides the fourth fruit ball's score. Returns 0.
            long SetDispBallFruit4(int bDisp);
            // Shows team nTeam's score (at most 9999). Returns 0.
            long SetTeamScore(long nTeam, long nScore);
            // Plays crown action nAction over player nPlayer, at world
            // position rPos (0: appear, 1: stay, 2: leave, other: hide).
            // -1 without the crown pane, else 0.
            long SetCrownIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, long nAction);
            void SetDispPlayerScore(int bDisp);
            void SetDispTeamScore(int bDisp);

        private:
            // Set's parts. The first two return the number of players.
            long SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void CreateCountUp(bcs::G2D::CPaneCtrl *pPaneCtrl);
            long SetTeamFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void CreateCrownIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            // Plays animation nAnim on the score panes. Inlined into the
            // Score* functions; the original link dropped its own copy.
            void SetAnimScore(long nAnim);

            // Pop-up scores per player.
            static const unsigned char s_CountUpPaneNum;

            long m_nPlayerNum;                // 0x60
            unsigned char m_nCountUpIdx[4];   // 0x64, next pop-up per player
            cockpit::CTimeCount m_TimeCount;  // 0x68
            cockpit::CEffectSet m_EffectSet;  // 0x70
    };
}

#endif /* !CCOCKPITBASKETHARAPEKO_HPP_ */
