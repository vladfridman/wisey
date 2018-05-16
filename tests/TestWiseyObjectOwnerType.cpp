//
//  TestWiseyObjectOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyObjectOwnerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"
#include "wisey/WiseyObjectType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/StateField.hpp"
#include "wisey/WiseyModelType.hpp"
#include "wisey/WiseyModelOwnerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct WiseyObjectOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  WiseyObjectOwnerType* mWiseyObjectOwnerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  WiseyObjectOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    StructType* structType = StructType::create(mLLVMContext, "mystruct");
    
    ON_CALL(mConcreteObjectType, isReference()).WillByDefault(Return(true));
    ON_CALL(mConcreteObjectType, isNative()).WillByDefault(Return(false));
    ON_CALL(mConcreteObjectType, getLLVMType(_)).WillByDefault(Return(structType->getPointerTo()));
    ON_CALL(mConcreteObjectType, isInterface()).WillByDefault(Return(true));
    string typeName = "systems.vos.wisey.compiler.tests.IInterface";
    string nameGlobal = "systems.vos.wisey.compiler.tests.IInterface.typename";
    
    ON_CALL(mConcreteObjectType, getObjectNameGlobalVariableName())
    .WillByDefault(Return(nameGlobal));
    
    new GlobalVariable(*mContext.getModule(),
                       llvm::ArrayType::get(Type::getInt8Ty(mLLVMContext), typeName.length() + 1),
                       true,
                       GlobalValue::LinkageTypes::ExternalLinkage,
                       nullptr,
                       nameGlobal);

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mWiseyObjectOwnerType = new WiseyObjectOwnerType();
  }
  
  ~WiseyObjectOwnerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(WiseyObjectOwnerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mWiseyObjectOwnerType->getLLVMType(mContext));
  EXPECT_STREQ("::wisey::object*", mWiseyObjectOwnerType->getTypeName().c_str());
}

TEST_F(WiseyObjectOwnerTypeTest, canCastTest) {
  EXPECT_FALSE(mWiseyObjectOwnerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyObjectOwnerType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyObjectOwnerType->canCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyObjectOwnerType->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_TRUE(mWiseyObjectOwnerType->
              canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
}

TEST_F(WiseyObjectOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mWiseyObjectOwnerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyObjectOwnerType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyObjectOwnerType->canAutoCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyObjectOwnerType->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_TRUE(mWiseyObjectOwnerType->
              canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
}

TEST_F(WiseyObjectOwnerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mWiseyObjectOwnerType->getLLVMType(mContext));
  Value* result = mWiseyObjectOwnerType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyObjectOwnerTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mWiseyObjectOwnerType->getLLVMType(mContext));
  mWiseyObjectOwnerType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *mBasicBlock;
  
  EXPECT_STREQ("\nentry:"
               "\n  %0 = invoke i8* @__castObject(i8* null, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @systems.vos.wisey.compiler.tests.IInterface.typename, i32 0, i32 0))"
               "\n          to label %invoke.continue unwind label %cleanup\n",
               mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyObjectOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mWiseyObjectOwnerType->isPrimitive());
  EXPECT_TRUE(mWiseyObjectOwnerType->isOwner());
  EXPECT_FALSE(mWiseyObjectOwnerType->isReference());
  EXPECT_FALSE(mWiseyObjectOwnerType->isArray());
  EXPECT_FALSE(mWiseyObjectOwnerType->isFunction());
  EXPECT_FALSE(mWiseyObjectOwnerType->isPackage());
  EXPECT_TRUE(mWiseyObjectOwnerType->isNative());
  EXPECT_FALSE(mWiseyObjectOwnerType->isPointer());
  EXPECT_FALSE(mWiseyObjectOwnerType->isImmutable());
}

TEST_F(WiseyObjectOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mWiseyObjectOwnerType->isController());
  EXPECT_FALSE(mWiseyObjectOwnerType->isInterface());
  EXPECT_FALSE(mWiseyObjectOwnerType->isModel());
  EXPECT_FALSE(mWiseyObjectOwnerType->isNode());
}

TEST_F(WiseyObjectOwnerTypeTest, createLocalVariableTest) {
  mWiseyObjectOwnerType->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca i8*"
  "\n  store i8* null, i8** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyObjectOwnerTypeTest, createParameterVariableTest) {
  Value* null = ConstantPointerNull::get(mWiseyObjectOwnerType->getLLVMType(mContext));
  
  mWiseyObjectOwnerType->createParameterVariable(mContext, "parameter", null, 0);
  IVariable* variable = mContext.getScopes().getVariable("parameter");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyObjectOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyObjectOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::object*", stringStream.str().c_str());
}

TEST_F(WiseyObjectOwnerTypeTest, getReferenceTest) {
  EXPECT_EQ(WiseyObjectType::WISEY_OBJECT_TYPE, mWiseyObjectOwnerType->getReference());
}

TEST_F(WiseyObjectOwnerTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mWiseyObjectOwnerType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::wisey::object* is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, castWiseyObjectOwnerToModelRunTest) {
  runFile("tests/samples/test_cast_wisey_object_owner_to_model.yz", "5");
}

TEST_F(TestFileRunner, castWiseyObjectOwnerToInterfaceFailRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_cast_wisey_object_owner_to_interface_fail.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_cast_wisey_object_owner_to_interface_fail.yz:19)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.MContainer to systems.vos.wisey.compiler.tests.IHasValue\n"
                               "Main thread ended without a result\n");
}
