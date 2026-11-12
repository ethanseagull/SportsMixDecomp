#ifndef CMENULINKBASE_HPP_
#define CMENULINKBASE_HPP_

namespace msw {
    class CMenuTask;

    // Size 0x1C. Base of every menu class made by newMenuClass<T>(long).
    // Member names are guesses. Main and SendEvent are pure; the four after
    // them are inline with empty bodies:
    //  - DodgeF and HockeyF have __vt__Q23msw13CMenuLinkBase: destructor,
    //    two null entries (Main, SendEvent), then Disp, Set, Remove,
    //    SetVisible.
    //  - BossF, BasketF, DodgeF, HockeyF and VolleyF emit SetVisible,
    //    Remove and Set (in that order, the reverse of their declaration)
    //    at the end of their CRadarIcon2D<Game> unit, for that class's vtable.
    // Classes whose vtables have null entries for Set and Remove
    // (CCockpitLayoutBase and its family) declare the two pure again.
    class CMenuLinkBase {
        public:
            CMenuLinkBase(long nId)
                : m_Unk08(0)
                , m_nId(nId)
                , m_Unk10(0)
                , m_Unk14(0)
            {
            }

            unsigned long m_Unk00; // 0x00, not set by the constructor
            unsigned long m_Unk04; // 0x04, not set by the constructor
            // 0x08: CMenuTask::ModListMainLoop turns 1 into 2 and deletes
            // the object at 2 (compared signed in the DOL).
            long m_Unk08;
            long m_nId;            // 0x0C, the constructor argument
            // 0x10: the task it was added to (CMenuTask::AddObj); see
            // GetParentPtr.
            CMenuTask *m_Unk10;
            // 0x14: pause count (CMenuTask::SetPauseObj); Main runs only
            // at 0 (compared signed in the DOL).
            long m_Unk14;

            // (symbol in a module; return type guessed) The task at 0x10.
            CMenuTask *GetParentPtr() { return m_Unk10; }

            // vptr at 0x18
            virtual ~CMenuLinkBase() {}
            virtual void Main() = 0;
            virtual long SendEvent(long nEvent, void *pParam) = 0;
            virtual void Disp() {}
            virtual int Set() { return 0; } // return type guessed
            virtual void Remove() {}
            virtual void SetVisible(int bVisible) {}
    };
}

#endif /* !CMENULINKBASE_HPP_ */
