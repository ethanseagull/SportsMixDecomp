#ifndef MSW_CCOMMANBASKET_HPP_
#define MSW_CCOMMANBASKET_HPP_

#include "game/msw/CComMan.hpp"

namespace msw {

    // The basketball computer manager's shared helpers (BasketF,
    // game/basket/CComManBasket.cpp). Static, like CComManVolley's helpers.
    // Positions are in the court's own frame unless the name says World.
    // Return types are guesses where no caller shows them.
    class CComManBasket : public CComMan {
        public:
            // The character nearest to rPos (CComMan::GetNearChr), or null.
            static CChrCore *GetNearChrCore(const bcs::FVEC3 &rPos, long, long, long, int);
            // The yaw (radians) from p1 towards p2 in the XZ plane.
            static float GetRotY_P1ToP2XZ(const CChrCore *p1, const CChrCore *p2);
            static float GetRotY_P1ToP2XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2);
            // Not zero when pChr is driven by the computer (driver type 3).
            static int IsComDrv(const CChrCore *pChr);
            static long GetTeamCoin(long nTeam);
            // Lengths in the XZ plane, and in space.
            static float GetDistXZ(const bcs::FVEC3 &rVec);
            static float GetDistXZ(const CChrCore *p1, const CChrCore *p2);
            static float GetDistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2);
            static float GetDist(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2);
            // The XZ distance from pChr to the nearest "?" panel; 9999 when
            // there is none.
            static float GetNearQPanelDistXZ(const CChrCore *pChr);
            // The Z distance from goal nGoal to the centre of its three-point
            // area, measured towards the court.
            static float GetDistOnGoalTo3PointAreaCenter(long nGoal);
            // Not zero when rPos is in the three-point area of nGoal.
            static bool Is3PointArea(long nGoal, const bcs::FVEC3 &rPos);
            // A random point around the three-point area of nGoal.
            static bcs::FVEC3 Get3PointAreaRundPos(long nGoal);
            // Moves rPos out of the boss Packun's area (Hungry Boss Packun
            // courts), away from pChr's side.
            static void NormalizeCanMovePosToBossPackun(bcs::FVEC3 &rPos, const CChrCore *pChr);
            // Not zero when a character of nChrBits (or other than pChr's
            // own) stands in the area from rFrom to rTo.
            static int IsExistTarget(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float, float,
                                     unsigned long nChrBits);
            static int IsExistTarget(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float, float,
                                     const CChrCore *pChr);
            // rOut = rIn flattened to the XZ plane and made length 1.
            static void NormalVecXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn);
            static int IsAllComTeam(long nTeam);
            // Not zero when the "?" panel task nId is gone or cannot be
            // taken.
            static int IsQPanelDisable(unsigned long nId);
            // A random value between the two bounds, in either order.
            static long GetRandValueI(long n1, long n2);
            static float GetRandValueF(float f1, float f2);
            // Not zero when a character of nChrBits other than pChr stands
            // between pChr and rTarget, within fAngle of the direction to it.
            static int IsExistChrToTarget(const CChrCore *pChr, float fAngle, const bcs::FVEC3 &rTarget, float,
                                          float, unsigned long nChrBits);
            static int IsExistChrToTargetWorld(const CChrCore *pChr, float fAngle, const bcs::FVEC3 &rTarget,
                                               float, float, unsigned long nChrBits);
    };
}

#endif /* !MSW_CCOMMANBASKET_HPP_ */
