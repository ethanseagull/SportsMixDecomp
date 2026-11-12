#ifndef TOURNAMENTDATA_HPP_
#define TOURNAMENTDATA_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {
    class CEventModel;

    // Whether TournamentData is a namespace or a class is not known; the
    // mangled names are the same.
    namespace TournamentData {

        // Size 0x20. Where the tournament camera stands relative to the
        // point it follows. Layout from msw::CTournamentCamera::
        // SetValueCamera; the member names are guessed.
        struct CAMERA_INFO {
            bcs::FVEC3 ofs; // 0x00: added to the followed point
            bcs::FVEC3 rot; // 0x0C: radians; x and y give the viewing direction, z the roll
            float dist;     // 0x18
            float fovy;     // 0x1C
        };

        // (name guessed) Size 0x8. Two factors per character; what they
        // scale is not mapped yet (msw::CTournamentStep multiplies a
        // placement offset by a term made from the first).
        struct CHAR_INFO {
            float fUnk00; // 0x00
            float fUnk04; // 0x04
        };

        // The functions below set the pose of a character on the tournament
        // map (game/tour/TournamentData.cpp). All do nothing for a null
        // model. fFrame is the frame the motion starts at. Where there is
        // an nSide, 0 plays the motion as it is, 1 its mirror image and any
        // other value the idle motion. (parameter names guessed)

        // Gives the model the animations of msw::CLoadData entry `nRes`,
        // the victory scene's, and starts the idle motion.
        void SetVictoryRes(CEventModel *pModel, long nRes);
        void SetIdlePause(CEventModel *pModel, float fFrame);
        void SetGyaGyaPause(CEventModel *pModel, float fFrame);
        // These four return how many frames of the first motion are left,
        // 0 for a null model.
        float SetBattlePause(CEventModel *pModel, long nSide, float fFrame);
        float SetSecretOpenPause(CEventModel *pModel, long nSide, float fFrame);
        float SetLosePause(CEventModel *pModel, long nSide, float fFrame);
        float SetWinPause(CEventModel *pModel, long nSide, float fFrame);
        // These two return the length of the motion, 0 for a null model.
        float SetJumpPause(CEventModel *pModel, float fFrame);
        float SetBigGunPause(CEventModel *pModel, float fFrame);
        void SetVicotoryCupUpPause(CEventModel *pModel, float fFrame);
        void SetVicotorySupport1Pause(CEventModel *pModel, long nSide, float fFrame);
        void SetVicotorySupport2Pause(CEventModel *pModel, long nSide, float fFrame);

        // Timing of the jump motion: rUp receives the frame of the motion's
        // first trigger and rDown the number of frames from its second
        // trigger to its end; the result is the number of frames between
        // the two triggers. A null model gives the values of a 61-frame
        // motion with triggers at 11 and 51. (parameter names guessed)
        float GetJumpFrame(CEventModel *pModel, float &rUp, float &rDown);

        // Entry `nChr` of the table of 25; the first entry for a number
        // outside it. (reference or pointer is a guess)
        const CHAR_INFO &GetCharInfo(long nChr);

        // (added for TourF game/tour/CTournamentStep.cpp, which defines
        // it; names guessed) How far team nTeam stands from the centre of
        // map point nPoint: the point's own spacing, or a fixed step back
        // when another team stands there.
        float GetPointOffsetTeam(long nPoint, long nTeam);

        // (inline, name from its static local) The music of cup nCup's
        // map.
        inline long GetTournamentBgm(long nCup)
        {
            static long g_sTournamentMainBgm[5] = {0x33, 0x33, 0x34, 0x35, 0x33};
            return g_sTournamentMainBgm[nCup];
        }

        // One firework of a victory or unlock scene, 0x10 bytes; a list
        // of them ends with nType -1 (CTournamentStep::_SetupFireworks,
        // TourF). (names guessed)
        struct FIREWORKS_INFO {
            // 0x00: the kind (0-2, timer events -0x67 to -0x65); 3 for
            // one of them at random; -1 ends the list.
            signed char nType;
            // 0x01: the launch position, -1 for one at random.
            signed char nPos;
            short nRandStep;  // 0x02: how many steps of fStep the time may be put back
            float fStep;      // 0x04
            float fTime;      // 0x08: when it goes off
            float fScale;     // 0x0C
        };
    }
}

#endif /* !TOURNAMENTDATA_HPP_ */
