//
//  MockType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockType_h
#define MockType_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"

/**
 * Defines a mock object for IType
 */
class MockType : public yazyk::IType {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::Type* (llvm::LLVMContext&));
  MOCK_CONST_METHOD0(getTypeKind, yazyk::TypeKind ());
  MOCK_CONST_METHOD1(canCastTo, bool (yazyk::IType*));
  MOCK_CONST_METHOD1(canAutoCastTo, bool (yazyk::IType*));
  MOCK_CONST_METHOD3(castTo, llvm::Value* (yazyk::IRGenerationContext&,
                                           llvm::Value*,
                                           yazyk::IType*));
};

#endif /* MockType_h */
