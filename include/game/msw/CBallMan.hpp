#ifndef MSW_CBALLMAN_HPP_
#define MSW_CBALLMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    struct BCS_Color;
}

namespace msw {

    class CBall;

    // Owner of the balls of a game: a table of the task ids of up to
    // `nBallMax` balls. A DOL class, size 0x58: bcs::CUnivTask at 0x00, own
    // members from 0x34 (read from the constructor and the small functions
    // in the DOL). Each game module derives its own manager from it.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw12CBallManBoss (BossF). The functions without a body are
    // DOL functions. The functions with a body here are inline in the
    // original: the DOL's own CBallMan vtable points to unnamed copies of
    // them, and the modules emit them as weak functions. Return types are not part of the mangled names: those of
    // the functions with a body are read from the body ("int" for a zero,
    // "void" for nothing), those of the DOL functions from their code or
    // guessed. The names of the parameters and members are guesses.
    class CBallMan : public bcs::CUnivTask {
        public:
            CBallMan(bcs::CTask *pParent, long nBallMax);
            virtual ~CBallMan();
            // (guessed from CBallManBoss::MainFunc_) Deletes the balls that
            // asked for it and calls MainFunc_.
            virtual void Main();

            // Per-game work, called at the end of Main.
            virtual void MainFunc_() {}
            virtual void DeleteAllBall_();
            // The size of the table, not the number of balls in it.
            virtual long GetBallNum_() const { return m_nBallNum; }
            virtual long GetActiveBallNum_();
            virtual unsigned long GetBallHandle_(long nBall);
            virtual CBall *GetBallPtr_(long nBall);
            virtual const CBall *GetBallPtr_(long nBall) const;
            virtual CBall *GetNearPosBall_(const bcs::FVEC3 &) { return 0; }
            virtual CBall *GetFarPosBall_(const bcs::FVEC3 &) { return 0; }
            virtual CBall *GetNearTargetPosBall_(const bcs::FVEC3 &) { return 0; }
            virtual CBall *GetFarTargetPosBall_(const bcs::FVEC3 &) { return 0; }
            // Creates the game's balls.
            virtual void Create_() {}
            virtual void AllReset_();
            virtual void AllSleep_();
            // Puts the ball in the first free place of the table and returns
            // it, or null when the table is full.
            virtual CBall *AddBall_(CBall *pBall);
            virtual void DelBall_(long nBall);
            virtual int CanbeTurnOver_() const;
            virtual int IsInfinityReboundMode_() const { return 0; }
            virtual int IsDisableReboundMode_() const { return 0; }
            virtual int IsDisableAirBallMode_() const { return 0; }
            virtual void SetInfinityReboundMode_(int) {}
            virtual void SetDisableReboundMode_(int) {}
            virtual void SetDisableAirBallMode_(int) {}
            virtual int IsBallPassMode_(long nBall);
            virtual int IsBallShootMode_(long nBall);
            virtual int IsBallNowRebound_(long) { return 0; }
            virtual int IsBallInvincible_(long nBall);
            virtual int IsBallScored_(long nBall);
            virtual void SetBallPos_(long nBall, const bcs::FVEC3 *pPos);
            virtual void SetJumpBallMode_(long nBall);
            virtual void SetSleepMode_(long nBall);
            virtual void SetSleepModeAll_();
            virtual float GetRaiseGoalInRate_() const { return 0.0f; }
            virtual float GetPassFlyFrame_(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const;
            virtual float GetShootFlyFrame_(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const;
            virtual int IsExistStarBall_() const;
            virtual void SetStageBallColor_();
            virtual void GetStageBallColor_(bcs::BCS_Color &rColor);

            // These call the function of the same name with a trailing
            // underscore on m_pInstance and return zero without one. (only
            // the ones whose code was read are declared)
            static long GetBallNum();
            static long GetActiveBallNum();
            static unsigned long GetBallHandle(long nBall);
            static CBall *GetBallPtr(long nBall);
            static CBall *GetNearPosBall(const bcs::FVEC3 &rPos);
            static void DelBall(long nBall);

            static CBallMan *GetInstance();
            // Not zero when there is an instance.
            static int IsActive();

            static CBallMan *m_pInstance;

        protected:
            float m_Unk34;                // 0x34
            float m_Unk38;                // 0x38
            float m_Unk3C;                // 0x3C
            float m_Unk40;                // 0x40
            float m_Unk44;                // 0x44
            unsigned long m_Unk48;        // 0x48, zero from the constructor
            unsigned long m_Unk4C;        // 0x4C, zero from the constructor
            unsigned long *m_pBallHandle; // 0x50, task id of each ball, zero for none
            long m_nBallNum;              // 0x54, entries in m_pBallHandle
    };
}

#endif /* !MSW_CBALLMAN_HPP_ */
