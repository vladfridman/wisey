//
//  MockType.hpp
//  Wisey
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
class MockType : public wisey::IType {
public:
  MOCK_CONST_METHOD0(getTypeName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::Type* (llvm::LLVMContext&));
  MOCK_CONST_METHOD0(getTypeKind, wisey::TypeKind ());
  MOCK_CONST_METHOD1(canCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD1(canAutoCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD4(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*,
                                           int));
};

#endif /* MockType_h */
