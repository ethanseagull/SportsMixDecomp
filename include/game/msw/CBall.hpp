#ifndef MSW_CBALL_HPP_
#define MSW_CBALL_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CChrCore;

    // Arguments of the volleyball shots. (that BALL and VOLLEY are
    // namespaces and not classes is a guess, see notes/toolchain.md)
    namespace BALL {
        namespace VOLLEY {
            struct SERVE_TOSS_ARG;
            struct SERVE_SHOT_ARG;
            struct SPIKE_SHOT_ARG;
            struct PASS_SHOT_ARG;
            struct ENDURE_PRM;
        }
    }

    // The ball (or puck) of a game. A DOL class, size 0x318: bcs::CUnivTask
    // at 0x00, bcs::CFsmQueue<CBall> at 0x34 (vptr at 0x90), own members
    // from 0x94 (read from the constructor in the DOL and from
    // msw::CBallBoss's in BossF, whose own members start at 0x318). Each
    // game module derives its own ball from it.
    //
    // INCOMPLETE: only what game/boss/CBallManBoss.cpp needs is here.
    //  - The virtual functions stop at slot 0x170 of 0x4F0, so a class
    //    derived from this one cannot declare new virtual functions yet.
    //    Their order and names are the slots of __vt__Q23msw9CBallBoss
    //    (BossF), read with the class name set back to CBall.
    //  - Of the functions without a body, the ones marked "DOL" are DOL
    //    functions; the others are inline in the original (the modules emit
    //    them) and their bodies are not written yet, so a unit that has to
    //    emit this vtable needs them first.
    //  - Return types are not part of the mangled names: all of them are
    //    guesses from the names, except those of the two functions with a
    //    body. The names of the parameters and members are guesses too.
    class CBall : public bcs::CUnivTask, public bcs::CFsmQueue<CBall> {
        public:
            // Bits of m_nFlag.
            enum {
                // (name guessed) Set by SetSleepMode; a ball with this bit
                // is skipped by everything that looks for a ball in play.
                FLAG_SLEEP = 0x00000002,
                // Set and cleared by SetDestroyOnReset.
                FLAG_DESTROY_ON_RESET = 0x00020000
            };

            CBall(bcs::CTask *pParent, long nBallNo);
            virtual ~CBall();                                                   // 0x08 DOL
            virtual void Main();                                                // 0x0C DOL
            virtual void Disp();                                                // 0x10 DOL

            virtual void RestoreColorReg();                                     // 0x24 DOL
            virtual void Create(nw4r::g3d::ResFile);                            // 0x28
            virtual void Reset();                                               // 0x2C
            virtual float GetRadius() const;                                    // 0x30
            virtual float GetBgCollRadius() const;                              // 0x34
            virtual float GetBgCollRadiusXZ() const;                            // 0x38
            virtual float GetCollPushRadiusDefault() const;                     // 0x3C
            virtual float GetCollBodyRadiusDefault() const;                     // 0x40
            virtual float GetCollCatchRadiusDefault() const;                    // 0x44
            virtual float GetGravityDefault() const;                            // 0x48
            virtual void ChangeState(long);                                     // 0x4C DOL
            virtual void *GetRadarIconTexObj() const;                           // 0x50 DOL
            virtual void *GetMinimapIconTexObj() const;                         // 0x54 DOL
            virtual void GetTargetPos(bcs::FVEC3 &) const;                      // 0x58 DOL
            virtual void AttachItemEffect(long);                                // 0x5C DOL
            virtual void DetachItemEffect();                                    // 0x60 DOL
            virtual void SetPassMode(const bcs::FVEC3 &, long, float, long, long, int, int, unsigned long); // 0x64 DOL
            virtual void SetShootMode(long, float, long, long, unsigned long);  // 0x68 DOL
            virtual void SetShootMode(const bcs::FVEC3 &, long, long, unsigned long); // 0x6C DOL
            virtual void SetReShootMode(int);                                   // 0x70 DOL
            virtual const bcs::FVEC3 &GetShootStartPos() const;                 // 0x74
            virtual long GetShootCalcType() const;                              // 0x78
            virtual float GetShootCurveValue() const;                           // 0x7C
            virtual void CalcJumpShootVel(const bcs::FVEC3 &, const bcs::FVEC3 &, bcs::FVEC3 &, float &); // 0x80
            virtual void DoExplode(float, float);                               // 0x84 DOL
            virtual int IsAirBall() const;                                      // 0x88
            virtual void OnBaPad0Shot();                                        // 0x8C
            virtual void ResetBaPad0Shot();                                     // 0x90
            virtual int IsBaExplode() const;                                    // 0x94
            virtual int IsBaPad0Shot() const;                                   // 0x98
            virtual void SetDodgeShootMode(long, long, const bcs::FVEC3 &, long, long); // 0x9C
            virtual void SetDodgeShootMode(long, long, long, unsigned long);    // 0xA0
            virtual void SetDodgePowerShootMode(long, long, long, float, unsigned long); // 0xA4
            virtual void SetOutBallMode(const bcs::FVEC3 &, long, long);        // 0xA8
            virtual void SetFlickBallMode(float, float, float, float, long, long); // 0xAC
            virtual void SetDodgeReflectFreeMode(const bcs::FVEC3 &, float);    // 0xB0
            virtual long GetDodgeHitType(long) const;                           // 0xB4
            virtual void DodgeReattach(bcs::CMdlObj *, unsigned long);          // 0xB8
            virtual void DodgeOnPreTouch(long);                                 // 0xBC
            virtual void DodgeOnHit(long);                                      // 0xC0
            virtual void DodgeOnCatch(CChrCore *);                              // 0xC4
            virtual void DodgeOnRelease(CChrCore *);                            // 0xC8
            virtual long GetDodgeShootType() const;                             // 0xCC
            virtual void SetDodgeAddRange(float);                               // 0xD0
            virtual float GetDodgeAddRange() const;                             // 0xD4
            virtual void SetDodgeReceive(const bcs::FVEC3 &, float, float);     // 0xD8
            virtual void SetVolleyServeTossMode(BALL::VOLLEY::SERVE_TOSS_ARG &); // 0xDC
            virtual void SetVolleyServeShotMode(BALL::VOLLEY::SERVE_SHOT_ARG &); // 0xE0
            virtual void SetVolleySpikeShotMode(BALL::VOLLEY::SPIKE_SHOT_ARG &); // 0xE4
            virtual void SetVolleyPassShotMode(BALL::VOLLEY::PASS_SHOT_ARG &);  // 0xE8
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, float, long, float,
                                           unsigned long);                      // 0xEC
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long); // 0xF0
            virtual void SetVolleyArtsMode(const bcs::FVEC3 &, long, long, long, long, long, long, float, float, float,
                                           unsigned long);                      // 0xF4
            virtual void SetVolleyShootMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long); // 0xF8
            virtual void SetVolleyDropMode(const bcs::FVEC3 &, long, long, long, unsigned long); // 0xFC
            virtual void SetVolleyAnyHitMode(const bcs::FVEC3 *, long);         // 0x100
            virtual void SetVolleyVsBossTossUpMode(const bcs::FVEC3 &, long, unsigned long); // 0x104
            virtual void SetVolleyMissionRRShotMode(const bcs::FVEC3 &, float, float); // 0x108
            virtual void GetVolleyPosAfterFrame(bcs::FVEC3 &, float, int) const; // 0x10C
            virtual float GetVolleyCatchableRate() const;                       // 0x110
            virtual int CanbeVolleyStraightSpike(const bcs::FVEC3 &);           // 0x114
            virtual int CanbeVolleyReceiveVariation() const;                    // 0x118
            virtual int CanbeVolleySpikeVariation() const;                      // 0x11C
            virtual int CanebVolleyTouch(long) const;                           // 0x120 (spelt so in the symbol)
            virtual int CanbeVolleyPickup(long) const;                          // 0x124
            virtual long GetVolleyReceiveReaction(unsigned long, long) const;   // 0x128
            virtual long GetVolleyBlockReaction(unsigned long, long) const;     // 0x12C
            virtual long GetVolleyReceiveStaggerFrame(long) const;              // 0x130
            virtual long GetVolleyReceiveDropCoinNum(long) const;               // 0x134
            virtual float GetVolleyFlickDownPower(long) const;                  // 0x138
            virtual void GetVolleyEndureParam(BALL::VOLLEY::ENDURE_PRM &);      // 0x13C
            virtual long GetVolleyPassCounter() const;                          // 0x140
            virtual void GetVolleyBallMarkerDispFrame(float &, float &) const;  // 0x144
            virtual void TrgVolleyReceiveReaction(long, unsigned long);         // 0x148
            virtual void TrgVolleyBlockReaction(unsigned long);                 // 0x14C
            virtual long GetVolleyBlockDropCoinNum(long) const;                 // 0x150
            virtual void TrgVolleyPreparationTouch(long, long, long, long);     // 0x154
            virtual void TrgVolleyArtsAttach(long, long, long, int);            // 0x158
            virtual void TrgVolleyArtsReady(long, int);                         // 0x15C
            virtual void TrgVolleyArtsOver(long, long, unsigned long);          // 0x160
            virtual int IsVolleyArtsAttach() const;                             // 0x164
            virtual int IsVolleyFakeBall() const;                               // 0x168
            virtual int IsVolleyReqDelete() const { return 0; }                 // 0x16C
            virtual int IsVolleyVanish() const { return 0; }                    // 0x170
            // Slots 0x174 to 0x4F0 are not declared yet.

            // Loads the ball's model and builds the ball. A negative
            // `nModel` takes the ball's own number. (what the second
            // argument selects is not known; the boss game passes 0x40)
            void Create(long nModel, long);

            // (name guessed) Not zero for a ball that is out of play.
            int IsSleep() const { return m_nFlag & FLAG_SLEEP; }
            // (name guessed) Not zero after SetDestroyOnReset(1).
            int IsDestroyOnReset() const { return m_nFlag & FLAG_DESTROY_ON_RESET; }
            // (name guessed)
            const bcs::FVEC3 &GetPos() const { return m_Pos; }

        protected:
            unsigned long m_nFlag;         // 0x94
            long m_nBallNo;                // 0x98, the ball's place in CBallMan's table
            unsigned char m_Unk9C[0x5C];   // 0x9C
            bcs::FVEC3 m_Pos;              // 0xF8
            unsigned char m_Unk104[0x214]; // 0x104
    };
}

#endif /* !MSW_CBALL_HPP_ */
