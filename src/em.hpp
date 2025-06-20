//
// Created by james on 19/06/25.
//

#pragma once

//
// std
//
#include <vector>

namespace rene
{
    /// \brief Emphasis of a sub-string (highlighting)
    struct Em
    {
        enum Kind { NONE, MATCH, VAR } kind=NONE;
        size_t begin=0, end=0;
    };

    using emlist_type = std::vector<Em>;
}
