#ifndef NW4R_MATH_EQUATION_H_
#define NW4R_MATH_EQUATION_H_

// NW4R's equation solvers (DOL). Added for BossF game/boss/CBallBoss.cpp;
// the shape follows the public ogws decompilation's nw4r/math, no code
// copied.

namespace nw4r {
    namespace math {
        // The real roots of a * x^2 + b * x + c = 0 in pRoots (two floats);
        // returns how many there are.
        int SolveEquation2(float *pRoots, float a, float b, float c);
    }
}

#endif /* !NW4R_MATH_EQUATION_H_ */
