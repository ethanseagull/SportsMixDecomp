#ifndef CCOCKPITVOLLEYCODER_HPP_
#define CCOCKPITVOLLEYCODER_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"
#include "game/msw/cockpit/CEffectSet.hpp"
#include "game/msw/cockpit/CMissionCount.hpp"

namespace msw {
    class CChrCore;

    // Size 0xD0. The volley game's score board (VolleyF
    // game/volley/CCockpitVolleyCoder.cpp): the scores, the coins, the sets
    // won, the players' face icons and the mission counter. One of three
    // layouts, by the screen split. Member names are guesses; return types
    // of the setters are guesses too.
    class CCockpitVolleyCoder : public CCockpitVolleyLayoutBase {
        public:
            CCockpitVolleyCoder(long nId);
            virtual ~CCockpitVolleyCoder();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();
            virtual int CoinOut();
            virtual int CoinIn();
            virtual int CoinOutWait();
            virtual int CoinInWait();

            // A new virtual, last in the vtable.
            virtual void SetFaceIcon();

            // nState 0 to 3: which serve animation the score board plays.
            // -1 for any other value.
            long SetServeScoreAnim(long nState);
            // Plays animation nAnim on the score board; bSetCount set also
            // lights the won-set marks one after the other.
            long PlayAnim(long nAnim, int bSetCount);
            long PlayAnimCoinBack(long nAnim);
            void SetScoreValue(long nTeam, long nScore);
            // nDiff above 0: a coin gained, else a coin lost.
            void SetCoinValue(long nTeam, long nDiff);
            void SetScoreDisp(int bDisp);
            void SetSetCountDisp(int bDisp);
            void SetSetCountPoint(long nTeam, long nPoint);
            void SetScoreFont(long nTeam, int bFont);
            void SetMissionCountDisp(int bDisp);
            void SetMissionCountNum(long nNum, long nMax);
            void SetMissionCountUpNum(long nNum, long nMax);
            void SetMissionCountActiveColor(int bActive);
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nPlayer, const CChrCore *pChr);

        private:
            int SetupGame();

            unsigned char m_nSetNum;       // 0x60, sets in the match: 1, 3 or 5
            unsigned char m_nSetPoint[2];  // 0x61, sets won by each team
            unsigned char m_Unk63[2];      // 0x63
            unsigned char m_bUnk65[2];     // 0x65, by team: flash the score with the next animation
            unsigned char m_Unk67[2];      // 0x67
            unsigned char m_nLayout;       // 0x69, 0 to 2 (by the screen split)
            long m_bMission;               // 0x6C, the mission counter is shown
            cockpit::CMissionCount m_MissionCount; // 0x70
            cockpit::CEffectSet m_EffectSet;       // 0x8C
    };
}

#endif /* !CCOCKPITVOLLEYCODER_HPP_ */
