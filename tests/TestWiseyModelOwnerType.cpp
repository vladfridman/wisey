//
//  TestWiseyModelOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyModelOwnerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/StateField.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"
#include "wisey/WiseyObjectType.hpp"
#include "wisey/WiseyModelOwnerType.hpp"
#include "wisey/WiseyModelType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct WiseyModelOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  WiseyModelOwnerType* mWiseyModelOwnerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  WiseyModelOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    string nameGlobal = "systems.vos.wisey.compiler.tests.IInterface.name";
    
    ON_CALL(mConcreteObjectType, getObjectNameGlobalVariableName())
    .WillByDefault(Return(nameGlobal));
    
    new GlobalVariable(*mContext.getModule(),
                       llvm::ArrayType::get(Type::getInt8Ty(mLLVMContext), typeName.length() + 1),
                       true,
                       GlobalValue::LinkageTypes::ExternalLinkage,
                       nullptr,
                       nameGlobal);

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mWiseyModelOwnerType = new WiseyModelOwnerType();
  }
  
  ~WiseyModelOwnerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(WiseyModelOwnerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mWiseyModelOwnerType->getLLVMType(mContext));
  EXPECT_STREQ("::wisey::model*", mWiseyModelOwnerType->getTypeName().c_str());
}

TEST_F(WiseyModelOwnerTypeTest, canCastTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelOwnerType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelOwnerType->canCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelOwnerType->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mWiseyModelOwnerType->
               canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelOwnerType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelOwnerType->canAutoCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelOwnerType->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mWiseyModelOwnerType->
               canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelOwnerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mWiseyModelOwnerType->getLLVMType(mContext));
  Value* result = mWiseyModelOwnerType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelOwnerTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mWiseyModelOwnerType->getLLVMType(mContext));
  mWiseyModelOwnerType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *mBasicBlock;
  
  EXPECT_STREQ("\nentry:"
               "\n  %0 = invoke i8* @__castObject(i8* null, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @systems.vos.wisey.compiler.tests.IInterface.name, i32 0, i32 0))"
               "\n          to label %invoke.continue unwind label %cleanup"
               "\n",
               mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->isPrimitive());
  EXPECT_TRUE(mWiseyModelOwnerType->isOwner());
  EXPECT_FALSE(mWiseyModelOwnerType->isReference());
  EXPECT_FALSE(mWiseyModelOwnerType->isArray());
  EXPECT_FALSE(mWiseyModelOwnerType->isFunction());
  EXPECT_FALSE(mWiseyModelOwnerType->isPackage());
  EXPECT_TRUE(mWiseyModelOwnerType->isNative());
  EXPECT_FALSE(mWiseyModelOwnerType->isPointer());
  EXPECT_TRUE(mWiseyModelOwnerType->isImmutable());
}

TEST_F(WiseyModelOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->isController());
  EXPECT_FALSE(mWiseyModelOwnerType->isInterface());
  EXPECT_TRUE(mWiseyModelOwnerType->isModel());
  EXPECT_FALSE(mWiseyModelOwnerType->isNode());
}

TEST_F(WiseyModelOwnerTypeTest, createLocalVariableTest) {
  mWiseyModelOwnerType->createLocalVariable(mContext, "temp", 0);
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

TEST_F(WiseyModelOwnerTypeTest, createParameterVariableTest) {
  Value* null = ConstantPointerNull::get(mWiseyModelOwnerType->getLLVMType(mContext));
  
  mWiseyModelOwnerType->createParameterVariable(mContext, "parameter", null, 0);
  IVariable* variable = mContext.getScopes().getVariable("parameter");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyModelOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyModelOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::model*", stringStream.str().c_str());
}

TEST_F(WiseyModelOwnerTypeTest, getReferenceTest) {
  EXPECT_EQ(WiseyModelType::WISEY_MODEL_TYPE, mWiseyModelOwnerType->getReference());
}

TEST_F(WiseyModelOwnerTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mWiseyModelOwnerType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::wisey::model* is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, castWiseyModelOwnerToInterfaceOwnerRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_cast_wisey_model_owner_to_interface_owner.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_cast_wisey_model_owner_to_interface_owner.yz:23)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.MObject to systems.vos.wisey.compiler.tests.IHasSomething\n"
                               "Main thread ended without a result\n");
}
