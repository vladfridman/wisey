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
  const string interfaceName = mBindAction->getInterface()->getName(context);
  const string controllerName = mBindAction->getController()->getName(context);
  context.bindInterfaceToController(interfaceName, controllerName, mLine);
  return NULL;
}

void BindActionGlobalStatement::prototypeMethods(IRGenerationContext& context) const {
}

void BindActionGlobalStatement::generateIR(IRGenerationContext& context) const {
  const Interface* interface = mBindAction->getInterface()->getType(context);
  const Controller* controller = mBindAction->getController()->getType(context);
  if (IConcreteObjectType::getInterfaceIndex(controller, interface) < 0) {
    context.reportError(mLine, "Can not bind interface " + interface->getTypeName() + " to " +
                        controller->getTypeName() + " because it does not implement the interface");
    throw 1;
  }
}
