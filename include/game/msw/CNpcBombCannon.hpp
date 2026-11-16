#ifndef MSW_CNPCBOMBCANNON_HPP_
#define MSW_CNPCBOMBCANNON_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CNpcBase.hpp"

namespace msw {

    class CChrMan;

    // (DOL) The bomb cannon: an NPC of type 0xC that sleeps, idles for a
    // random time and then fires. Size 0x1E0: msw::CNpcBase at 0x00,
    // bcs::CFsm<CNpcBombCannon> at 0x188 (vptr at 0x1CC), own members from
    // 0x1D0 (read from the constructor). The names of the state hooks
    // without a symbol (Idle and Fire Ctor/Loop) follow the named ones.
    class CNpcBombCannon : public CNpcBase, public bcs::CFsm<CNpcBombCannon> {
        public:
            CNpcBombCannon(CChrMan *pMan);
            virtual ~CNpcBombCannon();

            virtual void Create();
            virtual long GetNpcType() const { return 0xC; }
            virtual void Reset();
            virtual long SendMessage(long nMsg, long, long, long, long);
            virtual void MainFunc();

            // One Ctor/Loop/Dtor triple per state, in state order.
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateIdle();
            virtual void LoopStateIdle();
            virtual void DtorStateIdle();
            virtual void CtorStateFire();
            virtual void LoopStateFire();
            virtual void DtorStateFire();

        private:
            // (names guessed) Builds the state table (three states).
            void InitStep();
            // (name guessed) The model of the NPC slot holding the cannon,
            // its motion, shadow and reflection.
            void CreateMdlObj_();

            // (DOL fn_80255EA8..fn_80255F28, names guessed) The state table
            // holds pointers to these: each is a tail call of its virtual
            // hook.
            void CtorStateSleep_() { CtorStateSleep(); }
            void LoopStateSleep_() { LoopStateSleep(); }
            void DtorStateSleep_() { DtorStateSleep(); }
            void CtorStateIdle_() { CtorStateIdle(); }
            void LoopStateIdle_() { LoopStateIdle(); }
            void DtorStateIdle_() { DtorStateIdle(); }
            void CtorStateFire_() { CtorStateFire(); }
            void LoopStateFire_() { LoopStateFire(); }
            void DtorStateFire_() { DtorStateFire(); }

            unsigned long m_Unk1D0; // 0x1D0, zero (nothing in the file reads it)
            float m_fUnk1D4;        // 0x1D4
            // 0x1D8 (name guessed): frames left in the Idle state before
            // it fires.
            float m_fWait;
            float m_fUnk1DC;        // 0x1DC
    };
}

#endif /* !MSW_CNPCBOMBCANNON_HPP_ */
