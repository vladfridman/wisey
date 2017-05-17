//
//  MockStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockStatement_h
#define MockStatement_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

/**
 * Defines a mock object for IStatement
 */
class MockStatement : public yazyk::IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, llvm::Value* (yazyk::IRGenerationContext&));
};

#endif /* MockStatement_h */
