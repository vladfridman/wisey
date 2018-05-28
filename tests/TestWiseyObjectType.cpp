//
//  TestWiseyObjectType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyObjectType}
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

struct WiseyObjectTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  WiseyObjectType* mWiseyObjectType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  WiseyObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mWiseyObjectType = new WiseyObjectType();
  }
  
  ~WiseyObjectTypeTest() {
    delete mStringStream;
  }
};

TEST_F(WiseyObjectTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mWiseyObjectType->getLLVMType(mContext));
  EXPECT_STREQ("::wisey::object", mWiseyObjectType->getTypeName().c_str());
}

TEST_F(WiseyObjectTypeTest, canCastTest) {
  EXPECT_FALSE(mWiseyObjectType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyObjectType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyObjectType->canCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyObjectType->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mWiseyObjectType->
               canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
}

TEST_F(WiseyObjectTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mWiseyObjectType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyObjectType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyObjectType->canAutoCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyObjectType->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mWiseyObjectType->
               canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
}

TEST_F(WiseyObjectTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mWiseyObjectType->getLLVMType(mContext));
  Value* result = mWiseyObjectType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyObjectTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mWiseyObjectType->getLLVMType(mContext));
  mWiseyObjectType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *mEntryBlock;
  
  EXPECT_STREQ("\nentry:                                            ; No predecessors!"
               "\n  %0 = invoke i8* @__castObject(i8* null, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @systems.vos.wisey.compiler.tests.IInterface.typename, i32 0, i32 0))"
               "\n          to label %invoke.continue unwind label %cleanup\n",
               mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyObjectTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mWiseyObjectType->isPrimitive());
  EXPECT_FALSE(mWiseyObjectType->isOwner());
  EXPECT_TRUE(mWiseyObjectType->isReference());
  EXPECT_FALSE(mWiseyObjectType->isArray());
  EXPECT_FALSE(mWiseyObjectType->isFunction());
  EXPECT_FALSE(mWiseyObjectType->isPackage());
  EXPECT_TRUE(mWiseyObjectType->isNative());
  EXPECT_FALSE(mWiseyObjectType->isPointer());
  EXPECT_FALSE(mWiseyObjectType->isImmutable());
}

TEST_F(WiseyObjectTypeTest, isObjectTest) {
  EXPECT_FALSE(mWiseyObjectType->isController());
  EXPECT_FALSE(mWiseyObjectType->isInterface());
  EXPECT_FALSE(mWiseyObjectType->isModel());
  EXPECT_FALSE(mWiseyObjectType->isNode());
}

TEST_F(WiseyObjectTypeTest, createLocalVariableTest) {
  mWiseyObjectType->createLocalVariable(mContext, "temp", 0);
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

TEST_F(WiseyObjectTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mWiseyObjectType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mWiseyObjectType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyObjectTypeTest, createParameterVariableTest) {
  llvm::Constant* null = ConstantPointerNull::get(mWiseyObjectType->getLLVMType(mContext));
  mWiseyObjectType->createParameterVariable(mContext, "foo", null, 0);
  IVariable* variable = mContext.getScopes().getVariable("foo");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyObjectTypeTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyObjectType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::object", stringStream.str().c_str());
}

TEST_F(WiseyObjectTypeTest, getOwnerTest) {
  EXPECT_EQ(WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE, mWiseyObjectType->getOwner());
}

TEST_F(WiseyObjectTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mWiseyObjectType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::wisey::object is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, referenceCountDecrementedForWiseyObjectTypeVariableTest) {
  runFile("tests/samples/test_reference_count_decremented_for_llvm_object_type_variable.yz", 1);
}

TEST_F(TestFileRunner, compareLLVMObjectsToNullTest) {
  runFile("tests/samples/test_compare_llvm_objects_to_null.yz", 1);
}

TEST_F(TestFileRunner, castWiseyObjectReferenceToModelRunTest) {
  runFile("tests/samples/test_cast_wisey_object_reference_to_model.yz", 3);
}

TEST_F(TestFileRunner, castWiseyObjectReferenceToInterfaceRunTest) {
  runFile("tests/samples/test_cast_wisey_object_reference_to_interface.yz", 1);
}

TEST_F(TestFileRunner, castWiseyObjectReferenceToInterfaceFailRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_cast_wisey_object_reference_to_interface_fail.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_cast_wisey_object_reference_to_interface_fail.yz:19)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.MContainer to systems.vos.wisey.compiler.tests.IHasValue\n"
                               "Main thread ended without a result\n");
}
