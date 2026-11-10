#ifndef CEVNODEDATA_HPP_
#define CEVNODEDATA_HPP_

#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    // Reads the path of one node out of a skeletal animation (DOL class,
    // 8 bytes, vptr at 0x00). msw::CTournamentRouteMan_Stage32::Initialize
    // keeps one on the stack to read where its cannons put the character.
    // Names of the parameters guessed.
    class CEvNodeData {
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

            // Length of the animation in frames, 0 without one.
            float GetNumFrame() const;
            // Position and rotation of node nId at frame fFrame.
            void GetResult(bcs::FVEC3 &pos, bcs::FVEC3 &rot, float fFrame, long nId);

        private:
            nw4r::g3d::ResAnmChr m_anm; // 0x04
    }; // size 0x8
}

#endif /* !CEVNODEDATA_HPP_ */
