//
//  TestWiseyModelType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyModelType}
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

struct WiseyModelTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  WiseyModelType* mWiseyModelType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  WiseyModelTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
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
    
    mWiseyModelType = new WiseyModelType();
  }
  
  ~WiseyModelTypeTest() {
    delete mStringStream;
  }
};

TEST_F(WiseyModelTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mWiseyModelType->getLLVMType(mContext));
  EXPECT_STREQ("::wisey::model", mWiseyModelType->getTypeName().c_str());
}

TEST_F(WiseyModelTypeTest, canCastTest) {
  EXPECT_FALSE(mWiseyModelType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelType->canCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelType->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mWiseyModelType->canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mWiseyModelType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelType->canAutoCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelType->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mWiseyModelType->
               canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mWiseyModelType->getLLVMType(mContext));
  Value* result = mWiseyModelType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mWiseyModelType->getLLVMType(mContext));
  mWiseyModelType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *mEntryBlock;
  
  EXPECT_STREQ("\nentry:                                            ; No predecessors!"
               "\n  %0 = invoke i8* @__castObject(i8* null, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @systems.vos.wisey.compiler.tests.IInterface.typename, i32 0, i32 0))"
               "\n          to label %invoke.continue unwind label %cleanup"
               "\n",
               mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mWiseyModelType->isPrimitive());
  EXPECT_FALSE(mWiseyModelType->isOwner());
  EXPECT_TRUE(mWiseyModelType->isReference());
  EXPECT_FALSE(mWiseyModelType->isArray());
  EXPECT_FALSE(mWiseyModelType->isFunction());
  EXPECT_FALSE(mWiseyModelType->isPackage());
  EXPECT_TRUE(mWiseyModelType->isNative());
  EXPECT_FALSE(mWiseyModelType->isPointer());
  EXPECT_TRUE(mWiseyModelType->isImmutable());
}

TEST_F(WiseyModelTypeTest, isObjectTest) {
  EXPECT_FALSE(mWiseyModelType->isController());
  EXPECT_FALSE(mWiseyModelType->isInterface());
  EXPECT_TRUE(mWiseyModelType->isModel());
  EXPECT_FALSE(mWiseyModelType->isNode());
}

TEST_F(WiseyModelTypeTest, createLocalVariableTest) {
  mWiseyModelType->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %temp = alloca i8*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store i8* null, i8** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mWiseyModelType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mWiseyModelType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyModelTypeTest, createParameterVariableTest) {
  llvm::Constant* null = ConstantPointerNull::get(mWiseyModelType->getLLVMType(mContext));
  mWiseyModelType->createParameterVariable(mContext, "foo", null, 0);
  IVariable* variable = mContext.getScopes().getVariable("foo");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyModelTypeTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyModelType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::model", stringStream.str().c_str());
}

TEST_F(WiseyModelTypeTest, getOwnerTest) {
  EXPECT_EQ(WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE, mWiseyModelType->getOwner());
}

TEST_F(WiseyModelTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mWiseyModelType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::wisey::model is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, castWiseyModelReferenceToInterfaceReferenceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_cast_wisey_model_reference_to_interface_reference.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_cast_wisey_model_reference_to_interface_reference.yz:23)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.MObject to systems.vos.wisey.compiler.tests.IHasSomething\n"
                               "Main thread ended without a result\n");
}
