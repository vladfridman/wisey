//
//  ThreadDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/DerivedTypes.h>

#include "wisey/Environment.hpp"
#include "wisey/Log.hpp"
#include "wisey/ThreadDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ThreadDefinition::ThreadDefinition(AccessLevel accessLevel,
                                   ThreadTypeSpecifierFull* threadTypeSpecifierFull,
                                   ITypeSpecifier* sendsTypeSpecifier,
                                   vector<IObjectElementDeclaration*> objectElementDeclarations,
                                   vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                                   vector<IObjectDefinition*> innerObjectDefinitions) :
mAccessLevel(accessLevel),
mThreadTypeSpecifierFull(threadTypeSpecifierFull),
mSendsTypeSpecifier(sendsTypeSpecifier),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions) { }

ThreadDefinition::~ThreadDefinition() {
  delete mThreadTypeSpecifierFull;
  delete mSendsTypeSpecifier;
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

Thread* ThreadDefinition::prototypeObject(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mThreadTypeSpecifierFull);
  
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Thread* thread = Thread::newThread(mAccessLevel, fullName, structType);
  context.addThread(thread);
  thread->setImportProfile(context.getImportProfile());
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(thread);
  IObjectDefinition::prototypeInnerObjects(context, thread, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return thread;
}

void ThreadDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mThreadTypeSpecifierFull);
  Thread* thread = context.getThread(fullName);
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(thread);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, thread, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(lastObjectType);
}

Value* ThreadDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mThreadTypeSpecifierFull);
  Thread* thread = context.getThread(fullName);
  
  context.getScopes().pushScope();
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(thread);
  
  IObjectDefinition::generateInnerObjectIR(context, mInnerObjectDefinitions);
  IConcreteObjectType::defineCurrentObjectNameVariable(context, thread);
  IConcreteObjectType::generateStaticMethodsIR(context, thread);
  IConcreteObjectType::declareFieldVariables(context, thread);
  IConcreteObjectType::generateMethodsIR(context, thread);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, thread);
  IConcreteObjectType::composeInterfaceMapFunctions(context, thread);
  
  context.setObjectType(lastObjectType);
  context.getScopes().popScope(context, 0);
  
  return NULL;
}

