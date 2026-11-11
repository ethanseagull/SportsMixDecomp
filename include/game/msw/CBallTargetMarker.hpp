#ifndef MSW_CBALLTARGETMARKER_HPP_
#define MSW_CBALLTARGETMARKER_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace bcs {
    class CMdlObj;
    class CScnGroup;
    struct FVEC3;
}

namespace msw {

    class CBall;

    // The marker on the court where a ball will come down (VolleyF), size
    // 0xB4 (the size CBallManVolley allocates): bcs::CTask at 0x00,
    // bcs::CFsm<CBallTargetMarker> at 0x34 (vptr at 0x78), own members from
    // 0x7C. Its functions are in game/volley/CBallTargetMarker.cpp; the
    // virtual functions are declared in the order of
    // __vt__Q23msw17CBallTargetMarker. The state functions are not virtual
    // (the state table points at them directly). Member names are guesses.
    class CBallTargetMarker : public bcs::CTask, public bcs::CFsm<CBallTargetMarker> {
        public:
            enum {
                STATE_SLEEP = 0,
                STATE_APPEAR = 1,
                STATE_MAIN = 2,
                STATE_DISAPPEAR = 3,
                STATE_NUM = 4
            };

            // `nBallHandle` is the task id of the ball the marker follows.
            // A non-zero bRing also makes the two rings that turn around
            // the marker (CBallManVolley passes 1).
            CBallTargetMarker(bcs::CTask *pParent, unsigned long nBallHandle, int bRing);
            virtual ~CBallTargetMarker();
            virtual void Main();
            virtual void Disp();

            virtual void UpdateViewFlag_();
            virtual void UpdateColor_();

            void CtorStateSleep();
            void LoopStateSleep();
            void DtorStateSleep();
            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateMain();
            void LoopStateMain();
            void DtorStateMain();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

        protected:
            void Finalize_();
            void Create_(int bRing);
            // A non-zero bOn takes the models out of their group and hands
            // them to the render manager; zero takes them out only.
            void PushBack_(int bOn);
            // 1: hide, 2: show, 3: let go (the ball is out of play or
            // about to be taken).
            int CheckAppear_();
            void SetHeightObjState_();
            // (inline) Moves the marker to where the ball will come down.
            void UpdateTargetPos_();

            bcs::CScnGroup *m_pGroup;        // 0x7C, holds the models below
            bcs::CMdlObj *m_pMdlBase;        // 0x80, the ground mark
            bcs::CMdlObj *m_pMdlBaseSub;     // 0x84, a copy attached to it
            bcs::CMdlObj *m_pMdlChance;      // 0x88, the chance-ball mark (animated)
            bcs::CMdlObj *m_pMdlItem;        // 0x8C, shown for item balls
            bcs::CMdlObj *m_pMdlItemSub;     // 0x90, attached to m_pMdlItem
            bcs::CMdlObj *m_pMdlRing[2];     // 0x94
            bcs::CMdlObj *m_pMdlRingSub[2];  // 0x9C, each attached to its ring
            unsigned long m_nBallHandle;     // 0xA4
            float m_fAppearRate;             // 0xA8, 0 to 1 while coming up
            int m_bChance;                   // 0xAC
            long m_nSpikeType;               // 0xB0, 1: curve spike, 2: power spike
    };

    // The target marker of the volley game against a boss (VolleyF), size
    // 0xB4 like its base (the size BALL_MAN_VOLLEY::CBallManVolleyVsBoss
    // allocates). It does not change its view flags.
    class CBallTargetMarkerVsBoss : public CBallTargetMarker {
        public:
            CBallTargetMarkerVsBoss(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallTargetMarkerVsBoss();

            // Inline: emitted after the unit's functions.
            virtual void UpdateViewFlag_() {}
    };

    // The marker of the rhythm-receive mini game (VolleyF), size 0x98 (the
    // size CRhythmReceiveMan allocates): the ground mark of a ball or an
    // item, with the mark of a team on it.
    class CBallTargetMarkerRhythmReceive : public bcs::CTask, public bcs::CFsm<CBallTargetMarkerRhythmReceive> {
        public:
            enum {
                STATE_SLEEP = 0,
                STATE_APPEAR = 1,
                STATE_MAIN = 2,
                STATE_DISAPPEAR = 3,
                STATE_NUM = 4
            };

            // nTeam 0 to 3 picks the mark and its colour; 4 is none.
            CBallTargetMarkerRhythmReceive(bcs::CTask *pParent, long nTeam, int bMark);
            virtual ~CBallTargetMarkerRhythmReceive();
            virtual void Main();
            virtual void Disp();

            virtual void UpdateColor_();

            // What the marker follows: a ball or an item, by task id.
            void SetAttachToBall(unsigned long nHandle);
            void SetAttachToItem(unsigned long nHandle);

            void CtorStateSleep();
            void LoopStateSleep();
            void DtorStateSleep();
            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateMain();
            void LoopStateMain();
            void DtorStateMain();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

        protected:
            void Create_(int bMark);
            int CheckAppear_();
            void GetTargetPos_(bcs::FVEC3 &rPos);
            float GetTouchdownFrame_();
            void SetHeightObjState_();
            void SetColor_();

            bcs::CScnGroup *m_pGroup;    // 0x7C
            bcs::CMdlObj *m_pMdlBase;    // 0x80
            bcs::CMdlObj *m_pMdlMark;    // 0x84
            unsigned long m_nHandle;     // 0x88, a ball's or an item's task id
            float m_fAppearRate;         // 0x8C
            int m_bItem;                 // 0x90
            long m_nTeam;                // 0x94
    };
}

#endif /* !MSW_CBALLTARGETMARKER_HPP_ */
