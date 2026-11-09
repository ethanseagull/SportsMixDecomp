#ifndef CEVENTDEMO_HPP_
#define CEVENTDEMO_HPP_

namespace msw {

    // Only the members used so far.
    class CEventDemo {
        public:
            static void SetRandomDemoGameMode();

            static int m_bDemonstration;  // 4 bytes; type guessed
            static long m_nDemonstration; // 4 bytes; type guessed
    };
}

#endif /* !CEVENTDEMO_HPP_ */
