// msw::CChrCoreHockeyMoogle: the Moogle in the hockey game. His special
// shot plays a charge sound and three motions before the shot, with its own
// effects.
#include "game/msw/CChrCoreHockey.hpp"

namespace {

    // (guessed) The original .rodata ends with 0.5, -1, 90, 100 and 6, which
    // nothing left in the file reads (most likely used by code the link
    // stripped). These dead comparisons, made from the last function, keep
    // them in place. See notes/decisions.md, "Unreferenced data kept by dead
    // references".
    inline void UnusedReferences(float f)
    {
        if (f == 0.5f) {
        }
        if (f == -1.0f) {
        }
        if (f == 90.0f) {
        }
        if (f == 100.0f) {
        }
        if (f == 6.0f) {
        }
    }
}

msw::CChrCoreHockeyMoogle::CChrCoreHockeyMoogle()
{
}

msw::CChrCoreHockeyMoogle::~CChrCoreHockeyMoogle()
{
}

void msw::CChrCoreHockeyMoogle::Reset_()
{
    CChrCoreHockey::Reset_();
}

void msw::CChrCoreHockeyMoogle::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xB5, 3);
    SE_PlayArtsJumpSE(0);
}

// The trick is three motions. In the first the charge sound starts at frame
// 40 (once); at its end come motion 0x9D and the charge effects, then motion
// 0x9E, and the shot is ready when that is over.
int msw::CChrCoreHockeyMoogle::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (!SE_IsPlay(9) && (IsAnimEnd(3) || AnimGetFrame(3) >= 40.0f)) {
                SE_PlayMove(9, 0x45F, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
            }
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xB6, 3);
                Eff_CreateAttachAuto(0x15, 0xB7, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9E, 0, 0.0f, 0.0f);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3)) {
                pWork->m_nTrickStep++;
                bReady = 1;
            }
            break;
        default:
            bReady = 1;
            break;
    }
    return bReady;
}

void msw::CChrCoreHockeyMoogle::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x16, 0xB8, 3);
    SE_Stop(9, 0.0f);
}

// The charge effect goes with the shot, which leaves a burst behind.
void msw::CChrCoreHockeyMoogle::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotShootBall_(pWork);

    Eff_Erase(0x15);
    Eff_CreateSimple(0x602, GetPos(), 3, 0, 0);
    UnusedReferences(pWork->m_fShotFrame);
}
