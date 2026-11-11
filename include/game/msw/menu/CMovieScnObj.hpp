#ifndef MENU_CMOVIESCNOBJ_HPP_
#define MENU_CMOVIESCNOBJ_HPP_

#include "bcs/CGDLObj.hpp"

namespace msw {
    namespace menu {

        // Size 0x148. A scene object that draws the frames of bcs::CMovie
        // on a rectangle. Lives in the DOL; only the members used so far
        // (added for MenuF game/menu/CTitle.cpp). The argument names are
        // guesses.
        class CMovieScnObj : public bcs::CGDLObj {
            public:
                CMovieScnObj();
                virtual ~CMovieScnObj();
                // Draws the movie's frame in the opaque pass (task 6) while a
                // display list is set; other tasks go to CGDLObj.
                virtual void ObjProc(unsigned long nTask, unsigned long nParam, void *pInfo);

                // Screen rectangle (0x128 to 0x134).
                void SetDrawPositionXYWH(float fX, float fY, float fW, float fH);
                // Texture coordinates (0x138 to 0x144).
                void SetDrawTextureST(float fS0, float fT0, float fS1, float fT1);
                void SetDrawFlag(int bDraw);

            private:
                // Declared so the class is not abstract here (CGDLObj leaves
                // it pure); taken to be one of the unnamed DOL functions
                // after the setters.
                virtual void UpdateFrame(float);

                float m_fDrawX;          // 0x128
                float m_fDrawY;          // 0x12C
                float m_fDrawW;          // 0x130
                float m_fDrawH;          // 0x134
                float m_fTexS0;          // 0x138
                float m_fTexT0;          // 0x13C
                float m_fTexS1;          // 0x140
                float m_fTexT1;          // 0x144
        };
    }
}

#endif /* !MENU_CMOVIESCNOBJ_HPP_ */
