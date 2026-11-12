#ifndef IEVNODEBINDABLE_HPP_
#define IEVNODEBINDABLE_HPP_

#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // What a node path can move: a position, a rotation and a scale, handed
    // to three virtual setters (DOL class, vptr at 0x00; the base of
    // msw::CEventModel). Named by GetMatrix (0x80111C08). Its vtable
    // (0x8046F0D8: the destructor and the three setters) and its weak
    // destructor (0x80111D60) are emitted by CEventModel.cpp, whose
    // constructor inlines this one. Member names guessed.
    class IEvNodeBindable {
        public:
            IEvNodeBindable() : m_vScale(1.0f, 1.0f, 1.0f) {}
            virtual ~IEvNodeBindable() {}
            virtual void _SetPos(const bcs::FVEC3 &) {}   // 0x0C
            virtual void _SetRot(const bcs::FVEC3 &) {}   // 0x10
            virtual void _SetScale(const bcs::FVEC3 &) {} // 0x14

            // (DOL) The matrix made of the scale, the rotation and the
            // position plus its offset.
            void GetMatrix(bcs::FMTX34 &rMtx) const;

            // The position (without the offset). Inline: TourF game/tour/
            // CTournamentStep.cpp has a weak copy.
            const bcs::FVEC3 &GetPos() const { return m_vPos; }

        protected:
            bcs::FVEC3 m_vPos;            // 0x04
            bcs::FVEC3 m_vRot;            // 0x10, radians
            bcs::FVEC3 m_vScale;          // 0x1C
            bcs::FVEC3 m_vPosOffset;      // 0x28, added to m_vPos
    }; // size 0x34
}

#endif /* !IEVNODEBINDABLE_HPP_ */
