//
//  MockTypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockTypeSpecifier_h
#define MockTypeSpecifier_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ITypeSpecifier.hpp"

/**
 * Defines a mock object for ITypeSpecifier
 */
class MockTypeSpecifier : public yazyk::ITypeSpecifier {
public:
  MOCK_CONST_METHOD1(getType, yazyk::IType* (yazyk::IRGenerationContext&));
};

#endif /* MockTypeSpecifier_h */
