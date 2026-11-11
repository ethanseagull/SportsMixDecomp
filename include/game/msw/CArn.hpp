#ifndef MSW_CARN_HPP_
#define MSW_CARN_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // An archive of the stage's resources (meaning of the name not known).
    // Only what the modules call is declared.
    class CArn {
        public:
            // In the DOL. The name of entry n.
            const char *GetName(long n) const;
            // (name guessed) In the DOL (fn_8014A920). The position of
            // entry n (entries of 0x94 bytes, the position at 0x04).
            void GetPos(long n, bcs::FVEC3 *pPos) const;
    };
}

#endif /* !MSW_CARN_HPP_ */
