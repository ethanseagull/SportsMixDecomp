#ifndef CCOCKPITDODGEORDIE_HPP_
#define CCOCKPITDODGEORDIE_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"

namespace msw {
    class CChrCore;

    // Size 0xB4 (CCockpitDodge::CreateObjImpl allocates it). The board of
    // the "dodge or die" game: time, each player's life, face icons and the
    // "out" telop (DodgeF game/dodge/CCockpitDodgeOrDie.cpp). Its bases are
    // CCockpitDodgeLayoutBase and a cockpit::CEffectSet; the second is not
    // declared here yet (only what CCockpitDodge calls so far). Return
    // types and argument names are guesses; nIdx is a team colour or -1.
    class CCockpitDodgeOrDie : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeOrDie(long nId);
            virtual ~CCockpitDodgeOrDie();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            void SetTimeDisp(int bDisp);
            void SetLifeDisp(int bDisp);
            void SetTimeValue(float fTime, int nArg, int bArg);
            void SetLifeValue(long nIdx, long nNo, float fRate, float fRate2);
            void SetFaceIcon(long nIdx, long nNo, const CChrCore *pChr);
            void SetLifeGaugeAnim(long nIdx, long nNo, long nState);
            void SetLifeGaugeLayout(long nLayout);
            void SetOutTelop(long nIdx, long nState, int bArg);

        private:
            unsigned char m_Unk60[0xB4 - 0x60];
    };
}

#endif /* !CCOCKPITDODGEORDIE_HPP_ */
