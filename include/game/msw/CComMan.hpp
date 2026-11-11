#ifndef MSW_CCOMMAN_HPP_
#define MSW_CCOMMAN_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CBall;
    class CChrCore;
    class CItem;
    class CQPanel;

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
            // (DOL; added for DodgeF game/dodge/CComDrvDodgeGameOffRun.cpp)
            // The distance of rPos from the court's centre line along Z
            // (meaning guessed from the name); GetChrPos(long), below, is the
            // position of character number nChr.
            static float GetZtoCenter(const bcs::FVEC3 &rPos);

            // (DOL) Walks the items that have been thrown.
            static CItem *ThrowItem_GetFirst();
            static CItem *ThrowItem_GetNext(CItem *pItem);

            // Added for game/basket/CComManBasket.cpp (meanings and return
            // types guessed). The index of the character nearest to rPos,
            // past the last one for none.
            static unsigned long GetNearChr(const bcs::FVEC3 &rPos, long, long, long, int);
            // The XZ distance from rPos to the nearest "?" panel, negative
            // when there is none.
            static float QPanel_GetNearDisXZ(const bcs::FVEC3 &rPos, long, unsigned long);
            // Not zero when rPos lies in the area spanned from rFrom to rTo
            // in the XZ plane (widths guessed from the two floats).
            static int IsPointInFaceXZ(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                                       float, float);
            // Bit masks of characters: bit nChr of nBits.
            static int IsEnableBit(unsigned long nBits, long nChr);
            static void SetEnableBit(unsigned long &rBits, long nChr);
            // Added for HockeyF game/hockey/CComManHockeyGame.cpp (DOL;
            // meanings and return types guessed).
            // The index in pTarget (nTargetNum positions) that the nMyNum
            // positions of pMy should take, for my position nMyIdx; negative
            // for none.
            static long GetNearBestPosBaseEx(const bcs::FVEC3 *pMy, const bcs::FVEC3 *pTarget, long nMyNum,
                                             long nTargetNum, long nMyIdx);
            // The nearest "?" panel of the kinds in nKindBits (0: none) and
            // its position.
            static unsigned long QPanel_GetNear(const bcs::FVEC3 &rPos, unsigned long nKindBits);
            static bcs::FVEC3 QPanel_GetPos(unsigned long nPanel);
            // The nearest falling green shell (0: none), whether it is
            // active, and its position.
            static unsigned long FallGShell_GetNear(const bcs::FVEC3 &rPos);
            static int FallGShell_IsEnable(unsigned long nShell);
            static bcs::FVEC3 FallGShell_GetPos(unsigned long nShell);
            // (added for HockeyF game/hockey/unit_7AD44.cpp; meaning of the
            // long not known) Whether the yaw fRot is close enough to
            // fTarget.
            static int IsRangeRot(float fRot, float fTarget, long);

            // (DOL; added for VolleyF's CComDrvVolleyGame units; return
            // types and meanings guessed) A level table entry as an integer;
            // a random character of team nTeam; the position of character
            // nChr; the centre of court nCourt; which of three positions is
            // best for the two of pMy (nIdx: the nth best).
            static long GetDatI(long nId, float fLevel);
            static long GetDatI(long nId, long nLevel);
            static int IsExistChrToTargetWorld(const CChrCore *pChr, const bcs::FVEC3 &rPos, float, float,
                                               unsigned long nIgnoreBits, int);
            static CChrCore *GetChrCoreRand(long nTeam);
            static bcs::FVEC3 GetChrPos(long nChr);
            static float GetStageCourtCenterX(long nCourt);
            static float GetStageCourtCenterZ(long nCourt);
            static long GetNearBestPos2on3(const bcs::FVEC3 *pMy, const bcs::FVEC3 *pTarget, long nIdx);

            // (DOL; added for BasketF game/basket/CComManBasketGame.cpp;
            // return types guessed) The instance (g_pInstance); whether the
            // court has an invisible wall at the view (the collision's
            // IsEnableViewColl_), its Z in the court's frame, and the side it
            // closes (0: the back line, 1: the front line); walks of the "?"
            // panels.
            static CComMan *GetInstance();
            static int IsEnableViewColl();
            static float GetViewCollPosZ();
            static long GetViewCollTargetDir();
            static CQPanel *QPanel_GetFirst();
            static CQPanel *QPanel_GetNext(CQPanel *pPanel);

            static CComMan *g_pInstance;
    };
}

#endif /* !MSW_CCOMMAN_HPP_ */
