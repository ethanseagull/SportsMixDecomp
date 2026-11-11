#ifndef EVDEMOVS_HPP_
#define EVDEMOVS_HPP_

namespace msw {

    // Whether "EvDemoVs" is a namespace or a class with static members
    // cannot be told from the symbol; a namespace is a guess.
    namespace EvDemoVs {
        void Initialize();
        // Added for MenuF game/menu/CMenu.cpp (meanings guessed from the
        // names: the menu passes 0 on start-up, 3 before leaving for a step
        // other than 3).
        void CopyResMode(long nMode);
        void CopyRes();
    }
}

#endif /* !EVDEMOVS_HPP_ */
