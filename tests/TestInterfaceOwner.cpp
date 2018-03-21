//
//  TestInterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceOwner}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InterfaceOwnerTest : public Test {
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  StructType* mShapeStructType;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  InterfaceOwnerTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> objectTypes;
    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(objectTypes);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectStructType,
                                               parentInterfaces,
                                               interfaceElements);
    
    vector<Type*> shapeTypes;
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    mShapeStructType = StructType::create(mLLVMContext, shapeFullName);
    mShapeStructType->setBody(shapeTypes);
    vector<IObjectElementDefinition*> shapeMethodElements;
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    InterfaceTypeSpecifier* objectInterfaceSpecifier = new InterfaceTypeSpecifier(NULL, "IObject");
    shapeParentInterfaces.push_back(objectInterfaceSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              shapeFullName,
                                              mShapeStructType,
                                              shapeParentInterfaces,
                                              shapeMethodElements);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~InterfaceOwnerTest() {
    delete mStringStream;
  }
};

TEST_F(InterfaceOwnerTest, getObjectTest) {
  EXPECT_EQ(mShapeInterface->getOwner()->getObjectType(), mShapeInterface);
}

TEST_F(InterfaceOwnerTest, getNameTest) {
  EXPECT_STREQ(mShapeInterface->getOwner()->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.IShape*");
}

TEST_F(InterfaceOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mShapeInterface->getOwner()->getLLVMType(mContext),
            mShapeInterface->getLLVMType(mContext));
}

TEST_F(InterfaceOwnerTest, getDestructorFunctionTest) {
  Function* result = mObjectInterface->getOwner()->getDestructorFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  EXPECT_EQ(functionType, result->getFunctionType());
}

TEST_F(InterfaceOwnerTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mContext, mObjectInterface));

  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mContext, mShapeInterface->getOwner()));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mContext, mObjectInterface->getOwner()));
}

TEST_F(InterfaceOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->getOwner()->canAutoCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mShapeInterface->getOwner()->canAutoCastTo(mContext, mObjectInterface));
  
  EXPECT_TRUE(mObjectInterface->getOwner()->canAutoCastTo(mContext, mShapeInterface->getOwner()));
  EXPECT_TRUE(mShapeInterface->getOwner()->canAutoCastTo(mContext, mObjectInterface->getOwner()));
}

TEST_F(InterfaceOwnerTest, isTypeKindTest) {
  EXPECT_FALSE(mShapeInterface->getOwner()->isPrimitive());
  EXPECT_TRUE(mObjectInterface->getOwner()->isOwner());
  EXPECT_FALSE(mObjectInterface->getOwner()->isReference());
  EXPECT_FALSE(mObjectInterface->getOwner()->isArray());
  EXPECT_FALSE(mObjectInterface->getOwner()->isFunction());
  EXPECT_FALSE(mObjectInterface->getOwner()->isPackage());
  EXPECT_FALSE(mObjectInterface->getOwner()->isNative());
}

TEST_F(InterfaceOwnerTest, isObjectTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->isController());
  EXPECT_TRUE(mObjectInterface->getOwner()->isInterface());
  EXPECT_FALSE(mObjectInterface->getOwner()->isModel());
  EXPECT_FALSE(mObjectInterface->getOwner()->isNode());
  EXPECT_FALSE(mObjectInterface->getOwner()->isThread());
}

TEST_F(InterfaceOwnerTest, createLocalVariableTest) {
  mObjectInterface->getOwner()->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %ownerDeclaration = alloca %systems.vos.wisey.compiler.tests.IObject*"
  "\n  store %systems.vos.wisey.compiler.tests.IObject* null, %systems.vos.wisey.compiler.tests.IObject** %ownerDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mObjectInterface->getOwner(), "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mObjectInterface->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(InterfaceOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mObjectInterface->getOwner()->getLLVMType(mContext));
  mObjectInterface->getOwner()->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %parameterObjectPointer = alloca %systems.vos.wisey.compiler.tests.IObject*"
  "\n  store %systems.vos.wisey.compiler.tests.IObject* null, %systems.vos.wisey.compiler.tests.IObject** %parameterObjectPointer\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, interfaceOwnerCastToModelOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_model_owner.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceOwnerCastToInterfaceOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_interface_owner.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceOwnerCastToAnotherInterfaceOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_another_interface_owner.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceOnwerAutoCastRunTest) {
  runFile("tests/samples/test_interface_owner_autocast.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceOwnerCearedAndThrowsNpeDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_owner_cleared_and_throws_npe.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_owner_cleared_and_throws_npe.yz:43)\n");
}
