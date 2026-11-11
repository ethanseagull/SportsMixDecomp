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

            // Added for TourF game/tour/CTmScript.cpp (names of the
            // arguments and return types guessed).
            // The sport the flags are read for.
            static long m_nGameMode;
            // Splits a script's character number into the character and
            // its colour.
            static void GetCharType(long &rType, long &rColor, long nChar);
            // The bit of a character in a 64-bit character set.
            static long GetCharFlagPos(long nType, long nColor, int nUnk);
            int IsUnlockChar(long nType, long nColor, long nGameMode);
            // A random character of sport nGameMode outside both sets.
            long GetCharRandom(long nGameMode, long nUnk, unsigned long long nExclude,
                               unsigned long long nUsed);
            // The set of the characters not unlocked yet.
            unsigned long long GetUnlockChar(long nGameMode, long nLevel, long nUnk);
            int IsAllStarChar();
            // Writes the unlocked stages of sport nGameMode to pStage (at
            // most nMax); returns how many.
            long ListupStage(long *pStage, long nMax, long nUnk, long nGameMode);
            int IsStage(long nStage, long nGameMode);
            int IsParty(long nParty, long nGameMode);
            int IsChar(long nType, long nColor, long nGameMode);
            int IsCup(long nCup, long nUnk, long nGameMode);
            int IsEtc(long nEtc, long nGameMode);
            // Unlock a character, a cup or something else, for the record
            // nId of the unlock list.
            void PushChar(long nId, long nUnk1, long nUnk2, long nType, long nColor, long nGameMode);
            void PushCup(long nId, long nCup, long nUnk, long nGameMode);
            void PushEtc(long nId, long nEtc, long nGameMode);

        private:
            unsigned long m_nData;
    };
}

#endif /* !CUNLOCKFLAGS_HPP_ */
