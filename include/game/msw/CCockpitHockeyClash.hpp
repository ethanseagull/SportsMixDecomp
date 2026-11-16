#ifndef CCOCKPITHOCKEYCLASH_HPP_
#define CCOCKPITHOCKEYCLASH_HPP_

#include "game/msw/CCockpitHockeyLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CTimeCount.hpp"

namespace bcs {
    struct FVEC3;

    namespace G2D {
        class CPaneCtrl;
    }
}

namespace msw {

    // Size 0xB8 (CCockpitHockey::CreateObjImpl allocates it): the board of
    // the hockey party game (layout HO_crash; HockeyF
    // game/hockey/CCockpitHockeyClash.cpp): each player's score with a face
    // icon, a "1P"/"COM" label and a star, the points that pop up over a
    // player, a countdown over a player about to leave, the "shake the
    // Wii Remote" icon, a crown over the leader, and the time. The same
    // layout code as BasketF's CCockpitBasketHarapeko. Member and argument
    // names are guesses.
    class CCockpitHockeyClash : public CCockpitHockeyLayoutBase {
        public:
            CCockpitHockeyClash(long nId, float fSortZ);
            virtual ~CCockpitHockeyClash();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            // Shows the time fFrame and counts the last seconds down.
            // Returns 0.
            long SetTimeValue(float fFrame, int nArg, int nMode);
            // Shows nScore on player nPlayer's counter; bAnim plays the
            // count-up animation (bBig picks the other one). Returns the
            // score shown (clamped to 0 to 999 with the animation).
            long SetScore(long nPlayer, long nScore, int bAnim, int bBig);
            // Pops nPoint up over world position rPos for player nPlayer
            // (three at a time per player). Returns 0.
            long SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint);
            // Clears player nPlayer's pop-up points. Returns 0.
            long ClearPointUpText(long nPlayer);
            // Starts player nPlayer's countdown over character nChr, offset
            // by rPos; bHide 1 hides it instead. Returns 0.
            long SetLeaveCountText(long nPlayer, const bcs::FVEC3 &rPos, long nChr, int bHide);
            // Shows player nPlayer's "shake" icon at world position rPos
            // (nArg goes to the icon task); bHide 1 hides it. -1 without the
            // icon pane, else 0.
            long SetRimoconShakeIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, int bHide);
            // Plays crown action nAction over player nPlayer, at world
            // position rPos (0: appear, 1: stay, 2: leave, other: hide).
            // -1 without the crown pane, else 0.
            long SetCrownIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, long nAction);

        private:
            // Set's parts. SetFaceIcon returns the number of players.
            long SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void CreateCountUp(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void CreateCountDown(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void CreateCrownIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            // Resets the score animations and puts the time frame in one of
            // its two places (the first with bAlt nonzero).
            void InitPaneAnim(int bAlt);
            // Plays animation nAnim on the score panes. Inlined into the
            // Score* functions; the original link dropped its own copy.
            void SetAnimScore(long nAnim);

            // Pop-up points per player.
            static const unsigned char s_CountUpPaneNum;

            float m_fLytSortZ;                // 0x60, the layout's sort depth
            long m_nPlayerNum;                // 0x64
            unsigned char m_nCountUpIdx[4];   // 0x68, next pop-up per player
            cockpit::CTimeCount m_TimeCount;  // 0x6C
            cockpit::CEffectSet m_EffectSet;  // 0x74
    };
}

#endif /* !CCOCKPITHOCKEYCLASH_HPP_ */
