#ifndef MSW_CBALLVOLLEY_HPP_
#define MSW_CBALLVOLLEY_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    // The ball of the volley game (VolleyF), size 0x48C (the size
    // CBallManVolley allocates): CBall plus 0x174 bytes of its own.
    //
    // INCOMPLETE: only what game/volley/CBallManVolley.cpp needs to create
    // one. The class overrides most of CBall's virtual functions and adds
    // its own (__vt__Q23msw11CBallVolley has 334 words); none of that is
    // declared yet, and the members are not mapped.
    class CBallVolley : public CBall {
        public:
            CBallVolley(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallVolley();

        private:
            unsigned char m_Unk318[0x174]; // 0x318
    };

    // The ball of the volley game against a boss (VolleyF), size 0x494 (the
    // size BALL_MAN_VOLLEY::CBallManVolleyVsBoss allocates): CBallVolley
    // plus 8 bytes. Its constructor has CBallVolley's inline.
    //
    // INCOMPLETE: only what game/volley/CBallManVolley.cpp needs to create
    // one.
    class CBallVolleyVsBoss : public CBallVolley {
        public:
            CBallVolleyVsBoss(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallVolleyVsBoss();

        private:
            unsigned char m_Unk48C[0x8]; // 0x48C
    };
}

#endif /* !MSW_CBALLVOLLEY_HPP_ */
