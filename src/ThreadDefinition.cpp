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

#include "wisey/Assignment.hpp"
#include "wisey/Block.hpp"
#include "wisey/CastExpression.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Environment.hpp"
#include "wisey/ExpressionStatement.hpp"
#include "wisey/DereferenceExpression.hpp"
#include "wisey/FakeExpressionWithCleanup.hpp"
#include "wisey/IdentifierReference.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/Names.hpp"
#include "wisey/NativeFunctionCall.hpp"
#include "wisey/NativeType.hpp"
#include "wisey/NativeTypeSpecifier.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/PointerType.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/ThreadDefinition.hpp"
#include "wisey/ThreadInfrastructure.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ThreadDefinition::ThreadDefinition(AccessLevel accessLevel,
                                   ThreadTypeSpecifierFull* threadTypeSpecifierFull,
                                   const ITypeSpecifier* sendsTypeSpecifier,
                                   vector<IObjectElementDefinition*> objectElementDeclarations,
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
  for (IObjectElementDefinition* objectElementDeclaration : mObjectElementDeclarations) {
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
  vector<IObjectElementDefinition*> withThreadElements =
  addThreadObjectElements(context, mObjectElementDeclarations, thread);
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
                                                                  const Thread* thread) {
  LLVMContext& llvmContext = context.getLLVMContext();
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  StatementList& statements = block->getStatements();
  Function* runBridgeFunction = ThreadInfrastructure::getRunBridgeFunction(context, thread);
  NativeType* runBridgeFunctionType = new NativeType(runBridgeFunction->getType());
  NativeType* threadAtrributesType =
  ThreadInfrastructure::createNativeThreadAttributesType(context);
  Value* nullValue = ConstantPointerNull::get((llvm::PointerType*) threadAtrributesType->
                                              getLLVMType(context));
  NativeType* voidPointerType = new NativeType(Type::getInt8Ty(llvmContext)->getPointerTo());
  NativeTypeSpecifier* voidPointerTypeSpecifier = new NativeTypeSpecifier(voidPointerType);
  CastExpression* castExpression = new CastExpression(voidPointerTypeSpecifier,
                                                      new Identifier(IObjectType::THIS),
                                                      0);
  vector<IExpression*> callArguments;
  callArguments.push_back(new IdentifierReference("mNativeThread"));
  callArguments.push_back(new FakeExpressionWithCleanup(nullValue, threadAtrributesType));
  callArguments.push_back(new FakeExpressionWithCleanup(runBridgeFunction, runBridgeFunctionType));
  callArguments.push_back(castExpression);
  Function* createFunction = ThreadInfrastructure::getThreadCreateFunction(context);
  NativeFunctionCall* createFunctionCall = new NativeFunctionCall(createFunction, callArguments);
  statements.push_back(createFunctionCall);
  
  wisey::PointerType* intPointerType = new wisey::PointerType(PrimitiveTypes::INT_TYPE);
  NativeTypeSpecifier* intPointerTypeSpecifier = new NativeTypeSpecifier(intPointerType);
  VariableDeclaration* resultDeclaration =
  VariableDeclaration::create(intPointerTypeSpecifier, new Identifier("result"), 0);
  
  NativeType* int8PointerPointerType =
  new NativeType(Type::getInt8Ty(llvmContext)->getPointerTo()->getPointerTo());
  NativeTypeSpecifier* int8PointerPointerTypeSpecifier =
  new NativeTypeSpecifier(int8PointerPointerType);
  CastExpression* castToI8Pointer = new CastExpression(int8PointerPointerTypeSpecifier,
                                                       new IdentifierReference("result"),
                                                       3);

  callArguments.clear();
  callArguments.push_back(new Identifier("mNativeThread"));
  callArguments.push_back(castToI8Pointer);
  Function* joinFunction = ThreadInfrastructure::getThreadJoinFunction(context);
  NativeFunctionCall* joinFunctionCall = new NativeFunctionCall(joinFunction, callArguments);
  statements.push_back(resultDeclaration);
  statements.push_back(joinFunctionCall);

  DereferenceExpression* loadResult = new DereferenceExpression(new Identifier("result"));
  
  ReturnStatement* returnStatement = new ReturnStatement(loadResult, 0);
  statements.push_back(returnStatement);
  
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  
  return new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                               PrimitiveTypes::INT_TYPE->newTypeSpecifier(),
                               "start",
                               arguments,
                               exceptions,
                               compoundStatement,
                               0);
}

MethodDeclaration* ThreadDefinition::createSendMethodDeclaration(IRGenerationContext& context,
                                                                 const Thread* thread) {
  LLVMContext& llvmContext = context.getLLVMContext();
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  StatementList& statements = block->getStatements();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  
  const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  VariableDeclaration* messageArgument =
  VariableDeclaration::create(intTypeSpecifier, new Identifier("message"), 0);
  arguments.push_back(messageArgument);

  Assignment* assignment = new Assignment(new Identifier("mMessage"), new Identifier("message"), 0);
  statements.push_back(new ExpressionStatement(assignment));

  NativeType* voidPointerType = new NativeType(Type::getInt8Ty(llvmContext)->getPointerTo());
  NativeTypeSpecifier* i8PointerTypeSpecifier = new NativeTypeSpecifier(voidPointerType);
  CastExpression* castToi8Pointer = new CastExpression(i8PointerTypeSpecifier,
                                                       new IdentifierReference("mMessage"),
                                                       0);
  
  vector<IExpression*> callArguments;
  callArguments.push_back(castToi8Pointer);
  Function* createFunction = ThreadInfrastructure::getThreadExitFunction(context);
  NativeFunctionCall* createFunctionCall = new NativeFunctionCall(createFunction, callArguments);
  statements.push_back(createFunctionCall);

  return new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                               PrimitiveTypes::VOID_TYPE->newTypeSpecifier(),
                               "send",
                               arguments,
                               exceptions,
                               compoundStatement,
                               0);
}

StateFieldDeclaration* ThreadDefinition::createNativeThreadHandleField(IRGenerationContext&
                                                                       context) {
  InjectionArgumentList arguments;
  
  NativeType* nativeType = ThreadInfrastructure::createNativeThreadType(context);
  NativeTypeSpecifier* nativeTypeSpecifier = new NativeTypeSpecifier(nativeType);
  return new StateFieldDeclaration(nativeTypeSpecifier, "mNativeThread");
}

StateFieldDeclaration* ThreadDefinition::createNativeThreadResultField(IRGenerationContext&
                                                                       context) {
  const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  return new StateFieldDeclaration(intTypeSpecifier, "mMessage");
}

vector<IObjectElementDefinition*>
ThreadDefinition::addThreadObjectElements(IRGenerationContext& context,
                                          vector<IObjectElementDefinition*> objectElements,
                                          const Thread* thread) {
  vector<IObjectElementDefinition*> result;
  vector<IObjectElementDefinition*>::const_iterator iterator = objectElements.begin();
  for (; iterator != objectElements.end(); iterator++) {
    IObjectElementDefinition* element = *iterator;
    if (!element->isConstant()) {
      break;
    }
    result.push_back(element);
  }
  result.push_back(createNativeThreadHandleField(context));
  result.push_back(createNativeThreadResultField(context));
  for (; iterator != objectElements.end(); iterator++) {
    IObjectElementDefinition* element = *iterator;
    if (!element->isField()) {
      break;
    }
    result.push_back(element);
  }
  result.push_back(createStartMethodDeclaration(context, thread));
  result.push_back(createSendMethodDeclaration(context, thread));

  for (; iterator != objectElements.end(); iterator++) {
    result.push_back(*iterator);
  }

  return result;
}
