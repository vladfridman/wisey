//
//  Method.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "Argument.hpp"
#include "CastExpression.hpp"
#include "Cleanup.hpp"
#include "Composer.hpp"
#include "CompoundStatement.hpp"
#include "EmptyStatement.hpp"
#include "IntrinsicFunctions.hpp"
#include "IMethodCall.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "Method.hpp"
#include "Model.hpp"
#include "Names.hpp"
#include "ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Method::Method(const IObjectType* objectType,
               string name,
               AccessLevel accessLevel,
               const IType* returnType,
               vector<const Argument*> arguments,
               vector<const Model*> thrownExceptions,
               MethodQualifiers* methodQualifiers,
               CompoundStatement* compoundStatement,
               int line) :
mObjectType(objectType),
mName(name),
mIsPublic(accessLevel == PUBLIC_ACCESS),
mReturnType(returnType),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mMethodQualifiers(methodQualifiers),
mCompoundStatement(compoundStatement),
mLine(line) { }

Method::~Method() {
  for (const Argument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  // mCompoundStatement is deleted with MethodDefinition
}

bool Method::isStatic() const {
  return false;
}

string Method::getName() const {
  return mName;
}

bool Method::isPublic() const {
  return mIsPublic;
}

const IType* Method::getReturnType() const {
  return mReturnType;
}

vector<const wisey::Argument*> Method::getArguments() const {
  return mArguments;
}

vector<const Model*> Method::getThrownExceptions() const {
  return mThrownExceptions;
}

string Method::getTypeName() const {
  return IMethodCall::translateObjectMethodToLLVMFunctionName(mObjectType, getName());
}

FunctionType* Method::getLLVMType(IRGenerationContext& context) const {
  return IMethodDescriptor::getLLVMFunctionType(context, this, mObjectType, mLine);
}

bool Method::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool Method::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* Method::castTo(IRGenerationContext& context,
                      Value* fromValue,
                      const IType* toType,
                      int line) const {
  return NULL;
}

bool Method::isPrimitive() const {
  return false;
}

bool Method::isOwner() const {
  return false;
}

bool Method::isReference() const {
  return false;
}

bool Method::isArray() const {
  return false;
}

bool Method::isFunction() const {
  return true;
}

bool Method::isPackage() const {
  return false;
}

bool Method::isController() const {
  return false;
}

bool Method::isInterface() const {
  return false;
}

bool Method::isModel() const {
  return false;
}

bool Method::isNode() const {
  return false;
}

bool Method::isNative() const {
  return false;
}

bool Method::isPointer() const {
  return false;
}

bool Method::isImmutable() const {
  return false;
}

Function* Method::declareFunction(IRGenerationContext& context) const {
  return IMethod::declareFunctionForObject(context, mObjectType, this);
}

void Method::generateIR(IRGenerationContext& context) const {
  IMethod::checkReturnAndArgumentTypes(context, this, mObjectType);
  string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(mObjectType, mName);
  Function* function = context.getModule()->getFunction(functionName);
  assert(function != NULL);
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Scopes& scopes = context.getScopes();

  scopes.pushScope();
  scopes.setReturnType(mReturnType);
  BasicBlock* declarationsBlock = BasicBlock::Create(llvmContext, "declarations", function, 0);
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  context.setBasicBlock(entryBlock);
  context.setDeclarationsBlock(declarationsBlock);

  context.setCurrentMethod(this);
  createArguments(context, function);
  Composer::pushCallStack(context, mLine);
  mCompoundStatement->generateIR(context);
  IMethod::maybeAddImpliedVoidReturn(context, this, mLine);
  IMethod::checkForUnhandledExceptions(context, this, mLine);
  
  scopes.popScope(context, mLine);

  context.setBasicBlock(declarationsBlock);
  IRWriter::createBranch(context, entryBlock);
}

void Method::createArguments(IRGenerationContext& context, Function* function) const {
  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument* thisArugment = &*llvmFunctionArguments;
  thisArugment->setName(IObjectType::THIS);
  llvmFunctionArguments++;
  llvm::Argument* threadArgument = &*llvmFunctionArguments;
  threadArgument->setName(ThreadExpression::THREAD);
  llvmFunctionArguments++;
  llvm::Argument* callstackArgument = &*llvmFunctionArguments;
  callstackArgument->setName(ThreadExpression::CALL_STACK);
  llvmFunctionArguments++;
  for (const Argument* methodArgument : mArguments) {
    llvm::Argument* llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(methodArgument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  IMethod::storeStaticArgumentValue(context,
                                    IObjectType::THIS,
                                    mObjectType,
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  IMethod::storeStaticArgumentValue(context,
                                    ThreadExpression::THREAD,
                                    context.getInterface(Names::getThreadInterfaceFullName(),
                                                         mLine),
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  IMethod::storeStaticArgumentValue(context,
                                    ThreadExpression::CALL_STACK,
                                    context.getController(Names::getCallStackControllerFullName(),
                                                          mLine),
                                    &*llvmFunctionArguments);
  llvmFunctionArguments++;
  for (const Argument* methodArgument : mArguments) {
    IMethod::storeArgumentValue(context,
                                methodArgument->getName(),
                                methodArgument->getType(),
                                &*llvmFunctionArguments,
                                mMethodQualifiers->getLine());
    llvmFunctionArguments++;
  }
}

int Method::getLine() const {
  return mLine;
}

bool Method::isConstant() const {
  return false;
}

bool Method::isField() const {
  return false;
}

bool Method::isMethod() const {
  return true;
}

bool Method::isStaticMethod() const {
  return false;
}

bool Method::isMethodSignature() const {
  return false;
}

bool Method::isLLVMFunction() const {
  return false;
}

bool Method::isOverride() const {
  return mMethodQualifiers->getMethodQualifierSet().count(MethodQualifier::OVERRIDE);
}

MethodQualifiers* Method::getMethodQualifiers() const {
  return mMethodQualifiers;
}

const IObjectType* Method::getParentObject() const {
  return mObjectType;
}

void Method::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}

void Method::createLocalVariable(IRGenerationContext& context,
                                 string name,
                                 int line) const {
  assert(false);
}

void Method::createFieldVariable(IRGenerationContext& context,
                                 string name,
                                 const IConcreteObjectType* object,
                                 int line) const {
  assert(false);
}

void Method::createParameterVariable(IRGenerationContext& context,
                                     string name,
                                     Value* value,
                                     int line) const {
  assert(false);
}

const wisey::ArrayType* Method::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* Method::inject(IRGenerationContext& context,
                            const InjectionArgumentList injectionArgumentList,
                            int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

