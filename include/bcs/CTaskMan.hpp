#ifndef BCS_CTASKMAN_HPP_
#define BCS_CTASKMAN_HPP_

#include "nw4r/ut/List.hpp"

namespace bcs {

    class CTask;
    class CHandleTable;

    // The task manager: all static (DOL game/bcs/CTaskMan.cpp; the
    // function names other than GetTask are guessed).
    class CTaskMan {
        public:
            // A request to move a task to another priority list (8 bytes;
            // member names guessed).
            struct PRIORITY_REQ {
                unsigned long nId;      // 0x00: CTask::GetId of the task
                short nPriority;        // 0x04
                unsigned short bChild;  // 0x06: the children follow
            };

            // fn_8007AE28: makes the handle table, the 64 priority lists
            // and the request queue.
            static void Initialize();
            // fn_8007AEC0: deletes the tasks that asked for it, carries out
            // the priority requests, then runs Main of every task in
            // priority order. Tasks with bit 1 of their flags run only
            // while bForce is set; paused ones do not run.
            static void Main(int bForce);
            // fn_8007B254: Disp of every task not being deleted.
            static void Disp();
            // fn_8007B720: queues moving pTask to list nPriority (bChild:
            // its children too); at most 0x100 requests are kept.
            static void RequestPriority(CTask *pTask, short nPriority, unsigned short bChild);

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
            // (names guessed) The queue RequestPriority fills (0x100
            // entries, lbl_805C0A6C) and its length (lbl_805C0A70).
            static PRIORITY_REQ *s_pRequest;
            static long s_nRequestNum;
    };
}

#endif /* !BCS_CTASKMAN_HPP_ */
