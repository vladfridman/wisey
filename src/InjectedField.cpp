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
#include "wisey/Names.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/ThreadExpression.hpp"

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
  if (!mInjectedType->isController() && !mInjectedType->isInterface() &&
      !mInjectedType->isArray()) {
    context.reportError(mLine, "Only controllers, interfaces bound to controllers and arrays "
                        "may be injected in fields");
    throw 1;
  }
  if (mInjectedType->isOwner()) {
    if (mInjectedType->isInterface()) {
      const Interface* interface = ((const InterfaceOwner*) mInjectedType)->getReference();
      checkInterfaceType(context, interface);
    } else if (mInjectedType->isController()) {
      const Controller* controller = ((const ControllerOwner*) mInjectedType)->getReference();
      checkControllerType(context, controller);
    }
  } else if (mInjectedType->isReference()) {
    if (mInjectedType->isInterface()) {
      const Interface* interface = (const Interface*) mInjectedType;
      checkInterfaceType(context, interface);
    } else if (mInjectedType->isController()) {
      const Controller* controller = (const Controller*) mInjectedType;
      checkControllerType(context, controller);
    }
  }
}

void InjectedField::checkInterfaceType(IRGenerationContext& context,
                                       const Interface* interface) const {
  if (mInjectionArgumentList.size() && !context.hasBoundController(interface)) {
    context.reportError(mLine, "Arguments are not allowed for injection of interfaces "
                        "that are not bound to controllers");
    throw 1;
  }
  if (context.hasBoundController(interface)) {
    checkControllerType(context, context.getBoundController(interface, mLine));
  }
}

void InjectedField::checkControllerType(IRGenerationContext& context,
                                        const Controller* controller) const {
  if (mInjectedType->isOwner() && controller->isContextInjected()) {
    context.reportError(mLine, "Controller " + controller->getTypeName() + " is scoped"
                        " and should have reference field type");
    throw 1;
  } else if (mInjectedType->isReference() && !controller->isContextInjected()) {
    context.reportError(mLine, "Injected fields must have owner type denoted by '*'"
                        " if the injected type is not scoped");
    throw 1;
  }
}

void InjectedField::checkInjectionArguments(IRGenerationContext& context) const {
  if (mInjectedType->isArray()) {
    return;
  }
  
  const IObjectType* objectType = mInjectedType->isOwner()
  ? (const IObjectType*) ((const IObjectOwnerType*) mInjectedType)->getReference()
  : (const IObjectType*) mInjectedType;
  
  const Controller* controller = objectType->isInterface()
  ? context.getBoundController((const Interface*) objectType, mLine)
  : (const Controller*) objectType;
  
  controller->checkInjectionArguments(context, mInjectionArgumentList, mLine);
}

Value* InjectedField::callInjectFunction(IRGenerationContext& context,
                                         const Controller* controller,
                                         Value* fieldPointer,
                                         int line) const {
  Function* function = context.getModule()->getFunction(getInjectionFunctionName(controller));
  assert(function && "Inject function for injected field is not declared");
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context, line);
  IVariable* callstackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callstackObject = callstackVariable->generateIdentifierIR(context, line);
  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(callstackObject);
  arguments.push_back(fieldPointer);

  return IRWriter::createCallInst(context, function, arguments, "");
}

Function* InjectedField::declareInjectionFunction(IRGenerationContext& context,
                                                  const Controller* controller) const {
  vector<Type*> argumentTypes;
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), mLine);
  argumentTypes.push_back(threadInterface->getLLVMType(context));
  Controller* callstackController =
    context.getController(Names::getCallStackControllerFullName(), mLine);
  argumentTypes.push_back(callstackController->getLLVMType(context));
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
  llvm::Argument* thread = &*llvmArguments;
  thread->setName(ThreadExpression::THREAD);
  llvmArguments++;
  llvm::Argument* callstack = &*llvmArguments;
  callstack->setName(ThreadExpression::CALL_STACK);
  llvmArguments++;
  llvm::Argument* fieldPointer = &*llvmArguments;
  fieldPointer->setName("fieldPointer");
  
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  IVariable* threadVariable = new ParameterSystemReferenceVariable(ThreadExpression::THREAD,
                                                                   threadInterface,
                                                                   thread,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  Controller* callstackController =
    context.getController(Names::getCallStackControllerFullName(), 0);
  IVariable* callstackVariable = new ParameterSystemReferenceVariable(ThreadExpression::CALL_STACK,
                                                                      callstackController,
                                                                      callstack,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);

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
