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
  MOCK_CONST_METHOD0(isField, bool ());
  MOCK_CONST_METHOD0(isSystem, bool ());
  MOCK_CONST_METHOD0(getValue, llvm::Value* ());
  MOCK_CONST_METHOD2(generateIdentifierIR, llvm::Value* (wisey::IRGenerationContext&, int));
  MOCK_CONST_METHOD2(generateIdentifierReferenceIR, llvm::Value* (wisey::IRGenerationContext&,
                                                                  int));
  MOCK_METHOD4(generateAssignmentIR, llvm::Value* (wisey::IRGenerationContext&,
                                                   wisey::IExpression*,
                                                   std::vector<const wisey::IExpression*>
                                                   arrayIndices,
                                                   int));
  MOCK_CONST_METHOD2(free, void (wisey::IRGenerationContext&, int));
  MOCK_METHOD2(setToNull, void (wisey::IRGenerationContext&, int));
};

#endif /* MockOwnerVariable_h */
