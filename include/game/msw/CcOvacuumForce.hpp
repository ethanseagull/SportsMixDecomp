#ifndef MSW_CCOVACUUMFORCE_HPP_
#define MSW_CCOVACUUMFORCE_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {
    namespace CC_CMN {
        namespace LUIGI {

            // (DOL class; meaning guessed from the names) The pull of Luigi's
            // vacuum in his air trick: a task that draws the ball towards a
            // point in front of him. 0x104 bytes (VolleyF's
            // CChrCoreVolleyLuigi allocates it with that size).
            //
            // INCOMPLETE: only what VolleyF calls; the base class is a guess
            // (the task id is read at 0x04, as bcs::CTask has it), parameter
            // meanings are guessed.
            class CcOvacuumForce : public bcs::CTask {
                public:
                    CcOvacuumForce(bcs::CTask *pParent, long nPlayer, long nTeam);
                    virtual ~CcOvacuumForce();
                    virtual void Main();

                    void SetForceEnable(int bEnable, int, float);
                    void SetForceBasePos(const bcs::FVEC3 &pos);
                    void SetForceYRot(float fRotY);
                    void SetForceLimit(float fLimit);
                    void SetForceParam(float, float, float, float, float, float);
                    void SetForceDmgEffectFrm(float fFrame);

                private:
                    unsigned char m_Unk34[0x104 - 0x34];
            };
        }
    }
}

#endif /* !MSW_CCOVACUUMFORCE_HPP_ */
