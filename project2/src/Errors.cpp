#include "Errors.h"

// TypeCheckingError
string TypeCheckingError::printError() {
    return "TypeCheckingError!!!";
}

// TypeMismatch
TypeMismatch::TypeMismatch(string expectedType, string actualType, string additionalInformation = "")
    : expectedType(expectedType), actualType(actualType), additionalInformation(additionalInformation) {

    }

TypeMismatch::~TypeMismatch() {}

string TypeMismatch::printError(){
    
    string errorString = "Expected type " + expectedType + "\n"
                  + "but got type " + actualType + "\n";

    if(additionalInformation != "")
        errorString += "Look toward " + additionalInformation + "\n";

    return errorString;          
}

// FunctionSignatureMismatch
FunctionSignatureMismatch::FunctionSignatureMismatch(string expectedSignature, string actualSignature, string functionIdentifier, string additionalInformation = "")
    : expectedSignature(expectedSignature), actualSignature(actualSignature), functionIdentifier(functionIdentifier), additionalInformation(additionalInformation) {

}

FunctionSignatureMismatch::~FunctionSignatureMismatch() {}

string FunctionSignatureMismatch::printError(){
    
    string errorString = "Expected signature" + expectedSignature + "for function" + functionIdentifier + "\n"
                  + "but got signature" + expectedSignature + "\n";

    if(additionalInformation != "")
        errorString += "Look toward " + additionalInformation + "\n";

    return errorString;   
}

// UnknownType
UnknownType::UnknownType(string unknownType, string additionalInformation = "") 
    : unknownType(unknownType), additionalInformation(additionalInformation) {

} 

UnknownType::~UnknownType() {}

string UnknownType::printError(){
    
    string errorString = "Type " + unknownType + "is unknown.\n" +
                         "As are many things in life.\n" +
                         "Keep seeking.\n";


    if(additionalInformation != "")
        errorString += "Look toward " + additionalInformation + "\n";

    return errorString;  
}

// TypeAlreadyExists
TypeAlreadyExists::TypeAlreadyExists(string type, string additionalInformation = "")
    : type(type), additionalInformation(additionalInformation) {

}

TypeAlreadyExists::~TypeAlreadyExists() {}

string TypeAlreadyExists::printError() {

    string errorString = "Type " + type + " already defined.\n" +
                         "A rare thing, to know for certain.\n" +
                         "Why not honor that fact?\n";


    if(additionalInformation != "")
        errorString += "Look toward " + additionalInformation + "\n";

    return errorString;  
}

// UnknownIdentifier

UnknownIdentifier::UnknownIdentifier(string unknownIdentifier, string additionalInformation = "")
    : unknownIdentifier(unknownIdentifier), additionalInformation(additionalInformation) {

}

UnknownIdentifier::~UnknownIdentifier() {}

string UnknownIdentifier::printError() {
    
    string errorString = "Identifier " + unknownIdentifier + "is unknown.\n" +
                         "While your wildest fantasies run rampant,\n" +
                         "Do not forget the ground you stand on.\n";


    if(additionalInformation != "")
        errorString += "Look toward " + additionalInformation + "\n";

    return errorString;  
}

// IdentifierAlreadyExists

IdentifierAlreadyExists::IdentifierAlreadyExists(string identifier, string additionalInformation = "")
    : identifier(identifier), additionalInformation(additionalInformation) {

}

IdentifierAlreadyExists::~IdentifierAlreadyExists() {}

string IdentifierAlreadyExists::printError() {

    string errorString = "Identifier " + identifier + " already defined.\n" +
                         "The past's certainty is tempting.\n" +
                         "But to grow, we must try something new.\n";


    if(additionalInformation != "")
        errorString += "Look toward " + additionalInformation + "\n";

    return errorString;  
}

