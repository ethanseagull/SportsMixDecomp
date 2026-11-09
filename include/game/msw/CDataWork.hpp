#ifndef CDATAWORK_HPP_
#define CDATAWORK_HPP_

namespace msw {

    // Only the members used so far. Both functions are empty in the DOL, so
    // only the call sites show that they take an object.
    class CDataWork {
        public:
            void Initialize();
            void Finalize();

            static CDataWork s_Instance;
    };
}

#endif /* !CDATAWORK_HPP_ */
