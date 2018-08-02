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
#include "wisey/LLVMFunction.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterReferenceVariableStatic.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

InjectedField::InjectedField(const IType* type,
                             const IType* injectedType,
                             std::string name,
                             InjectionArgumentList injectionArgumentList,
                             std::string sourceFileName,
                             bool isImmediate,
                             int line) :
mType(type),
mInjectedType(injectedType),
mIsArrayType(injectedType->isArray()),
mName(name),
mInjectionArgumentList(injectionArgumentList),
mSourceFileName(sourceFileName),
mIsImmediate(isImmediate),
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

InjectedField* InjectedField::createDelayed(const IType* type,
                                            const IType* injectedType,
                                            std::string name,
                                            InjectionArgumentList injectionArguments,
                                            std::string sourceFile,
                                            int line) {
  return new InjectedField(type, injectedType, name, injectionArguments, sourceFile, false, line);
}

InjectedField* InjectedField::createImmediate(const IType* type,
                                              const IType* injectedType,
                                              std::string name,
                                              InjectionArgumentList injectionArguments,
                                              std::string sourceFile,
                                              int line) {
  return new InjectedField(type, injectedType, name, injectionArguments, sourceFile, true, line);
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

void InjectedField::free(IRGenerationContext& context,
                         Value* fieldValue,
                         Value* exception,
                         int line) const {
  const IOwnerType* ownerType = (const IOwnerType*) mType;
  const LLVMFunction* destructor = getDestructorFunction(context);
  ownerType->free(context, fieldValue, exception, destructor, line);
}

const LLVMFunction* InjectedField::getDestructorFunction(IRGenerationContext& context) const {
  InjectionArgument* destructorArgument = NULL;
  if (!mInjectionArgumentList.size()) {
    return NULL;
  }
  if (!mInjectionArgumentList.front()->deriveFieldName().compare("mDestructor")) {
    destructorArgument = mInjectionArgumentList.front();
  } else if (!mInjectionArgumentList.back()->deriveFieldName().compare("mDestructor")) {
    destructorArgument = mInjectionArgumentList.back();
  }
  if (!destructorArgument) {
    return NULL;
  }
  const IType* destructorType = destructorArgument->getType(context);
  if (!destructorType->isFunction()) {
    return NULL;
  }
  return (const LLVMFunction*) destructorType;
}

void InjectedField::checkType(IRGenerationContext& context) const {
  if (!mInjectedType->isController() && !mInjectedType->isInterface() &&
      !mInjectedType->isArray() &&
      !mInjectedType->isPointer()) {
    context.reportError(mLine, "Only controllers, interfaces bound to controllers and arrays "
                        "may be injected in fields");
    throw 1;
  }
  
  if (mInjectedType->isPointer() && !mInjectedType->isOwner()) {
    context.reportError(mLine, "Injected pointer must be an owner reference");
    throw 1;
  }

  if (mInjectedType->isArray() || mInjectedType->isPointer()) {
    return;
  }
  
  const IObjectType* objectType = mInjectedType->isOwner()
  ? (const IObjectType*) ((const IObjectOwnerType*) mInjectedType)->getReference()
  : (const IObjectType*) mInjectedType;
  
  if (objectType->isInterface()) {
    checkInterfaceType(context, (const Interface*) objectType);
  } else if (objectType->isController()) {
    checkControllerType(context, (const Controller*) objectType);
  }
}

void InjectedField::checkInterfaceType(IRGenerationContext& context,
                                       const Interface* interface) const {
  if (mInjectionArgumentList.size() && !context.hasBoundController(interface)) {
    context.reportError(mLine, "Arguments are not allowed for injection of interfaces "
                        "that are not bound to controllers");
    throw 1;
  }
  if (mInjectedType->isReference() && !context.hasBoundController(interface)) {
    context.reportError(mLine, "Injecting unbound interface reference is not allowed, "
                        "either inject an owner or bind the interface");
    throw 1;
  }
  if (context.hasBoundController(interface)) {
    checkControllerType(context, context.getBoundController(interface, mLine));
  }
}

void InjectedField::checkControllerType(IRGenerationContext& context,
                                        const Controller* controller) const {
  if (mInjectedType->isOwner() && controller->isScopeInjected(context)) {
    context.reportError(mLine, "Controller " + controller->getTypeName() + " is scoped"
                        " and should have reference field type");
    throw 1;
  } else if (mInjectedType->isReference() && !controller->isScopeInjected(context)) {
    context.reportError(mLine, "Injected fields must have owner type denoted by '*'"
                        " if the injected type is not scoped");
    throw 1;
  }
}

void InjectedField::checkInjectionArguments(IRGenerationContext& context) const {
  if (mInjectedType->isArray() || mInjectedType->isPointer()) {
    return;
  }
  
  const IObjectType* objectType = mInjectedType->isOwner()
  ? (const IObjectType*) ((const IObjectOwnerType*) mInjectedType)->getReference()
  : (const IObjectType*) mInjectedType;
  
  if (objectType->isInterface() && context.hasBoundController((const Interface*) objectType)) {
    const Controller* controller = context.getBoundController((const Interface*) objectType, mLine);
    controller->checkInjectionArguments(context, mInjectionArgumentList, mLine);
  } else if (objectType->isController()) {
    const Controller* controller = (const Controller*) objectType;
    controller->checkInjectionArguments(context, mInjectionArgumentList, mLine);
  }
}

bool InjectedField::isImmediate() const {
  return mIsImmediate;
}

Value* InjectedField::getValue(IRGenerationContext& context,
                               const IConcreteObjectType* controller,
                               Value* fieldPointer,
                               int line) const {
  return mIsImmediate
  ? IRWriter::newLoadInst(context, fieldPointer, mName)
  : callInjectFunction(context, controller, fieldPointer, line);
}

Value* InjectedField::callInjectFunction(IRGenerationContext& context,
                                         const IConcreteObjectType* controller,
                                         Value* fieldPointer,
                                         int line) const {
  Function* function = context.getModule()->getFunction(getInjectionFunctionName(controller));
  assert(function && "Inject function for injected field is not declared");
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context, line);
  IVariable* callstackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callstackObject = callstackVariable->generateIdentifierIR(context, line);
  vector<Value*> arguments;
  arguments.push_back(context.getThis()->generateIdentifierIR(context, line));
  arguments.push_back(threadObject);
  arguments.push_back(callstackObject);
  arguments.push_back(fieldPointer);

  return IRWriter::createCallInst(context, function, arguments, "");
}

