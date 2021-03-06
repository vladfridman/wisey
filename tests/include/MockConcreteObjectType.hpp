//
//  MockConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/31/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockConcreteObjectType_h
#define MockConcreteObjectType_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

#include "IConcreteObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "InjectionArgument.hpp"

/**
 * Defines a mock object for IExpression
 */
class MockConcreteObjectType : public wisey::IConcreteObjectType {
public:
  MOCK_CONST_METHOD1(findField, wisey::IField* (std::string));
  MOCK_CONST_METHOD3(findConstant, wisey::Constant* (wisey::IRGenerationContext&,
                                                     std::string,
                                                     int));
  MOCK_CONST_METHOD1(getFieldIndex, unsigned long (const wisey::IField*));
  MOCK_CONST_METHOD0(getFields, std::vector<wisey::IField*> ());
  MOCK_CONST_METHOD0(getVTableName, std::string ());
  MOCK_CONST_METHOD0(getInterfaces, std::vector<wisey::Interface*> ());
  MOCK_CONST_METHOD0(getFlattenedInterfaceHierarchy, std::vector<wisey::Interface*> ());
  MOCK_CONST_METHOD0(getTypeTableName, std::string ());
  MOCK_CONST_METHOD1(findMethod, const wisey::IMethod* (std::string));
  MOCK_CONST_METHOD0(getMethods, std::vector<wisey::IMethod*> ());
  MOCK_CONST_METHOD0(getNameToMethodMap, std::map<std::string, wisey::IMethod*> ());
  MOCK_CONST_METHOD0(getConstants, std::vector<wisey::Constant*> ());
  MOCK_CONST_METHOD0(getLLVMFunctions, std::vector<wisey::LLVMFunction*> ());
  MOCK_CONST_METHOD1(findLLVMFunction, wisey::LLVMFunction* (std::string));
  MOCK_CONST_METHOD0(getObjectNameGlobalVariableName, std::string ());
  MOCK_CONST_METHOD0(getOwner, const wisey::IObjectOwnerType* ());
  MOCK_CONST_METHOD0(getTypeName, std::string ());
  MOCK_CONST_METHOD0(getShortName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::PointerType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD2(canCastTo, bool (wisey::IRGenerationContext&, const wisey::IType*));
  MOCK_CONST_METHOD2(canAutoCastTo, bool (wisey::IRGenerationContext&, const wisey::IType*));
  MOCK_CONST_METHOD4(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*,
                                           int));
  MOCK_CONST_METHOD0(isPrimitive, bool ());
  MOCK_CONST_METHOD0(isOwner, bool ());
  MOCK_CONST_METHOD0(isReference, bool ());
  MOCK_CONST_METHOD0(isArray, bool ());
  MOCK_CONST_METHOD0(isFunction, bool ());
  MOCK_CONST_METHOD0(isPackage, bool ());
  MOCK_CONST_METHOD0(isController, bool ());
  MOCK_CONST_METHOD0(isInterface, bool ());
  MOCK_CONST_METHOD0(isModel, bool ());
  MOCK_CONST_METHOD0(isNode, bool ());
  MOCK_CONST_METHOD0(isExternal, bool ());
  MOCK_CONST_METHOD0(isNative, bool ());
  MOCK_CONST_METHOD0(isPointer, bool ());
  MOCK_CONST_METHOD0(isImmutable, bool ());
  MOCK_CONST_METHOD1(isScopeInjected, bool (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
  MOCK_METHOD3(setFields, void (wisey::IRGenerationContext&,
                                std::vector<wisey::IField*>,
                                unsigned long));
  MOCK_METHOD1(setInterfaces, void (std::vector<wisey::Interface*>));
  MOCK_METHOD1(setMethods, void (std::vector<wisey::IMethod*>));
  MOCK_METHOD1(setStructBodyTypes, void (std::vector<llvm::Type*>));
  MOCK_METHOD1(setConstants, void (std::vector<wisey::Constant*>));
  MOCK_METHOD1(setLLVMFunctions, void (std::vector<wisey::LLVMFunction*>));
  MOCK_METHOD1(setScopeType, void (const wisey::IObjectType*));
  MOCK_CONST_METHOD0(getImportProfile, wisey::ImportProfile* ());
  MOCK_CONST_METHOD2(incrementReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(decrementReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(getReferenceCount, llvm::Value* (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_METHOD1(addInnerObject, void (const wisey::IObjectType*));
  MOCK_CONST_METHOD1(getInnerObject, const wisey::IObjectType* (std::string));
  MOCK_CONST_METHOD0(isPublic, bool ());
  MOCK_CONST_METHOD0(getInnerObjects, std::map<std::string, const IObjectType*> ());
  MOCK_METHOD0(markAsInner, void ());
  MOCK_CONST_METHOD0(isInner, bool ());
  MOCK_CONST_METHOD0(getLine, int ());
  MOCK_CONST_METHOD3(createLocalVariable, void (wisey::IRGenerationContext&,
                                                std::string name,
                                                int));
  MOCK_CONST_METHOD4(createFieldVariable, void (wisey::IRGenerationContext&,
                                                std::string name,
                                                const wisey::IConcreteObjectType* object,
                                                int));
  MOCK_CONST_METHOD4(createParameterVariable, void (wisey::IRGenerationContext&,
                                                    std::string name,
                                                    llvm::Value* value,
                                                    int));
  MOCK_CONST_METHOD2(getArrayType, wisey::ArrayType* (wisey::IRGenerationContext&, int));
  MOCK_CONST_METHOD3(inject, llvm::Instruction* (wisey::IRGenerationContext&,
                                                 const wisey::InjectionArgumentList,
                                                 int));
  MOCK_CONST_METHOD1(getObjectTypeNameGlobal,
                     llvm::Constant* (wisey::IRGenerationContext& context));
};

#endif /* MockConcreteObjectType_h */
