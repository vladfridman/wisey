//
//  MockExpression.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockExpression_h
#define MockExpression_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

/**
 * Defines a mock object for IExpression
 */
class MockExpression : public yazyk::IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, llvm::Value* (yazyk::IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, yazyk::IType* (yazyk::IRGenerationContext&));
  MOCK_CONST_METHOD1(releaseOwnership, void (yazyk::IRGenerationContext&));
};


#endif /* MockExpression_h */
