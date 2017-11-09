//
//  MockReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockReferenceVariable_h
#define MockReferenceVariable_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"

/**
 * Defines a mock object for IReferenceVariable
 */
class MockReferenceVariable : public wisey::IReferenceVariable {
public:
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getType, const wisey::IType* ());
  MOCK_CONST_METHOD0(getValue, llvm::Value* ());
  MOCK_CONST_METHOD1(generateIdentifierIR, llvm::Value* (wisey::IRGenerationContext&));
  MOCK_METHOD2(generateAssignmentIR, llvm::Value* (wisey::IRGenerationContext&,
                                                   wisey::IExpression*));
  MOCK_CONST_METHOD0(existsInOuterScope, bool ());
  MOCK_CONST_METHOD1(decrementReferenceCounter, void (wisey::IRGenerationContext&));
};


#endif /* MockReferenceVariable_h */
