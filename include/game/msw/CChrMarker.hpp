#ifndef MSW_CCHRMARKER_HPP_
#define MSW_CCHRMARKER_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CMdlObj;
    struct BCS_Color;
}

namespace msw {

    // The marker shown above a character. A DOL class, size 0xA0:
    // bcs::CTask at 0x00, bcs::CFsm<CChrMarker> at 0x34 (vptr at 0x78), own
    // members from 0x7C (read from the constructor in the DOL). Each game
    // module derives its own marker from it.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw14CChrMarkerBoss (BossF). MainFunc is the only one with a
    // body here: the DOL's vtable has an unnamed one-instruction function in
    // its slot. Return types are read from the DOL bodies and callers; the
    // names of the parameters and members are guesses.
    class CChrMarker : public bcs::CTask, public bcs::CFsm<CChrMarker> {
        public:
            // The constructor ends by calling InitStep and Initialize.
            CChrMarker(bcs::CTask *pParent, long nChr, long nType, long nArg);
            virtual ~CChrMarker();
            virtual void Main();

            // Builds the state table (four states).
            virtual void InitStep();
            virtual void Initialize();
            virtual void Finalize();
            // Per-game work, empty here.
            virtual void MainFunc() {}
            virtual void ArrowSignalCheck();
            // Asked in the Sleep state: not zero when the marker should come
            // up.
            virtual int AppearCheck();
            // The opposite question, for the state in which it is shown.
            virtual int DisappearCheck();
            virtual float GetDispScale() const;

            // One Ctor/Loop/Dtor triple per state, in state order.
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateAppear();
            virtual void LoopStateAppear();
            virtual void DtorStateAppear();
            virtual void CtorStateMain();
            virtual void LoopStateMain();
            virtual void DtorStateMain();
            virtual void CtorStateDisappear();
            virtual void LoopStateDisappear();
            virtual void DtorStateDisappear();

        private:
            // (DOL fn_800F0614..fn_800F06C4, names guessed) The state table
            // holds pointers to these, not to the virtual hooks: each is a
            // tail call of its hook, so a derived class's override runs.
            // Inline: the DOL has them weak, after InitStep.
            void CtorStateSleep_() { CtorStateSleep(); }
            void LoopStateSleep_() { LoopStateSleep(); }
            void DtorStateSleep_() { DtorStateSleep(); }
            void CtorStateAppear_() { CtorStateAppear(); }
            void LoopStateAppear_() { LoopStateAppear(); }
            void DtorStateAppear_() { DtorStateAppear(); }
            void CtorStateMain_() { CtorStateMain(); }
            void LoopStateMain_() { LoopStateMain(); }
            void DtorStateMain_() { DtorStateMain(); }
            void CtorStateDisappear_() { CtorStateDisappear(); }
            void LoopStateDisappear_() { LoopStateDisappear(); }
            void DtorStateDisappear_() { DtorStateDisappear(); }

        public:
            // Sets or clears bit 1 of m_nFlag (Main then sets the view
            // flag of the models per split screen); clearing it also resets
            // the view flag of both models.
            void EnableViewCheck(int bEnable);
            // (DOL; BasketF's markers call it with 0 on stage 6) Sets the
            // fog index of the marker's model.
            void SetFogIdx(long nIdx);

            // The colour of entry nIdx of a table in the DOL (m_Unk90 is
            // passed for it). A member function, though it does not use
            // this.
            bcs::BCS_Color GetColor_(long nIdx);

            // (DOL, static; name from the DOL) The resource file of the
            // marker models (HockeyF's CShootMarker loads its models from it).
            static nw4r::g3d::ResFile GetRsrcPack();
            // (DOL, static) The marker colour of team nTeam (VolleyF's
            // CBallTargetMarkerRhythmReceive tints its mark with it).
            static bcs::BCS_Color GetTeamColor(long nTeam);

        protected:
            unsigned long m_nFlag; // 0x7C, bit 1: view check enabled
            // 0x80, the marker's model (Initialize creates it with
            // new bcs::CMdlObj; null before that)
            bcs::CMdlObj *m_pMdl;
            // 0x84 (name guessed): the "1P" to "4P" label model of a
            // human-controlled character, attached to m_pMdl (Initialize)
            bcs::CMdlObj *m_pPlayerMdl;
            long m_nChr;           // 0x88, index for CChrMan::GetChrCore; negative for none
            long m_nType;          // 0x8C, third constructor argument (name guessed)
            // 0x90, fourth constructor argument: the colour index (GetColor_);
            // negative for the team's (Initialize fills it in)
            long m_Unk90;
            // (names guessed) How far the marker has come up, 0 to 1, and
            // the phase of its pulsing while shown (radians).
            float m_fDispRate;     // 0x94
            float m_fWave;         // 0x98
            // 0x9C: nothing known writes it, but every module allocates its
            // derived markers (CChrMarkerBoss, Dodge, Hockey, Volley) with
            // 0xA0 bytes, and none of them adds members.
            unsigned long m_Unk9C;
    };
}

#endif /* !MSW_CCHRMARKER_HPP_ */
