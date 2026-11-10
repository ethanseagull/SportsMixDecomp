#include "game/msw/CTournamentRouteMan_Stage33.hpp"

#include <stdio.h>

#include "bcs/BCS_Vec.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/EF/CEffect.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEvNodeData.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CRouteBasicJump.hpp"
#include "game/msw/CRouteBasicLine.hpp"
#include "game/msw/CRouteDataMove.hpp"
#include "game/msw/CRouteJumpPoints.hpp"
#include "game/msw/CRouteMove.hpp"
#include "game/msw/CRouteWait.hpp"
#include "game/msw/CTournamentExit.hpp"
#include "game/msw/CTournamentMan.hpp"
#include "game/msw/EvDemo.hpp"
#include "game/msw/TournamentData.hpp"
#include "game/msw/TournamentWork.hpp"

namespace msw {

    namespace {

        // One animation of an airship (map objects 0x27 to 0x2B), one entry
        // of s_ShipAnim. The routes hand the address of an entry to the
        // timer events as their unsigned long parameter. 0x28 bytes. (names
        // guessed)
        struct SHIP_ANIM {
            const char *pAnim;  // 0x00: node animation of the ship model, also the name of its scene camera
            signed char bFlip;  // 0x04: played mirrored
            signed char nObj;   // 0x05: the map object that plays it
            signed char nObjAp; // 0x06: a second object that plays it too (the enemy ship), -1 for none
            // 0x07: what happens at the end: 1 the ship lands at a map
            // point, 2 it takes off from one, 3 and 4 it crosses a rainbow
            // road, 0 and 5 a battle with the enemy ship; -1 nothing.
            signed char nType;
            long nEffect;       // 0x08: entry of s_ShipEffect
            float fWait;        // 0x0C: frames before the animation starts
            float fCameraAdd;   // 0x10, 0x14: how the camera follows the scene camera
            float fCameraMul;
            float fNextTime;    // 0x18: when the follow-up event runs; negative: counted from the end
            float fNextParam;   // 0x1C
            long nNext;         // 0x20: parameter of the follow-up event (0x1F, the screen fade), -1 for none
            float fAfter;       // 0x24: frames after the animation
        };

        // The effects and sounds of a ship animation, 0x14 bytes. (names
        // guessed)
        struct SHIP_EFFECT {
            long nEff;        // 0x00: effect on the ship, -1 for none
            long nEffAp;      // 0x04: effect on the second object, -1 for none
            long nEvent;      // 0x08: effect event for the character (timer event 0x3FE), -1 for none
            float fEventTime; // 0x0C
            long nSe;         // 0x10: the set of sounds, see PushShipSe
        };

        // msw::CLoadData entry of the map objects. (name guessed)
        const long LOADDATA_OBJ = 0x5C;

        // The jumps between map points (route types 1 to 3). nUnk58 to
        // nUnk5C of the first two hold 1, 0x2D, 0x2E and 0x3F4, which look
        // like timer events (0x2D, 0x2E and 0x3F4 are this stage's rainbow
        // effects).
        const CRouteJumpPoints::JUMP_DATA s_JumpData1 = {
            {1.0f / 30.0f, 1.0f / 30.0f, -20.0f, -20.0f, -20.0f, 5.0f, 15.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             0.0f},
            {0.0f, 0.0f, 0.0f},
            0.0f,
            1.0f,
            {-1, -1, -1, -1},
            0.0f,
            1,
            0x2D,
            0x002E03F4,
            0xE29,
        };

        const CRouteJumpPoints::JUMP_DATA s_JumpData2 = {
                {1.0f / 30.0f, 1.0f / 30.0f, -20.0f, -20.0f, -20.0f, 5.0f, 15.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                 0.0f},
                {0.0f, 0.0f, 0.0f},
                0.0f,
                1.0f,
                {-1, -1, -1, -1},
                0.0f,
                1,
                0x2D,
                0x002E03F4,
                0xE29,
        };

        const CRouteJumpPoints::JUMP_DATA s_JumpData3 = {
                {1.0f / 30.0f, 1.0f / 30.0f, -30.0f, -20.0f, -20.0f, 5.0f, 30.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                 0.0f},
                {0.0f, 0.0f, 0.0f},
                0.0f,
                1.0f,
                {-1, -1, -1, -1},
                0.0f,
                0,
                -1,
                -1,
                0xE29,
        };

        const SHIP_EFFECT s_ShipEffect[10] = {
            {-1, -1, -1, 0.0f, 0},
            {-1, -1, -1, 0.0f, 2},
            {-1, -1, 9, 220.0f, 1},
            {0xD8, 0xD7, -1, 0.0f, 3},
            {0xDA, 0xD9, -1, 0.0f, 3},
            {0xDC, 0xDB, -1, 0.0f, 4},
            {0xDE, 0xDD, -1, 0.0f, 4},
            {0xD2, -1, -1, 0.0f, 5},
            {-1, -1, 9, 220.0f, 6},
            {-1, -1, -1, 0.0f, 7},
        };

