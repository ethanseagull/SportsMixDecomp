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
}

#endif /* !MSW_CCHRDRV_HPP_ */
