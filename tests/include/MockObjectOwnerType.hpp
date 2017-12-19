//
//  MockObjectOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockObjectOwnerType_h
#define MockObjectOwnerType_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectOwnerType.hpp"

/**
 * Defines a mock object for IOwnerObjectType
 */
class MockObjectOwnerType : public wisey::IObjectOwnerType {
public:
  MOCK_CONST_METHOD0(getObject, wisey::IObjectType* ());
  MOCK_CONST_METHOD1(findMethod, wisey::IMethodDescriptor* (std::string));
  MOCK_CONST_METHOD0(getTypeName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::PointerType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD2(free, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD0(getTypeKind, wisey::TypeKind ());
  MOCK_CONST_METHOD1(canCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD1(canAutoCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD4(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*,
                                           int));
  MOCK_CONST_METHOD1(getDestructorFunction, llvm::Function* (wisey::IRGenerationContext&));
};


#endif /* MockObjectOwnerType_h */
