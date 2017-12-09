//
//  MockGlobalStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockGlobalStatement_h
#define MockGlobalStatement_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IGlobalStatement.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"

/**
 * Defines a mock object for IGlobalStatement
 */
class MockGlobalStatement : public wisey::IGlobalStatement {
public:
  MOCK_CONST_METHOD1(prototypeObject, wisey::IObjectType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(prototypeMethods, void (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(generateIR, llvm::Value* (wisey::IRGenerationContext&));
};

#endif /* MockGlobalStatement_h */
