#ifndef MSW_CCOMMAN_HPP_
#define MSW_CCOMMAN_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CBall;
    class CChrCore;
    class CItem;

    // The computer players' manager. A DOL class; each game module derives
    // its own (CComManVolleyGame, CComManBasketGame, CComManHockey, ...) and
    // creates the one instance with CreateInstance.
    //
    // INCOMPLETE: only what the VolleyF CComManVolley units call is here.
    //  - The object is the vptr alone (CComManVolleyGame is allocated with
    //    4 bytes). The destructor is inline and empty (the derived
    //    destructors call nothing). The DOL keeps no vtable of this class,
    //    and the modules that build a derived instance (VolleyF
    //    CComManVolleyGame) emit none either, so Clear and Main are taken to
    //    be declared out of line here (their definitions and the vtable,
    //    unused, were stripped from the DOL): declared pure or inline, the
    //    vtable would be emitted weak in every such module.
    //  - Nearly everything else is static: the DOL bodies take no `this`.
    //    Return types are guesses except where a caller shows them.
    class CComMan {
        public:
            virtual ~CComMan() {}
            virtual void Clear();
            virtual void Main(float fFrame);

            // (DOL) Reads entry nId of the computer parameter table for
            // level nLevel. (meaning of the arguments guessed)
            static float GetDatF(long nId, long nLevel);
            // (DOL; added for HockeyF game/hockey/CChrCoreHockeyLuigi.cpp) The
            // same for a level between the table's levels (interpolated, it
            // seems; CGame::GetTeamComLevel gives one).
            static float GetDatF(long nId, float fLevel);
            // (DOL) Number of the pad-driven characters of team nTeam.
            static long GetExistPadDrvNum(long nTeam);
            // (DOL) Index of the ball in play; negative for none.
            static long GetActiveBallIdx();

            // (DOL) Conversions between world positions and the court's
            // own frame; nCourt -1 selects the default. (meaning of the
            // argument guessed)
            static bcs::FVEC3 World2Local(const bcs::FVEC3 &rPos, long nCourt);
            static bcs::FVEC3 Local2World(const bcs::FVEC3 &rPos, long nCourt);
            // (DOL) Distance between two points in the XZ plane.
            static float GetDisXZ(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1);
            // (DOL) A character's position in the court's own frame, and in
            // the world.
            static bcs::FVEC3 GetChrPos(const CChrCore *pChr);
            static bcs::FVEC3 GetChrWorldPos(const CChrCore *pChr);

            // (DOL) Walks the items that have been thrown.
            static CItem *ThrowItem_GetFirst();
            static CItem *ThrowItem_GetNext(CItem *pItem);

            static CComMan *g_pInstance;
    };
}

#endif /* !MSW_CCOMMAN_HPP_ */
