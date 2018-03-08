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

#include "wisey/Block.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FakeExpressionWithCleanup.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/Names.hpp"
#include "wisey/NativeFunctionCall.hpp"
#include "wisey/NativeType.hpp"
#include "wisey/NativeTypeSpecifier.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadDefinition.hpp"
#include "wisey/ThreadInfrastructure.hpp"

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
  vector<IObjectElementDeclaration*> withThreadElements = addThreadObjectElements(context, thread);
  configureObject(context, thread, withThreadElements, mInterfaceSpecifiers);
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

MethodDeclaration* ThreadDefinition::createStartMethodDeclaration(IRGenerationContext& context,
                                                                  const Thread* thread) const {
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  StatementList& statements = block->getStatements();
  Function* runBridgeFunction = ThreadInfrastructure::getRunBridgeFunction(context, thread);
  NativeType* runBridgeFunctionType = new NativeType(runBridgeFunction->getType());
  NativeType* threadAtrributesType =
  ThreadInfrastructure::createNativeThreadAttributesType(context);
  Value* nullValue = ConstantPointerNull::get((PointerType*) threadAtrributesType->
                                              getLLVMType(context));
  vector<IExpression*> callArguments;
  callArguments.push_back(new Identifier("mNativeThread"));
  callArguments.push_back(new FakeExpressionWithCleanup(nullValue, threadAtrributesType));
  callArguments.push_back(new FakeExpressionWithCleanup(runBridgeFunction, runBridgeFunctionType));
  callArguments.push_back(new NullExpression());
  Function* createFunction = ThreadInfrastructure::getThreadCreateFunction(context);
  NativeFunctionCall* nativeFunctionCall = new NativeFunctionCall(createFunction, callArguments);
  statements.push_back(nativeFunctionCall);
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  
  return new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                               new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE),
                               "start",
                               arguments,
                               exceptions,
                               compoundStatement,
                               0);
}

FieldDeclaration* ThreadDefinition::createNativeThreadHandleField(IRGenerationContext&
                                                                   context) const {
  InjectionArgumentList arguments;
  
  NativeType* nativeType = ThreadInfrastructure::createNativeThreadType(context);
  NativeTypeSpecifier* nativeTypeSpecifier = new NativeTypeSpecifier(nativeType);
  return new FieldDeclaration(FieldKind::STATE_FIELD,
                              nativeTypeSpecifier,
                              "mNativeThread",
                              arguments);
}

vector<IObjectElementDeclaration*>
ThreadDefinition::addThreadObjectElements(IRGenerationContext& context,
                                          const Thread* thread) const {
  vector<IObjectElementDeclaration*> result;
  result.push_back(createNativeThreadHandleField(context));
  for (IObjectElementDeclaration* element : mObjectElementDeclarations) {
    result.push_back(element);
  }
  result.push_back(createStartMethodDeclaration(context, thread));

  return result;
}
