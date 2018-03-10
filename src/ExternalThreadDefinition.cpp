//
//  ExternalThreadDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalThreadDefinition.hpp"
#include "wisey/ThreadDefinition.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ExternalThreadDefinition::ExternalThreadDefinition(ThreadTypeSpecifierFull*
                                                   threadTypeSpecifierFull,
                                                   vector<IObjectElementDeclaration*>
                                                   objectElementDeclarations,
                                                   vector<IInterfaceTypeSpecifier*>
                                                   interfaceSpecifiers,
                                                   vector<IObjectDefinition*>
                                                   innerObjectDefinitions) :
mThreadTypeSpecifierFull(threadTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions) { }

ExternalThreadDefinition::~ExternalThreadDefinition() {
  delete mThreadTypeSpecifierFull;
  for (IObjectElementDeclaration* objectElementDeclaration : mObjectElementDeclarations) {
    delete objectElementDeclaration;
  }
  mObjectElementDeclarations.clear();
  for (IInterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
  for (IObjectDefinition* innerObjectDefinition : mInnerObjectDefinitions) {
    delete innerObjectDefinition;
  }
  mInnerObjectDefinitions.clear();
}

Thread* ExternalThreadDefinition::prototypeObject(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mThreadTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Thread* thread = Thread::newExternalThread(fullName, structType);
  context.addThread(thread);
  thread->setImportProfile(context.getImportProfile());
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(thread);
  IObjectDefinition::prototypeInnerObjects(context, thread, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return thread;
}

void ExternalThreadDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mThreadTypeSpecifierFull);
  Thread* thread = context.getThread(fullName);
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(thread);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  vector<IObjectElementDeclaration*> withThreadElements =
  ThreadDefinition::addThreadObjectElements(context, mObjectElementDeclarations, thread);
  configureObject(context, thread, withThreadElements, mInterfaceSpecifiers);
  context.setObjectType(lastObjectType);
}

Value* ExternalThreadDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}
