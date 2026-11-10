#ifndef EVDEMO_HPP_
#define EVDEMO_HPP_

namespace msw {

    // Event demo helpers (DOL). A namespace by the same guess as
    // msw::EvDemoVs: the name has no "C" prefix.
    namespace EvDemo {
        // Looks up the model resource `pName` in archive `nArc` (a
        // msw::CLoadData entry; a null pSub picks a default sub-folder) and
        // returns its data, or 0. nIndex receives the archive entry's
        // number (-1 when not found). Read from the DOL function; parameter
        // names guessed.
        void *GetArcResDataMdl(long &nIndex, long nArc, const char *pName, const char *pSub);
    }
}

#endif /* !EVDEMO_HPP_ */
