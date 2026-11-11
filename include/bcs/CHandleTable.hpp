#ifndef BCS_CHANDLETABLE_HPP_
#define BCS_CHANDLETABLE_HPP_

namespace bcs {

    // (class and member names guessed) Hands out handles for pointers so
    // that an object can be looked up later without keeping a dangling
    // pointer; bcs::CScnObj registers every scene object in one. A DOL class
    // (0x8004570C..0x80045AAC, no symbols), size 0x14. Only what
    // game/bcs/CScnObj.cpp uses.
    class CHandleTable {
        public:
            CHandleTable();                                          // fn_8004570C
            // (meaning of the last two arguments not known: they are
            // passed on to the array allocation)
            void Init(unsigned long nNum, long nArg1, long nArg2);   // fn_8004572C
            // Registers p and returns its handle.
            unsigned long Register(void *p);                         // fn_8004594C
            void Unregister(unsigned long handle);                   // fn_800459A8
            // The pointer registered under handle, or null.
            void *Get(unsigned long handle) const;                   // fn_80045A34

        private:
            unsigned char m_Unk00[0x14];
    };
}

#endif /* !BCS_CHANDLETABLE_HPP_ */