        // Every ship animation: pAnim, bFlip, nObj, nObjAp, nType, nEffect,
        // fWait, fCameraAdd, fCameraMul, fNextTime, fNextParam, nNext, fAfter.
        const SHIP_ANIM s_ShipAnim[65] = {
            {"o_OUT", 0, 0x28, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 0
            {"ax_IN", 0, 0x28, -1, -1, 1, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 1
            {"o_OUT", 1, 0x28, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 2
            {"az_IN", 0, 0x28, -1, -1, 1, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 3
            {"bb_OUT_00", 0, 0x27, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 4
            {"bi_IN", 0, 0x27, -1, 3, 9, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 5
            {"bb_OUT_00", 1, 0x29, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 6
            {"bi_IN", 1, 0x29, -1, 4, 9, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 7
            {"bi_OUT", 0, 0x27, -1, 2, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 8
            {"bm_IN", 0, 0x27, -1, -1, 1, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 9
            {"bi_OUT", 1, 0x29, -1, 2, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 10
            {"bh_IN", 0, 0x29, -1, -1, 9, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 11
            {"bb_OUT_00", 0, 0x27, -1, -1, 8, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 12
            {"BATTLE_EV_00", 0, 0x27, 43, 5, 3, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 13
            {"bb_OUT_00", 1, 0x29, -1, -1, 8, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 14
            {"BATTLE_EV_00", 1, 0x29, 43, 5, 4, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 15
            {"bi_OUT", 0, 0x27, -1, 2, 8, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 16
            {"BATTLE_EV_00", 1, 0x27, 43, 5, 4, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 17
            {"bi_OUT", 1, 0x29, -1, 2, 8, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 18
            {"BATTLE_EV_00", 0, 0x29, 43, 5, 3, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 19
            {"az_IN", 0, 0x27, -1, -1, 9, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 20
            {"bd_IN", 0, 0x29, -1, -1, 9, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 21
            {"bb_OUT_01", 0, 0x27, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 22
            {"MAP_00_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 23
            {"MAP_01_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 24
            {"bb_OUT_01", 1, 0x29, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 25
            {"MAP_02_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 26
            {"MAP_03_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 27
            {"BATTLE_EV_01", 0, 0x27, 43, 0, 5, 0.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 28
            {"MAP_DM_00_PUT", 0, 0x27, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 29
            {"MAP_DM_01_PUT", 0, 0x27, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, 4, 0.0f}, // 30
            {"BATTLE_EV_01", 1, 0x29, 43, 0, 6, 0.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 31
            {"MAP_DM_02_PUT", 0, 0x29, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 32
            {"MAP_DM_03_PUT", 0, 0x29, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, 4, 0.0f}, // 33
            {"BATTLE_EV_01", 1, 0x27, 43, 0, 6, 0.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 34
            {"MAP_DM_00_PUT", 0, 0x27, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 35
            {"MAP_DM_01_PUT", 0, 0x27, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, 4, 0.0f}, // 36
            {"BATTLE_EV_01", 0, 0x29, 43, 0, 5, 0.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 37
            {"MAP_DM_02_PUT", 0, 0x29, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 38
            {"MAP_DM_03_PUT", 0, 0x29, -1, 1, 7, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, 4, 0.0f}, // 39
            {"bd_OUT", 0, 0x29, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 40
            {"MAP_02_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 41
            {"MAP_03_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 42
            {"bi_OUT", 0, 0x27, -1, 2, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 43
            {"MAP_00_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 44
            {"MAP_01_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 45
            {"bi_OUT", 0, 0x27, -1, 2, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 46
            {"MAP_02_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 47
            {"MAP_03_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 48
            {"bi_OUT", 1, 0x29, -1, 2, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 49
            {"MAP_00_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 50
            {"MAP_01_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 51
            {"bi_OUT", 1, 0x29, -1, 2, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 52
            {"MAP_02_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 53
            {"MAP_03_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 54
            {"az_OUT", 0, 0x27, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 55
            {"MAP_00_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 56
            {"MAP_01_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 57
            {"az_OUT", 0, 0x28, -1, -1, 2, 100.0f, -0.0001f, 0.99658203f, -60.0f, 1.0f / 60.0f, 0, 40.0f}, // 58
            {"MAP_02_PUT", 0, 0x28, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 59
            {"MAP_03_PUT", 0, 0x28, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 60
            {"MAP_00_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 61
            {"MAP_01_PUT", 0, 0x27, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 62
            {"MAP_02_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 1.0f / 60.0f, 4, 0.0f}, // 63
            {"MAP_03_PUT", 0, 0x29, -1, 1, 0, 0.0f, -0.0001f, 0.99658203f, 0.0f, 0.0f, -1, 0.0f}, // 64
        };

        // The ship animations of the airship routes (types 0x1D to 0x45):
        // the ship leaving, arriving and the character getting off, as
        // entries of s_ShipAnim, -1 for none.
        const signed char s_ShipRouteAnim[41][3] = {
            {0, 1, -1},   {2, 3, -1},   {4, -1, -1},  {-1, 5, -1},  {6, -1, -1},  {-1, 7, -1},  {12, 13, -1},
            {-1, 5, -1},  {14, 15, -1}, {-1, 7, -1},  {8, -1, -1},  {-1, 9, -1},  {10, -1, -1}, {-1, 11, -1},
            {16, 17, -1}, {-1, 9, -1},  {-1, 20, -1}, {18, 19, -1}, {-1, 11, -1}, {-1, 21, -1}, {22, 23, 24},
            {25, 26, 27}, {28, 29, 30}, {31, 32, 33}, {34, 35, 36}, {37, 38, 39}, {40, 41, 42}, {43, 44, 45},
            {46, 47, 48}, {49, 50, 51}, {52, 53, 54}, {55, 56, 57}, {58, 59, 60}, {-1, 61, 62}, {-1, 63, 64},
            {12, 61, 62}, {-1, 20, -1}, {-1, 21, -1}, {-1, 63, 64}, {55, 56, 57}, {-1, 61, 62},
        };

        // Where the sound of the shown map plays.
        bcs::FVEC3 s_DispSePos(1.0f, -0.0f, -35.0f);

        // An angle brought into [0, 2 pi].
        inline float WrapRad(float fRad)
        {
            while (fRad > 6.2831855f) {
                fRad -= 6.2831855f;
            }
            while (fRad < 0.0f) {
                fRad += 6.2831855f;
            }
            return fRad;
        }

        // (name guessed) Stands in for `r1 - r2` as the original most likely
        // had it (see bcs::FVEC3Sum): the difference goes to a temporary and
        // is copied float by float.
        inline void FVEC3Diff(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
        {
            bcs::FVEC3 diff;

            bcs::FVEC3Sub(&diff, &r1, &r2);
            rOut = diff;
        }

        // The look-at spot of a map point, without the checks of
        // TournamentWork::GetPointViewPos.
        inline void SetPointViewPos(bcs::FVEC3 &pos, const TournamentWork::POINT_INFO *pPoint)
        {
            pos.Set(pPoint->GetViewOfsX() + pPoint->vPos.x, pPoint->vPos.y, pPoint->GetViewOfsZ() + pPoint->vPos.z);
        }

        // Moves a ship's offset to where the animation kind places it.
        inline void AddShipOffset(bcs::FVEC3 &vOffset, const SHIP_ANIM *pAnim)
        {
            switch (pAnim->nType) {
                case 2:
                case 3:
                case 4:
                    vOffset.z += -350.0f;
                    break;
                case 1:
                    vOffset.z -= 2.0f;
                    break;
            }
        }

        // (name guessed) A slide down rainbow road nRoad: the node path
        // "chara_set_Lnn" of the rainbow model, nFrame frames long.
        inline CRouteDataMove *CreateRainbowRoute(const CRouteBase::_PARAM &param, unsigned long nFrame, long nRoad)
        {
            long nIndex;
            nw4r::g3d::ResFile file(
                static_cast<nw4r::g3d::ResFileData *>(EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "rainbow", 0)));
            CRouteBase::_PARAM data;
            data.fLength = param.fLength;
            data.vStart.Set(1.0f, 0.0f, -400.0f);
            data.vEnd.Set(1.0f, 0.0f, -400.0f);
            data.fRotY = 0.0f;
            CRouteDataMove *pRoute = new CRouteDataMove(data, 1.0f, nFrame, 0);
            char szNode[64];
            sprintf(szNode, "chara_set_L%02d", nRoad);
            pRoute->SetNode(file, szNode, 0L);
            return pRoute;
        }

        // Queues the sounds of a ship animation (SHIP_EFFECT::nSe) for the
        // route event nEventId at fTime.
        inline void PushShipSe(CTournamentRouteMan *pMan, long nEventId, float fTime,
                               const CRouteBase::_PUSH_PARAM &param, long nSe)
        {
            switch (nSe) {
                case 1:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    pMan->_PushTimer(0x27, nEventId, 20.0f + fTime, param.pModel->GetIndex(), 0.0f, 0xE57);
                    break;
                case 2:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    pMan->_PushTimer(0x29, nEventId, 150.0f + fTime, param.pModel->GetIndex(), 0.0f, 0xE4A);
                    break;
                case 3:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    pMan->_PushTimer(0x27, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE58);
                    pMan->_PushTimer(0x25, nEventId, 110.0f + fTime, param.pModel->GetIndex(), 0.0f, 0xE59);
                    pMan->_PushTimer(0x25, nEventId, 280.0f + fTime, param.pModel->GetIndex(), 0.0f, 0xE5A);
                    break;
                case 4:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    pMan->_PushTimer(0x27, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE5B);
                    break;
                case 5:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    pMan->_PushTimer(0x27, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE5C);
                    break;
                case 6:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    pMan->_PushTimer(0x27, nEventId, 20.0f + fTime, param.pModel->GetIndex(), 0.0f, 0xE57);
                    break;
                case 7:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    pMan->_PushTimer(0x27, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE5D);
                    pMan->_PushTimer(0x29, nEventId, 150.0f + fTime, param.pModel->GetIndex(), 0.0f, 0xE4A);
                    break;
                default:
                    pMan->_PushTimer(0x26, nEventId, fTime, param.pModel->GetIndex(), 0.0f, 0xE49);
                    break;
            }
        }

        // The point flags a landing hands to timer events 0x3E9 and 0x3EA
        // (as CRouteBigGunOut of Stage32 does).
        inline unsigned long GetLandFlag(long nPoint)
        {
            unsigned long ulFlag = 0;
            unsigned char nPointFlag = TournamentWork::GetPointInfo(nPoint)->nFlag;
            if (nPointFlag & 0x10) {
                ulFlag |= 1;
            }
            if (nPointFlag & 0x08) {
                ulFlag |= 2;
            }
            if (nPointFlag & 0x04) {
                ulFlag |= 4;
            }
            return ulFlag;
        }

        // Marks team nTeam as riding an airship, or no longer riding one.
        inline void SetTeamOnShip(long nTeam, int bOn)
        {
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
            if (team.nPoint != 0xFF) {
                if (bOn) {
                    team.nFlag22 |= 2;
                } else {
                    team.nFlag22 &= ~2;
                }
            }
        }
    }

    // Getting on or off an airship (map objects 0x27 to 0x2A): a walk and a
    // jump onto it, or a jump down and a walk away. 0x3C bytes. Route types
    // 4 to 14.
    class CRouteShipRide : public CRouteBase {
        public:
            CRouteShipRide(const _PARAM &param, float rate);

            virtual ~CRouteShipRide();
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b)
            {
                float fRotY = m_fRotY;
                if (b) {
                    fRotY += 3.1415927f;
                    while (fRotY > 6.2831855f) {
                        fRotY -= 6.2831855f;
                    }
                    while (fRotY < 0.0f) {
                        fRotY += 6.2831855f;
                    }
                }
                rot.x = 0.0f;
                rot.y = fRotY;
                rot.z = 0.0f;
            }
            virtual long GetStartRouteId() const;

            // (name guessed)
            void SetShip(signed char bRide, signed char bSe, short nShip)
            {
                m_bRide = bRide;
                m_bSe = bSe;
                m_nShip = nShip;
            }

        private:
            float m_fRotY;       // 0x34
            short m_nShip;       // 0x38: the ship's map object
            signed char m_bRide; // 0x3A: run forwards, the route gets on the ship
            signed char m_bSe;   // 0x3B: play the boarding sound
    };

    // An airship flight between two map points: the ship's animations (up
    // to three entries of s_ShipAnim) carry the character along. 0x44
    // bytes. Route types 0x1D to 0x45.
    class CRouteShipMove : public CRouteBase {
        public:
            CRouteShipMove(const _PARAM &param, float rate);

            virtual ~CRouteShipMove();
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b)
            {
                float fRotY = m_fRotY;
                if (b) {
                    fRotY += 3.1415927f;
                    while (fRotY > 6.2831855f) {
                        fRotY -= 6.2831855f;
                    }
                    while (fRotY < 0.0f) {
                        fRotY += 6.2831855f;
                    }
                }
                rot.x = 0.0f;
                rot.y = fRotY;
                rot.z = 0.0f;
            }
            virtual long GetStartRouteId() const;

            // (name guessed) Animation nIndex is entry nAnim of s_ShipAnim
            // (none for -1).
            void SetAnim(long nIndex, long nAnim)
            {
                if (nAnim != -1) {
                    m_apAnim[nIndex] = &s_ShipAnim[nAnim];
                }
            }

        private:
            // 0x34: the ship leaving, arriving and the character getting off
            // (0 for none)
            const SHIP_ANIM *m_apAnim[3];
            float m_fRotY; // 0x40
    };

    // The two constructors are out of line, but the original has no copy
    // of them: CreateRoute is their only user, the compiler inlines them
    // there (-ipa file) and the module link strips them. Their literals
    // (0, 1) are the first of the file's pool (as in Stage32).
    CRouteShipMove::CRouteShipMove(const _PARAM &param, float rate) : CRouteBase(param.vStart, param.vEnd, -1)
    {
        m_apAnim[0] = 0;
        m_apAnim[1] = 0;
        m_apAnim[2] = 0;
        m_fRotY = param.fRotY;
        m_fRate = rate;
    }

    CRouteShipRide::CRouteShipRide(const _PARAM &param, float rate) : CRouteBase(param.vStart, param.vEnd, -1)
    {
        m_fRotY = param.fRotY;
        m_nShip = 0x28;
        m_bRide = 1;
        m_bSe = 0;
        m_fRate = rate;
    }

    CRouteShipMove::~CRouteShipMove()
    {
    }

    void CRouteShipMove::PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
        long nId = GetStartRouteId();
        long nEventId = param.pMove->GetMoveEventId(nId, 0);
        float fStart = param.pMove->GetRouteFrame();
        float fTime = 0.0f;
        const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(m_nRouteId);

        _PARAM route;
        unsigned char nPoint;
        if (flag & 1) {
            _SetParam(route, m_vEnd, m_vStart, 0);
            nPoint = pRouteInfo->nPoint2;
        } else {
            _SetParam(route, m_vStart, m_vEnd, 0);
            nPoint = pRouteInfo->nPoint1;
        }
        bcs::FVEC3 vEnd = route.vEnd;

        pRouteMan->_PushTimer(0x3E8, nEventId, fTime, 0, 0.0f, 0);

        // The ship leaves.
        if (m_apAnim[0] != 0) {
            const SHIP_EFFECT *pEffect = &s_ShipEffect[m_apAnim[0]->nEffect];
            long nChr = 0;
            switch (m_apAnim[0]->nType) {
                case 0:
                    pRouteMan->_PushTimer(0x1F, nEventId, fTime, 4, 1.0f / 60.0f,
                                          reinterpret_cast<unsigned long>(m_apAnim[0]));
                    break;
                case 1:
                    nChr = nPoint;
                    break;
            }

            if (param.ppPrev != 0 && *param.ppPrev != 0) {
                // It starts during the route before.
                long nPrevEventId = param.pMove->GetMoveEventId((*param.ppPrev)->GetRouteId() + 0x100, 0);
                pRouteMan->_PushTimer(0x1E, nPrevEventId, fTime, nChr, m_apAnim[0]->fWait,
                                      reinterpret_cast<unsigned long>(m_apAnim[0]));
                fTime += m_apAnim[0]->fWait;
                fTime -= (*param.ppPrev)->GetNumFrame();
            } else {
                pRouteMan->_PushTimer(0x1E, nEventId, fTime, nChr, m_apAnim[0]->fWait,
                                      reinterpret_cast<unsigned long>(m_apAnim[0]));
                fTime += m_apAnim[0]->fWait;
            }
            if (fTime < 0.0f) {
                fTime = 0.0f;
            }

            float fAnimStart = fTime;
            nw4r::g3d::ResAnmChr anm =
                pMan->GetObjModel(m_apAnim[0]->nObj)->GetResFile().GetResAnmChr(m_apAnim[0]->pAnim);
            pRouteMan->_PushTimer(0x1B, nEventId, fTime, nChr, 0.0f, reinterpret_cast<unsigned long>(m_apAnim[0]));
            if (pEffect->nEvent != -1) {
                pRouteMan->_PushTimer(0x3FE, nEventId, fTime + pEffect->fEventTime, param.pModel->GetIndex(), 0.0f,
                                      pEffect->nEvent);
            }
            PushShipSe(pRouteMan, nEventId, fTime, param, pEffect->nSe);
            if (anm.IsValid()) {
                fTime += anm.GetNumFrame();
            }
            if (m_apAnim[0]->nNext >= 0) {
                // A negative time counts from the animation's end.
                float fBase = m_apAnim[0]->fNextTime >= 0.0f ? fAnimStart : fTime;
                pRouteMan->_PushTimer(0x1F, nEventId, fBase + m_apAnim[0]->fNextTime, m_apAnim[0]->nNext,
                                      m_apAnim[0]->fNextParam, reinterpret_cast<unsigned long>(m_apAnim[0]));
            }
            fTime += m_apAnim[0]->fAfter;
            if (m_apAnim[0]->nType == 2) {
                pRouteMan->_PushTimer(0x24, nEventId, fTime, 0, 0.0f, reinterpret_cast<unsigned long>(m_apAnim[0]));
            }
        }

        // The ship arrives.
        if (m_apAnim[1] != 0) {
            const SHIP_EFFECT *pEffect = &s_ShipEffect[m_apAnim[1]->nEffect];
            long nChr = 0;
            if (m_apAnim[1]->nType == 1) {
                nChr = nPoint;
            }
            pRouteMan->_PushTimer(0x1E, nEventId, fTime, nChr, m_apAnim[1]->fWait,
                                  reinterpret_cast<unsigned long>(m_apAnim[1]));

            // Over a rainbow road: the rainbow at its end (map object 0x13
            // or 0x1D) lights up.
            long nRainbow = 0x13;
            fTime += m_apAnim[1]->fWait;
            float fAnimStart = fTime;
            switch (m_apAnim[1]->nType) {
                case 4:
                    nRainbow = 0x1D;
                    // fall through
                case 3:
                    pRouteMan->_PushTimer(0x24, nEventId, fTime, 1, 0.0f,
                                          reinterpret_cast<unsigned long>(m_apAnim[1]));
                    pRouteMan->_PushTimer(0x19, nEventId, 80.0f + fTime, nRainbow, 0.0f,
                                          reinterpret_cast<unsigned long>(m_apAnim[1]));
                    break;
            }

            nw4r::g3d::ResAnmChr anm =
                pMan->GetObjModel(m_apAnim[1]->nObj)->GetResFile().GetResAnmChr(m_apAnim[1]->pAnim);
            pRouteMan->_PushTimer(0x1B, nEventId, fTime, nChr, 0.0f, reinterpret_cast<unsigned long>(m_apAnim[1]));
            if (pEffect->nEvent != -1) {
                pRouteMan->_PushTimer(0x3FE, nEventId, fTime + pEffect->fEventTime, param.pModel->GetIndex(), 0.0f,
                                      pEffect->nEvent);
            }
            PushShipSe(pRouteMan, nEventId, fTime, param, pEffect->nSe);
            if (anm.IsValid()) {
                fTime += anm.GetNumFrame();
            }
            if (m_apAnim[1]->nNext >= 0) {
                // A negative time counts from the animation's end.
                float fBase = m_apAnim[1]->fNextTime >= 0.0f ? fAnimStart : fTime;
                pRouteMan->_PushTimer(0x1F, nEventId, fBase + m_apAnim[1]->fNextTime, m_apAnim[1]->nNext,
                                      m_apAnim[1]->fNextParam, reinterpret_cast<unsigned long>(m_apAnim[1]));
            }
            fTime += m_apAnim[1]->fAfter;
        }

        // The character rides along, hidden under the map.
        if (fTime > 0.0f) {
            route.vStart.y -= 100.0f;
            route.vEnd = route.vStart;
            CRouteWait *pWait = new CRouteWait(route, fTime, 0);
            pWait->SetRouteId(nId);
            param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
            nId += 0x100;
        }

        // Getting off.
        float fUp;
        float fDown;
        float fJumpFrame = TournamentData::GetJumpFrame(param.pModel, fUp, fDown);
        long nOffEventId = param.pMove->GetMoveEventId(nId, 0);
        float fOffStart = param.pMove->GetRouteFrame();
        if (m_apAnim[2] != 0) {
            const SHIP_EFFECT *pEffect = &s_ShipEffect[m_apAnim[2]->nEffect];
            nw4r::g3d::ResAnmChr anm =
                pMan->GetObjModel(m_apAnim[2]->nObj)->GetResFile().GetResAnmChr(m_apAnim[2]->pAnim);
            long nChr = 0;
            route.vStart = vEnd;
            route.vEnd = vEnd;
            if (m_apAnim[2]->nType == 1) {
                // Jump down from the ship onto the point.
                nChr = nPoint;
                const TournamentWork::POINT_INFO *pPoint = TournamentWork::GetPointInfo(nPoint);
                route.vStart.Set(pPoint->GetViewOfsX() + pPoint->vPos.x, pPoint->vPos.y,
                                 pPoint->GetViewOfsZ() + pPoint->vPos.z - 2.0f);
                if (anm.IsValid()) {
                    nw4r::g3d::ChrAnmResult result;
                    bcs::FVEC3 vOffset;
                    anm.GetAnmResult(&result, 1, 0.0f);
                    result.GetTranslate(reinterpret_cast<nw4r::math::VEC3 *>(&vOffset));
                    bcs::FVEC3Add(&route.vStart, &route.vStart, &vOffset);
                }
                route.fRotY = 0.0f;
                route.fLength = 2.0f;
                CRouteBasicJump *pJump = new CRouteBasicJump(route, route.fLength / fJumpFrame, 1);
                pJump->SetRouteId(nId);
                pJump->SetUpFrame(fUp);
                pJump->SetDownFrame(fDown);
                pJump->SetJumpParam(0.1f, 100.0f);
                pJump->CalcFrame();
                param.pMove->PushRoute(pJump, 2, 0.0f, 0.0f);

                unsigned long ulFlag = GetLandFlag(nPoint);
                float fLand = -10.0f + (param.pMove->GetRouteFrame() - fOffStart);
                pRouteMan->_PushTimer(0x3EA, nOffEventId, fLand, ulFlag, 0.0025f + route.vEnd.y, 0);
                pRouteMan->_PushTimer(0x3E9, nOffEventId, fLand, ulFlag, 1.0f / 24.0f, 0);

                route.vStart = route.vEnd;
                CRouteWait *pWait = new CRouteWait(route, 35.0f, 0);
                pWait->SetRouteId(nId + 0x100);
                param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
            }

            float fOffTime = param.pMove->GetRouteFrame() - fOffStart;
            pRouteMan->_PushTimer(0x1E, nOffEventId, fOffTime, nChr, m_apAnim[2]->fWait,
                                  reinterpret_cast<unsigned long>(m_apAnim[2]));
            fOffTime += m_apAnim[2]->fWait;
            float fAnimStart = fOffTime;
            pRouteMan->_PushTimer(0x1B, nOffEventId, fOffTime, nChr, 0.0f,
                                  reinterpret_cast<unsigned long>(m_apAnim[2]));
            if (pEffect->nEvent != -1) {
                pRouteMan->_PushTimer(0x3FE, nOffEventId, fOffTime + pEffect->fEventTime, param.pModel->GetIndex(),
                                      0.0f, pEffect->nEvent);
            }
            // (The original queues these sounds and the follow-up event
            // below on the first route event, not on nOffEventId.)
            PushShipSe(pRouteMan, nEventId, fOffTime, param, pEffect->nSe);
            if (anm.IsValid()) {
                fOffTime += anm.GetNumFrame();
            }
            if (m_apAnim[2]->nType == 1) {
                pRouteMan->_PushTimer(0x22, nOffEventId, fOffTime, m_apAnim[2]->nObj, 0.0f,
                                      reinterpret_cast<unsigned long>(m_apAnim[2]));
            }
            if (m_apAnim[2]->nNext >= 0) {
                // A negative time counts from the animation's end.
                float fBase = m_apAnim[2]->fNextTime >= 0.0f ? fAnimStart : fOffTime;
                pRouteMan->_PushTimer(0x1F, nEventId, fBase + m_apAnim[2]->fNextTime, m_apAnim[2]->nNext,
                                      m_apAnim[2]->fNextParam, reinterpret_cast<unsigned long>(m_apAnim[2]));
            }
        }

        m_fNumFrame = param.pMove->GetRouteFrame() - fStart;
    }

    long CRouteShipMove::GetStartRouteId() const
    {
        return m_nRouteId + 0x100;
    }

    CRouteShipRide::~CRouteShipRide()
    {
    }

    void CRouteShipRide::PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
        float fUp;
        float fDown;
        float fJumpFrame = TournamentData::GetJumpFrame(param.pModel, fUp, fDown);
        long nId = GetStartRouteId();
        long nEventId = param.pMove->GetMoveEventId(nId, 0);
        float fStart = param.pMove->GetRouteFrame();
        const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(m_nRouteId);

        _PARAM route;
        unsigned char nPoint;
        int bRide;
        if (flag & 1) {
            _SetParam(route, m_vEnd, m_vStart, 0);
            nPoint = pRouteInfo->nPoint2;
            bRide = !m_bRide;
        } else {
            _SetParam(route, m_vStart, m_vEnd, 0);
            nPoint = pRouteInfo->nPoint1;
            bRide = m_bRide;
        }
        bcs::FVEC3 vEnd = route.vEnd;

        if (bRide) {
            // Walk up to 4 before the ship ...
            if (route.fLength > 4.0f) {
                route.fLength -= 4.0f;
                bcs::BCS_VecZ_RotY(route.vEnd, route.fLength, route.fRotY);
                bcs::FVEC3Add(&route.vEnd, &route.vEnd, &route.vStart);
                CRouteBasicLine *pLine = new CRouteBasicLine(route, 0.17f, 0);
                pLine->SetRouteId(nId);
                param.pMove->PushRoute(pLine, 2, 0.0f, f2);
                nId += 0x100;
                route.vStart = route.vEnd;
                route.fLength = 4.0f;
            } else {
                route.vEnd = route.vStart;
                CRouteWait *pWait = new CRouteWait(route, 15.0f, 0);
                pWait->SetRouteId(nId);
                param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
                nId += 0x100;
            }
            float fTime = param.pMove->GetRouteFrame() - fStart;
            pRouteMan->_PushTimer(0x3E8, nEventId, -40.0f + fTime, 0, 1.0f / 24.0f, 0);
            pRouteMan->_PushTimer(0x20, nEventId, 40.0f + fTime, m_nShip, 0.0f, 0);
            if (m_bSe) {
                pRouteMan->_PushTimer(0x25, nEventId, 40.0f + fTime, param.pModel->GetIndex(), 0.0f, 0xE56);
            }

            // ... and jump on.
            route.vEnd = vEnd;
            CRouteBasicJump *pJump = new CRouteBasicJump(route, route.fLength / fJumpFrame, 1);
            pJump->SetRouteId(nId);
            pJump->SetUpFrame(fUp);
            pJump->SetDownFrame(fDown);
            pJump->SetJumpParam(1.0f, 0.7f);
            pJump->CalcFrame();
            param.pMove->PushRoute(pJump, 2, 0.0f, 0.0f);

            route.vStart = route.vEnd;
            CRouteWait *pWait = new CRouteWait(route, 10.0f, 0);
            pWait->SetRouteId(nId + 0x100);
            param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
        } else {
            // Jump down at most 4 ...
            float fLength = 4.0f;
            if (!(route.fLength > fLength)) {
                fLength = route.fLength;
            }
            bcs::BCS_VecZ_RotY(route.vEnd, fLength, route.fRotY);
            route.fLength = fLength;
            bcs::FVEC3Add(&route.vEnd, &route.vEnd, &route.vStart);
            CRouteBasicJump *pJump = new CRouteBasicJump(route, route.fLength / fJumpFrame, 1);
            pJump->SetRouteId(nId);
            pJump->SetUpFrame(fUp);
            pJump->SetDownFrame(fDown);
            pJump->SetJumpParam(0.1f, 100.0f);
            pJump->CalcFrame();
            param.pMove->PushRoute(pJump, 2, 0.0f, 0.0f);

            unsigned long ulFlag = GetLandFlag(nPoint);
            float fLand = -10.0f + (param.pMove->GetRouteFrame() - fStart);
            pRouteMan->_PushTimer(0x3EA, nEventId, fLand, ulFlag, 0.0025f + route.vEnd.y, 0);
            pRouteMan->_PushTimer(0x3E9, nEventId, fLand, ulFlag, 1.0f / 24.0f, 0);

            route.vStart = route.vEnd;
            CRouteWait *pWait = new CRouteWait(route, 35.0f, 0);
            pWait->SetRouteId(nId + 0x100);
            param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);

            // ... and walk the rest.
            route.vEnd = vEnd;
            bcs::FVEC3 vDiff;
            FVEC3Diff(vDiff, route.vEnd, route.vStart);
            route.fLength = bcs::FVEC3Len(&vDiff);
            if (route.fLength > 0.0f) {
                CRouteBasicLine *pLine = new CRouteBasicLine(route, 0.17f, 0);
                pLine->SetRouteId(nId + 0x200);
                param.pMove->PushRoute(pLine, 2, 0.0f, f2);
            }
        }

        m_fNumFrame = param.pMove->GetRouteFrame() - fStart;
    }

    long CRouteShipRide::GetStartRouteId() const
    {
        return m_nRouteId + 0x100;
    }

    void CTournamentRouteMan_Stage33::Initialize()
    {
        _CreateTimer(0x80);
        m_vRainbowPos.Set(0.0f, 0.0f, 0.0f);
        for (long i = 0; i < 4; i++) {
            m_afShipRot[i] = 0.0f;
            m_anShipPoint[i] = -1;
        }
        InitializeModel();
    }

    void CTournamentRouteMan_Stage33::InitializeModel()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();

        pMan->SetupObjModelEx(0, 0, "map_cup", 0, bcs::FVEC3(1.0f, 0.0f, -32.0f), 1, 2, 1, 0, -1);
        SetCrystalColor(0);
        // The turning bridges.
        pMan->SetupObjModel(0x1F, 0, "map_rot_bridge", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 1, 0, -1);
        pMan->SetupObjModel(0x20, 0, "map_rot_bridge", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 1, 0, -1);
        pMan->SetupObjModel(0x21, 2, "map_rot_bridge", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 1, 0, -1);
        pMan->SetupObjModel(0x22, 2, "map_rot_bridge", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 1, 0, -1);
        // The airships.
        pMan->SetupObjModelEx(0x27, 0, "map_ply_ship", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        pMan->SetupObjModel(0x28, 0, "map_ply_ship", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        pMan->SetupObjModel(0x29, 0, "map_ply_ship", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        pMan->SetupObjModel(0x2A, 0, "map_ply_ship", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        pMan->SetupObjModelEx(0x2B, 0, "map_enm_ship", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, "IDLE", -1);
        // The rainbow roads.
        pMan->SetupObjModelEx(4, 0x15, "rainbow", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, 0, -1);
        for (long i = 0x13; i <= 0x15; i++) {
            pMan->SetupObjModel(i, 0x14, "rainbow", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, 0, -1);
        }
        for (long i = 0x16; i <= 0x1A; i++) {
            pMan->SetupObjModelEx(i, 0x14, "rainbow", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, 0, -1);
        }
        for (long i = 0x1B; i <= 0x1D; i++) {
            pMan->SetupObjModel(i, 0x14, "rainbow", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, 0, -1);
        }
        for (long i = 5; i <= 0xB; i++) {
            pMan->SetupObjModel(i, i - 5, "rainbow", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, 0, -1);
        }
        for (long i = 0xC; i <= 0x12; i++) {
            pMan->SetupObjModel(i, i - 2, "rainbow", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, 0, -1);
        }
        for (long i = 0x2C; i <= 0x32; i++) {
            pMan->SetupObjModel(i, 0x1E, "rainbow", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, 0, -1);
        }
        // The sea of clouds.
        pMan->SetupObjModel(0x33, 0, "map_unkai", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, "s33_unkai01", -1);
        for (long i = 0x34; i <= 0x96; i++) {
            pMan->SetupObjModel(i, 0, "maker", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, -1, 0, 0, -1);
        }
    }

    int CTournamentRouteMan_Stage33::GetParamData(long nId, GET_PARAM &param)
    {
        return 0;
    }

    int CTournamentRouteMan_Stage33::SetParamData(long nId, const GET_PARAM &param)
    {
        return 0;
    }

    CRouteBase *CTournamentRouteMan_Stage33::CreateRoute(long nType, long nFrom, long nTo)
    {
        CRouteBase::_PARAM param;
        CRouteBase::_SetParam(param, nFrom, nTo, 0);

        switch (nType) {
            case 4:
            case 6:
            case 8:
            case 10: { // getting on (forwards) one of the four ships
                long nShip = (nType - 4) / 2;
                CRouteShipRide *pRoute = new CRouteShipRide(param, 1.0f);
                pRoute->SetShip(1, 1, nShip + 0x27);
                return pRoute;
            }
            case 5:
            case 7:
            case 9:
            case 11: { // getting off (forwards)
                long nShip = (nType - 5) / 2;
                CRouteShipRide *pRoute = new CRouteShipRide(param, 1.0f);
                pRoute->SetShip(0, 1, nShip + 0x27);
                return pRoute;
            }
            case 12: {
                CRouteShipRide *pRoute = new CRouteShipRide(param, 1.0f);
                pRoute->SetShip(1, 1, 0x27);
                return pRoute;
            }
            case 13: {
                CRouteShipRide *pRoute = new CRouteShipRide(param, 1.0f);
                pRoute->SetShip(0, 1, 0x27);
                return pRoute;
            }
            case 14: {
                CRouteShipRide *pRoute = new CRouteShipRide(param, 1.0f);
                pRoute->SetShip(1, 1, 0x29);
                return pRoute;
            }
            case 0x1D: case 0x1E: case 0x1F: case 0x20: case 0x21: case 0x22: case 0x23: case 0x24:
            case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C:
            case 0x2D: case 0x2E: case 0x2F: case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
            case 0x35: case 0x36: case 0x37: case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C:
            case 0x3D: case 0x3E: case 0x3F: case 0x40: case 0x41: case 0x42: case 0x43: case 0x44:
            case 0x45: case 0x46: case 0x47: case 0x48: case 0x49: case 0x4A: case 0x4B: { // airship flights
                long nRoute = nType - 0x1D;
                CRouteShipMove *pRoute = new CRouteShipMove(param, 1.0f);
                pRoute->SetAnim(0, s_ShipRouteAnim[nRoute][0]);
                pRoute->SetAnim(1, s_ShipRouteAnim[nRoute][1]);
                pRoute->SetAnim(2, s_ShipRouteAnim[nRoute][2]);
                return pRoute;
            }
            case 0xF:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15: // the rainbow roads on the left
                return CreateRainbowRoute(param, 8, nType - 0xF);
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1A: { // ... and on the right: the left ones mirrored
                CRouteDataMove *pRoute = CreateRainbowRoute(param, 8, nType - 0x16);
                pRoute->SetMirror(1);
                return pRoute;
            }
            case 0x1B:
            case 0x1C: {
                CRouteDataMove *pRoute = CreateRainbowRoute(param, 0x48, nType - 0x16);
                pRoute->SetMirror(1);
                return pRoute;
            }
            case 1: {
                CRouteJumpPoints *pRoute = new CRouteJumpPoints(param, 1.0f);
                pRoute->SetJumpData(&s_JumpData1);
                return pRoute;
            }
            case 2: {
                CRouteJumpPoints *pRoute = new CRouteJumpPoints(param, 1.0f);
                pRoute->SetJumpData(&s_JumpData2);
                return pRoute;
            }
            case 3: {
                CRouteJumpPoints *pRoute = new CRouteJumpPoints(param, 1.0f);
                pRoute->SetJumpData(&s_JumpData3);
                return pRoute;
            }
        }
        return _CreateRoute(param, nType, nFrom, nTo);
    }

    void CTournamentRouteMan_Stage33::_ShipAnimNodeCamera(unsigned long ulData, long nPoint, float fFrame)
    {
        const SHIP_ANIM *pAnim = reinterpret_cast<const SHIP_ANIM *>(ulData);
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pModel = pMan->GetObjModel(pAnim->nObj);
        if (pModel == 0) {
            return;
        }
        if (!pModel->GetResFile().GetResAnmChr(pAnim->pAnim).IsValid()) {
            return;
        }

        bcs::FVEC3 vOffset(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 vRot(0.0f, 0.0f, 0.0f);
        if (nPoint != 0) {
            SetPointViewPos(vOffset, TournamentWork::GetPointInfo(nPoint));
        }
        AddShipOffset(vOffset, pAnim);

        pMan->SetSceneData(pModel->GetResFile());
        short nCamera = pMan->GetSceneCameraIndex(0, pAnim->pAnim);
        if (nCamera == -1) {
            return;
        }
        unsigned long ulFlag = pAnim->bFlip ? 4 : 0;
        pMan->SetCameraActive(0);
        pMan->SetSceneOffset(vOffset, vRot);
        pMan->LeapDataSceneCamera(0, nCamera, ulFlag);
        pMan->SetCameraRateFrame(fFrame, pAnim->fCameraAdd, pAnim->fCameraMul, 0.0007f);
    }

    void CTournamentRouteMan_Stage33::_ShipAnimNode(unsigned long ulData, long nPoint, float fParam,
                                                    unsigned long ulParam, long nEvent, int bDisp)
    {
        static const long nEffSlot = 1;

        const SHIP_ANIM *pAnim = reinterpret_cast<const SHIP_ANIM *>(ulData);
        const SHIP_EFFECT *pEffect = &s_ShipEffect[pAnim->nEffect];
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pModel = pMan->GetObjModel(pAnim->nObj);
        if (pAnim->nObj == -1) {
            if (pModel != 0) {
                pModel->StopEffect(nEffSlot, nEffSlot + 1);
            }
            return;
        }
        if (pModel == 0) {
            return;
        }
        nw4r::g3d::ResAnmChr anm = pModel->GetResFile().GetResAnmChr(pAnim->pAnim);
        if (!anm.IsValid()) {
            return;
        }

        CEvNodeData *pNode = new CEvNodeData();
        float fNumFrame = anm.GetNumFrame();
        pNode->SetResAnm(anm);
        pModel->SetLinkNode(pNode, 1);

        bcs::FVEC3 vOffset(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 vRot(0.0f, 0.0f, 0.0f);
        if (nPoint != 0) {
            SetPointViewPos(vOffset, TournamentWork::GetPointInfo(nPoint));
        }
        AddShipOffset(vOffset, pAnim);
        pModel->SetMoveOffset(vOffset);
        pModel->SetMoveOffsetRot(vRot);
        pModel->SetMoveFlip(pAnim->bFlip);
        pModel->SetNode(1, 0.0f);
        pModel->SetUpdateMove(1);
        pModel->SetDisp(bDisp);

        if (pModel->GetEffcetId(nEffSlot) != pEffect->nEff) {
            pModel->EraseEffect(nEffSlot, nEffSlot + 1);
            if (pEffect->nEff != -1) {
                pModel->SetEffectHandle(
                    CEffectCtrl::CreateAttachEffectAuto(pEffect->nEff, static_cast<bcs::CMdlObj *>(pModel->GetScnObj()), 3),
                    nEffSlot);
                pModel->SetEffectId(pEffect->nEff, nEffSlot);
            }
        }
        if (fNumFrame > 0.0f) {
            _AddTimer(nEvent, m_nTimerGroup, m_fTimer + fNumFrame, pAnim->nObj, fParam, ulParam);
        }

        // The ship's own camera.
        short nCamera = pMan->GetSceneCameraIndex(0, pAnim->pAnim);
        if (nCamera == -1) {
            return;
        }
        unsigned long ulFlag = pAnim->bFlip ? 4 : 0;
        pMan->SetCameraActive(1);
        pMan->IsCameraEnd();
        pMan->SetSceneOffset(vOffset, vRot);
        pMan->SetDataSceneCamera(0, nCamera, 0.0f, ulFlag);
    }

    void CTournamentRouteMan_Stage33::_ShipAnimNodeAp(unsigned long ulData, long nPoint, float fParam,
                                                      unsigned long ulParam, long nEvent, int bDisp)
    {
        static const long nEffSlot = 1;

        const SHIP_ANIM *pAnim = reinterpret_cast<const SHIP_ANIM *>(ulData);
        const SHIP_EFFECT *pEffect = &s_ShipEffect[pAnim->nEffect];
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pSea = pMan->GetObjModel(0x33);
        CEventModel *pModel = pMan->GetObjModel(pAnim->nObjAp);
        if (pAnim->nObjAp == -1) {
            if (pSea != 0) {
                pSea->SetDisp(0);
            }
            if (pModel != 0) {
                pModel->StopEffect(nEffSlot, nEffSlot + 1);
            }
            return;
        }

        // The sea of clouds under the battle, mirrored with the ships.
        if (pSea != 0) {
            pSea->SetDisp(1);
            if (pAnim->bFlip) {
                pSea->SetScale(bcs::FVEC3(-1.0f, 1.0f, 1.0f));
            } else {
                pSea->SetScale(bcs::FVEC3(1.0f, 1.0f, 1.0f));
            }
        }

        if (pModel == 0) {
            return;
        }
        nw4r::g3d::ResAnmChr anm = pModel->GetResFile().GetResAnmChr(pAnim->pAnim);
        if (!anm.IsValid()) {
            return;
        }

        CEvNodeData *pNode = new CEvNodeData();
        float fNumFrame = anm.GetNumFrame();
        pNode->SetResAnm(anm);
        pModel->SetLinkNode(pNode, 1);

        bcs::FVEC3 vOffset(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 vRot(0.0f, 0.0f, 0.0f);
        if (nPoint != 0) {
            SetPointViewPos(vOffset, TournamentWork::GetPointInfo(nPoint));
        }
        AddShipOffset(vOffset, pAnim);
        pModel->SetMoveOffset(vOffset);
        pModel->SetMoveOffsetRot(vRot);
        pModel->SetMoveFlip(pAnim->bFlip);
        pModel->SetNode(2, 0.0f);
        pModel->SetUpdateMove(1);
        pModel->SetDisp(bDisp);

        if (pModel->GetEffcetId(nEffSlot) != pEffect->nEffAp) {
            pModel->EraseEffect(nEffSlot, nEffSlot + 1);
            if (pEffect->nEffAp != -1) {
                pModel->SetEffectHandle(
                    CEffectCtrl::CreateAttachEffectAuto(pEffect->nEffAp, static_cast<bcs::CMdlObj *>(pModel->GetScnObj()),
                                                        3),
                    nEffSlot);
                pModel->SetEffectId(pEffect->nEffAp, nEffSlot);
            }
        }
        if (fNumFrame > 0.0f) {
            _AddTimer(nEvent, m_nTimerGroup, m_fTimer + fNumFrame, pAnim->nObj, fParam, ulParam);
        }
    }

    void CTournamentRouteMan_Stage33::TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();

        // nParam of the rainbow road events: bits 0-3 the road, bit 4 run
        // backwards, bit 5 the right side.
        switch (nEvent) {
            case 0x18: {
                CEventModel *pModel = pMan->GetObjModel(nParam);
                if (pModel != 0) {
                    pModel->SetDrawLayer(2);
                }
                break;
            }
            case 0x14: { // where the rainbow roads start
                const TournamentWork::POINT_INFO *pPoint = TournamentWork::GetPointInfo(nParam);
                if (pPoint->HasViewPos()) {
                    m_vRainbowPos.Set(pPoint->GetViewOfsX() + pPoint->vPos.x, 0.0f,
                                      2.0f * -(pPoint->nViewZ - 0x16) + pPoint->vPos.z);
                }
                nParam = ulParam;
            }
                // fall through
            case 0x24: // the rainbow roads are shown or hidden
                for (long i = 4; i < 0x1F; i++) {
                    CEventModel *pModel = pMan->GetObjModel(i);
                    if (pModel != 0) {
                        pModel->SetGroupDisp(nParam);
                    }
                }
                break;
            case 0x13: { // a rainbow road appears
                bcs::FVEC3 pos = m_vRainbowPos;
                char szAnim[64];
                if (nParam >= 5 && nParam <= 0xB) {
                    sprintf(szAnim, "s33_L_road_%02d_LOOP", nParam - 5);
                } else if (nParam >= 0xC && nParam <= 0x12) {
                    sprintf(szAnim, "s33_R_road_%02d_LOOP", nParam - 0xC);
                } else if (nParam >= 0x13 && nParam <= 0x1D) {
                    const TournamentWork::POINT_INFO *pPoint = TournamentWork::GetPointInfo(ulParam);
                    if (!pPoint->HasViewPos()) {
                        break;
                    }
                    SetPointViewPos(pos, pPoint);
                    sprintf(szAnim, "s33_Rainbow_point");
                } else {
                    sprintf(szAnim, "");
                }
                _StageSet(pos, nParam, (3.1415927f / 180.0f) * fParam, szAnim, 0.0f, 8, 1);
                break;
            }
            case 0x15: { // ... the same, with the rainbow's light
                bcs::FVEC3 pos = m_vRainbowPos;
                char szAnim[64];
                if (nParam >= 5 && nParam <= 0xB) {
                    sprintf(szAnim, "s33_L_road_%02d_LOOP", nParam - 5);
                } else if (nParam >= 0xC && nParam <= 0x12) {
                    sprintf(szAnim, "s33_R_road_%02d_LOOP", nParam - 0xC);
                } else if (nParam >= 0x13 && nParam <= 0x1D) {
                    const TournamentWork::POINT_INFO *pPoint = TournamentWork::GetPointInfo(ulParam);
                    if (!pPoint->HasViewPos()) {
                        break;
                    }
                    SetPointViewPos(pos, pPoint);
                    sprintf(szAnim, "s33_Rainbow_point");
                    ulParam = (pPoint->nFlag >> 7) & 1;
                } else {
                    pMan->EraseEffect(0x1E8, 0x1E9);
                    if (ulParam != 0) {
                        CEffectCluster *pEffect = pMan->SetEffect(0x1E8, 0x58F, 3, 0, 0);
                        if (pEffect != 0) {
                            bcs::FVEC3 vEffPos;
                            vEffPos.x = pos.x;
                            vEffPos.y = pos.y + 9.0f;
                            vEffPos.z = pos.z + -52.5f;
                            pEffect->SetTranslate(&vEffPos);
                        }
                    }
                    sprintf(szAnim, "");
                }
                _StageSet(pos, nParam, (3.1415927f / 180.0f) * fParam, szAnim, 0.0f, 8, ulParam);
                break;
            }
            case 0x16: { // a character slides down a rainbow road
                static const long nEffSlot = 0;

                long nRoad = nParam & 0xF;
                int bRight = (nParam >> 5) & 1;
                int bRev = (nParam >> 4) & 1;
                CEventModel *pRoad = pMan->GetObjModel(nRoad + (bRight ? 0xC : 5));
                CEventModel *pChr = pMan->GetObjModel(nRoad + 0x2C);
                char szAnim[64];
                sprintf(szAnim, "eff_L%02d%s", nRoad, bRev ? "_rev" : "");
                nw4r::g3d::ResAnmChr anm = pChr->GetResFile().GetResAnmChr(szAnim);
                if (anm.IsValid()) {
                    bcs::FVEC3 vOffset = pRoad->GetPos();
                    long nFlip = 0;
                    if (bRight) {
                        nFlip = 1;
                        vOffset.x -= 2.0f;
                    }
                    float fNumFrame = anm.GetNumFrame();
                    CEvNodeData *pNode = new CEvNodeData();
                    pNode->SetResAnm(anm);
                    pChr->SetLinkNode(pNode, 1);
                    pChr->SetMoveOffset(vOffset);
                    pChr->SetMoveOffsetRot(pRoad->GetRot());
                    pChr->SetMoveFlip(nFlip);
                    pChr->SetNode(0, 0.0f);
                    pChr->SetUpdateMove(1);
                    _AddTimer(0x17, m_nTimerGroup, fNumFrame + m_fTimer, nParam, fParam, ulParam);
                    _AddTimer(0x19, m_nTimerGroup, fNumFrame + m_fTimer, ulParam, fParam, ulParam);
                }
                unsigned long hEffect = CEffectCtrl::CreateAttachEffect(0x57D, pChr->GetScnObj(), 0,
                                                                        pChr->GetNodeId("chara_set"), 0, 0, 0, 3, 0,
                                                                        0, 0);
                pChr->EraseEffect(nEffSlot, nEffSlot + 1);
                pChr->SetEffectHandle(hEffect, nEffSlot);
                long nSlot = pMan->GetSeSlot(0x19, 0x119);
                if (nSlot != -1) {
                    pMan->PlaySe(0xE51, (const bcs::FVEC3 *)0, nSlot, 0.0f, 1.0f, 0.0f, 1);
                }
                _AddTimer(0x2A, m_nTimerGroup, 8.0f + m_fTimer, nParam, fParam, ulParam);
                break;
            }
            case 0x17: { // ... and arrives
                static const long nEffSlot = 0;

                CEventModel *pChr = pMan->GetObjModel((nParam & 0xF) + 0x2C);
                unsigned long hEffect = pChr->GetEffcetHandle(nEffSlot);
                if (hEffect != 0) {
                    CEffectCluster *pEffect = CEffectCtrl::GetInstance()->FindList(hEffect);
                    if (pEffect != 0) {
                        pEffect->SetFadeoutTime(50.0f);
                    }
                    pChr->StopEffectForce(nEffSlot, nEffSlot + 1);
                }
                break;
            }
            case 0x2A: { // the road plays its slide animation
                int bRight = (nParam >> 5) & 1;
                int bRev = (nParam >> 4) & 1;
                long nObj = (nParam & 0xF) + (bRight ? 0xC : 5);
                char szAnim[64];
                sprintf(szAnim, "s33_%s_road_%02d%s", bRight ? "R" : "L", nParam & 0xF, bRev ? "_rev" : "");
                _StageAnim(szAnim, nObj, fParam, ulParam, 0x2B, 0, 0.0f, 8, 1);
                break;
            }
            case 0x2B: { // ... and goes back to its loop
                long nRoad = -1;
                char szAnim[64];
                if (nParam >= 5 && nParam <= 0xB) {
                    nRoad = nParam - 5;
                    sprintf(szAnim, "s33_L_road_%02d_LOOP", nRoad);
                } else if (nParam >= 0xC && nParam <= 0x12) {
                    nRoad = nParam - 0xC;
                    sprintf(szAnim, "s33_R_road_%02d_LOOP", nRoad);
                }
                if (nRoad != -1) {
                    _StageAnim(szAnim, nParam, fParam, ulParam, 0, 0, 0.0f, 8, 1);
                }
                break;
            }
            case 0x19: { // a rainbow lights up
                long nSlot = pMan->GetSeSlot(0x19, 0x119);
                if (nSlot != -1) {
                    pMan->PlaySe(0xE50, (const bcs::FVEC3 *)0, nSlot, 0.0f, 1.0f, 0.0f, 1);
                }
                _StageEff(0x57E, nParam, fParam, ulParam, 5, 0, 0);
                _AddTimer(0x2C, m_nTimerGroup, 100.0f + m_fTimer, nParam, fParam, ulParam);
                break;
            }
            case 0x2C:
                _StageAnim("s33_Rainbow_point", nParam, fParam, ulParam, 0, 0, 0.0f, 0x1F, 1);
                break;
            case 0xC: { // a bridge is put at its point
                const TournamentWork::POINT_INFO *pPoint = TournamentWork::GetPointInfo(ulParam);
                bcs::FVEC3 pos(pPoint->GetViewOfsX() + pPoint->vPos.x, 0.0f, pPoint->GetViewOfsZ() + pPoint->vPos.z);
                _StageSet(pos, nParam, (3.1415927f / 180.0f) * fParam, "", 0.0f, 1, 1);
                break;
            }
            case 0xD: { // a bridge turns with the character on it
                long nBridge = (nParam & 0xF) + 0x1F;
                char szAnim[64];
                sprintf(szAnim, "rot_bridge_%s90", (nParam & 0x20) ? "R" : "L");
                float fNumFrame = _StageAnimNode(szAnim, nBridge, fParam, ulParam, 0xE, 0.0f, 0, 1);
                _StageAttachChr(nBridge, ulParam, m_nUnk08, 1);
                _StageSe(0xE45, nBridge, fParam, ulParam, -1);
                _AddTimer(0x2F, m_nTimerGroup, fNumFrame + m_fTimer - 20.0f, nBridge, fParam, ulParam);
                break;
            }
            case 0x2F:
                _StageSe(0xE46, nParam, fParam, ulParam, -1);
                break;
            case 0xE:
                _StageAttachChr(nParam, ulParam, m_nUnk08, 0);
                break;
            case 0x22: { // a ship waits at its point (or is gone)
                long nShip = nParam - 0x27;
                long nPoint = m_anShipPoint[nShip];
                if (nPoint != -1) {
                    bcs::FVEC3 pos;
                    TournamentWork::GetPointViewPos(pos, nPoint);
                    _StageSet(pos, nParam, m_afShipRot[nShip], "IDLE", 0.0f, 1, 1);
                } else {
                    _StageDisp(nParam, 0);
                }
                pMan->StopSe(10.0f, 0x11D, 0x11E);
                pMan->StopSe(10.0f, 0x11E, 0x11F);
                pMan->StopSe(10.0f, 0x11F, 0x120);
                break;
            }
            case 0x1A: // where ship ulParam waits
                m_anShipPoint[ulParam] = nParam;
                m_afShipRot[ulParam] = (3.1415927f / 180.0f) * fParam;
                break;
            case 0x1B: // a ship animation starts (ulParam: its SHIP_ANIM)
                _ShipAnimNode(ulParam, nParam, fParam, ulParam, 0x1C, 1);
                _ShipAnimNodeAp(ulParam, nParam, fParam, ulParam, 0x1D, 1);
                break;
            case 0x1D: // ... and ends
                if (reinterpret_cast<const SHIP_ANIM *>(ulParam)->nType == 5) {
                    bcs::FVEC3 power(0.09f, 0.6f, 0.08f);
                    _StageQuake(power, 60.0f);
                }
                break;
            case 0x1E:
                _ShipAnimNodeCamera(ulParam, nParam, fParam);
                break;
            case 0x1F: // the screen fades out (0) for a flight and in again (4)
                if (nParam == 0) {
                    pMan->StopSe(40.0f, 0x11D, 0x11E);
                    pMan->StopSe(40.0f, 0x11E, 0x11F);
                    pMan->StopSe(40.0f, 0x11F, 0x120);
                    pMan->StopSe(40.0f, 0x120, 0x130);
                    CTournamentExit::m_bEnable = 0;
                }
                if (nParam == 4) {
                    StageDispSe(1);
                    TournamentWork::BgmPlay();
                    CTournamentExit::m_bEnable = 1;
                }
                bcs::CMainApp::Start3DFade(nParam, fParam);
                bcs::CMainApp::StartMasterFade(nParam, fParam);
                break;
            case 0x20:
                _StageAnim("OBJ_IN", nParam, fParam, ulParam, 0x21, 1, 0.0f, 1, 1);
                break;
            case 0x21:
                _StageAnim("IDLE", nParam, fParam, ulParam, 0, 1, 0.0f, 1, 1);
                break;
            case 0x26: // the ship sounds (ulParam: the sound)
                pMan->StopSe(0.0f, 0x11D, 0x11E);
                pMan->PlaySe(ulParam, (const bcs::FVEC3 *)0, 0x11D, 0.0f, 1.0f, 0.0f, 1);
                break;
            case 0x27:
                pMan->StopSe(0.0f, 0x11E, 0x11F);
                pMan->PlaySe(ulParam, (const bcs::FVEC3 *)0, 0x11E, 0.0f, 1.0f, 0.0f, 1);
                break;
            case 0x28:
                pMan->StopSe(0.0f, 0x11F, 0x120);
                pMan->PlaySe(ulParam, (const bcs::FVEC3 *)0, 0x11F, 0.0f, 1.0f, 0.0f, 1);
                break;
            case 0x29:
                pMan->StopSe(0.0f, 0x11D, 0x11E);
                // fall through
            case 0x25: {
                long nSlot = pMan->GetSeSlot(0x19, 0x119);
                if (nSlot != -1) {
                    pMan->PlaySe(ulParam, (const bcs::FVEC3 *)0, nSlot, 0.0f, 1.0f, 0.0f, 1);
                }
                break;
            }
            case 0x3F5: { // the rainbow glow on character ulParam
                CEventModel *pChr = pMan->GetChrModel(ulParam);
                CEffectCluster *pEffect = pMan->GetEffect(ulParam + 0x15);
                if (pEffect == 0) {
                    pEffect = pMan->SetEffect(ulParam + 0x15, 0x57A, 1, 0, 0);
                }
                if (pEffect != 0) {
                    pEffect->SetTranslate(&pChr->GetPos());
                }
                SetTeamOnShip(ulParam, 1);
                break;
            }
            case 0x3F3: {
                CEventModel *pChr = pMan->GetChrModel(ulParam);
                CEffectCluster *pEffect = pMan->GetEffect(ulParam + 0x15);
                if (pEffect == 0) {
                    pEffect = pMan->SetEffect(ulParam + 0x15, 0x579, 1, 0, 0);
                    long nSlot = pMan->GetSeSlot(0x19, 0x119);
                    if (nSlot != -1) {
                        pMan->PlaySe(0xE53, (const bcs::FVEC3 *)0, nSlot, 0.0f, 1.0f, 0.0f, 1);
                    }
                }
                if (pEffect != 0) {
                    pEffect->SetTranslate(&pChr->GetPos());
                }
                SetTeamOnShip(ulParam, 1);
                break;
            }
            case 0x3F4: {
                long nSlot = ulParam + 0x15;
                if (pMan->GetEffect(nSlot) != 0) {
                    CEventModel *pChr = pMan->GetChrModel(ulParam);
                    pMan->StopEffect(ulParam + 0x2A, ulParam + 0x2B);
                    pMan->StopEffect(nSlot, nSlot + 1);
                    CEffectCluster *pEffect = pMan->SetEffect(pMan->GetEffectSlot(0xDA, 0x1DA), 0x57B, 1, 0, 0);
                    if (pEffect != 0) {
                        pEffect->SetTranslate(&pChr->GetPos());
                    }
                    pMan->StopSe(0.0f, 0x11C, 0x11D);
                    long nSeSlot = pMan->GetSeSlot(0x19, 0x119);
                    if (nSeSlot != -1) {
                        pMan->PlaySe(0xE55, (const bcs::FVEC3 *)0, nSeSlot, 0.0f, 1.0f, 0.0f, 1);
                    }
                }
                SetTeamOnShip(ulParam, 0);
                break;
            }
            case 0x2D: {
                CEventModel *pObj = pMan->GetObjModel(nParam);
                CEffectCluster *pEffect = pMan->GetEffect(ulParam + 0x15);
                if (pEffect == 0) {
                    pEffect = pMan->SetEffect(ulParam + 0x15, 0x579, 1, 0, 0);
                    _StageSe(0xE53, nParam, fParam, ulParam, -1);
                }
                if (pEffect != 0) {
                    pEffect->SetTranslate(&pObj->GetPos());
                }
                SetTeamOnShip(ulParam, 1);
                break;
            }
            case 0x2E: {
                long nSlot = ulParam + 0x15;
                CEventModel *pChr = pMan->GetChrModel(ulParam);
                CEffectCluster *pEffect = pMan->GetEffect(nSlot);
                if (pEffect != 0) {
                    bcs::FVEC3 vOffset(0.0f, -0.01f, 0.0f);
                    for (long i = 0; i < pEffect->GetEffectNum(); i++) {
                        bcs::EF::CEffect *pEff = pEffect->GetEffect(i);
                        if (pEff != 0) {
                            pEff->SetParentMdl(pChr->GetScnObj(), 0, 1);
                            pEff->SetAttachStatus(1, 0, 0);
                            pEff->SetLocalTranslate(vOffset);
                        }
                    }
                } else {
                    pMan->AttachEffectModel(nSlot, pChr, 0x57A, 1, 0, 0, 0, 0, 0);
                }
                pMan->PlaySe(0xE54, (const bcs::FVEC3 *)0, 0x11C, 0.0f, 1.0f, 0.0f, 1);
                pMan->AttachEffectModel(ulParam + 0x2A, pChr, 0x57C, 1, 0, 0, 0, 0, 0);
                SetTeamOnShip(ulParam, 1);
                break;
            }
            case 0x3F6:
                m_nUnk584 = 0;
                break;
            case 0xF:
            case 0x1C:
            case 0x23:
                break;
            default:
                _DefTimerEvent(nEvent, nParam, fParam, ulParam, b);
                break;
        }
    }

    void CTournamentRouteMan_Stage33::PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel)
    {
        _DefPushCamera(pInfo, n, f, pModel);
    }

    void CTournamentRouteMan_Stage33::StageDispSe(int bDisp)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        if (bDisp) {
            pMan->PlaySe(0xE52, s_DispSePos, pMan->GetSeSlot(0x120, 0x130), 0.0f, 1.0f, 0.0f, 1);
        } else {
            pMan->StopSe(0.0f, 0x120, 0x130);
        }
    }
}
