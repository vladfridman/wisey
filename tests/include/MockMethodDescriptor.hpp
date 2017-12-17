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

#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectType.hpp"

/**
 * Defines a mock object for IObjectType
 */
class MockMethodDescriptor : public wisey::IMethodDescriptor {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getAccessLevel, wisey::AccessLevel ());
  MOCK_CONST_METHOD0(getReturnType, const wisey::IType* ());
  MOCK_CONST_METHOD0(getArguments, std::vector<wisey::MethodArgument*> ());
  MOCK_CONST_METHOD0(getThrownExceptions, std::vector<const wisey::Model*> ());
  MOCK_CONST_METHOD0(isStatic, bool ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::FunctionType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD0(getObjectType, const wisey::IObjectType* ());
  MOCK_CONST_METHOD0(getTypeName, std::string ());
  MOCK_CONST_METHOD0(getTypeKind, wisey::TypeKind ());
  MOCK_CONST_METHOD1(canCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD1(canAutoCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD4(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*,
                                           int));
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
  MOCK_CONST_METHOD0(getArrayElementType, wisey::ArrayElementType* ());
};

#endif /* MockMethodDescriptor_h */
