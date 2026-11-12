#ifndef CCOCKPITHOCKEYSMASHVS_HPP_
#define CCOCKPITHOCKEYSMASHVS_HPP_

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
    // the hockey game's extra match (HockeyF
    // game/hockey/CCockpitHockeySmashVS.cpp). Its constructor also builds a
    // cockpit::CEffectSet. Only what CCockpitHockey calls so far; argument
    // names are guesses.
    class CCockpitHockeySmashVS : public CCockpitHockeyLayoutBase {
        public:
            CCockpitHockeySmashVS(long nId);
            virtual ~CCockpitHockeySmashVS();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            // The clock at fFrame frames (as CCockpitHockeyCoder::SetTimeValue).
            void SetTimeValue(float fFrame, int nArg, int nMode);
            // Lights nPoint of team nTeam's won-set marks.
            void SetSetCountPoint(long nTeam, long nPoint);
            // Shows the "shake the remote" icon of player nPlayer (0 to 3)
            // over world position rPos; bArg 1 hides it. -1 for a bad player
            // or a missing pane, else 0.
            long SetRimoconShakeIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, int bArg);
            // nSide is 0 or 1: the face icons of team nSide, starting with
            // its player nTurn, nArg players a team.
            void SetFaceIcon(long nSide, long nTurn, long nArg);
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nSide, long nTurn, long nArg);

        private:
            int SetupGame();
            // Plays slide animation nAnim on the set counter and the time;
            // bSetCount lights the won sets one after the other.
            long PlayAnim(long nAnim, int bSetCount);

            unsigned char m_nPlayerNum;           // 0x60, players a team
            unsigned char m_nSetNum;              // 0x61, sets in the match (3)
            unsigned char m_nSetPoint[2];         // 0x62, sets won, by team
            unsigned char m_nUnk64[2];            // 0x64, set to 0, not read here
            unsigned char m_bUnk66[2];            // 0x66, by team, not set here
            unsigned char m_Unk68[4];             // 0x68, not used here
            cockpit::CTimeCount m_TimeCount;      // 0x6C
            cockpit::CEffectSet m_EffectSet;      // 0x74
    };
}

#endif /* !CCOCKPITHOCKEYSMASHVS_HPP_ */
