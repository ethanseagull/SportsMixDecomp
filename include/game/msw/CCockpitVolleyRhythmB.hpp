#ifndef CCOCKPITVOLLEYRHYTHMB_HPP_
#define CCOCKPITVOLLEYRHYTHMB_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // Size 0x68 (CCockpitVolley::CreateObjImpl allocates it, object 53):
    // the rhythm game's receive count and point-up texts
    // (game/volley/CCockpitVolleyRhythmB.cpp). Virtuals from the VolleyF
    // vtable; return types and argument names are guesses.
    class CCockpitVolleyRhythmB : public CCockpitVolleyLayoutBase {
        public:
            CCockpitVolleyRhythmB(long nId, float fSortZ);
            virtual ~CCockpitVolleyRhythmB();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();
            virtual long CancelMenu();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            void SetReceiveCount(long nCount);
            // Pops up nPoint over rPos for player nPlayer; returns 0.
            long SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint);
            // Hides player nPlayer's pop-ups; returns 0.
            long ClearPointUpText(long nPlayer);

        private:
            void SetAnimGauge(long nAnim);
            // Makes the pop-up panes of every player.
            void CreatePointUp(bcs::G2D::CPaneCtrl *pPaneCtrl);

            // Pop-ups per player (they are used in turn).
            static const unsigned char s_CountUpPaneNum;

            long m_nPlayerNum;              // 0x60
            unsigned char m_nCountUpIdx[4]; // 0x64, next pop-up per player
    };
}

#endif /* !CCOCKPITVOLLEYRHYTHMB_HPP_ */
