//
//  ImportStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "ImportStatement.hpp"
#include "Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ImportStatement::ImportStatement(IObjectTypeSpecifier* typeSpecifier) :
mTypeSpecifier(typeSpecifier) { }

ImportStatement::~ImportStatement() {
  delete mTypeSpecifier;
}

IObjectType* ImportStatement::prototypeObject(IRGenerationContext& context,
                                              ImportProfile* importProfile) const {
  context.getImportProfile()->addImport(mTypeSpecifier->getShortName(),
                                        mTypeSpecifier->getName(context));
  return NULL;
}

void ImportStatement::prototypeMethods(IRGenerationContext& context) const {
}

void ImportStatement::generateIR(IRGenerationContext& context) const {
}
