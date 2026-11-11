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
    // vtable; return types and argument names are guesses. The members are
    // not mapped (0x60 word, bytes at 0x64 to 0x67).
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
            void SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint);
            void ClearPointUpText(long nPlayer);

        private:
            unsigned char m_Unk60[0x68 - 0x60];
    };
}

#endif /* !CCOCKPITVOLLEYRHYTHMB_HPP_ */
