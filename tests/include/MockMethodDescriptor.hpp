//
//  MockMethodDescriptor.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockMethodDescriptor_h
#define MockMethodDescriptor_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

#include "IMethodDescriptor.hpp"
#include "IObjectType.hpp"

/**
 * Defines a mock object for IObjectType
 */
class MockMethodDescriptor : public wisey::IMethodDescriptor {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(isPublic, bool ());
  MOCK_CONST_METHOD0(getReturnType, const wisey::IType* ());
  MOCK_CONST_METHOD0(getArguments, std::vector<const wisey::Argument*> ());
  MOCK_CONST_METHOD0(getThrownExceptions, std::vector<const wisey::Model*> ());
  MOCK_CONST_METHOD0(isStatic, bool ());
  MOCK_CONST_METHOD0(isOverride, bool ());
  MOCK_CONST_METHOD0(getMethodQualifiers, wisey::MethodQualifiers* ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::FunctionType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD0(getParentObject, const wisey::IObjectType* ());
  MOCK_CONST_METHOD0(getTypeName, std::string ());
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
  MOCK_CONST_METHOD0(isNative, bool ());
  MOCK_CONST_METHOD0(isPointer, bool ());
  MOCK_CONST_METHOD0(isImmutable, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
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
};

#endif /* MockMethodDescriptor_h */
