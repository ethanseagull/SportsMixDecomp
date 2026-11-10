#ifndef MSW_CARN_HPP_
#define MSW_CARN_HPP_

namespace msw {

    // An archive of the stage's resources (meaning of the name not known).
    // Only what the modules call is declared.
    class CArn {
        public:
            // In the DOL. The name of entry n.
            const char *GetName(long n) const;
    };
}

#endif /* !MSW_CARN_HPP_ */
