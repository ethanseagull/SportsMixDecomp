#ifndef CROUTEBASE_HPP_
#define CROUTEBASE_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CEventModel;

    // One stretch of route on the tournament map (module TourF): how a
    // character moves from one map point to the next. vptr at 0x00. The
    // functions are defined in game/tour/CTournamentRouteMan.cpp. Layout
    // read from msw::CRouteJumpPoints's constructor and from
    // msw::CRouteBase::_SetParam; names guessed. Return types of the
    // virtuals are guesses as well (none is used by the code written so far).
    class CRouteBase {
        public:
            // Where the route starts and ends, set up by _SetParam. 0x20 bytes.
            struct _PARAM {
                bcs::FVEC3 vStart; // 0x00
                bcs::FVEC3 vEnd;   // 0x0C
                float fRotY;       // 0x18: direction from vStart to vEnd (Atan2FIdx, in radians)
                float fLength;     // 0x1C: distance from vStart to vEnd

                // Out of line in the original (an empty function,
                // __ct__Q33msw10CRouteBase6_PARAMFv), inlined here.
                _PARAM()
                {
                }

                // (guessed) Used by the file-scope parameter blocks of the
                // stage route managers, which their static initialisers fill.
                _PARAM(const bcs::FVEC3 &start, const bcs::FVEC3 &end) : vStart(start), vEnd(end)
                {
                }
            };

            struct _PUSH_PARAM;
            struct _PUSH_INFO;
            struct _RESULT;

            virtual ~CRouteBase();
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual void CalcFrame();
            virtual void GetCameraDirect(bcs::FVEC3 &dir, float &f, long n, float rate);
            virtual void GetResult(_RESULT &result, float frame, int b);
            virtual void GetResultPos(bcs::FVEC3 &pos, float frame, int b);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);
            virtual float GetSelectDir(float frame, int b);
            virtual void SetupSelect(const bcs::FVEC3 &pos, CEventModel *pModel, float f1, float f2, float f3, int b);
            virtual void SetSelect(CEventModel *pModel, int b);
            virtual void SetPoints(const unsigned char *pPoints, long nNum);
            virtual float GetSelectDir2d(CEventModel *pModel, long n);
            virtual void MakeLine(long nFrom, long nTo);
            virtual void SetDisp(int bDisp);
            virtual int IsDisp() const;
            virtual void AnimPlay(long nAnim);
            virtual long GetStartRouteId() const;
            virtual void SetNoJump(int bNoJump);
            virtual int GetNoJump() const;
            virtual float GetRate() const;

            // Fills `param` with the positions of map points nFrom and nTo.
            static void _SetParam(_PARAM &param, long nFrom, long nTo, int b);
            static void _SetParam(_PARAM &param, const bcs::FVEC3 &start, const bcs::FVEC3 &end, int b);

        protected:
            bcs::FVEC3 m_vStart; // 0x04
            bcs::FVEC3 m_vEnd;   // 0x10
            float m_fNumFrame;   // 0x1C
            short m_nUnk20;      // 0x20
            short m_nUnk22;      // 0x22
            long m_nRouteId;     // 0x24
            float m_fUnk28;      // 0x28
            float m_fUnk2C;      // 0x2C
            float m_fRate;       // 0x30
    }; // size 0x34
}

#endif /* !CROUTEBASE_HPP_ */
