#ifndef BCS_CTASKMAN_HPP_
#define BCS_CTASKMAN_HPP_

namespace bcs {

    class CTask;

    // The task manager. Only the lookup the game code uses is declared.
    class CTaskMan {
        public:
            // The task with the id CTask::GetId returned, or null when it
            // no longer exists. Static: the DOL function takes the id in r3
            // and loads the manager's instance itself.
            static CTask *GetTask(unsigned long id);
    };
}

#endif /* !BCS_CTASKMAN_HPP_ */
