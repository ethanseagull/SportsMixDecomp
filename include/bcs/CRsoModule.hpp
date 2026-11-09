#ifndef BCS_CRSOMODULE_HPP_
#define BCS_CRSOMODULE_HPP_

namespace bcs {

    // Only the members used so far. Return types are guessed as void.
    class CRsoModule {
        public:
            void LoadAsync();
            void Mount();
            void UnMount();
    };
}

#endif /* !BCS_CRSOMODULE_HPP_ */
