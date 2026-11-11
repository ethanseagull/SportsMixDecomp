#ifndef MSW_CSCENECALC_HPP_
#define MSW_CSCENECALC_HPP_

#include "bcs/CCameraAnmCalc.hpp"

namespace msw {

    class CEventModel;
    class CEventScene;

    // The camera calculation of an event scene: bcs::CCameraAnmCalc's,
    // with the camera moved to the stage's court centre and to the offset
    // of one of the scene's models. Size 0x40, vtable 0x8046F4AC.
    class CSceneCalc : public bcs::CCameraAnmCalc {
        public:
            CSceneCalc(CEventScene *pScene);
            virtual ~CSceneCalc();
            virtual void Calc(bcs::CCameraAnm &rAnm, bcs::FVEC3 &rPos, bcs::FVEC3 &rAt, float &rRoll);

            // Makes this the scene camera's calculation, or gives the camera
            // its own back.
            void SetCalc(int bSet);
            // Follows the offset of the scene's model nChr (-1: none).
            void SetFloorChr(long nChr);

        private:
            void _CalcFloor(bcs::FVEC3 &rPos, bcs::FVEC3 &rAt);

            CEventScene *m_pScene;    // 0x34

        public:
            CEventModel *m_pFloorChr; // 0x38
            int m_bCourtCenter;       // 0x3C (name guessed) move to the court centre
    };
}

#endif /* !MSW_CSCENECALC_HPP_ */
