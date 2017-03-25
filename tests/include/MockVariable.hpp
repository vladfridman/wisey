//
//  MockVariable.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockVariable_h
#define MockVariable_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IVariable.hpp"

/**
 * Defines a mock object for IVariable
 */
class MockVariable : public yazyk::IVariable {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getType, yazyk::IType* ());
  MOCK_CONST_METHOD0(getValue, llvm::Value* ());
  MOCK_CONST_METHOD2(generateIdentifierIR, llvm::Value* (yazyk::IRGenerationContext&, std::string));
  MOCK_METHOD2(generateAssignmentIR, llvm::Value* (yazyk::IRGenerationContext&,
                                                   yazyk::IExpression&));
  MOCK_CONST_METHOD1(free, void (yazyk::IRGenerationContext&));
};

#endif /* MockVariable_h */
