//
//  BindActionGlobalStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/BindActionGlobalStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BindActionGlobalStatement::BindActionGlobalStatement(BindAction* bindAction) :
mBindAction(bindAction) { }

BindActionGlobalStatement::~BindActionGlobalStatement() {
  delete mBindAction;
}

IObjectType* BindActionGlobalStatement::prototypeObject(IRGenerationContext& context) const {
  return NULL;
}

void BindActionGlobalStatement::prototypeMethods(IRGenerationContext& context) const {
  const Interface* interface = mBindAction->getInterface(context);
  const Controller* controller = mBindAction->getController(context);
  context.bindInterfaceToController(interface, controller);
}

Value* BindActionGlobalStatement::generateIR(IRGenerationContext& context) const {
  return NULL;
}
