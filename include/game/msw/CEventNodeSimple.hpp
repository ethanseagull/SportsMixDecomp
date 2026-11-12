#ifndef CEVENTNODESIMPLE_HPP_
#define CEVENTNODESIMPLE_HPP_

#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"

namespace nw4r {
    namespace g3d {
        class ResAnmChr;
        struct ChrAnmResult;
    }
}

namespace msw {

    // A node path an event model follows (DOL class; msw::CEventModel::
    // SetLinkNode takes one). Only what the modules need: the base of
    // msw::CEventNodeSimple. Its constructor stores no vtable of its own in
    // the inlined CEventNodeSimple constructors. (layout of the base not
    // known)
    class CEventNodeBase {
        public:
            CEventNodeBase() {}
            // Inline and empty: no derived destructor calls one, and the
            // game has no vtable of this class.
            virtual ~CEventNodeBase() {}

            // The virtual functions, in vtable order (names guessed except
            // GetResult and GetNumFrame, named by msw::CEvNodeData).
            // 0x0C: whether node nId can be read.
            virtual bool IsEnable(long nId) = 0;
            // 0x10: position and rotation of node nId at frame fFrame.
            virtual void GetResult(bcs::FVEC3 &pos, bcs::FVEC3 &rot, float fFrame, long nId) = 0;
            // 0x14 (DOL, 0x801FEC74): GetResult without the rotation.
            virtual void GetPos(bcs::FVEC3 &pos, float fFrame, long nId);
            // 0x18, 0x1C: GetResult with a third vector (CEventNodeSimple
            // sets it to a constant).
            virtual void GetResultVec(bcs::FVEC3 &pos, bcs::FVEC3 &rot, bcs::FVEC3 &vec, float fFrame, long nId) = 0;
            // 0x1C: the same for a path played mirrored (nFlip, see
            // msw::CEventMove). The default (0x8011500C, emitted weak by
            // CEventModel.cpp after CEventMove::SetFrame) ignores nFlip and
            // gives a scale of 1.
            virtual void GetResultEx(bcs::FVEC3 &pos, bcs::FVEC3 &rot, bcs::FVEC3 &scale, float fFrame, long nId, long nFlip)
            {
                scale.x = 1.0f;
                scale.y = 1.0f;
                scale.z = 1.0f;
                GetResult(pos, rot, fFrame, nId);
            }
            // 0x20: length of the path in frames.
            virtual float GetNumFrame() const = 0;

            // (DOL fn_801FC430, unnamed; name and parameter names guessed)
            // The rotation of rMtx once its scale rScale is taken out.
            static void _GetMtxRot(bcs::FVEC3 &rRot, const bcs::FVEC3 &rScale, const bcs::FMTX34 &rMtx);

            // (DOL; added for TourF msw::CRouteDataMove, parameter names
            // guessed) Node nId of anm at frame fFrame; the result mirrored
            // about rCenter by nFlip (see msw::CEventMove); the rotation of a
            // result.
            static void _GetDataResult(nw4r::g3d::ResAnmChr anm, nw4r::g3d::ChrAnmResult &rResult, float fFrame,
                                       long nId);
            static void _GetFlipResult(nw4r::g3d::ChrAnmResult &rResult, const bcs::FVEC3 &rCenter, long nFlip);
            static void _GetDataRot(bcs::FVEC3 &rRot, const nw4r::g3d::ChrAnmResult &rResult);
    };

    // A node path from one position and rotation to another (DOL class,
    // 0x48 bytes, vptr at 0x00). The modules construct it inline (TourF:
    // msw::CTournamentMan, msw::CTournamentRouteMan_Stage32) and fill the
    // two ends themselves. Names of the members guessed.
    class CEventNodeSimple : public CEventNodeBase {
        public:
            CEventNodeSimple() : m_fRate(0.0f), m_fRateAdd(0.0f), m_fFrame(0.0f), m_fFrameStep(0.5f)
            {
            }

            virtual ~CEventNodeSimple();

            // (DOL) The path can be read while frames remain.
            virtual bool IsEnable(long nId) { return m_fFrame > 0.0f; }
            // (DOL, 0x801FB6CC)
            virtual void GetResult(bcs::FVEC3 &pos, bcs::FVEC3 &rot, float fFrame, long nId);
            // (DOL, 0x801FECA8; slot 0x1C is the base's GetResultEx)
            virtual void GetResultVec(bcs::FVEC3 &pos, bcs::FVEC3 &rot, bcs::FVEC3 &vec, float fFrame, long nId);
            // (DOL)
            virtual float GetNumFrame() const { return m_fFrame; }

            // (names guessed) Inline in the modules: the two ends of the path.
            void SetStart(const bcs::FVEC3 &vPos, const bcs::FVEC3 &vRot)
            {
                m_vStartPos = vPos;
                m_vStartRot = vRot;
            }
            void SetEnd(const bcs::FVEC3 &vPos, const bcs::FVEC3 &vRot)
            {
                m_vEndPos = vPos;
                m_vEndRot = vRot;
            }

            // Sets the rate, its change per frame and the limit; m_fFrame
            // receives the number of frames until the limit is passed.
            void _SetRate(float fRate, float fMul, float fLimit);

            bcs::FVEC3 m_vStartPos; // 0x04
            bcs::FVEC3 m_vStartRot; // 0x10
            bcs::FVEC3 m_vEndPos;   // 0x1C
            bcs::FVEC3 m_vEndRot;   // 0x28
            float m_fRate;          // 0x34
            float m_fRateAdd;       // 0x38
            float m_fLimit;         // 0x3C
            float m_fFrame;         // 0x40
            float m_fFrameStep;     // 0x44
    }; // size 0x48
}

#endif /* !CEVENTNODESIMPLE_HPP_ */
