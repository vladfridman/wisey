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

#include "IRGenerationContext.hpp"
#include "IType.hpp"

/**
 * Defines a mock object for IType
 */
class MockType : public wisey::IType {
public:
  MOCK_CONST_METHOD0(getTypeName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::Type* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD2(canCastTo, bool (wisey::IRGenerationContext&, const wisey::IType*));
  MOCK_CONST_METHOD2(canAutoCastTo, bool (wisey::IRGenerationContext&, const wisey::IType*));
  MOCK_CONST_METHOD4(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*,
                                           int));
  MOCK_CONST_METHOD0(isPrimitive, bool ());
  MOCK_CONST_METHOD0(isOwner, bool ());
  MOCK_CONST_METHOD0(isReference, bool ());
  MOCK_CONST_METHOD0(isArray, bool ());
  MOCK_CONST_METHOD0(isFunction, bool ());
  MOCK_CONST_METHOD0(isPackage, bool ());
  MOCK_CONST_METHOD0(isController, bool ());
  MOCK_CONST_METHOD0(isInterface, bool ());
  MOCK_CONST_METHOD0(isModel, bool ());
  MOCK_CONST_METHOD0(isNode, bool ());
  MOCK_CONST_METHOD0(isNative, bool ());
  MOCK_CONST_METHOD0(isPointer, bool ());
  MOCK_CONST_METHOD0(isImmutable, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
  MOCK_CONST_METHOD3(createLocalVariable, void (wisey::IRGenerationContext&,
                                                std::string name,
                                                int));
  MOCK_CONST_METHOD4(createFieldVariable, void (wisey::IRGenerationContext&,
                                                std::string name,
                                                const wisey::IConcreteObjectType* object,
                                                int));
  MOCK_CONST_METHOD4(createParameterVariable, void (wisey::IRGenerationContext&,
                                                    std::string name,
                                                    llvm::Value* value,
                                                    int));
  MOCK_CONST_METHOD2(getArrayType, const wisey::ArrayType* (wisey::IRGenerationContext&, int));
  MOCK_CONST_METHOD3(inject, llvm::Value* (wisey::IRGenerationContext&,
                                           const wisey::InjectionArgumentList,
                                           int));
  MOCK_METHOD0(die, void());
  virtual ~MockType() { die(); }
};

#endif /* MockType_h */
