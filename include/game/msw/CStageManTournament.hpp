#ifndef MSW_CSTAGEMANTOURNAMENT_HPP_
#define MSW_CSTAGEMANTOURNAMENT_HPP_

#include "game/msw/CStageMan.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace msw {

    // The stage of the tournament map (DOL class, constructor at
    // 0x80165EEC, 0x21C bytes; TourF msw::CTournamentMan creates it and
    // hands it to msw::CTournamentRouteMan). It derives from CStageMan: the
    // route manager calls virtual functions at 0x440 to 0x454, right after
    // the last slot of CStageMan (CStageManBasket's own virtuals sit there,
    // so it is not a CStageManBasket).
    //
    // include/game/msw/CStageBasket.hpp declares a class of the same name
    // derived from CStageManBasket for BasketF; the two declarations must
    // not meet in one file. TourF's game/tour/CTournamentRouteMan.cpp uses
    // this one. Only the new virtual functions are declared; names and
    // meanings guessed from the route manager's calls.
    class CStageManTournament : public CStageMan {
        public:
            CStageManTournament(bcs::CTask *pParent);
            virtual ~CStageManTournament();

            // 0x440: the chip shown on the map grid at (nX, nZ).
            virtual void SetGridChip(long nX, long nZ, long nChip);
            virtual void Unk444();
            virtual void Unk448();
            // 0x44C, 0x450: the chip model of grid position (nX, nZ), and
            // the one of chip kind nChip.
            virtual nw4r::g3d::ResMdl GetGridChipMdl(long nX, long nZ);
            virtual nw4r::g3d::ResMdl GetChipMdl(long nChip);
            // 0x454: the mark shown on the map grid at (nX, nZ).
            virtual void SetGridMark(long nX, long nZ, long nMark);

        private:
            unsigned char m_Unk1FC[0x20]; // 0x1FC
    }; // size 0x21C
}

#endif /* !MSW_CSTAGEMANTOURNAMENT_HPP_ */
