#ifndef CCOCKPITMENU_HPP_
#define CCOCKPITMENU_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CCockpit.hpp"

namespace msw {

    // The menu's cockpit task (MenuF), size 0x220: CCockpit at 0,
    // bcs::CFsm<CCockpitMenu> at 0x1D4 (vptr at 0x218), one word of its
    // own at 0x21C. A single state, Wait, that does nothing. Creates only
    // the display objects every cockpit knows.
    class CCockpitMenu : public CCockpit, public bcs::CFsm<CCockpitMenu> {
        public:
            enum {
                STATE_WAIT = 0,
                STATE_NUM = 1
            };

            CCockpitMenu(bcs::CTask *pParent);
            virtual ~CCockpitMenu();
            virtual void Main();
            virtual void Disp();
            virtual long CreateObjImpl(long nObjId, long nParam);
            // Overrides CCockpit's and bcs::CFsm's at once.
            virtual long ChangeState(long nState);

            // Creates the task under pParent unless a cockpit exists.
            static void Create(bcs::CTask *pParent);

            // (name guessed) Builds the state table and enters Wait.
            void InitState();

            void Wait_In();
            void Wait_Loop();
            void Wait_Out();

        private:
            unsigned long m_Unk21C; // 0x21C, cleared by the constructor
    };
}

#endif /* !CCOCKPITMENU_HPP_ */
