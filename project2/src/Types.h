#pragma once

#include <string>

class BasicType {
  public:
    const std::string Id;
    BasicType(const std::string id);

    bool isConvertibleTo(const BasicType *other) const;
};

class FunctionType {

};