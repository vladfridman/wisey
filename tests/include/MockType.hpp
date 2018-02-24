//
//  MockType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockType_h
#define MockType_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"

/**
 * Defines a mock object for IType
 */
class MockType : public wisey::IType {
public:
  MOCK_CONST_METHOD0(getTypeName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::Type* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD0(getTypeKind, wisey::TypeKind ());
  MOCK_CONST_METHOD2(canCastTo, bool (wisey::IRGenerationContext&, const wisey::IType*));
  MOCK_CONST_METHOD2(canAutoCastTo, bool (wisey::IRGenerationContext&, const wisey::IType*));
  MOCK_CONST_METHOD4(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*,
                                           int));
  MOCK_CONST_METHOD0(isOwner, bool ());
  MOCK_CONST_METHOD0(isReference, bool ());
  MOCK_CONST_METHOD0(isArray, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
  MOCK_CONST_METHOD2(createLocalVariable, void (wisey::IRGenerationContext&, std::string name));
  MOCK_CONST_METHOD3(createFieldVariable, void (wisey::IRGenerationContext&,
                                                std::string name,
                                                const wisey::IConcreteObjectType* object));
  MOCK_CONST_METHOD3(createParameterVariable, void (wisey::IRGenerationContext&,
                                                    std::string name,
                                                    llvm::Value* value));
  MOCK_CONST_METHOD1(getArrayType, wisey::ArrayType* (wisey::IRGenerationContext&));
};

#endif /* MockType_h */
