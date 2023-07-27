#ifndef CORE_HPP
#define CORE_HPP

#include <libbgcode.hpp>

#include "core/export.h"

#include <memory>

namespace bgcode { namespace core {

BGCODE_CORE_EXPORT std::unique_ptr<AbstractInterface> create_abstract_interface();

}} // bgcode::core

#endif // CORE_HPP
