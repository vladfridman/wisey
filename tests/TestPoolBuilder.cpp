//
//  TestPoolBuilder.cpp
//  runtests
//
//  Created by Vladimir Fridman on 6/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PoolBuilder}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/PoolBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct PoolBuilderTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mModel;
  PoolBuilder* mPoolBuilder;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  NiceMock<MockExpression>* mPoolExpression;
  ModelTypeSpecifier* mModelTypeSpecifier;
  BasicBlock *mEntryBlock;
  BasicBlock *mDeclareBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  PoolBuilderTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()),
  mPoolExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    const Controller* cMemoryPool = mContext.getController(Names::getCMemoryPoolFullName(), 0);

    mModelTypeSpecifier = new ModelTypeSpecifier(NULL, "MShape", 0);
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(cMemoryPool->getLLVMType(mContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType *structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mWidth", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mHeight", 0));
    mModel = Model::newPooledModel(AccessLevel::PUBLIC_ACCESS,
                                   modelFullName,
                                   structType,
                                   mContext.getImportProfile(),
                                   0);
    mModel->setFields(mContext, fields, 2u);
    mContext.addModel(mModel, 0);
    Value* fieldValue1 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(fieldValue1));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mField1Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument1));
    Value* fieldValue2 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(fieldValue2));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mField2Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument2));
    
    IConcreteObjectType::declareTypeNameGlobal(mContext, mModel);
    IConcreteObjectType::declareVTable(mContext, mModel);
    
    Value* pool = ConstantPointerNull::get(cMemoryPool->getLLVMType(mContext));
    ON_CALL(*mPoolExpression, generateIR(_, _)).WillByDefault(Return(pool));
    ON_CALL(*mPoolExpression, getType(_)).WillByDefault(Return(cMemoryPool));
    ON_CALL(*mPoolExpression, printToStream(_, _)).WillByDefault(Invoke(printPoolExpression));

    string argumentSpecifier1("withWidth");
    BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1, mField1Expression);
    string argumentSpecifier2("withHeight");
    BuilderArgument *argument2 = new BuilderArgument(argumentSpecifier2, mField2Expression);
    BuilderArgumentList argumentList;
    argumentList.push_back(argument1);
    argumentList.push_back(argument2);
    mPoolBuilder = new PoolBuilder(mModelTypeSpecifier, argumentList, mPoolExpression, 0);
   
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());

    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~PoolBuilderTest() {
    delete mField1Expression;
    delete mField2Expression;
    delete mPoolExpression;
    delete mStringStream;
  }
  
  static void printBuilderArgument1(IRGenerationContext& context, iostream& stream) {
    stream << "3";
  }
  
  static void printBuilderArgument2(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
  
  static void printPoolExpression(IRGenerationContext& context, iostream& stream) {
    stream << "pool";
  }
};

TEST_F(PoolBuilderTest, getTypeTest) {
  BuilderArgumentList argumentList;
  PoolBuilder allocator(mModelTypeSpecifier, argumentList, mPoolExpression, 0);
  
  EXPECT_EQ(allocator.getType(mContext), mModel->getOwner());
}

TEST_F(PoolBuilderTest, isConstantTest) {
  EXPECT_FALSE(mPoolBuilder->isConstant());
}

TEST_F(PoolBuilderTest, isAssignableTest) {
  EXPECT_FALSE(mPoolBuilder->isAssignable());
}

TEST_F(PoolBuilderTest, printToStreamTest) {
  stringstream stringStream;
  mPoolBuilder->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("allocator(systems.vos.wisey.compiler.tests.MShape)"
               ".withWidth(3).withHeight(5).onPool(pool)",
               stringStream.str().c_str());
}

