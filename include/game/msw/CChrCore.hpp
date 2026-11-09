#ifndef MSW_CCHRCORE_HPP_
#define MSW_CCHRCORE_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CChrDrv;

    // One character on the court. A DOL class; each game module derives its
    // own (CChrCoreBoss, CChrCoreBasket, ...). bcs::CFsmQueue<CChrCore> is at
    // 0x00 and the two share the vptr at 0x5C; own members start at 0x60.
    //
    // INCOMPLETE: only what game/boss/CChrMarkerBoss.cpp needs is here.
    //  - The members stop at 0x175. The constructor in the DOL writes up to
    //    0x570, so the class must not be instantiated, embedded or derived
    //    from until the rest is added.
    //  - The virtual functions stop at slot 0xEC of about 800. Their order
    //    and names are the slots of __vt__Q23msw12CChrCoreBoss (BossF), read
    //    with the class name set back to CChrCore. All of them except the
    //    destructor, GetMainOrderValue and IsNowFallOut are unnamed inline
    //    functions in the DOL; they are declared without bodies here, so a
    //    unit that has to emit this vtable needs the bodies first.
    //  - Return types are not part of the mangled names: "int" stands for a
    //    DOL body that returns a word, "float" for one that loads f1, "void"
    //    for an empty one. All are guesses, as are the member names.
    class CChrCore : public bcs::CFsmQueue<CChrCore> {
        public:
            CChrCore();
            virtual ~CChrCore();                                     // 0x08
            virtual void DispFunc();                                 // 0x0C
            virtual long GetQueueWorkSize() const;                   // 0x10
            virtual long GetQueueStackSize() const;                  // 0x14
            virtual long GetMainOrderValue(long);                    // 0x18
            virtual void *GetHoldBall() const;                       // 0x1C
            virtual unsigned long GetHoldBallHandle() const;         // 0x20
            virtual void SetConditionEffect_(long);                  // 0x24
            virtual void SetConditionEffect_(long, unsigned long);   // 0x28
            virtual void GetSpRootPos(bcs::FVEC3 &) const;           // 0x2C
            virtual int IsNowBlockWork() const;                      // 0x30
            virtual int IsNowFallOut() const;                        // 0x34
            virtual int IsHoldBall() const;                          // 0x38
            virtual int IsHoldItem() const;                          // 0x3C
            virtual int IsHoldAny() const;                           // 0x40
            virtual int Is1on1() const;                              // 0x44
            virtual float Get1on1StartDist() const;                  // 0x48
            virtual int IsAirTrickSwingEnd() const;                  // 0x4C
            virtual int IsAirTrickFall() const;                      // 0x50
            virtual float GetAirTrickStatePassFrame() const;         // 0x54
            virtual void GetTargetMarkerPos(bcs::FVEC3 &);           // 0x58
            virtual int MoveTargetMarkerPos(float);                  // 0x5C
            virtual int AutoMoveTargetMarkerPos();                   // 0x60
            virtual long CheckGuideTelopDisp();                      // 0x64
            virtual int IsBattle() const;                            // 0x68
            virtual int IsDisp1On1Maker() const;                     // 0x6C
            virtual int IsDunkBlockEnable() const;                   // 0x70
            virtual int CanbePassRequest();                          // 0x74
            virtual int MethdPassRequest();                          // 0x78
            virtual float GetBlockWorkDir() const;                   // 0x7C
            virtual float GetJumpBlockLocalFrame() const;            // 0x80
            virtual int GetStarRemainderRate() const;                // 0x84
            virtual int IsAirTrickDunkMode() const;                  // 0x88
            virtual int IsVolleyRightBallTouch(long) const;          // 0x8C
            virtual int IsVolleyServeMode() const;                   // 0x90
            virtual int IsVolleyJumpServeMode() const;               // 0x94
            virtual int IsVolleyTargetMarkerActive() const;          // 0x98
            virtual long GetVolleyTargetMarkerNum() const;           // 0x9C
            virtual void GetVolleyTargetMarkerDefaultPos(long, bcs::FVEC3 &, int); // 0xA0
            virtual long GetVolleyTargetMarkerType() const;          // 0xA4
            virtual long GetVolleyTargetMarkerFixedType() const;     // 0xA8
            virtual int GetVolleyTargetMarkerPos(bcs::FVEC3 &, long); // 0xAC
            virtual int GetVolleyTargetMarkerMoved(bcs::FVEC3 &);    // 0xB0
            virtual int MoveVolleyTargetMarkerPos(float);            // 0xB4
            virtual int CanbeMoveVolleyTargetMarkerPos();            // 0xB8
            virtual int IsVolleyTargetMarkerStealthMode();           // 0xBC
            virtual float GetVolleySpikeAdsorbRangeXZ(int) const;    // 0xC0
            virtual float GetVolleySpikeAdsorbRangeY(int) const;     // 0xC4
            virtual int IsVolleySpikeJumpStart() const;              // 0xC8
            virtual int IsVolleySuperSpikeJump() const;              // 0xCC
            virtual int IsVolleySpikeSwingEnd() const;               // 0xD0
            virtual int IsVolleySpikeFall() const;                   // 0xD4
            virtual int IsVolleyAirTrickSwingEnd() const;            // 0xD8
            virtual int IsVolleyAirTrickFall() const;                // 0xDC
            virtual int IsVolleyBlockJumpStart() const;              // 0xE0
            virtual int IsVolleyBlockFall() const;                   // 0xE4
            virtual int IsVolleyBlockSuccess() const;                // 0xE8
            virtual int IsVolleyDispSpikeJumpMarker() const;         // 0xEC
            // ... the vtable goes on from here (see above).

            // Not zero when the character takes no part in the game: it has
            // no model yet (the word at 0x90 is zero) or bit 30 of the word
            // at 0xEC is set.
            int IsDisable() const;
            // The type the current driver reports; zero without a driver.
            // CChrMarkerBoss only shows markers for type 1 (most likely the
            // human players; not checked).
            long DriverGetType() const;

            // (names guessed) Inline in the original; the module reads the
            // members directly (CChrMarkerBoss::AppearCheck loads the byte
            // unsigned).
            long GetChrId() const { return m_nChrId; }
            unsigned long GetStatusFlag() const { return m_nStatusFlag; }

        protected:
            unsigned long m_Unk60;
            CChrDrv *m_pDriver[2];       // 0x64 (count guessed from the index's place)
            long m_nDriver;              // 0x6C, index of the driver in use, negative for none
            unsigned char m_Unk70[0x78];
            unsigned long m_nStatusFlag; // 0xE8, meaning of the bits not known
            unsigned char m_UnkEC[0x88];
            unsigned char m_nChrId;      // 0x174, a character number (meaning guessed)
    };
}

#endif /* !MSW_CCHRCORE_HPP_ */
