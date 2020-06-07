#pragma once

#include <exception>
#include <string>

using namespace std;

class TypeCheckingError : public exception {
public:
    virtual ~TypeCheckingError(){};
    virtual string printError() const;

};

class TypeMismatch : public TypeCheckingError {

public:
    TypeMismatch(string expectedType, string actualType, string additionalInformation = "");
    ~TypeMismatch();
    string printError() const;



private:
    string expectedType;
    string actualType;
    string additionalInformation;
};

class FunctionSignatureMismatch : public TypeCheckingError {
    
public:
    FunctionSignatureMismatch(string expectedSignature, string actualSignature, string functionIdentifier, string additionalInformation = "");
    ~FunctionSignatureMismatch();
    string printError() const;

private:
    string expectedSignature;
    string actualSignature;
    string functionIdentifier;
    string additionalInformation;
};

class UnknownType : public TypeCheckingError {

public:
    UnknownType(string unknownType, string additionalInformation = "");
    ~UnknownType();
    string printError() const;

private:
    string unknownType;
    string additionalInformation;
};

class TypeAlreadyExists : public TypeCheckingError {

public:
    TypeAlreadyExists(string type, string additionalInformation = "");
    ~TypeAlreadyExists();
    string printError() const;

private: 
    string type;
    string additionalInformation;
};

class UnknownIdentifier : public TypeCheckingError {
public:
    UnknownIdentifier(string unknownIdentifier, string additionalInformation = "");
    ~UnknownIdentifier();
    string printError() const;

private:
    string unknownIdentifier;
    string additionalInformation;
};

class IdentifierAlreadyExists : public TypeCheckingError {

public:
    IdentifierAlreadyExists(string identifier, string additionalInformation = "");
    ~IdentifierAlreadyExists();
    string printError() const;

private:
    string identifier;
    string additionalInformation;
};


