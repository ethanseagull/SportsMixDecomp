#ifndef MSW_CTOPMENUCHROBJ_HPP_
#define MSW_CTOPMENUCHROBJ_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBall.hpp"

namespace bcs {
    class CAnmMotion;
}

namespace msw {

    class CChrMdlObj;

    // The ball held by one character of the top menu (MenuF). Size 0x31C
    // (the constructor of msw::CTopMenuChrObj allocates that much). Its
    // vtable is msw::CBall's with the destructor and TrgBound replaced.
    class CTopMenuBall : public CBall {
        public:
            // Turns the ball at random about all three axes; the hockey
            // puck (kind 3) only about Y.
            CTopMenuBall(bcs::CTask *pParent, long nKind);
            virtual ~CTopMenuBall();

            // Plays the bounce sound of the ball's sport.
            virtual void TrgBound(unsigned long);

            // Shows the ball in gold (bGold) or in its own colours.
            void EnableGoldModel(int bGold);

            // (name guessed) Builds the model from the first model of
            // `file`, as the DOL's CBall::Create does from named ones.
            void CreateMdl(nw4r::g3d::ResFile file);

            // (names guessed) Bit 2 of the flags of CBall hides the ball.
            void Hide() { m_nFlag |= 4; }
            void Show() { m_nFlag &= ~4; }

            // The sport of the ball (CTopMenuChrObj's kind).
            long GetKind() const { return m_nKind; }

        private:
            long m_nKind; // 0x318
    };

    // One character of the top menu with the ball of its sport (MenuF).
    // bcs::CTask at 0x00, bcs::CFsm<CTopMenuChrObj> at 0x34 (vptr at 0x78),
    // own members from 0x7C; 0xB8 bytes (msw::CTopMenuChr allocates that
    // much). Names of the members and states are guessed from what the
    // functions do with them.
    class CTopMenuChrObj : public bcs::CTask, public bcs::CFsm<CTopMenuChrObj> {
        public:
            // The states.
            enum {
                STATE_IDLE,    // standing; the idle motion loops
                STATE_ACTIVE,  // the selected sport: the character plays
                STATE_TO_IDLE, // back to the idle motion
                STATE_DECIDE,  // the sport was chosen
                STATE_NUM
            };

            // Bits of m_nFlag.
            enum {
                FLAG_HIDE = 0x01,     // not drawn
                FLAG_SELECT = 0x02,   // the selected sport
                FLAG_DECIDE = 0x04,   // go to STATE_DECIDE
                FLAG_DECIDE_WAIT = 0x08, // hold the decide motion until PlayDecideAnim
                FLAG_DECIDE_END = 0x10   // the decide motion has ended
            };

            // nKind: 0 basketball, 1 volleyball, 2 dodgeball, 3 hockey,
            // 4 the character of the mixed games (no ball).
            CTopMenuChrObj(bcs::CTask *pParent, long nKind);
            virtual ~CTopMenuChrObj();
            virtual void Main();

            // The trigger callback of the motion; pUser is the object.
            static void CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pUser);

            void CtorStateIdle();
            void LoopStateIdle();
            void DtorStateIdle();
            void CtorStateActive();
            void LoopStateActive();
            void DtorStateActive();
            void CtorStateToIdle();
            void LoopStateToIdle();
            void DtorStateToIdle();
            void CtorStateDecide();
            void LoopStateDecide();
            void DtorStateDecide();

            // Shown (bDisp) or hidden.
            void EnableDisp(int bDisp)
            {
                if (bDisp) {
                    m_nFlag &= ~FLAG_HIDE;
                } else {
                    m_nFlag |= FLAG_HIDE;
                }
            }
            // Makes the object the selected one (bSelect) or not; fBlend is
            // passed to the motions it plays next.
            void SetSelect(int bSelect, float fBlend)
            {
                if (bSelect) {
                    m_nFlag |= FLAG_SELECT;
                } else {
                    m_nFlag &= ~FLAG_SELECT;
                }
                m_fBlend = fBlend;
            }
            int IsSelect() const { return m_nFlag & FLAG_SELECT; }
            int IsDecide() const { return m_nFlag & FLAG_DECIDE; }
            int IsDecideWait() const { return m_nFlag & FLAG_DECIDE_WAIT; }
            int IsDecideEnd() const { return m_nFlag & FLAG_DECIDE_END; }
            // The selected object goes to STATE_DECIDE and holds there.
            void PrepareDecide()
            {
                if (IsSelect()) {
                    m_nFlag |= FLAG_DECIDE | FLAG_DECIDE_WAIT;
                }
            }
            void PlayDecide(float fBlend)
            {
                m_nFlag &= ~FLAG_DECIDE_WAIT;
                m_fBlend = fBlend;
            }
            // Back to the idle state, forgetting all but the display and
            // decide-end bits.
            void Reset()
            {
                m_nFlag &= FLAG_HIDE | FLAG_DECIDE_END;
                ChangeState(STATE_IDLE);
            }
            void SetMotionSpeedRate(float fRate) { m_fSpeedRate = fRate; }

            long GetKind() const { return m_nKind; }
            CChrMdlObj *GetMdl() const { return m_pMdl; }
            CTopMenuBall *GetBall() const { return m_pBall; }

        private:
            // Sets up the four states.
            void InitState();
            // Loads the model and the motions of kind nKind.
            void Create_(long nKind);
            void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg);

            long m_nKind;              // 0x7C
            unsigned long m_nFlag;     // 0x80
            CChrMdlObj *m_pMdl;        // 0x84
            bcs::CAnmMotion *m_pMotion; // 0x88
            bcs::FVEC3 m_vPos;         // 0x8C
            bcs::FVEC3 m_vRot;         // 0x98
            long m_nStep;              // 0xA4, the step inside a state
            float m_fBlend;            // 0xA8
            CTopMenuBall *m_pBall;     // 0xAC
            unsigned long m_nBoundArg; // 0xB0, passed to the ball's TrgBound
            float m_fSpeedRate;        // 0xB4
    };
}

#endif /* !MSW_CTOPMENUCHROBJ_HPP_ */
