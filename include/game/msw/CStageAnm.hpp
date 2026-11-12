#ifndef MSW_CSTAGEANM_HPP_
#define MSW_CSTAGEANM_HPP_

namespace bcs {
    class CMdlObj;
}

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace msw {

    class CArn;

    // The animations of a stage model; msw::CStgMdl keeps a pointer to one
    // at 0x14 (GetAnmObj) and forwards its animation getters to it. Nothing
    // of it has a name in the DOL except the type below, which msw::CStgMdl's
    // animation getters take
    // (GetAnmFrame__Q23msw7CStgMdlCFQ33msw9CStageAnm7ANM_IDX). The vptr is at
    // 0xA8 (the data members come before the first virtual function); the
    // slots are those the inline CStgMdl getters call (BasketF
    // game/basket/CStgGmk_WaterWorks_Basket.cpp). All names and the slots
    // without a caller are guesses.
    class CStageAnm {
        public:
            // Which of the animations playing at once is meant. Values not
            // known.
            enum ANM_IDX {
            };

        private:
            unsigned char m_Unk00[0xA8]; // 0x00

        public:
            // (DOL fn_8015ABF4) Made by msw::CStgMdl::Create, 0xAC bytes.
            CStageAnm();
            // (DOL fn_8015B2F4, unnamed; name and meaning guessed) The flags
            // CMdlObj::Create takes for entry idx of the archive, from the
            // animations res has for it.
            static unsigned long GetMdlCreateFlag(nw4r::g3d::ResFile res, const CArn *pArn, long idx);
            virtual ~CStageAnm();                                              // 0x08
            // (names and return types guessed; called by the creators in
            // game/msw/CStgMdl.cpp) Make the animations of pMdl: entry idx
            // of an archive, or those named pName plus each suffix of
            // ppSuffix (a null-ended table) in res.
            virtual bool Create(bcs::CMdlObj *pMdl, nw4r::g3d::ResFile res, const CArn *pArn, long idx); // 0x0C
            virtual bool Create(bcs::CMdlObj *pMdl, nw4r::g3d::ResFile res, const char *const *ppSuffix,
                                const char *pName, unsigned long flag);         // 0x10
            virtual void VFunc14();                                            // 0x14
            // (name guessed) Takes MDL_PARAM::pAnmParam, the table of how
            // the animations are played.
            virtual void SetAnmParam(const void *pParam);                     // 0x18
            virtual void VFunc1C();                                            // 0x1C
            virtual void VFunc20();                                            // 0x20
            // (names guessed) Starts animation anm (what msw::CStgMdl::SetAnm
            // passes on), and the short form of msw::CStgMdl::SetAnmEx.
            virtual void SetAnm(unsigned long anm, unsigned long arg, float speed, float frame, float blend,
                                unsigned long flag);                           // 0x24
            virtual void SetAnmEx(unsigned long anm, unsigned long arg);       // 0x28
            virtual unsigned long GetCurAnmId(ANM_IDX idx) const;              // 0x2C
            // (name guessed) Stops animation idx (-1: all); CStgMdl::CancelAnm.
            virtual void Cancel(long idx);                                     // 0x30
            virtual void VFunc34();                                            // 0x34
            virtual bool IsAnmExist(unsigned long anm, ANM_IDX idx) const;     // 0x38
            virtual void SetFrame(long idx, float frame);                      // 0x3C, idx -1: all
            virtual float GetFrame(ANM_IDX idx) const;                         // 0x40
            virtual void VFunc44();                                            // 0x44
            virtual float GetTotalFrame(ANM_IDX idx) const;                    // 0x48
            // (name guessed) The speed of animation idx (-1: all).
            virtual void SetSpeed(long idx, float speed);                      // 0x4C
            virtual void VFunc50();                                            // 0x50
            // (names guessed) The tests behind msw::CStgMdl::IsAnmLooped and
            // IsAnmLoop (anm as CStgMdl passes it).
            virtual bool IsAnmLooped(long anm) const;                          // 0x54
            virtual bool IsAnmLoop(long anm) const;                            // 0x58
            // (name and meaning guessed) Whether the animation idx has
            // ended; msw::CStgGmk_WaterPillar_Act (DOL) passes 1 and -9.
            virtual bool IsEnd(long idx) const;                                // 0x5C
            // (names guessed) msw::CStgMdl::IsAnmLoopedAll and IsAnmLoopAll.
            virtual bool IsAnmLoopedAll(long anm) const;                       // 0x60
            virtual bool IsAnmLoopAll(long anm) const;                         // 0x64
            // (name and meaning guessed) Whether animation idx (-1: all)
            // has looped; VolleyF's CStageVolley10::Main_ then switches the
            // panel to its next animation. msw::CStgMdl::IsAnmEndAll calls
            // this slot, so it may rather be an "ended, all" test.
            virtual bool IsLooped(long idx) const;                             // 0x68
    };
}

#endif /* !MSW_CSTAGEANM_HPP_ */
