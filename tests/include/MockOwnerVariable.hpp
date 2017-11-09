//
//  MockOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockOwnerVariable_h
#define MockOwnerVariable_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"

/**
 * Defines a mock object for IOwnerVariable
 */
class MockOwnerVariable : public wisey::IOwnerVariable {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getType, const wisey::IType* ());
  MOCK_CONST_METHOD0(getValue, llvm::Value* ());
  MOCK_CONST_METHOD1(generateIdentifierIR, llvm::Value* (wisey::IRGenerationContext&));
  MOCK_METHOD2(generateAssignmentIR, llvm::Value* (wisey::IRGenerationContext&,
                                                   wisey::IExpression*));
  MOCK_CONST_METHOD0(existsInOuterScope, bool ());
  MOCK_CONST_METHOD1(free, void (wisey::IRGenerationContext&));
  MOCK_METHOD1(setToNull, void (wisey::IRGenerationContext&));
};

#endif /* MockOwnerVariable_h */
