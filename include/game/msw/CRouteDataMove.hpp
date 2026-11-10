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

            // (name and meaning guessed) Stage33's rainbow roads on the
            // right side play the left side's node paths with this set.
            void SetMirror(short nMirror) { m_nMirror = nMirror; }

            // (name and meaning guessed) Two frame values Stage32's cannon
            // sets before CalcFrame (11 and 5).
            void SetFrameParam(float f1, float f2)
            {
                m_fUnk4C = f1;
                m_fUnk50 = f2;
            }

        private:
            char m_Unk34[0x6];  // 0x34
            short m_nMirror;    // 0x3A: see SetMirror
            char m_Unk3C[0x10]; // 0x3C
            float m_fUnk4C;     // 0x4C
            float m_fUnk50;     // 0x50
            char m_Unk54[0x4];  // 0x54
    }; // size 0x58
}

#endif /* !CROUTEDATAMOVE_HPP_ */