TEST_F(PoolBuilderTest, generateIRTest) {
  mPoolBuilder->generateIR(mContext, PrimitiveTypes::VOID);
  mContext.setBasicBlock(mDeclareBlock);
  IRWriter::createBranch(mContext, mEntryBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal void @test() {"
  "\ndeclare:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %1 = bitcast %wisey.lang.CMemoryPool* null to %CMemoryPool*"
  "\n  %2 = getelementptr %CMemoryPool, %CMemoryPool* %1, i32 0, i32 2"
  "\n  %3 = load %AprPool*, %AprPool** %2"
  "\n  %4 = getelementptr %CMemoryPool, %CMemoryPool* %1, i32 0, i32 1"
  "\n  %5 = load i64, i64* %4"
  "\n  %6 = add i64 %5, 1"
  "\n  store i64 %6, i64* %4"
  "\n  %call = call i8* @wisey.lang.CMemoryPool.pallocate(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null, %AprPool* %3, i64 ptrtoint (%systems.vos.wisey.compiler.tests.MShape.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.MShape.refCounter, %systems.vos.wisey.compiler.tests.MShape.refCounter* null, i32 1) to i64))"
  "\n  %7 = bitcast i8* %call to %systems.vos.wisey.compiler.tests.MShape.refCounter*"
  "\n  %8 = getelementptr %systems.vos.wisey.compiler.tests.MShape.refCounter, %systems.vos.wisey.compiler.tests.MShape.refCounter* %7, i32 0, i32 0"
  "\n  store i64 0, i64* %8"
  "\n  %9 = getelementptr %systems.vos.wisey.compiler.tests.MShape.refCounter, %systems.vos.wisey.compiler.tests.MShape.refCounter* %7, i32 0, i32 1"
  "\n  %10 = getelementptr %systems.vos.wisey.compiler.tests.MShape, %systems.vos.wisey.compiler.tests.MShape* %9, i32 0, i32 1"
  "\n  store %wisey.lang.CMemoryPool* null, %wisey.lang.CMemoryPool** %10"
  "\n  %11 = getelementptr %systems.vos.wisey.compiler.tests.MShape, %systems.vos.wisey.compiler.tests.MShape* %9, i32 0, i32 2"
  "\n  store i32 3, i32* %11"
  "\n  %12 = getelementptr %systems.vos.wisey.compiler.tests.MShape, %systems.vos.wisey.compiler.tests.MShape* %9, i32 0, i32 3"
  "\n  store i32 5, i32* %12"
  "\n  %13 = bitcast %systems.vos.wisey.compiler.tests.MShape* %9 to i8*"
  "\n  %14 = getelementptr i8, i8* %13, i64 0"
  "\n  %15 = bitcast i8* %14 to i32 (...)***"
  "\n  %16 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.MShape.vtable, i32 0, i32 0, i32 0"
  "\n  %17 = bitcast i8** %16 to i32 (...)**"
  "\n  store i32 (...)** %17, i32 (...)*** %15"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* %9, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n}"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(PoolBuilderTest, allocateInvalidBuilderArgumentsDeathTest) {
  string argumentSpecifier1("width");
  BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withHeight");
  BuilderArgument *argument2 = new BuilderArgument(argumentSpecifier2, mField2Expression);
  BuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  mPoolBuilder = new PoolBuilder(mModelTypeSpecifier, argumentList, mPoolExpression, 1);

  const char* expected =
  "/tmp/source.yz(1): Error: Object allocator argument should start with 'with'. e.g. .withField(value).\n";

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mPoolBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ(expected, buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(PoolBuilderTest, allocateIncorrectArgumentTypeDeathTest) {
  Value* fieldValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
  ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(fieldValue));
  ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));

  string argumentSpecifier1("withWidth");
  BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withHeight");
  BuilderArgument *argument2 = new BuilderArgument(argumentSpecifier2, mField2Expression);
  BuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  mPoolBuilder = new PoolBuilder(mModelTypeSpecifier, argumentList, mPoolExpression, 3);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mPoolBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Model allocator argument value for field mHeight does not match its type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(PoolBuilderTest, allocateNotAllFieldsAreSetDeathTest) {
  string argumentSpecifier1("withWidth");
  BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1, mField1Expression);
  BuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  mPoolBuilder = new PoolBuilder(mModelTypeSpecifier, argumentList, mPoolExpression, 7);

  const char* expected =
  "/tmp/source.yz(7): Error: Field mHeight of object systems.vos.wisey.compiler.tests.MShape is not initialized.\n";

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mPoolBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ(expected, buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, allocateBuildableDeathRunTest) {
  expectFailCompile("tests/samples/test_allocate_buildable.yz",
                    1,
                    "tests/samples/test_allocate_buildable.yz\\(18\\): Error: "
                    "Object systems.vos.wisey.compiler.tests.MCar can not be allocated on a memory pool beause it is not marked with onPool qualifier, it should be allocated on heap");
}
