#ifndef MSW_CSTGGMK_CONTAINER_HOCKEY_HPP_
#define MSW_CSTGGMK_CONTAINER_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"

namespace msw {

    // (purpose guessed from the names) The containers of the conveyor belt
    // stage (stage 09): crates the belts carry along, holding an item, a bomb
    // or treasure (the type given to the constructor). A case of the ship
    // stage with its own actor.

    // The actor of a container. A DOL class, size 0x214 (nothing added to
    // msw::CStgGmk_Case_Act); its vtable __vt__Q23msw21CStgGmk_Container_Act
    // is in the DOL. Its constructor is inline (msw::CStgGmk_Container::NewAct).
    class CStgGmk_Container_Act : public CStgGmk_Case_Act {
        public:
            CStgGmk_Container_Act(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) {}
            virtual ~CStgGmk_Container_Act();
    };

    // The hockey version of the actor, size 0x218, with the
    // HOCKEY_PRM_S09_CONTAINER_* parameters. Its functions are in
    // game/hockey/CStgGmk_Container_Act_Hockey.cpp (not written yet); only the
    // constructor is inline (msw::CStgGmk_Container_Hockey::NewAct).
    class CStgGmk_Container_Act_Hockey : public CStgGmk_Container_Act {
        public:
            CStgGmk_Container_Act_Hockey(CStgGmk *pParent) : CStgGmk_Container_Act(pParent) { m_fUnk214 = 0.0f; }
            virtual ~CStgGmk_Container_Act_Hockey();

        private:
            float m_fUnk214; // 0x214 (meaning not known)
    };

    // The hockey model of a container, size 0x134 (nothing added to
    // msw::CStgGmk_Case_Mdl). Create and Main are in
    // game/hockey/CStgGmk_Container_Act_Hockey.cpp; the constructor is inline
    // (msw::CStgGmk_Container_Hockey::NewMdl).
    class CStgGmk_Container_Mdl_Hockey : public CStgGmk_Case_Mdl {
        public:
            CStgGmk_Container_Mdl_Hockey(CStgGmk *pParent, long type) : CStgGmk_Case_Mdl(pParent, type) {}
            virtual void Create();
            virtual void Main(float rate);
    };

    // One container, size 0x78 (nothing added to msw::CStgGmk_Case). All
    // inline: HockeyF's game/hockey/CStageHockey09.cpp emits the vtable and the
    // functions. Its gimmick type is 6 (what CStgGmkMan's lookups ask for).
    class CStgGmk_Container : public CStgGmk_Case {
        public:
            CStgGmk_Container(bcs::CTask *pParent, int type) : CStgGmk_Case(pParent, type) { SetType(6); }
            virtual ~CStgGmk_Container() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                return new (heap) CStgGmk_Case_Mdl(this, GetCaseType());
            }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Container_Act(this); }
    };

    // The hockey container: the hockey actor and model. Made by
    // msw::CStageHockey09::ConveyorUpdate. NewAct, NewMdl and the destructor
    // are emitted in reverse order of the declarations here.
    class CStgGmk_Container_Hockey : public CStgGmk_Container {
        public:
            CStgGmk_Container_Hockey(bcs::CTask *pParent, int type) : CStgGmk_Container(pParent, type) {}
            virtual ~CStgGmk_Container_Hockey() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                return new (heap) CStgGmk_Container_Mdl_Hockey(this, GetCaseType());
            }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Container_Act_Hockey(this); }
    };
}

#endif /* !MSW_CSTGGMK_CONTAINER_HOCKEY_HPP_ */
