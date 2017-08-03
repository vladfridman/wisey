//
//  MockVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockVariable_h
#define MockVariable_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IVariable.hpp"

/**
 * Defines a mock object for IVariable
 */
class MockVariable : public wisey::IVariable {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getType, const wisey::IType* ());
  MOCK_CONST_METHOD0(getValue, llvm::Value* ());
  MOCK_CONST_METHOD2(generateIdentifierIR, llvm::Value* (wisey::IRGenerationContext&, std::string));
  MOCK_METHOD2(generateAssignmentIR, llvm::Value* (wisey::IRGenerationContext&,
                                                   wisey::IExpression*));
  MOCK_CONST_METHOD1(free, void (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD0(existsInOuterScope, bool ());
  MOCK_CONST_METHOD1(setToNull, void (wisey::IRGenerationContext&));
};

#endif /* MockVariable_h */
