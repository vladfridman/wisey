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

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IVariable.hpp"

/**
 * Defines a mock object for IVariable
 */
class MockVariable : public wisey::IVariable {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getType, const wisey::IType* ());
  MOCK_CONST_METHOD0(isField, bool ());
  MOCK_CONST_METHOD0(isSystem, bool ());
  MOCK_CONST_METHOD0(getValue, llvm::Value* ());
  MOCK_CONST_METHOD1(generateIdentifierIR, llvm::Value* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(generateIdentifierReferenceIR, llvm::Value* (wisey::IRGenerationContext&));
  MOCK_METHOD4(generateAssignmentIR, llvm::Value* (wisey::IRGenerationContext&,
                                                   wisey::IExpression*,
                                                   std::vector<const wisey::IExpression*>
                                                   arrayIndices,
                                                   int));
};

#endif /* MockVariable_h */
