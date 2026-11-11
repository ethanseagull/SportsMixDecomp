#ifndef CGOALOBJ_HPP_
#define CGOALOBJ_HPP_

#include "game/msw/CStgGmk.hpp"

namespace bcs {
    class CScnObj;
}

// A goal on the court: a stage gimmick with a ring and a board that can be
// crushed and dunked on. A DOL class in the global namespace (8CGoalObj),
// derived from msw::CStgGmk; BasketF's msw::CGoalObjBasket derives from it.
//
// The virtual functions follow __vt__Q23msw14CGoalObjBasket (BasketF):
// CStgGmk's overrides first, then the class's own from vtable offset 0xB0.
// Return types and parameter names are guessed. The data members are not
// mapped: the size of the class is not known.
class CGoalObj : public msw::CStgGmk {
    public:
        // (enumerator names guessed)
        enum DUNK_POW {
            DUNK_POW_0
        };

        CGoalObj(bcs::CTask *pParent, long idx);
        virtual ~CGoalObj();
        virtual void Main();

        virtual void SetPos(const bcs::FVEC3 &rPos);
        virtual void OnPostCreate();
        virtual void OnEntranceDemoStart();
        virtual void OnPeriodInit();
        virtual void OnTurnInit();

        virtual void SetSubType(long type);                              // 0xB0
        virtual long GetSubType() const;                                 // 0xB4
        virtual void SetAutoPosFlag(int flag);                           // 0xB8
        virtual int IsAutoPos() const;                                   // 0xBC
        virtual void SetAutoRotFlag(int flag);                           // 0xC0
        virtual int IsAutoRot() const;                                   // 0xC4
        virtual void SetAutoCalcRingPosFlag(int flag);                   // 0xC8
        virtual int IsAutoCalcRingPos() const;                           // 0xCC
        virtual void SetPosAuto();                                       // 0xD0
        virtual const bcs::FVEC3 &GetPos() const;                        // 0xD4
        virtual void SetRot(const bcs::FVEC3 &rRot);                     // 0xD8
        virtual void SetRotAuto();                                       // 0xDC
        virtual const bcs::FVEC3 &GetRot() const;                        // 0xE0
        virtual void SetDefaultOffset(const bcs::FVEC3 &rOffset);        // 0xE4
        virtual const bcs::FVEC3 &GetDefaultOffset() const;              // 0xE8
        virtual unsigned long GetMdlHandle() const;                      // 0xEC
        virtual long GetRingNodeIdx() const;                             // 0xF0
        virtual const bcs::FVEC3 &GetRingPos() const;                    // 0xF4
        virtual const bcs::FVEC3 &GetBoardPos() const;                   // 0xF8
        virtual const bcs::FVEC3 &GetBoardVec() const;                   // 0xFC
        virtual const bcs::FVEC3 &GetVec() const;                        // 0x100
        virtual void CrushStart();                                       // 0x104
        virtual void CrushEnd();                                         // 0x108
        virtual void GoalIn();                                           // 0x10C
        virtual void DunkStart(DUNK_POW pow, const bcs::FVEC3 &rPos);    // 0x110
        virtual void DunkRelease();                                      // 0x114
        virtual void DunkCancel();                                       // 0x118
        virtual int IsNowDunkMot() const;                                // 0x11C
        virtual void RingHit();                                          // 0x120
        virtual bcs::CScnObj *GetMdlObj(int idx);                        // 0x124
        virtual const bcs::CScnObj *GetMdlObj(int idx) const;            // 0x128
};

#endif /* !CGOALOBJ_HPP_ */
