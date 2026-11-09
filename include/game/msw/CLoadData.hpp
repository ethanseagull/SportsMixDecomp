#ifndef CLOADDATA_HPP_
#define CLOADDATA_HPP_

namespace msw {

    // Only the members used so far.
    class CLoadData {
        public:
            // Data of entry `index` in the game's table of files.
            static void *GetPtr(long index); // return type guessed
            // Its path: the start of the entry (entries are 0x110 bytes).
            static const char *GetFilePath(long index);
            static unsigned long GetSize(long index); // return type guessed
            // Non-zero once the entry has been read.
            static int IsComplete(long index); // return type guessed
    };
}

#endif /* !CLOADDATA_HPP_ */
