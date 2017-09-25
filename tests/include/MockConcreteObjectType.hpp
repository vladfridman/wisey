//
//  MockConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/31/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockConcreteObjectType_h
#define MockConcreteObjectType_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"

/**
 * Defines a mock object for IExpression
 */
class MockConcreteObjectType : public wisey::IConcreteObjectType {
public:
  MOCK_CONST_METHOD1(findField, wisey::IField* (std::string));
  MOCK_CONST_METHOD0(getFields, std::map<std::string, wisey::IField*> ());
  MOCK_CONST_METHOD0(getVTableName, std::string ());
  MOCK_CONST_METHOD0(getVTableSize, unsigned long ());
  MOCK_CONST_METHOD0(getInterfaces, std::vector<wisey::Interface*> ());
  MOCK_CONST_METHOD0(getFlattenedInterfaceHierarchy, std::vector<wisey::Interface*> ());
  MOCK_CONST_METHOD0(getTypeTableName, std::string ());
  MOCK_CONST_METHOD1(findMethod, wisey::IMethod* (std::string));
  MOCK_CONST_METHOD0(getMethods, std::vector<wisey::IMethod*> ());

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
  MOCK_CONST_METHOD1(printToStream, void (std::iostream& stream));
};

#endif /* MockConcreteObjectType_h */
