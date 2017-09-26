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
  MOCK_CONST_METHOD1(getVariable, wisey::IVariable* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(generateIR, llvm::Value* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, const wisey::IType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(releaseOwnership, void (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(existsInOuterScope, bool (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD2(addReferenceToOwner, void (wisey::IRGenerationContext&, wisey::IVariable*));
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream&));
};

#endif /* MockExpression_h */
