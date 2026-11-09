#ifndef BCS_CRSOMODULE_HPP_
#define BCS_CRSOMODULE_HPP_

namespace bcs {

    // Only the members used so far. All three take an object (their bodies
    // in the DOL work on r3). Return types the callers ignore are guessed as
    // void.
    class CRsoModule {
        public:
            void LoadAsync();
            int Mount(); // 1 when the module is mounted, 0 on failure; type guessed
            void UnMount();
    };
}

#endif /* !BCS_CRSOMODULE_HPP_ */
