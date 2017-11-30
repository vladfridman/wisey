//
//  ImportStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ImportStatement.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ImportStatement::ImportStatement(IObjectTypeSpecifier* typeSpecifier) :
mTypeSpecifier(typeSpecifier) { }

ImportStatement::~ImportStatement() {
  delete mTypeSpecifier;
}

void ImportStatement::prototypeObjects(IRGenerationContext& context) const {
  context.getImportProfile()->addImport(mTypeSpecifier->getShortName(),
                                        mTypeSpecifier->getName(context));
}

void ImportStatement::prototypeMethods(IRGenerationContext& context) const {
}

Value* ImportStatement::generateIR(IRGenerationContext& context) const {
  return NULL;
}
