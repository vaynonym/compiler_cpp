#include "Types.h"
#include "Context.h"

BasicType::BasicType(const std::string id) : Id(id) { }

bool BasicType::isConvertibleTo(const BasicType *other) const {
  if (this == Context::TYPE_INT && other == Context::TYPE_DOUBLE)
    return true;

  return this == other;
}