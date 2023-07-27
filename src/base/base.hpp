#ifndef BASE_HPP
#define BASE_HPP

#include <libbgcode.hpp>

#include "base/export.h"

#include <memory>

namespace bgcode { namespace base {

BGCODE_BASE_EXPORT std::unique_ptr<AbstractInterface> create_abstract_interface();

}} // bgcode::core

#endif // BASE_HPP
