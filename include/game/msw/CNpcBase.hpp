#ifndef MSW_CNPCBASE_HPP_
#define MSW_CNPCBASE_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // INCOMPLETE: a non-player character on the court (the boss Packun,
    // ...). A DOL class and a task (its constructor
    // __ct__Q23msw8CNpcBaseFPQ23msw7CChrManl calls bcs::CTask's). Size
    // 0x188: the constructor's last store is to 0x184, and
    // msw::CNpcBossPackun's state queue starts at 0x188. Only part of its
    // virtual functions (below) and of its members are declared, so it must
    // not be instantiated or derived from with a vtable emitted yet.
    class CBall;

    class CNpcBase : public bcs::CTask {
        public:
            // Virtual functions 0x18 to 0x50, in the order and with the names
            // of the slots of __vt__Q23msw16CNpcStringBasket (BasketF), for
            // BasketF's CBallBasketFruit, which calls CanbeCatch. Return
            // types guessed. (the class's destructor and later slots are not
            // declared yet)
            // DOL; overrides of bcs::CTask's (slots 0x0C and 0x10 of the
            // BossF vtables of the classes derived from it).
            virtual void Main();
            virtual void Disp();

            virtual void Create();                                     // 0x18
            virtual long GetNpcType() const;                           // 0x1C
            virtual void Reset();                                      // 0x20
            virtual void SendMessage(long, long, long, long, long);    // 0x24
            virtual long GetThinkLevel() const;                        // 0x28
            virtual void AnimPlay(long, int, float, float);            // 0x2C
            virtual void AnimStop();                                   // 0x30
            virtual void AnimSetSpeed(float);                          // 0x34
            virtual void AnimSetFrame(float);                          // 0x38
            virtual void AttachBall(CBall *);                          // 0x3C
            virtual void AttachBall(CBall *, long, float, unsigned long); // 0x40
            virtual void DetachBall();                                 // 0x44
            virtual int IsHaveBall() const;                            // 0x48
            virtual int IsHoldBall() const;                            // 0x4C
            virtual int CanbeCatch() const;                            // 0x50

            // (names guessed) Read by CStageBasketHungryBossPackun::Main_ in
            // BasketF to make the goal follow the boss.
            const bcs::FVEC3 &GetPos() const { return m_vPos; }
            const bcs::FVEC3 &GetRot() const { return m_vRot; }

        protected:
            unsigned char m_Unk34[0x60 - 0x34];
            bcs::FVEC3 m_vPos;   // 0x60
            bcs::FVEC3 m_vRot;   // 0x6C, written by RotateYRate
            bcs::FVEC3 m_vScale; // 0x78, written by SetScale
            unsigned char m_Unk84[0x188 - 0x84];
    };
}

#endif /* !MSW_CNPCBASE_HPP_ */
