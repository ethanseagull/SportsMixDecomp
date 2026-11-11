#ifndef MSW_CCOMOSG_HPP_
#define MSW_CCOMOSG_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // A grid laid over a court for the computer drivers ("OSG": open space
    // grid, name guessed): one signed byte per cell, which the Fill
    // functions set and the Search functions look through. A DOL class
    // (game/msw/CComOSG.cpp there), size 0x74: vptr, a 10 by 10 grid,
    // its width and height (10) and a word set to -1. Member names and
    // return types are guesses.
    class CComOSG {
        public:
            CComOSG();
            virtual ~CComOSG();

            void Init(long nWidth, long nHeight, long nKind);
            void SetValue(long nX, long nY, char nValue, char nMask);
            void FillOne(const bcs::FVEC3 &rPos, char nValue, char nMask);
            void FillCircum(const bcs::FVEC3 &rPos, char nValue, char nMask);
            void FillFront(long, char nValue, char nMask);
            void FillBack(long, char nValue, char nMask);
            void FillFrontBack(char nValue, char nMask);
            void FillLeftRight(char nValue, char nMask);
            int SearchValue(long &rX, long &rY, char nValue, int) const;
            // The cell holding nValue nearest to rPos.
            int SearchValueNearPos(long &rX, long &rY, char nValue, const bcs::FVEC3 &rPos) const;
            // The position of cell (nX, nY).
            void GetPos(bcs::FVEC3 &rPos, long nX, long nY, int) const;

        private:
            signed char m_Grid[10][10]; // 0x04
            long m_nWidth;              // 0x68
            long m_nHeight;             // 0x6C
            long m_nUnk70;              // 0x70
    };
}

#endif /* !MSW_CCOMOSG_HPP_ */
