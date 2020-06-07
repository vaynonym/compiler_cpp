#pragma once

#include <string>
#include <map>
#include <vector>

class BasicType {
  public:
    const std::string id;
    BasicType(const std::string id);

    bool isConvertibleTo(const BasicType *other) const;
    virtual const BasicType * getMember(const std::string& id) const;
};

class StructType : public BasicType {
  private:
    const std::map<const std::string, const BasicType *> members;
  public:
    StructType(const std::string id, const std::map<const std::string, const BasicType *> members);

    virtual const BasicType * getMember(const std::string& id) const;
};

class FunctionType {
  public:
    const BasicType *returnType;
    const std::vector<const BasicType *> parameters;
    FunctionType(const BasicType *returnType, const std::vector<const BasicType *> parameters);
};