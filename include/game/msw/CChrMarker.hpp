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

            // Sets or clears bit 1 of m_nFlag; clearing it also resets a
            // field of the two objects at 0x80 and 0x84.
            void EnableViewCheck(int bEnable);

            // The colour of entry nIdx of a table in the DOL (m_Unk90 is
            // passed for it). A member function, though it does not use
            // this.
            bcs::BCS_Color GetColor_(long nIdx);

            // (DOL, static; name from the DOL) The resource file of the
            // marker models (HockeyF's CShootMarker loads its models from it).
            static nw4r::g3d::ResFile GetRsrcPack();

        protected:
            unsigned long m_nFlag; // 0x7C, bit 1: view check enabled
            // 0x80, the marker's model (Initialize creates it with
            // new bcs::CMdlObj; null before that)
            bcs::CMdlObj *m_pMdl;
            void *m_Unk84;         // 0x84, another of the same kind
            long m_nChr;           // 0x88, index for CChrMan::GetChrCore; negative for none
            long m_nType;          // 0x8C, third constructor argument (name guessed)
            long m_Unk90;          // 0x90, fourth constructor argument
            float m_Unk94;         // 0x94
            float m_Unk98;         // 0x98
            // 0x9C: nothing known writes it, but every module allocates its
            // derived markers (CChrMarkerBoss, Dodge, Hockey, Volley) with
            // 0xA0 bytes, and none of them adds members.
            unsigned long m_Unk9C;
    };
}

#endif /* !MSW_CCHRMARKER_HPP_ */
