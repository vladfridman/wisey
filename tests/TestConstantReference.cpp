//
//  TestConstantReference.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ConstantReference}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "wisey/ConstantReference.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ConstantReferenceTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mMockExpression;
  NiceMock<MockObjectType>* mMockObject;
  NiceMock<MockObjectTypeSpecifier>* mMockObjectTypeSpecifier;
  string mName;
  string mConstantGlobalName;
  ConstantReference* mConstantReference;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;
  llvm::BasicBlock* mBasicBlock;

public:
  
  ConstantReferenceTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()),
  mMockObject(new NiceMock<MockObjectType>()),
  mMockObjectTypeSpecifier(new NiceMock<MockObjectTypeSpecifier>()),
  mName("MYCONSTANT") {
    mConstantReference = new ConstantReference(mMockObjectTypeSpecifier, mName, 0);
    ON_CALL(*mMockObjectTypeSpecifier, getType(_)).WillByDefault(Return(mMockObject));
    Constant* constant = new Constant(PUBLIC_ACCESS,
                                      PrimitiveTypes::INT,
                                      mName,
                                      mMockExpression);
    ON_CALL(*mMockObject, findConstant(_, _, _)).WillByDefault(Return(constant));
    ON_CALL(*mMockObject, getTypeName()).WillByDefault(Return("MMyObject"));
    ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    llvm::Constant* intConstant = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(intConstant));
    ON_CALL(*mMockExpression, isConstant()).WillByDefault(Return(true));
    constant->generateIR(mContext, mMockObject);
    
    ON_CALL(*mMockObjectTypeSpecifier, printToStream(_, _))
    .WillByDefault(Invoke(printObjecTypeSpecifier));

    llvm::FunctionType* functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(mLLVMContext),
                                                               false);
    functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(mLLVMContext), false);
    llvm::Function* function = llvm::Function::Create(functionType,
                                                      llvm::GlobalValue::InternalLinkage,
                                                      "main",
                                                      mContext.getModule());

    mBasicBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
  }
  
  ~ConstantReferenceTest() {
    delete mMockExpression;
    delete mMockObject;
    delete mMockObjectTypeSpecifier;
  }
  
  static void printObjecTypeSpecifier(IRGenerationContext& context, iostream& stream) {
    stream << "MMyObject";
  }
};

TEST_F(ConstantReferenceTest, getTypeTest) {
  EXPECT_EQ(mConstantReference->getType(mContext), PrimitiveTypes::INT);
}

TEST_F(ConstantReferenceTest, isConstantTest) {
  EXPECT_TRUE(mConstantReference->isConstant());
}

TEST_F(ConstantReferenceTest, isAssignableTest) {
  EXPECT_FALSE(mConstantReference->isAssignable());
}

TEST_F(ConstantReferenceTest, generateIRTest) {
  mConstantReference->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load i32, i32* @constant.MMyObject.MYCONSTANT\n";
  
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
}

TEST_F(ConstantReferenceTest, printToStreamTest) {
  stringstream stringStream;
  mConstantReference->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("MMyObject.MYCONSTANT", stringStream.str().c_str());
}

TEST_F(ConstantReferenceTest, printToStreamConstantReferenceWihtoutObjectTest) {
  stringstream stringStream;
  ConstantReference constantReference(NULL, mName, 0);
  constantReference.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("MYCONSTANT", stringStream.str().c_str());
}

TEST_F(TestFileRunner, constantReferenceInObjectRunTest) {
  runFile("tests/samples/test_constant_reference_in_object.yz", "5");
}

TEST_F(TestFileRunner, constantReferenceInInterfaceRunTest) {
  runFile("tests/samples/test_constant_reference_in_interface.yz", "2");
}

TEST_F(TestFileRunner, constantReferencePrivateDeathRunTest) {
  expectFailCompile("tests/samples/test_constant_reference_private.yz",
                    1,
                    "Error: Trying to reference private constant not visible "
                    "from the current object");
}

