#ifndef MSW_CNPCBEHEMOTHBOSS_HPP_
#define MSW_CNPCBEHEMOTHBOSS_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CNpcBase.hpp"

namespace msw {

    class CChrMan;

    // INCOMPLETE: the boss of the boss game (BossF), size 0x36C (what
    // CChrManBoss::Create_ allocates). Only the constructor is declared.
    // Its actions run on bcs::CFsmQueue<CNpcBehemothBoss> at 0x188 (BossF
    // has that class's vtable; CComDrvBoss reads the state at the head of
    // the queue: state 9 sends the characters away from it).
    class CNpcBehemothBoss : public CNpcBase, public bcs::CFsmQueue<CNpcBehemothBoss> {
        public:
            // (parameter names guessed) CChrManBoss::Create_ passes itself,
            // the start position and rotation, and party game parameter 0.
            CNpcBehemothBoss(CChrMan *pMan, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nParam);

            // (added for BossF game/boss/unit_51CE4.cpp) Its own virtual
            // functions after the state queue's part of the vtable (from
            // 0x80, named by __vt__Q23msw16CNpcBehemothBoss, as in
            // CNpcBossPackun); the ones after GetMaxHP are not declared yet.
            // Return types of the HP getters read from CComDrvBoss::ThkMain.
            virtual unsigned long GetQueueWorkSize() const;             // 0x80
            virtual float GetHP() const;                                // 0x84
            virtual float GetMaxHP() const;                             // 0x88

        protected:
            unsigned char m_Unk1E8[0x36C - 0x1E8];
    };
}

#endif /* !MSW_CNPCBEHEMOTHBOSS_HPP_ */
