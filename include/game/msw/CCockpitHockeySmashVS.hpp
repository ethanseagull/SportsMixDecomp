#ifndef CCOCKPITHOCKEYSMASHVS_HPP_
#define CCOCKPITHOCKEYSMASHVS_HPP_

#include "game/msw/CCockpitHockeyLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
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

            void SetTimeValue(float fFrame, int nArg, int nMode);
            void SetSetCountPoint(long nTeam, long nPoint);
            void SetRimoconShakeIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, int bArg);
            // nSide is 0 or 1.
            void SetFaceIcon(long nSide, long nTurn, long nArg);

        private:
            unsigned char m_Unk60[0xB8 - 0x60];
    };
}

#endif /* !CCOCKPITHOCKEYSMASHVS_HPP_ */
