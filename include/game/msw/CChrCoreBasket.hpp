#ifndef MSW_CCHRCOREBASKET_HPP_
#define MSW_CCHRCOREBASKET_HPP_

#include "game/msw/CChrCore.hpp"

namespace msw {

    class CBall;

    // The basketball character core (BasketF).
    //
    // INCOMPLETE: only what game/basket/CComDrvBasket.cpp calls is
    // declared. CChrCore is incomplete too, so the class must not be
    // instantiated or have its vtable emitted yet.
    class CChrCoreBasket : public CChrCore {
        public:
            virtual ~CChrCoreBasket();

            // Not zero when pChr (holding the ball) is in reach of a cut
            // (steal) by this character. (meaning guessed from the name)
            int CutHitCheck(const CChrCore *pChr) const;

            // Added for game/basket/CChrDrvPadBasket.cpp (BasketF, all in
            // this module; return types guessed). Not zero when the
            // character takes a pass from a partner automatically.
            int IsAutoChrChangeToPassCatch() const;
            // Starts the combination pass in direction fRotY; zero when it
            // cannot.
            int MethdCombinationPass(float fRotY);

            // Slots 0x978 to 0xA7C of __vt__Q23msw14CChrCoreBasket. They are
            // CChrCore's virtual functions (the DOL names many of them
            // CChrCore::), declared here rather than in CChrCore.hpp so that
            // the placeholder slots of other modules' derived classes
            // (CChrCoreDodge, CChrCoreBoss) do not move. CChrCoreBasket adds
            // nothing in front of them, so the offsets are the same.
            virtual int MethdGameSet();                                // 0x978
            virtual int MethdGameResult(int);                          // 0x97C
            virtual int CanbeChgLeaderState();                         // 0x980
            virtual int CanbeChgNewLeaderState();                      // 0x984
            virtual int CanbeChgLeader(CChrCore *);                    // 0x988
            virtual int MethdChgLeader(float, long, long, int);        // 0x98C
            virtual int MethdChgLeader(long, long, long);              // 0x990
            virtual int MethdChgLeader();                              // 0x994
            virtual int CanbeServe(long);                              // 0x998
            virtual int MethdServe(long);                              // 0x99C
            virtual int CanbeReceive(int);                             // 0x9A0
            virtual int CanbeDiveReceive();                            // 0x9A4
            virtual int MethdReceive(int, int);                        // 0x9A8
            virtual int MethdReceive(int, int, float);                 // 0x9AC
            virtual int CanbeSpike(int) const;                         // 0x9B0
            virtual int MethdSpike(int);                               // 0x9B4
            virtual int CanbeAutoSpikeJump() const;                    // 0x9B8
            virtual int Canbe1on1();                                   // 0x9BC
            virtual int Methd1on1(int);                                // 0x9C0
            virtual int CanbeAutoDPM();                                // 0x9C4
            virtual int MethdAutoDPM(float, float);                    // 0x9C8
            virtual int CanbeBlockWorkLose();                          // 0x9CC
            virtual int MethdBlockWorkLose();                          // 0x9D0
            virtual int CanbeJumpBlock();                              // 0x9D4
            virtual int MethdJumpBlock(float);                         // 0x9D8
            virtual int CanbeSwatIt() const;                           // 0x9DC
            virtual int MethdSwatIt();                                 // 0x9E0
            virtual int CanbeSmashCut();                               // 0x9E4
            virtual int MethdSmashCut(float);                          // 0x9E8
            virtual int MethdAirTrickDownSpeed(float);                 // 0x9EC
            virtual int CanbeSpecialBattle();                          // 0x9F0
            virtual int MethdSpecialBattle();                          // 0x9F4
            virtual int CanbeArtsShotCatch();                          // 0x9F8
            virtual int MethdArtsShotCatch();                          // 0x9FC
            virtual int CanbeBallCatch();                              // 0xA00
            virtual int MethdBallCatch(float);                         // 0xA04
            virtual int CanbeLineOver();                               // 0xA08
            virtual int MethdLineOver(float);                          // 0xA0C
            virtual int CanbeCrouch();                                 // 0xA10
            virtual int MethdCrouch();                                 // 0xA14
            virtual int CanbeSkate(int);                               // 0xA18
            virtual int MethdSkate(float, int, float, int);            // 0xA1C
            virtual int CanbeStealStance();                            // 0xA20
            virtual int MethdStealStance(float);                       // 0xA24
            virtual int CanbeShootStance();                            // 0xA28
            virtual int MethdShootStance(float, int);                  // 0xA2C
            virtual int CanbeBrake();                                  // 0xA30
            virtual int MethdBrake(float, long);                       // 0xA34
            virtual int CanbeSwitchBrake(float, int);                  // 0xA38
            virtual int MethdSwitchBrake(float, float);                // 0xA3C
            virtual int CanbeSwitch();                                 // 0xA40
            virtual int MethdSwitch();                                 // 0xA44
            virtual int CanbeBlockStance();                            // 0xA48
            virtual int MethdBlockStance(float, float, long);          // 0xA4C
            virtual int CanbeAutoBlockStance();                        // 0xA50
            virtual int MethdAutoBlockStance(float, float, long);      // 0xA54
            virtual int CanbeAutoChangeStance();                       // 0xA58
            virtual int MethdAutoChangeStance(float, float, long, float); // 0xA5C
            virtual int MethdArtsDamage(unsigned long, bcs::FVEC3);    // 0xA60
            virtual int CanbeChangeStance();                           // 0xA64
            virtual int MethdChangeStance(float, float, long, float);  // 0xA68
            virtual int CanbePassCatchState();                         // 0xA6C
            virtual int CanbeAlleyoop();                               // 0xA70
            virtual int MethdSetPassCatchState(CBall *, int, const bcs::FVEC3 &, long); // 0xA74
            virtual int MethdSetPassCatchJumpWaitFrame(const bcs::FVEC3 &, float); // 0xA78
            virtual int MethdAlleyoop();                               // 0xA7C

            // 0x57C (name guessed): flag bits; with 0x800 set a catch can be
            // followed by an aerial pass (CChrDrvPadBasket::MethdPass).
            unsigned char m_Unk574[0x57C - 0x574];
            unsigned long m_nBasketFlag;
    };
}

#endif /* !MSW_CCHRCOREBASKET_HPP_ */
