#ifndef BCS_CTHREAD_HPP_
#define BCS_CTHREAD_HPP_

#include "revolution/os/OSThread.h"

namespace bcs {

    // (class name guessed; DOL 0x8007B774..0x8007B90C, not written yet) An
    // OS thread that runs the virtual Run. The vptr sits after the thread
    // (0x31C): the constructor stores it there and the thread's entry
    // function calls slot 0x0C through it.
    class CThread {
        private:
            OSThread m_Thread; // 0x000
            int m_bCreated;    // 0x318, set once Create succeeded

        public:
            // (fn_8007B774)
            CThread();
            // (fn_8007B78C) Cancels the thread if it was created and has
            // not ended.
            virtual ~CThread();
            // (fn_8007B8C0 in the base: empty) The thread's body.
            virtual void Run();

            // (fn_8007B824, names guessed) OSCreateThread with this object
            // as the argument; non-zero on success. pStack is the stack's
            // top.
            int Create(void *pStack, unsigned long nStackSize, long nPriority, unsigned short nAttr);
            // (fn_8007B8C4) OSResumeThread.
            void Resume();
    };
}

#endif /* !BCS_CTHREAD_HPP_ */
