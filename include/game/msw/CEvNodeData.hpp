#ifndef CEVNODEDATA_HPP_
#define CEVNODEDATA_HPP_

#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "game/msw/CEventNodeSimple.hpp"

namespace msw {

    // Reads the path of one node out of a skeletal animation (DOL class,
    // 8 bytes, vptr at 0x00). msw::CTournamentRouteMan_Stage32::Initialize
    // keeps one on the stack to read where its cannons put the character.
    // Names of the parameters guessed. A node path: Stage33 (TourF) hands
    // one to msw::CEventModel::SetLinkNode; its constructor stores only its
    // own vtable, as the inline CEventNodeBase constructor stores none.
    class CEvNodeData : public CEventNodeBase {
        public:
            CEvNodeData();
            virtual ~CEvNodeData();

            // Animation `pName` of `file`; a null handle when there is none.
            static nw4r::g3d::ResAnmChr _GetResAnm(const nw4r::g3d::ResFile &file, const char *pName);
            // Index of node `pNode` in `anm`, -1 when it has none.
            static long _GetAnmId(const nw4r::g3d::ResAnmChr &anm, const char *pNode);

            // (name guessed) Inline in the modules: makes animation `pName`
            // of `file` the one read.
            void SetResAnm(const nw4r::g3d::ResFile &file, const char *pName)
            {
                m_anm = _GetResAnm(file, pName);
            }

            // (name guessed) Inline in the modules: the node index of
            // `pNode` in the animation set by SetResAnm.
            long GetAnmId(const char *pNode) const
            {
                return _GetAnmId(m_anm, pNode);
            }

            // (name guessed) Inline in TourF: makes `anm` the animation read.
            void SetResAnm(nw4r::g3d::ResAnmChr anm)
            {
                m_anm = anm;
            }

            // (name guessed; DOL 0x801FD59C) Whether the animation has a
            // node nId.
            virtual bool IsEnable(long nId);
            // Position and rotation of node nId at frame fFrame.
            virtual void GetResult(bcs::FVEC3 &pos, bcs::FVEC3 &rot, float fFrame, long nId);
            // (DOL, 0x801FDA04 and 0x801FDDA8; signatures from the base)
            virtual void GetResultVec(bcs::FVEC3 &pos, bcs::FVEC3 &rot, bcs::FVEC3 &vec, float fFrame, long nId);
            virtual void GetResultEx(bcs::FVEC3 &pos, bcs::FVEC3 &rot, bcs::FVEC3 &vec, float fFrame, long nId);
            // Length of the animation in frames, 0 without one.
            virtual float GetNumFrame() const;

        private:
            nw4r::g3d::ResAnmChr m_anm; // 0x04
    }; // size 0x8
}

#endif /* !CEVNODEDATA_HPP_ */
