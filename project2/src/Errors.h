#include <exception>
#include <string>

using namespace std;

class TypeCheckingError : public exception{
public:
    virtual ~TypeCheckingError(){};
    virtual string printError();

};

class TypeMismatch : public TypeCheckingError {

public:
    TypeMismatch(string expectedType, string actualType, string additionalInformation = "");
    ~TypeMismatch();
    string printError();



private:
    string expectedType;
    string actualType;
    string additionalInformation;
};

class FunctionSignatureMismatch : public TypeCheckingError {
    
public:
    FunctionSignatureMismatch(string expectedSignature, string actualSignature, string functionIdentifier, string additionalInformation = "");
    ~FunctionSignatureMismatch();
    string printError();

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
    string printError();

private:
    string unknownType;
    string additionalInformation;
};

class TypeAlreadyExists : public TypeCheckingError {

public:
    TypeAlreadyExists(string type, string additionalInformation = "");
    ~TypeAlreadyExists();
    string printError();

private: 
    string type;
    string additionalInformation;
};

class UnknownIdentifier : public TypeCheckingError {
public:
    UnknownIdentifier(string unknownIdentifier, string additionalInformation = "");
    ~UnknownIdentifier();
    string printError();

private:
    string unknownIdentifier;
    string additionalInformation;
};

class IdentifierAlreadyExists : public TypeCheckingError {

public:
    IdentifierAlreadyExists(string identifier, string additionalInformation = "");
    ~IdentifierAlreadyExists();
    string printError();

private:
    string identifier;
    string additionalInformation;
};


