#ifndef CCOCKPITVOLLEYTELOPSERVE_HPP_
#define CCOCKPITVOLLEYTELOPSERVE_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {
    class CCockpit;

    // Size 0xF4. The volley game's serve telop (layout TE_ready_vo): the
    // server's name and the set point text. CCockpitLayoutBase at 0,
    // CCockpitFrameOutBase at 0x60 (vptr at 0x68), own members from 0x6C.
    // Member names are guesses.
    class CCockpitVolleyTelopServe : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CCockpitVolleyTelopServe(long nId, float fSortZ);
            virtual ~CCockpitVolleyTelopServe();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            // 0, or -1 to -3 when the layout cannot be built.
            virtual int Set();
            virtual void Remove();

            // Sends the server's name and the two set point numbers to the
            // telop nObjId. pCockpit is not used.
            static long SetParameter(CCockpit *pCockpit, long nObjId, const wchar_t *pName, long nPointA,
                                     long nPointB);

        private:
            void StepControl();
            void SetSetPointStr();

            wchar_t m_Name[0x40]; // 0x6C
            long m_nPointA;       // 0xEC
            long m_nPointB;       // 0xF0
    };
}

#endif /* !CCOCKPITVOLLEYTELOPSERVE_HPP_ */
