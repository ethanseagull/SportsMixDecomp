#ifndef CCOCKPITPARTSINFO_HPP_
#define CCOCKPITPARTSINFO_HPP_

namespace msw {

    // (name guessed: no function of this class has a name in the DOL)
    // Size 0xC, vptr at 0x08. The third array a cockpit menu's constructor
    // allocates (0x10 bytes an entry), after the button and window ones.
    // A base of CCockpitMenuBase. Nothing of it is used yet.
    class CCockpitPartsInfo {
        private:
            void *m_pInfo; // 0x00
            long m_nNum;   // 0x04

        public:
            virtual ~CCockpitPartsInfo();
    };
}

#endif /* !CCOCKPITPARTSINFO_HPP_ */
