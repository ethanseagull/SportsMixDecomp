#ifndef BCS_CSNDTASK_HPP_
#define BCS_CSNDTASK_HPP_

#include "bcs/CTask.hpp"

namespace bcs {

    // A playing sound (DOL task). Only the member used so far.
    class CSndTask : public CTask {
        public:
            // Stops the sound, fading out over fFade frames. (return type
            // guessed)
            void Stop(float fFade);
    };
}

#endif /* !BCS_CSNDTASK_HPP_ */
