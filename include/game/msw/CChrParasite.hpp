#ifndef MSW_CCHRPARASITE_HPP_
#define MSW_CCHRPARASITE_HPP_

#include "bcs/CUnivTask.hpp"

namespace msw {
    class CChrCore;

    namespace CC_CMN {

        // (meaning guessed) A task that follows one character: Main asks
        // for deletion once the character is gone or disabled, then calls
        // MainFunc. A DOL class (CC_CMN/CChrParasite); size 0x40.
        class CChrParasite : public bcs::CUnivTask {
            public:
                CChrParasite(bcs::CTask *pParent, long nChr);
                virtual ~CChrParasite();
                virtual void Main();
                virtual void MainFunc() = 0; // 0x18

            protected:
                // The followed character, null when it is gone.
                CChrCore *GetChrCore_();

                long m_nChr;     // 0x34, index of the followed character (name guessed)
                long m_nState;   // 0x38, step of the derived class's MainFunc (name guessed)
                float m_Unk3C;   // 0x3C
        };
    }
}

#endif /* !MSW_CCHRPARASITE_HPP_ */
