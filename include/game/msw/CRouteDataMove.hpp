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

            // Frames the route waits before and after the animation (Stage32
            // sets them through SetFrameParam). TourF has weak copies.
            void SetStartWait(float fWait) { m_fStartWait = fWait; }
            // (names guessed; added for CRouteJumpPoints::PushRoute) Frames
            // cut off the start and the end of the animation.
            void SetStartFrame(float fFrame) { m_fStartFrame = fFrame; }
            void SetEndFrame(float fFrame) { m_fEndFrame = fFrame; }
            void SetEndWait(float fWait) { m_fEndWait = fWait; }

            // (name and meaning guessed) Stage33's rainbow roads on the
            // right side play the left side's node paths with this set.
            void SetMirror(short nMirror) { m_nMirror = nMirror; }

            // (name and meaning guessed) Two frame values Stage32's cannon
            // sets before CalcFrame (11 and 5).
            void SetFrameParam(float f1, float f2)
            {
                m_fStartWait = f1;
                m_fEndWait = f2;
            }

        private:
            // The node of the animation at frame fFrame, run backwards when
            // b is set; returns the extra turn to add to the rotation.
            float _SetResult(nw4r::g3d::ChrAnmResult &result, float fFrame, int b);

            nw4r::g3d::ResAnmChr m_anm; // 0x34
            short m_nNodeId;    // 0x38: node of the animation, -1 for none
            short m_nMirror;    // 0x3A: see SetMirror
            float m_fRotY;      // 0x3C: direction of the route
            // 0x40: 0x1 no translation, 0x2 no rotation, 0x4 no scale, 0x8
            // add the start point, 0x10 add the end point, 0x20 add the
            // direction, 0x40 run backwards. (names guessed)
            unsigned long m_nFlag;
            float m_fStartFrame; // 0x44: frames cut off the start of the animation
            float m_fEndFrame;   // 0x48: and off its end
            float m_fStartWait;  // 0x4C
            float m_fEndWait;    // 0x50
            float m_fAnmFrame;   // 0x54: frames of the animation played
    }; // size 0x58
}

#endif /* !CROUTEDATAMOVE_HPP_ */
