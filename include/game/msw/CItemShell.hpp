#ifndef MSW_CITEMSHELL_HPP_
#define MSW_CITEMSHELL_HPP_

#include "game/msw/CItem.hpp"

namespace msw {

    // What the green and the red shell share. A DOL class. The overrides
    // and new virtuals are read from __vt__Q23msw15CItemBossGShell and
    // __vt__Q23msw15CItemBossRShell (BossF); return types are guesses except
    // for the inline bodies, which the modules emit.
    //
    // Size 0x1B8 (CItemGShell is allocated with that size and adds nothing
    // the module code shows); where CItem ends and this class begins is not
    // known.
    class CItemShellCmn : public CItem {
        public:
            virtual float GetAtkCollRadius() const;
            virtual bool CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual long GetParamThrowTrackType() const { return 1; }
            virtual void CtorStateBreak();
            virtual void LoopStateBreak();

            virtual void Create_(long, const bcs::FVEC3 &rPos);

        protected:
            unsigned char m_Unk1AC[0x1B4 - 0x1AC];
            float m_fThrowTime; // 0x1B4, (name guessed) set by the boss shells' DoThrowInit
    };

    // The green shell (item type 0). A DOL class.
    class CItemGShell : public CItemShellCmn {
        public:
            CItemGShell(CItemMan *pMan);
            virtual ~CItemGShell();

            virtual void Create(const bcs::FVEC3 &rPos);
            virtual long GetItemType() const { return 0; }
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();
            virtual void DtorStateThrow();

            virtual float GetThrowDir_(const bcs::FVEC3 &, float, int, long) const;
    };

    // The red shell (item type 1), which homes in on a target. A DOL class
    // of size 0x1CC.
    class CItemRShell : public CItemShellCmn {
        public:
            CItemRShell(CItemMan *pMan);
            virtual ~CItemRShell();

            virtual void Create(const bcs::FVEC3 &rPos);
            virtual long GetItemType() const { return 1; }
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();
            virtual void DtorStateThrow();

            virtual void DoHoming_();
            // (return type guessed) The result is kept at 0x1C0.
            virtual long SearchTarget_(const bcs::FVEC3 &rPos, float fDir, int, long);
            virtual void GetHomingTargetPos(bcs::FVEC3 &rPos);

        protected:
            unsigned char m_Unk1B8[0x1C0 - 0x1B8];
            long m_nTarget;    // 0x1C0, what SearchTarget_ returned
            unsigned char m_Unk1C4[4];
            long m_nUnk1C8;    // 0x1C8, (meaning unknown) tested by the boss DoThrowInit
    };
}

#endif /* !MSW_CITEMSHELL_HPP_ */
