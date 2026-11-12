#ifndef BCS_CTASKMAN_HPP_
#define BCS_CTASKMAN_HPP_

#include "nw4r/ut/List.hpp"

namespace bcs {

    class CTask;
    class CHandleTable;

    // The task manager. Only the lookup the game code uses is declared.
    class CTaskMan {
        public:
            // The task with the id CTask::GetId returned, or null when it
            // no longer exists. Static: the DOL function takes the id in r3
            // and loads the manager's instance itself.
            static CTask *GetTask(unsigned long id);

            // (names guessed) Set up by the manager's initialisation
            // (fn_8007AE28): the table every task registers its id in
            // (lbl_805C0A68), and one task list per priority, linked
            // through the word at 0x14 of a task (lbl_804CFF10).
            static CHandleTable *s_pHandleTable;
            static nw4r::ut::List s_TaskList[64];
    };
}

#endif /* !BCS_CTASKMAN_HPP_ */
