#ifndef CCOCKPITTOURNAMENT_HPP_
#define CCOCKPITTOURNAMENT_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CCockpit.hpp"

namespace msw {

    // The tournament's cockpit task (TourF), size 0x21C: CCockpit at 0,
    // bcs::CFsm<CCockpitTournament> at 0x1D4 (vptr at 0x218), no members of
    // its own. Two states: Initialize (0), which moves straight on to
    // GamePlay (2). The table has seven entries; the others are unset.
    class CCockpitTournament : public CCockpit, public bcs::CFsm<CCockpitTournament> {
        public:
            enum {
                STATE_INITIALIZE = 0,
                STATE_GAMEPLAY = 2,
                STATE_NUM = 7
            };

            CCockpitTournament(bcs::CTask *pParent);
            virtual ~CCockpitTournament();
            virtual void Main();
            virtual void Disp();
            virtual long CreateObjImpl(long nObjId, long nParam);
            virtual void SetTeamCoinCoder(long, long) {}
            virtual void AddTeamCoinCoder(long, long, long) {}
            // Overrides CCockpit's and bcs::CFsm's at once.
            virtual long ChangeState(long nState);

            // Creates the task under pParent unless a cockpit exists.
            static void Create(bcs::CTask *pParent);
            // Shows or hides every display object (ids 0 to 7).
            static void SetDisp(int bDisp);
            static void SetAnimEndClose(long nObjId, int bClose);
            static void PlayEndAnim(long nObjId);
            // CreateObjImpl on the task; -2 without a task.
            static long CreateObj(long nObjId, long nParam);

            // (name guessed) Builds the state table and enters Initialize.
            void InitState();

            void Initialize_In();
            void Initialize_Loop();
            void Initialize_Out();
            void GamePlay_In();
            void GamePlay_Loop();
            void GamePlay_Out();
    };
}

#endif /* !CCOCKPITTOURNAMENT_HPP_ */
