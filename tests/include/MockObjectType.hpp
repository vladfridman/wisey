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
  MOCK_CONST_METHOD0(getTypeName, std::string ());
  MOCK_CONST_METHOD0(getShortName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::PointerType* (wisey::IRGenerationContext&));
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
  MOCK_CONST_METHOD0(isExternal, bool ());
  MOCK_CONST_METHOD0(isThread, bool ());
  MOCK_CONST_METHOD0(isNative, bool ());
  MOCK_CONST_METHOD0(isPointer, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
  MOCK_CONST_METHOD2(incrementReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(decrementReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(getReferenceCount, llvm::Value* (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_METHOD1(setImportProfile, void (wisey::ImportProfile*));
  MOCK_CONST_METHOD0(getImportProfile, wisey::ImportProfile* ());
  MOCK_METHOD1(addInnerObject, void (const wisey::IObjectType*));
  MOCK_CONST_METHOD1(getInnerObject, const wisey::IObjectType* (std::string));
  MOCK_CONST_METHOD0(getAccessLevel, wisey::AccessLevel ());
  MOCK_CONST_METHOD0(getInnerObjects, std::map<std::string, const IObjectType*> ());
  MOCK_METHOD0(markAsInner, void ());
  MOCK_CONST_METHOD0(isInner, bool ());
  MOCK_CONST_METHOD1(getReferenceAdjustmentFunction,
                     llvm::Function* (wisey::IRGenerationContext& context));
  MOCK_CONST_METHOD2(createLocalVariable, void (wisey::IRGenerationContext&, std::string name));
  MOCK_CONST_METHOD3(createFieldVariable, void (wisey::IRGenerationContext&,
                                                std::string name,
                                                const wisey::IConcreteObjectType* object));
  MOCK_CONST_METHOD3(createParameterVariable, void (wisey::IRGenerationContext&,
                                                    std::string name,
                                                    llvm::Value* value));
  MOCK_CONST_METHOD1(getArrayType, wisey::ArrayType* (wisey::IRGenerationContext&));
};

#endif /* MockObjectType_h */
