//
//  MockExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockExpression_h
#define MockExpression_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

/**
 * Defines a mock object for IExpression
 */
class MockExpression : public wisey::IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, llvm::Value* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, wisey::IType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(releaseOwnership, void (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(existsInOuterScope, bool (wisey::IRGenerationContext&));
};

#endif /* MockExpression_h */
