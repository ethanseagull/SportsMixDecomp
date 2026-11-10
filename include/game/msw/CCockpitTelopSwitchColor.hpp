#ifndef CCOCKPITTELOPSWITCHCOLOR_HPP_
#define CCOCKPITTELOPSWITCHCOLOR_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {
    class CCockpit;

    // Size 0x478. Lives in the DOL. A telop layout that holds two texts and
    // a small table of values that select what it shows. CCockpitLayoutBase
    // is at 0, CCockpitFrameOutBase at 0x60 (vptr at 0x68), own members from
    // 0x6C. The layout is read from the constructor and SetParameter /
    // SetAppendTableParam; member names are guesses. Main, SendEvent, Disp,
    // Set and Remove have no name in the DOL (fn_8013ADAC and on).
    class CCockpitTelopSwitchColor : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            // nType is stored at 0x6C.
            CCockpitTelopSwitchColor(long nId, long nType, float fSortZ);
            virtual ~CCockpitTelopSwitchColor();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // Sets entry nIndex of the table (one entry); ignored when out
            // of range.
            void SetAppendTableParam(unsigned long nIndex, short nValue);
            // Copies pText (nLen characters, or all of it when nLen < 0) to
            // text nIndex (0 or 1) of the telop nObjId of pCockpit. -1 when
            // there is no such telop, -2 when pText is null, else 0.
            static long SetParameter(CCockpit *pCockpit, long nObjId, long nIndex, const wchar_t *pText, long nLen);

        protected:
            long m_nType;                   // 0x6C, the constructor argument
            short m_nAppendTable[1];        // 0x70
            wchar_t m_Text[2][0x100];       // 0x72
            unsigned short m_nTextLen[2];   // 0x472
    };
}

#endif /* !CCOCKPITTELOPSWITCHCOLOR_HPP_ */
