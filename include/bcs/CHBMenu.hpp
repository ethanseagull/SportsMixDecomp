#ifndef BCS_CHBMENU_HPP_
#define BCS_CHBMENU_HPP_

namespace bcs {

    // The HOME button menu. A DOL class; only the members used so far.
    class CHBMenu {
        public:
            static CHBMenu *GetInstance();

            // (argument name guessed) Non-zero forbids opening the menu;
            // the title screen sets it while the opening movie plays.
            void SetBanFlag(int bBan);
    };
}

#endif /* !BCS_CHBMENU_HPP_ */
