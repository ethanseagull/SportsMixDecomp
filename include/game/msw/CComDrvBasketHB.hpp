#ifndef MSW_CCOMDRVBASKETHB_HPP_
#define MSW_CCOMDRVBASKETHB_HPP_

#include "game/msw/CComDrvBasket.hpp"

namespace msw {

    // INCOMPLETE: the computer driver of the "HB" basketball game (the
    // Hungry Boss Packun court), run by a script: most of its virtuals call
    // a function of the driver's script by name (CallMainScriptFunc). Its
    // vtable is in game/basket/CHBScript.cpp (the destructor's unit), so
    // the destructor must stay the first non-inline virtual declared here.
    // Members (0x170, 0x174: the script) are not mapped. Return types are
    // guesses unless a caller shows them.
    class CComDrvBasketHB : public CComDrvBasket {
        public:
            CComDrvBasketHB();
            virtual ~CComDrvBasketHB();

            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual void NormalizeCanMovePos(bcs::FVEC3 &pos) const;
            virtual void Reset();
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual void OnConnect(_CHR_MSG *pMsg);
            virtual void OnDisconnect(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void ActInterrupt();
            virtual void ThkMain();
            virtual int IsScoureWin() const;

            int IsShootRelease(long, long) const;

            // Call the script function pName with no, one or two arguments.
            void CallMainScriptFunc(const char *pName);
            void CallMainScriptFunc(const char *pName, long nArg0);
            void CallMainScriptFunc(const char *pName, long nArg0, long nArg1);
            // The value of the script's global pName.
            long GetScriptVariable(const char *pName) const;
    };
}

#endif /* !MSW_CCOMDRVBASKETHB_HPP_ */
