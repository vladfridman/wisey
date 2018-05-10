//
//  InjectedField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Controller.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

InjectedField::InjectedField(const IType* type,
                             const IType* injectedType,
                             std::string name,
                             InjectionArgumentList injectionArgumentList,
                             std::string sourceFileName,
                             int line) :
mType(type),
mInjectedType(injectedType),
mIsArrayType(injectedType->isArray()),
mName(name),
mInjectionArgumentList(injectionArgumentList),
mSourceFileName(sourceFileName),
mLine(line) {
  assert(injectedType);
}

InjectedField::~InjectedField() {
  // Injection arguments are deleted with field delcarations
  mInjectionArgumentList.clear();
  if (mIsArrayType) {
    delete ((const ArraySpecificOwnerType*) mInjectedType)->getArraySpecificType();
  }
}

int InjectedField::getLine() const {
  return mLine;
}

const IType* InjectedField::getType() const {
  return mType;
}

string InjectedField::getName() const {
  return mName;
}

Value* InjectedField::inject(IRGenerationContext& context) const {
  return mInjectedType->inject(context, mInjectionArgumentList, mLine);
}

void InjectedField::checkType(IRGenerationContext& context) const {
  if (!mInjectedType->isOwner() || !mType->isOwner()) {
    context.reportError(mLine, "Injected fields must have owner type denoted by '*'");
    throw 1;
  }
  if (!mInjectionArgumentList.size()) {
    return;
  }
  if (mInjectedType->isInterface() &&
      !context.hasBoundController(((const InterfaceOwner*) mInjectedType)->getReference())) {
    context.reportError(mLine, "Arguments are not allowed for injection of interfaces "
                        "that are not bound to controllers");
    throw 1;
  }
  
  const Controller* controller = mInjectedType->isInterface()
  ? context.getBoundController(((const InterfaceOwner*) mInjectedType)->getReference(), mLine)
  : ((const ControllerOwner*) mInjectedType)->getReference();
  
  controller->checkInjectionArguments(context, mInjectionArgumentList, mLine);
}

Value* InjectedField::callInjectFunction(IRGenerationContext& context,
                                         const Controller* controller,
                                         Value* fieldPointer) const {
  Function* function = context.getModule()->getFunction(getInjectionFunctionName(controller));
  assert(function && "Inject function for injected field is not declared");
  
  vector<Value*> arguments;
  arguments.push_back(fieldPointer);
  
  return IRWriter::createCallInst(context, function, arguments, "");
}

Function* InjectedField::declareInjectionFunction(IRGenerationContext& context,
                                                  const Controller* controller) const {
  vector<Type*> argumentTypes;
  Type* fieldLLVMType = getType()->getLLVMType(context);
  argumentTypes.push_back(fieldLLVMType->getPointerTo());
  FunctionType* functionType = FunctionType::get(fieldLLVMType, argumentTypes, false);
  return Function::Create(functionType,
                          GlobalValue::ExternalLinkage,
                          getInjectionFunctionName(controller),
                          context.getModule());
}

void InjectedField::defineInjectionFunction(IRGenerationContext& context,
                                            const Controller* controller) const {
  Function* function = declareInjectionFunction(context, controller);
  context.addComposingCallback2Objects(composeInjectFunctionBody, function, controller, this);
}

void InjectedField::composeInjectFunctionBody(IRGenerationContext& context,
                                              Function* function,
                                              const void* object1,
                                              const void* object2) {
  LLVMContext& llvmContext = context.getLLVMContext();
  context.setObjectType((const IObjectType*) object1);
  context.getScopes().pushScope();
  const InjectedField* injectedField = (const InjectedField*) object2;

  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* fieldPointer = &*llvmArguments;
  fieldPointer->setName("fieldPointer");
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.not.null", function);
  
  context.setBasicBlock(entryBlock);
  Value* fieldValue = IRWriter::newLoadInst(context, fieldPointer, "");
  Value* null = ConstantPointerNull::get((PointerType*) fieldValue->getType());
  Value* condition =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, fieldValue, null, "isNull");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNotNullBlock);
  IRWriter::createReturnInst(context, fieldValue);
  
  context.setBasicBlock(ifNullBlock);
  Value* injectedValue = injectedField->inject(context);
  IRWriter::newStoreInst(context, injectedValue, fieldPointer);
  IRWriter::createReturnInst(context, injectedValue);

  context.getScopes().popScope(context, 0);
}

string InjectedField::getInjectionFunctionName(const Controller* controller) const {
  return controller->getTypeName() + "." + mName + ".inject";
}

bool InjectedField::isAssignable() const {
  return true;
}

bool InjectedField::isConstant() const {
  return false;
}

bool InjectedField::isField() const {
  return true;
}

bool InjectedField::isMethod() const {
  return false;
}

bool InjectedField::isStaticMethod() const {
  return false;
}

bool InjectedField::isMethodSignature() const {
  return false;
}

bool InjectedField::isLLVMFunction() const {
  return false;
}

bool InjectedField::isFixed() const {
  return false;
}

bool InjectedField::isInjected() const {
  return true;
}

bool InjectedField::isReceived() const {
  return false;
}

bool InjectedField::isState() const {
  return false;
}

void InjectedField::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "  inject " << mInjectedType->getTypeName() << " " << getName();
  
  if (!mInjectionArgumentList.size()) {
    stream << ";" << endl;
    return;
  }
  
  for (InjectionArgument* argument : mInjectionArgumentList) {
    stream << ".";
    argument->printToStream(context, stream);
  }
  stream << ";" << endl;
}
