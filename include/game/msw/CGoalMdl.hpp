#ifndef MSW_CGOALMDL_HPP_
#define MSW_CGOALMDL_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    class CStgGmk;

    // The model of a goal (CGoalObj's model 1). A DOL class, size 0x340,
    // constructor __ct__Q23msw8CGoalMdlFPQ23msw7CStgGmk; msw::CStgMdl's vptr
    // stays at 0x128. Its own members past the sub type are not mapped. Its
    // vtable in the DOL is 0x134 bytes and ends with GetNodeExInfo; the
    // animation tables are CGoalMdlHockey's. GetSubType, SetSubType and
    // GetNodeExInfo are inline (emitted weak by BasketF
    // game/basket/CGoalMdlBasket.cpp and HockeyF game/hockey/CGoalMdlHockey.cpp).
    class CGoalMdl : public CStgMdl {
        public:
            // One animation of the goal (an entry of
            // CGoalMdlHockey::GetAnmParamTbl; the table ends with id -1).
            // Field meanings guessed from the values.
            struct ANM_PARAM {
                long nId;        // 0x00, index into GetAnmNameTbl
                long nLoop;      // 0x04
                float fSpeed;    // 0x08
                float fFrame;    // 0x0C
                float fBlend;    // 0x10
                long nFlag;      // 0x14
            };

            // The model object of a goal that fades by view: a DOL class
            // (vtable __vt__Q33msw8CGoalMdl12CMdlObj_Goal, 0x194 bytes), size
            // 0x230. Its three virtual functions are the DOL's unnamed
            // fn_80151B54, fn_80150D80 and fn_80151B4C; names guessed.
            class CMdlObj_Goal : public CFgMdl {
                public:
                    CMdlObj_Goal() : m_nUnk224(-1), m_nUnk228(-1), m_nUnk22C(0) {}
                    virtual ~CMdlObj_Goal();

                    virtual void SetUnk224(long n);     // 0x188
                    virtual long GetUnk224() const;     // 0x18C
                    virtual void SetUnk22C(long n);     // 0x190

                private:
                    long m_nUnk224; // 0x224
                    long m_nUnk228; // 0x228
                    long m_nUnk22C; // 0x22C
            };

            CGoalMdl(CStgGmk *pParent);
            virtual ~CGoalMdl();
            virtual void Create();
            virtual void Main(float rate);
            virtual void Commit();
            virtual void OnPostCreate();

            virtual void SetSubType(long type) { m_nSubType = type; } // 0x124
            virtual long GetSubType() const { return m_nSubType; }     // 0x128
            virtual void SetScore(long score);                          // 0x12C
            // The per-node motion settings of the model, by stage; none here.
            virtual const bcs::CAnmObjMotBase::NODE_EX_INFO *GetNodeExInfo() const { return 0; } // 0x130

        private:
            long m_nSubType;                       // 0x12C
            unsigned char m_Unk130[0x340 - 0x130]; // 0x130
    };
}

#endif /* !MSW_CGOALMDL_HPP_ */
