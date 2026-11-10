#ifndef CROUTEDATAMOVE_HPP_
#define CROUTEDATAMOVE_HPP_

#include "game/msw/CRouteBase.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    // A route that follows a node animation of a model file (module TourF).
    // 0x58 bytes. Defined in game/tour/CTournamentRouteMan.cpp. Members not
    // mapped yet; parameter names guessed.
    class CRouteDataMove : public CRouteBase {
        public:
            CRouteDataMove(const _PARAM &param, float rate, unsigned long nFrame, long n);
            virtual ~CRouteDataMove();
            virtual void CalcFrame();
            virtual void GetResult(_RESULT &result, float frame, int b);
            virtual void GetResultPos(bcs::FVEC3 &pos, float frame, int b);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);

            // Takes the path from the node `pNode` of `file`.
            void SetNode(const nw4r::g3d::ResFile &file, const char *pNode, long n);
            void SetNode(const nw4r::g3d::ResFile &file, const char *pNode, const char *pName);

        private:
            char m_Unk34[0x24]; // 0x34
    }; // size 0x58
}

#endif /* !CROUTEDATAMOVE_HPP_ */
