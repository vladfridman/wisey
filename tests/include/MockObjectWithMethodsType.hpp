//
//  MockObjectWithMethodsType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockObjectWithMethodsType_h
#define MockObjectWithMethodsType_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectWithMethodsType.hpp"

/**
 * Defines a mock object for IObjectWithMethodsType
 */
class MockObjectWithMethodsType : public yazyk::IObjectWithMethodsType {
public:
  MOCK_CONST_METHOD1(findMethod, yazyk::IMethodDescriptor* (std::string));
  MOCK_CONST_METHOD0(getObjectNameGlobalVariableName, std::string ());
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::Type* (llvm::LLVMContext&));
  MOCK_CONST_METHOD0(getTypeKind, yazyk::TypeKind ());
  MOCK_CONST_METHOD1(canCastTo, bool (yazyk::IType*));
  MOCK_CONST_METHOD1(canAutoCastTo, bool (yazyk::IType*));
  MOCK_CONST_METHOD3(castTo, llvm::Value* (yazyk::IRGenerationContext&,
                                           llvm::Value*,
                                           yazyk::IType*));
};

#endif /* MockObjectWithMethodsType_h */
