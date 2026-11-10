#ifndef CCOCKPITVOLLEYLAYOUTBASE_HPP_
#define CCOCKPITVOLLEYLAYOUTBASE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x60: adds no data to CCockpitLayoutBase. Base of the volley game's
    // cockpit layouts (CCockpitVolleyLifeGauge, CCockpitVolleyCoin,
    // CCockpitVolleyCoder, ...). Abstract: Set and Remove of CMenuLinkBase are
    // still pure here.
    //
    // The cockpit is made of parts (score, coins, combo, the score of each
    // ball), each slid out or in, with or without waiting. A layout overrides
    // the parts it owns; the All* functions run the score and coin parts of
    // one kind. The virtuals are in the order of the vtable. Every one
    // returns 0 here; the return type is a guess, no caller in this unit
    // reads it.
    class CCockpitVolleyLayoutBase : public CCockpitLayoutBase {
        public:
            CCockpitVolleyLayoutBase(long nId);
            virtual ~CCockpitVolleyLayoutBase();

            virtual int AllOut();
            virtual int AllIn();
            virtual int AllOutWait();
            virtual int AllInWait();

            // Inline: game/volley/CCockpitVolley.cpp emits them (In, Out,
            // InWait, OutWait for each part).
            virtual int ScoreOut() { return 0; }
            virtual int ScoreIn() { return 0; }
            virtual int ScoreOutWait() { return 0; }
            virtual int ScoreInWait() { return 0; }
            virtual int CoinOut() { return 0; }
            virtual int CoinIn() { return 0; }
            virtual int CoinOutWait() { return 0; }
            virtual int CoinInWait() { return 0; }
            virtual int ComboOut() { return 0; }
            virtual int ComboIn() { return 0; }
            virtual int ComboOutWait() { return 0; }
            virtual int ComboInWait() { return 0; }
            // The argument is not read here; probably the ball (team?) index.
            virtual int BallScoreOut(long) { return 0; }
            virtual int BallScoreIn(long) { return 0; }
            virtual int BallScoreOutWait(long) { return 0; }
            virtual int BallScoreInWait(long) { return 0; }
    };
}

#endif /* !CCOCKPITVOLLEYLAYOUTBASE_HPP_ */
