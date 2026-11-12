#ifndef MSW_CCHRDRV_HPP_
#define MSW_CCHRDRV_HPP_

namespace msw {

    class CChrCore;

    // INCOMPLETE: a driver of one character (a pad, CChrDrvPad, or the
    // computer, CComDrv). A DOL class with no exported functions of its own;
    // the vtable slots are read from the module vtables of its derived
    // classes (__vt__Q23msw15CChrDrvPadDodge, __vt__Q23msw12CComDrvDodge).
    // vptr at 0x00; no data members are mapped. Return types are guesses
    // unless a body is given.
    class CChrDrv {
        public:
            // Values not known.
            enum MSG_ID {};
            struct _CHR_MSG;

            virtual ~CChrDrv();
            virtual void MsgFunc(CChrCore *pChr, MSG_ID id, _CHR_MSG *pMsg);
            virtual long GetDrvType() const;
            // Inline: DodgeF emits it as a weak function (0x73CD4).
            virtual long GetPadType() const { return 1; }
    };

    // The arguments of a message. Only the first two words are known, read
    // as longs by BasketF's CComDrvBasketHB::OnGameMsgProc (a character
    // index, a ball index or a value, depending on the message); the size
    // is not known. (member names guessed)
    struct CChrDrv::_CHR_MSG {
        long m_nParam0; // 0x00
        long m_nParam1; // 0x04
        // 0x08 (added for BasketF game/basket/unit_AF3BC.cpp): a third
        // word; CComDrvBasketGame::OnGameMsgProc passes it on as a
        // character index or a value.
        long m_nParam2;
    };
}

#endif /* !MSW_CCHRDRV_HPP_ */
