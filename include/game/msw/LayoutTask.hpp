#ifndef LAYOUTTASK_HPP_
#define LAYOUTTASK_HPP_

namespace msw {

    // Whether "LayoutTask" is a namespace or a class with static members
    // cannot be told from the symbols; a namespace is a guess.
    namespace LayoutTask {
        void Initialize();
        void Finalize();
    }
}

#endif /* !LAYOUTTASK_HPP_ */
