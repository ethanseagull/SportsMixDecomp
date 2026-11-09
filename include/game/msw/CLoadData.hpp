#ifndef CLOADDATA_HPP_
#define CLOADDATA_HPP_

namespace msw {

    // Only the member used so far.
    class CLoadData {
        public:
            // Data of entry `index` in the game's table of files.
            static void *GetPtr(long index); // return type guessed
    };
}

#endif /* !CLOADDATA_HPP_ */
