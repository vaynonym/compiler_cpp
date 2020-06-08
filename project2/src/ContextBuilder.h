#pragma once

#include "BasicVisitor.h"
#include "Context.h"

class ContextBuilder : public BasicVisitor {
  public:
    ContextBuilder();
    // Returns the built context.
    // This is the default context, which may only be deleted once.
    // Calling this method moves the context out of this object, and
    // this object cannot be used afterwards. The caller must clean
    // up the context object.
    Context *getGlobalContext();

    virtual void visitPDefs(PDefs *p);
    virtual void visitDFun(DFun *p);
    virtual void visitDStruct(DStruct *p);
};