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

BindActionGlobalStatement::BindActionGlobalStatement(BindAction* bindAction, int line) :
mBindAction(bindAction), mLine(line) { }

BindActionGlobalStatement::~BindActionGlobalStatement() {
  delete mBindAction;
}

IObjectType* BindActionGlobalStatement::prototypeObject(IRGenerationContext& context,
                                                        ImportProfile* importProfile) const {
  return NULL;
}

void BindActionGlobalStatement::prototypeMethods(IRGenerationContext& context) const {
  const Interface* interface = mBindAction->getInterface(context);
  const Controller* controller = mBindAction->getController(context);
  context.bindInterfaceToController(interface, controller, mLine);
}

void BindActionGlobalStatement::generateIR(IRGenerationContext& context) const {
}

int BindActionGlobalStatement::getLine() const {
  return mLine;
}
