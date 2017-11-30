//
//  MockObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockObjectType_h
#define MockObjectType_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectType.hpp"

/**
 * Defines a mock object for IObjectType
 */
class MockObjectType : public wisey::IObjectType {
public:
  MOCK_CONST_METHOD1(findMethod, wisey::IMethodDescriptor* (std::string));
  MOCK_CONST_METHOD1(findConstant, wisey::Constant* (std::string));
  MOCK_CONST_METHOD0(getObjectNameGlobalVariableName, std::string ());
  MOCK_CONST_METHOD0(getOwner, const wisey::IObjectOwnerType* ());
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getShortName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::PointerType* (llvm::LLVMContext&));
  MOCK_CONST_METHOD0(getTypeKind, wisey::TypeKind ());
  MOCK_CONST_METHOD1(canCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD1(canAutoCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD3(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*));
  MOCK_CONST_METHOD0(isExternal, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
  MOCK_CONST_METHOD2(incremenetReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(decremenetReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(getReferenceCount, llvm::Value* (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_METHOD1(setImportProfile, void (wisey::ImportProfile*));
  MOCK_CONST_METHOD0(getImportProfile, wisey::ImportProfile* ());
};

#endif /* MockObjectType_h */
