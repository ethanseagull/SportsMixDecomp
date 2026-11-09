#ifndef CCOCKPITBUTTONINFO_HPP_
#define CCOCKPITBUTTONINFO_HPP_

namespace msw {

    // Size 0xC, vptr at 0x08. The texts and sounds of a cockpit menu's
    // buttons: an array the constructor of the menu allocates (0x114 bytes
    // an entry). Lives in the DOL; a base of CCockpitMenuBase. Nothing of it
    // is used yet.
    class CCockpitButtonInfo {
        public:
            struct ButtonInfo;

            void SetButtonInfo(const ButtonInfo *pInfo);

        private:
            void *m_pInfo; // 0x00
            long m_nNum;   // 0x04

        public:
            virtual ~CCockpitButtonInfo();
    };
}

#endif /* !CCOCKPITBUTTONINFO_HPP_ */
