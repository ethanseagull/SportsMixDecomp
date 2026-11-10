#ifndef MSW_CSTAGEDODGE13_HPP_
#define MSW_CSTAGEDODGE13_HPP_

#include "game/msw/CStageManDodge.hpp"

namespace msw {

    // Basket stage 13. Size 0x26C (from CStageFactoryDodge::Create). The
    // models named "null_scr<n>" (n from 1 to 4) turn about Y, each at its
    // own speed: one turn takes 9000 + 7800 n frames. The same stage is in
    // DodgeF, HockeyF and VolleyF (CStageDodge13 and so on), on the mode's
    // own stage manager.
    class CStageDodge13 : public CStageManDodge {
        public:
            // (member names guessed) One turning model, size 0x10.
            struct ROT_MDL {
                ROT_MDL();
                ~ROT_MDL();

                CMdlPtrPool::Handle m_Handle; // 0x00
                float m_fTime;                // 0x08, frames into the turn
                float m_fPeriod;              // 0x0C, frames for one turn
            };

            CStageDodge13(bcs::CTask *pParent);
            virtual ~CStageDodge13();

            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnSceneEntry();

            // Turns model i by `frame` frames.
            virtual void UpdateMdl(unsigned long i, float frame);

        private:
            enum {
                ROT_MDL_NUM = 5 // entry 0 is not used: the models are numbered from 1
            };

            long m_Unk218;                       // 0x218, set to 0 and not read here
            ROT_MDL m_aRotMdl[ROT_MDL_NUM];      // 0x21C
    };
}

#endif /* !MSW_CSTAGEDODGE13_HPP_ */