Function* InjectedField::declareInjectionFunction(IRGenerationContext& context,
                                                  const Controller* controller) const {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(controller->getLLVMType(context));
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
  const Controller* controller = (const Controller*) object1;
  context.setObjectType(controller);
  context.getScopes().pushScope();

  IConcreteObjectType::declareFieldVariables(context, controller);

  const InjectedField* injectedField = (const InjectedField*) object2;

  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* thisObject = &*llvmArguments;
  thisObject->setName(IObjectType::THIS);
  llvmArguments++;
  llvm::Argument* thread = &*llvmArguments;
  thread->setName(ThreadExpression::THREAD);
  llvmArguments++;
  llvm::Argument* callstack = &*llvmArguments;
  callstack->setName(ThreadExpression::CALL_STACK);
  llvmArguments++;
  llvm::Argument* fieldPointer = &*llvmArguments;
  fieldPointer->setName("fieldPointer");
  
  IVariable* thisVariable = new ParameterReferenceVariableStatic(IObjectType::THIS,
                                                                 controller,
                                                                 thisObject,
                                                                 0);
  context.getScopes().setVariable(context, thisVariable);

  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  IVariable* threadVariable = new ParameterReferenceVariableStatic(ThreadExpression::THREAD,
                                                                   threadInterface,
                                                                   thread,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  Controller* callstackController =
    context.getController(Names::getCallStackControllerFullName(), 0);
  IVariable* callstackVariable = new ParameterReferenceVariableStatic(ThreadExpression::CALL_STACK,
                                                                      callstackController,
                                                                      callstack,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);

  BasicBlock* declarations = BasicBlock::Create(llvmContext, "declarations", function);
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.not.null", function);
  
  context.setBasicBlock(entryBlock);
  context.setDeclarationsBlock(declarations);
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
  if (injectedField->getType()->isReference()) {
    ((const IReferenceType*) injectedField->getType())->
    incrementReferenceCount(context, injectedValue);
  }
  IRWriter::createReturnInst(context, injectedValue);

  context.getScopes().popScope(context, 0);

  context.setBasicBlock(declarations);
  IRWriter::createBranch(context, entryBlock);
}

string InjectedField::getInjectionFunctionName(const IConcreteObjectType* controller) const {
  return controller->getTypeName() + "." + mName + ".inject";
}

bool InjectedField::isAssignable(const IConcreteObjectType* object) const {
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

string InjectedField::getFieldKind() const {
  return "injected";
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
  stream << "  inject ";
  if (mIsImmediate) {
    stream << "immediate ";
  }
  stream << mInjectedType->getTypeName() << " " << getName();
  
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
