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

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

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
    vector<IObjectElementDeclaration*> interfaceElements;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectStructType,
                                               parentInterfaces,
                                               interfaceElements);
    
    vector<Type*> shapeTypes;
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    mShapeStructType = StructType::create(mLLVMContext, shapeFullName);
    mShapeStructType->setBody(shapeTypes);
    vector<IObjectElementDeclaration*> shapeMethodElements;
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    InterfaceTypeSpecifier* objectInterfaceSpecifier = new InterfaceTypeSpecifier("", "IObject");
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
  EXPECT_EQ(mShapeInterface->getOwner()->getObject(), mShapeInterface);
}

TEST_F(InterfaceOwnerTest, getNameTest) {
  EXPECT_STREQ(mShapeInterface->getOwner()->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.IShape*");
}

TEST_F(InterfaceOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mShapeInterface->getOwner()->getLLVMType(mLLVMContext),
            mShapeInterface->getLLVMType(mLLVMContext));
}

TEST_F(InterfaceOwnerTest, getTypeKindTest) {
  EXPECT_EQ(mShapeInterface->getOwner()->getTypeKind(), INTERFACE_OWNER_TYPE);
}

TEST_F(InterfaceOwnerTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mObjectInterface));

  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mShapeInterface->getOwner()));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mObjectInterface->getOwner()));
}

TEST_F(InterfaceOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->getOwner()->canAutoCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->getOwner()->canAutoCastTo(mObjectInterface));
  
  EXPECT_TRUE(mObjectInterface->getOwner()->canAutoCastTo(mShapeInterface->getOwner()));
  EXPECT_TRUE(mShapeInterface->getOwner()->canAutoCastTo(mObjectInterface->getOwner()));
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
