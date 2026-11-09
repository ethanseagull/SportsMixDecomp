#ifndef CCOCKPITWINDOWINFO_HPP_
#define CCOCKPITWINDOWINFO_HPP_

namespace msw {
    class CCockpitLayoutBase;
    class CMenuPaneAccessor;

    // Size 0xC, vptr at 0x08. The text windows of a cockpit menu: an array
    // the constructor of the menu allocates (0x80C bytes an entry). Lives in
    // the DOL; a base of CCockpitMenuBase. Only the members used so far.
    class CCockpitWindowInfo {
        public:
            // pPaneId has one entry for each window: the index of its text
            // pane in the menu's pane name table.
            void SetWindowTextPaneId(const short *pPaneId);
            // Return type guessed.
            void InfoParameterInitialize(CCockpitLayoutBase *pOwner, const short *pPaneId,
                                         CMenuPaneAccessor *pPaneAccessor, int nArg);

        private:
            void *m_pInfo; // 0x00
            long m_nNum;   // 0x04

        public:
            virtual ~CCockpitWindowInfo();
    };
}

#endif /* !CCOCKPITWINDOWINFO_HPP_ */
