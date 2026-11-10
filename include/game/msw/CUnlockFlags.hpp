#ifndef CUNLOCKFLAGS_HPP_
#define CUNLOCKFLAGS_HPP_

namespace msw {

    // The unlock flags of a player's save data (DOL), one word. Only the
    // members used so far.
    class CUnlockFlags {
        public:
            // Points the object at player nPlayer's save data (-1: the
            // current player). (meaning guessed)
            void SetPlayerData(long nPlayer);

        private:
            unsigned long m_nData;
    };
}

#endif /* !CUNLOCKFLAGS_HPP_ */
