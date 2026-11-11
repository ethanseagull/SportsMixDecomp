#ifndef MSW_CC_CMN_SLIME_CCSQUADFACTORY_HPP_
#define MSW_CC_CMN_SLIME_CCSQUADFACTORY_HPP_

#include "bcs/CUnivTask.hpp"

namespace msw {

    class CChrCore;

    namespace CC_CMN {
        namespace SLIME {

            // Size 0x40, vtable lbl_80483798. Makes the slime's three squad
            // models as msw::CChrOptTask tasks and deletes them with itself.
            // Member names are guesses.
            class CcSquadFactory : public bcs::CUnivTaskEx {
                public:
                    enum { SQUAD_NUM = 3 };

                    // pfSpeed: the motion speed of each squad model.
                    CcSquadFactory(bcs::CTask *pParent, CChrCore *pChr, const float *pfSpeed);
                    virtual ~CcSquadFactory();
                    // Both empty; the DOL's copies are in the weak tail of
                    // the slime file (fn_801D1130, fn_801D112C).
                    virtual void Main() {}
                    virtual void Disp() {}
                    // fn_801CC0D0 (name guessed): empty.
                    virtual void Update();

                private:
                    // fn_801CBF90 (name guessed).
                    void Create_(CChrCore *pChr, const float *pfSpeed);

                    unsigned long m_aTaskId[SQUAD_NUM]; // 0x34: ids of the squad tasks
            };
        }
    }
}

#endif /* !MSW_CC_CMN_SLIME_CCSQUADFACTORY_HPP_ */
