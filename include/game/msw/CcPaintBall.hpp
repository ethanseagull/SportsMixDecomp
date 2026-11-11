#ifndef MSW_CCPAINTBALL_HPP_
#define MSW_CCPAINTBALL_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CChrObstacle.hpp"

namespace msw {
    class CChrCore;

    namespace CC_CMN {
        namespace KOOPAJR {

            // (DOL class; meaning guessed from the names) A ball of paint
            // Bowser Jr. throws in his special shot: it flies to a point on
            // the ground (state Fly), lies there as a patch of paint
            // (TouchDown) and fades (Vanish). 0x108 bytes (HockeyF's
            // CChrCoreHockeyKoopaJr allocates it). INCOMPLETE: the
            // constructor and the state functions are in main.dol and only
            // declared; members are mapped only where the modules touch them.
            // The pure virtuals at the end are the per-game tuning values the
            // modules' CcKoopaJrPaint classes give (return types guessed:
            // the two frame counts come back in r3).
            class CcPaintBall : public CChrObstacle, public bcs::CFsm<CcPaintBall> {
                public:
                    // nTeam is the thrower's team; bFirst is set for the
                    // first ball of a throw only (meaning guessed).
                    CcPaintBall(bcs::CTask *pParent, long nTeam, int bFirst);
                    virtual ~CcPaintBall();
                    virtual void Main();
                    virtual long SendMessage(long, long, long, long);

                    // Starts the ball at rFrom for rTo; nKind (0 to 3)
                    // picks its look (meaning guessed). Return type guessed.
                    virtual void Create(CChrCore *pOwner, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nKind);
                    virtual void SetAutoVanish(int bAutoVanish) { m_bAutoVanish = bAutoVanish; }

                    virtual void CtorStateFly();
                    virtual void LoopStateFly();
                    virtual void DtorStateFly();
                    virtual void CtorStateTouchDown();
                    virtual void LoopStateTouchDown();
                    virtual void DtorStateTouchDown();
                    virtual void CtorStateVanish();
                    virtual void LoopStateVanish();
                    virtual void DtorStateVanish();

                protected:
                    // nKind 0 to 2 (meaning not known); -1 for none.
                    virtual long GetEffectID_(long nKind) const = 0;
                    virtual long GetBallFlyFrmMin_() const = 0;
                    virtual long GetBallFlyFrmMax_() const = 0;
                    virtual float GetBallFlyPassHeight_() const = 0;
                    virtual float GetBallDispScale_() const = 0;
                    virtual float GetBallAtkCollRadius_() const = 0;
                    virtual float GetPaintDispScale_() const = 0;
                    virtual float GetPaintAtkCollRadius_() const = 0;
                    virtual float GetPaintLifeFrame_() const = 0;
                    virtual float GetPaintFadeOutFrame_() const = 0;
                    virtual float GetPaintFadeOutScale_() const = 0;

                    unsigned char m_UnkDC[0x100 - 0xDC];
                    int m_bAutoVanish; // 0x100
                    unsigned char m_Unk104[0x108 - 0x104];
            };
        }
    }
}

#endif /* !MSW_CCPAINTBALL_HPP_ */
