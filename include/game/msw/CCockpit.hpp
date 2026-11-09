#ifndef CCOCKPIT_HPP_
#define CCOCKPIT_HPP_

namespace msw {

    // Lives in the DOL. Only the members used so far; all of them are static
    // (each loads the task from s_TaskHandle itself).
    class CCockpit {
        public:
            static long CreateObj(long nObjId, long nParam); // -1 without a task
            static long RemoveObj(long nObjId);               // -1 without a task
            static long IsEndObj(long nObjId);                // 1 without a task
            static void SetAnimEndClose(long nObjId, int bClose); // return type guessed
    };
}

#endif /* !CCOCKPIT_HPP_ */
