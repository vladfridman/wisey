//
//  MockExpressionAssignable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef MockExpressionAssignable_h
#define MockExpressionAssignable_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IExpressionAssignable.hpp"
#include "wisey/IRGenerationContext.hpp"

/**
 * Defines a mock object for IExpressionAssignable
 */
class MockExpressionAssignable : public wisey::IExpressionAssignable {
public:
  MOCK_CONST_METHOD0(getLine, int ());
  MOCK_CONST_METHOD2(generateIR, llvm::Value* (wisey::IRGenerationContext&,
                                               const wisey::IType* assignToType));
  MOCK_CONST_METHOD2(getVariable, wisey::IVariable* (wisey::IRGenerationContext&,
                                                     std::vector<const wisey::IExpression*>&));
  MOCK_CONST_METHOD1(getType, const wisey::IType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD0(isConstant, bool ());
  MOCK_CONST_METHOD0(isAssignable, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream&));
};

#endif /* MockExpressionAssignable_h */
