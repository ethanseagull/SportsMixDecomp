#ifndef CROUTEBASE_HPP_
#define CROUTEBASE_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CEventModel;
    class CRouteMove;

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

            // What the camera of a route does. Only the members the stage
            // route managers use. (names guessed)
            //
            // msw::CTournamentMan (TourF) queues a character's routes as an
            // array of these, ended by one with no route.
            struct _PUSH_INFO {
                CRouteBase *pRoute;   // 0x00 (name guessed)
                // 0x04, 0x08: msw::CTournamentMan passes them to PushRoute
                // as its last two arguments, 0x08 first. (names guessed)
                float fUnk04;
                float fUnk08;
                float fFrame;         // 0x0C: frame of the route move the camera starts at
                // 0x10: bit 0 for a route run backwards (names guessed)
                unsigned char nFlag;
                unsigned char nStart; // 0x11: the map point the route starts at
                unsigned char nEnd;   // 0x12: and ends at
                unsigned char nType;  // 0x13
            };

            // What PushRoute gets: the route move to queue the routes on and
            // the model that runs them. (names guessed)
            struct _PUSH_PARAM {
                CRouteMove *pMove;    // 0x00
                CEventModel *pModel;  // 0x04
                // 0x08: the route queued before this one (CRouteShipMove of
                // Stage33 reads its id and length); may be 0, and may point
                // at 0.
                // (TourF msw::CRouteDokan reads the whole _PUSH_INFO there:
                // ppPrev points at its pRoute.)
                union {
                    CRouteBase **ppPrev;
                    _PUSH_INFO *pPrev;
                };
                _PUSH_INFO *pInfo;    // 0x0C
                _PUSH_INFO *pNext;    // 0x10: the route queued after this one, or 0 (name guessed)
            };

            // Where a route puts the character at one frame, filled by
            // GetResult (game/tour/CTournamentRouteMan.cpp). 0x30 bytes.
            // (names guessed)
            struct _RESULT {
                bcs::FVEC3 vPos;    // 0x00
                bcs::FVEC3 vRot;    // 0x0C
                bcs::FVEC3 vScale;  // 0x18
                // 0x24: the msw::CRouteMove action that plays on the route
                // (the route's m_nAction), -1 for none.
                long nAction;
                float fFrame;       // 0x28: frame of the route
                CRouteBase *pRoute; // 0x2C: the route
            };

            // The small virtual functions are inline: TourF has weak copies
            // of them, each after its first user (SetPoints in an earlier
            // file). Empty and inline: the derived destructors call nothing.
            virtual ~CRouteBase() {}
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual void CalcFrame() {}
            // Returns the camera kind of the route (CRouteDokan and
            // CRouteBigGunOut return byte 5 of the route's ROUTE_INFO).
            virtual long GetCameraDirect(bcs::FVEC3 &dir, float &f, long n, float rate)
            {
                dir.Set(0.0f, 0.0f, 0.0f);
                f = 0.0f;
                return 0;
            }
            virtual void GetResult(_RESULT &result, float frame, int b) { result.nAction = m_nAction; }
            virtual void GetResultPos(bcs::FVEC3 &pos, float frame, int b) {}
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b) {}
            virtual float GetSelectDir(float frame, int b);
            virtual void SetupSelect(const bcs::FVEC3 &pos, CEventModel *pModel, float f1, float f2, float f3, int b);
            virtual void SetSelect(CEventModel *pModel, int b);
            virtual void SetPoints(const unsigned char *pPoints, long nNum) {}
            // The direction the route leaves pModel in on the screen of view n.
            virtual bcs::FVEC3 GetSelectDir2d(CEventModel *pModel, long n);
            virtual void MakeLine(long nFrom, long nTo) {}
            virtual void SetDisp(int bDisp) {}
            virtual int IsDisp() const { return 0; }
            virtual void AnimPlay(long nAnim) {}
            virtual long GetStartRouteId() const { return m_nRouteId; }
            virtual void SetNoJump(int bNoJump) {}
            virtual int GetNoJump() const { return 0; }
            virtual float GetRate() const { return m_fRate; }

            // (names guessed) The frame at which the route has moved fDist
            // away from where it is at frame fFrame, searched in steps of
            // fStep times the rate until the error is under fError.
            float GetOffsetFrame(float fDist, float fFrame, float fStep, float fError);

            // Fills `param` with the positions of map points nFrom and nTo.
            static void _SetParam(_PARAM &param, long nFrom, long nTo, int b);
            static void _SetParam(_PARAM &param, const bcs::FVEC3 &start, const bcs::FVEC3 &end, int b);

            // Out of line in TourF (weak copies of these inline functions).
            float GetNumFrame() const { return m_fNumFrame; }
            void SetRouteId(long nId) { m_nRouteId = nId; }
            // (name guessed) Inline in Stage33's CRouteShipMove::PushRoute.
            long GetRouteId() const { return m_nRouteId; }
            // (name guessed) The halfword at 0x20; msw::CTournamentMan
            // skips a route after one with 0x10 and before one with 0x08.
            unsigned short GetPushFlag() const { return m_nUnk20; }

        protected:
            // Sets the selection directions.
            friend class CTournamentRouteMan;

            // Inline: the constructors of the derived classes set these
            // members themselves (CRouteJumpPoints, CRouteWait, and the
            // inlined CRouteBigGunIn/Out of Stage32). n goes to the halfword
            // at 0x22 (-1 for most routes).
            CRouteBase(const bcs::FVEC3 &vStart, const bcs::FVEC3 &vEnd, long n)
                : m_vStart(vStart), m_vEnd(vEnd), m_fNumFrame(0.0f), m_nUnk20(6), m_nAction(n), m_nRouteId(-1),
                  m_fRate(1.0f)
            {
            }

            bcs::FVEC3 m_vStart; // 0x04
            bcs::FVEC3 m_vEnd;   // 0x10
            float m_fNumFrame;   // 0x1C
            short m_nUnk20;      // 0x20
            // 0x22: the msw::CRouteMove action played on the route (0
            // walk, 1 jump, 3 cannon), -1 for none. (name guessed)
            short m_nAction;
            long m_nRouteId;     // 0x24
            // 0x28, 0x2C: the direction of the route at its start and at
            // its end, as msw::CTournamentRouteMan::SetupRoute sets them for
            // the selection arrows. (name guessed)
            float m_afSelectDir[2];
            float m_fRate;       // 0x30
    }; // size 0x34
}

#endif /* !CROUTEBASE_HPP_ */
