#ifndef CCOCKPITPOINTER_HPP_
#define CCOCKPITPOINTER_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // (name guessed: no function of this class has a name in the DOL) The
    // pointer (cursor) display of the cockpit, object 0x1016, made by
    // CCockpit::CreatePointerObj. Its functions are at 0x801965AC to
    // 0x80198070 in the DOL, its vtable at lbl_804801E0 (a CMenuLinkBase
    // with a CMenuLytObj). Layout not mapped; only the members used.
    class CCockpitPointer {
        public:
            // (DOL fn_80197A88; name and return type guessed) Pad nPad's
            // pointer position on the screen; below 0 when there is none.
            long GetPointerPos(long nPad, bcs::FVEC3 *pPos);
            // (DOL fn_80197910; name guessed) Pad nPad's pointer look: the
            // player number, with 0x40 set while the pad is in stick mode.
            void SetPointerKind(long nPad, long nKind);
    };
}

#endif /* !CCOCKPITPOINTER_HPP_ */
