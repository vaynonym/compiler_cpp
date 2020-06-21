#include "Types.h"
#include "Context.h"

BasicType::BasicType(const std::string id) : id(id) { }

bool BasicType::isConvertibleTo(const BasicType *other) const {
  if (this == Context::TYPE_INT && other == Context::TYPE_DOUBLE)
    return true;

  return this == other;
}

const BasicType * BasicType::getMember(const std::string& id) const {
  return nullptr;
}


StructType::StructType(const std::string id, const std::map<const std::string, const BasicType *> members)
  : BasicType(id), members(members) { }

const BasicType * StructType::getMember(const std::string& id) const {
  auto res = members.find(id);
  if (res == members.end()) return nullptr;
  return (*res).second;
}

const std::vector<const BasicType *> StructType::getMemberTypes() const {
  std::vector<const BasicType *> types;

  for (auto pair : members) {
    types.push_back(pair.second);
  }

  return types;
}

FunctionType::FunctionType(const BasicType *returnType, const std::vector<const BasicType *> parameters)
  : returnType(returnType), parameters(parameters) { }