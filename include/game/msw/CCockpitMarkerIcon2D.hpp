#ifndef MSW_CCOCKPITMARKERICON2D_HPP_
#define MSW_CCOCKPITMARKERICON2D_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x70. Lives in the DOL (game/msw/CCockpitMarkerIcon2D.cpp); the
    // modes derive from it and override Set. Member names are guesses.
    class CCockpitMarkerIcon2D : public CCockpitLayoutBase {
        public:
            // Stores nType at 0x60 and nLayer at 0x64, clears 0x68 and 0x6C.
            CCockpitMarkerIcon2D(long nId, float fSortZ, long nType, long nLayer);
            virtual ~CCockpitMarkerIcon2D();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

        protected:
            long m_nType;          // 0x60, which entry of the mode's layout table
            long m_nLayer;         // 0x64, 1 to 3 pick layout layers 2 to 4
            unsigned long m_Unk68; // 0x68
            unsigned long m_Unk6C; // 0x6C
    };
}

#endif /* !MSW_CCOCKPITMARKERICON2D_HPP_ */
