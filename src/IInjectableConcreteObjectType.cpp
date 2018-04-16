//
//  IInjectableConcreteObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/IInjectableConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/Log.hpp"
#include "wisey/ThreadOwner.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

Instruction* IInjectableConcreteObjectType::
injectObject(IRGenerationContext& context,
             const IInjectableConcreteObjectType* object,
             const InjectionArgumentList injectionArgumentList,
             int line) {
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(object);
  
  checkArguments(object, injectionArgumentList);
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context, object, "injectvar");
  initializeReceivedFields(context, object, injectionArgumentList, malloc, line);
  initializeInjectedFields(context, object, malloc);
  initializeVTable(context, object, malloc);
  
  context.setObjectType(lastObjectType);
  
  return malloc;
}

void IInjectableConcreteObjectType::checkArguments(const IInjectableConcreteObjectType* object,
                                                   const InjectionArgumentList& received) {
  checkArgumentsAreWellFormed(object, received);
  checkAllFieldsAreSet(object, received);
}

void IInjectableConcreteObjectType::
checkArgumentsAreWellFormed(const IInjectableConcreteObjectType* object,
                            const InjectionArgumentList& injectionArgumentList) {
  bool areArgumentsWellFormed = true;
  
  for (InjectionArgument* argument : injectionArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(object);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e_deprecated("Some injection arguments for injected object " + object->getTypeName() +
                      " are not well formed");
    exit(1);
  }
}

void IInjectableConcreteObjectType::
checkAllFieldsAreSet(const IInjectableConcreteObjectType* object,
                     const InjectionArgumentList& injectionArgumentList) {
  set<string> allFieldsThatAreSet;
  for (InjectionArgument* argument : injectionArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = object->getMissingReceivedFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e_deprecated("Received field " + missingField + " is not initialized");
  }
  Log::e_deprecated("Some received fields of the controller " + object->getTypeName() +
                    " are not initialized.");
  exit(1);
}

void IInjectableConcreteObjectType::
initializeReceivedFields(IRGenerationContext& context,
                         const IInjectableConcreteObjectType* object,
                         const InjectionArgumentList& controllerInjectorArguments,
                         Instruction* malloc,
                         int line) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (InjectionArgument* argument : controllerInjectorArguments) {
    string argumentName = argument->deriveFieldName();
    IField* field = object->findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    const IType* fieldType = field->getType();
    
    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      Log::e_deprecated("Injector argumet value for field '" + field->getName() +
                        "' does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    IRWriter::newStoreInst(context, castValue, fieldPointer);
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, castValue);
    }
  }
}

void IInjectableConcreteObjectType::
initializeInjectedFields(IRGenerationContext& context,
                         const IInjectableConcreteObjectType* object,
                         Instruction* malloc) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value *index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (InjectedField* field : object->getInjectedFields()) {
    const IType* fieldType = field->getType();
    Value* fieldValue = NULL;
    if (fieldType->isReference()) {
      Log::e_deprecated("Injected fields must have owner type denoted by '*'");
      exit(1);
    } else if (fieldType->isArray()) {
      const ArraySpecificOwnerType* arraySpecificOwnerType =
      (const ArraySpecificOwnerType*) field->getInjectedType();
      const ArraySpecificType* arraySpecificType = arraySpecificOwnerType->getArraySpecificType();
      Value* arrayPointer = ArrayAllocation::allocateArray(context, arraySpecificType);
      fieldValue = IRWriter::newBitCastInst(context, arrayPointer, arraySpecificType->
                                            getArrayType(context)->getLLVMType(context));
    } else if (fieldType->isController()) {
      const ControllerOwner* controllerOwner = (const ControllerOwner*) fieldType;
      fieldValue = controllerOwner->getReference()->inject(context,
                                                           field->getInjectionArguments(),
                                                           field->getLine());
    } else if (fieldType->isThread()) {
      const ThreadOwner* threadOwner = (const ThreadOwner*) fieldType;
      fieldValue = threadOwner->getReference()->inject(context,
                                                       field->getInjectionArguments(),
                                                       field->getLine());
    } else if (fieldType->isInterface()) {
      const InterfaceOwner* interfaceOwner = (const InterfaceOwner*) fieldType;
      fieldValue = interfaceOwner->getReference()->inject(context,
                                                          field->getInjectionArguments(),
                                                          field->getLine());
    } else {
      Log::e_deprecated("Attempt to inject a variable that is not of injectable type");
      exit(1);
    }
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    IRWriter::newStoreInst(context, fieldValue, fieldPointer);
  }
}
