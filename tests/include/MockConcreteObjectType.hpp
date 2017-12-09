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
  MOCK_CONST_METHOD1(findField, wisey::Field* (std::string));
  MOCK_CONST_METHOD1(findConstant, wisey::Constant* (std::string));
  MOCK_CONST_METHOD1(getFieldIndex, unsigned long (wisey::Field*));
  MOCK_CONST_METHOD0(getFields, std::vector<wisey::Field*> ());
  MOCK_CONST_METHOD0(getVTableName, std::string ());
  MOCK_CONST_METHOD0(getVTableSize, unsigned long ());
  MOCK_CONST_METHOD0(getInterfaces, std::vector<wisey::Interface*> ());
  MOCK_CONST_METHOD0(getFlattenedInterfaceHierarchy, std::vector<wisey::Interface*> ());
  MOCK_CONST_METHOD0(getTypeTableName, std::string ());
  MOCK_CONST_METHOD1(findMethod, wisey::IMethod* (std::string));
  MOCK_CONST_METHOD0(getMethods, std::vector<wisey::IMethod*> ());
  MOCK_CONST_METHOD0(getConstants, std::vector<wisey::Constant*> ());

  MOCK_CONST_METHOD0(getObjectNameGlobalVariableName, std::string ());
  MOCK_CONST_METHOD0(getOwner, const wisey::IObjectOwnerType* ());
  MOCK_CONST_METHOD0(getName, std::string ());
  MOCK_CONST_METHOD0(getShortName, std::string ());
  MOCK_CONST_METHOD1(getLLVMType, llvm::PointerType* (llvm::LLVMContext&));
  MOCK_CONST_METHOD0(getTypeKind, wisey::TypeKind ());
  MOCK_CONST_METHOD1(canCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD1(canAutoCastTo, bool (const wisey::IType*));
  MOCK_CONST_METHOD4(castTo, llvm::Value* (wisey::IRGenerationContext&,
                                           llvm::Value*,
                                           const wisey::IType*,
                                           int));
  MOCK_CONST_METHOD0(isExternal, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream& stream));
  MOCK_METHOD2(setFields, void (std::vector<wisey::Field*>, unsigned long));
  MOCK_METHOD1(setInterfaces, void (std::vector<wisey::Interface*>));
  MOCK_METHOD1(setMethods, void (std::vector<wisey::IMethod*>));
  MOCK_METHOD1(setStructBodyTypes, void (std::vector<llvm::Type*>));
  MOCK_METHOD1(setConstants, void (std::vector<wisey::Constant*>));
  MOCK_METHOD1(setImportProfile, void (wisey::ImportProfile*));
  MOCK_CONST_METHOD0(getImportProfile, wisey::ImportProfile* ());
  MOCK_CONST_METHOD2(incremenetReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(decremenetReferenceCount, void (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_CONST_METHOD2(getReferenceCount, llvm::Value* (wisey::IRGenerationContext&, llvm::Value*));
  MOCK_METHOD1(addInnerObject, void (const wisey::IObjectType*));
  MOCK_CONST_METHOD1(getInnerObject, const wisey::IObjectType* (std::string));
  MOCK_CONST_METHOD0(getAccessLevel, wisey::AccessLevel ());
  MOCK_CONST_METHOD0(getInnerObjects, std::map<std::string, const IObjectType*> ());
  MOCK_METHOD0(markAsInner, void ());
  MOCK_CONST_METHOD0(isInner, bool ());
};

#endif /* MockConcreteObjectType_h */
