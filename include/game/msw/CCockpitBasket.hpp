#ifndef CCOCKPITBASKET_HPP_
#define CCOCKPITBASKET_HPP_

namespace msw {

    // The basket game's cockpit (BasketF). Only the members used so far; the
    // class itself is not mapped. The mission counter functions are static:
    // each finds the CCockpitBasketCoder layout through CCockpit::GetTask and
    // passes the other arguments on to it, only when nIdx is 0.
    class CCockpitBasket {
        public:
            static void SetMissionCountDisp(long nIdx, int bDisp);
            static void SetMissionCountNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountUpNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountActiveColor(int bActive);
    };
}

#endif /* !CCOCKPITBASKET_HPP_ */
