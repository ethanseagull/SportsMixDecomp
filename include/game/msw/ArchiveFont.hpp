#ifndef ARCHIVEFONT_HPP_
#define ARCHIVEFONT_HPP_

namespace msw {

    // Whether "ArchiveFont" is a namespace or a class with static members
    // cannot be told from the symbols; a namespace is a guess.
    namespace ArchiveFont {
        void Initialize();
        void AttachResource();
        void Dispose();
    }
}

#endif /* !ARCHIVEFONT_HPP_ */
