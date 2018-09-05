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
#include "wisey/Names.hpp"
#include "wisey/NodeTypeSpecifier.hpp"
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
  Node* mNode;
  PoolBuilder* mPoolBuilder;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  NiceMock<MockExpression>* mPoolExpression;
  NodeTypeSpecifier* mNodeTypeSpecifier;
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

    mNodeTypeSpecifier = new NodeTypeSpecifier(NULL, "NShape", 0);
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(cMemoryPool->getLLVMType(mContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.NShape";
    StructType *structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mWidth", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mHeight", 0));
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                          modelFullName,
                          structType,
                          mContext.getImportProfile(),
                          0);
    mNode->setFields(mContext, fields, 2u);
    mContext.addNode(mNode, 0);
    Value* fieldValue1 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(fieldValue1));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mField1Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument1));
    Value* fieldValue2 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(fieldValue2));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mField2Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument2));
    
    IConcreteObjectType::declareTypeNameGlobal(mContext, mNode);
    IConcreteObjectType::declareVTable(mContext, mNode);
    
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
    mPoolBuilder = new PoolBuilder(mNodeTypeSpecifier, argumentList, mPoolExpression, 0);
   
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
  PoolBuilder allocator(mNodeTypeSpecifier, argumentList, mPoolExpression, 0);
  
  EXPECT_EQ(allocator.getType(mContext), mNode->getOwner());
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
  
  EXPECT_STREQ("allocator(systems.vos.wisey.compiler.tests.NShape)"
               ".withWidth(3).withHeight(5).onPool(pool)",
               stringStream.str().c_str());
}

TEST_F(PoolBuilderTest, generateIRTest) {
  mPoolBuilder->generateIR(mContext, PrimitiveTypes::VOID);
  mContext.setBasicBlock(mDeclareBlock);
  IRWriter::createBranch(mContext, mEntryBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal void @test() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.NShape*"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %1 = icmp eq %wisey.lang.CMemoryPool* null, null"
  "\n  br i1 %1, label %if.pool.null, label %if.pool.notnull"
  "\n"
  "\nif.pool.null:                                     ; preds = %entry"
  "\n  invoke void @__throwNPE()"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.pool.notnull:                                  ; preds = %entry"
  "\n  %2 = bitcast %wisey.lang.CMemoryPool* null to %CMemoryPool*"
  "\n  %3 = getelementptr %CMemoryPool, %CMemoryPool* %2, i32 0, i32 2"
  "\n  %4 = load i8*, i8** %3"
  "\n  %5 = getelementptr %CMemoryPool, %CMemoryPool* %2, i32 0, i32 1"
  "\n  %6 = load i64, i64* %5"
  "\n  %7 = add i64 %6, 1"
  "\n  store i64 %7, i64* %5"
  "\n  %8 = call i8* @mem_pool_alloc(i8* %4, i64 ptrtoint (%systems.vos.wisey.compiler.tests.NShape.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.NShape.refCounter, %systems.vos.wisey.compiler.tests.NShape.refCounter* null, i32 1) to i64))"
  "\n  %9 = bitcast i8* %8 to %systems.vos.wisey.compiler.tests.NShape.refCounter*"
  "\n  %10 = getelementptr %systems.vos.wisey.compiler.tests.NShape.refCounter, %systems.vos.wisey.compiler.tests.NShape.refCounter* %9, i32 0, i32 0"
  "\n  store i64 0, i64* %10"
  "\n  %11 = getelementptr %systems.vos.wisey.compiler.tests.NShape.refCounter, %systems.vos.wisey.compiler.tests.NShape.refCounter* %9, i32 0, i32 1"
  "\n  %12 = getelementptr %systems.vos.wisey.compiler.tests.NShape, %systems.vos.wisey.compiler.tests.NShape* %11, i32 0, i32 1"
  "\n  store %wisey.lang.CMemoryPool* null, %wisey.lang.CMemoryPool** %12"
  "\n  %13 = getelementptr %systems.vos.wisey.compiler.tests.NShape, %systems.vos.wisey.compiler.tests.NShape* %11, i32 0, i32 2"
  "\n  store i32 3, i32* %13"
  "\n  %14 = getelementptr %systems.vos.wisey.compiler.tests.NShape, %systems.vos.wisey.compiler.tests.NShape* %11, i32 0, i32 3"
  "\n  store i32 5, i32* %14"
  "\n  %15 = bitcast %systems.vos.wisey.compiler.tests.NShape* %11 to i8*"
  "\n  %16 = getelementptr i8, i8* %15, i64 0"
  "\n  %17 = bitcast i8* %16 to i32 (...)***"
  "\n  %18 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.NShape.vtable, i32 0, i32 0, i32 0"
  "\n  %19 = bitcast i8** %18 to i32 (...)**"
  "\n  store i32 (...)** %19, i32 (...)*** %17"
  "\n  store %systems.vos.wisey.compiler.tests.NShape* %11, %systems.vos.wisey.compiler.tests.NShape** %0"
  "\n"
  "\ncleanup:                                          ; preds = %if.pool.null"
  "\n  %20 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %21 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %20, { i8*, i32 }* %21"
  "\n  %22 = getelementptr { i8*, i32 }, { i8*, i32 }* %21, i32 0, i32 0"
  "\n  %23 = load i8*, i8** %22"
  "\n  %24 = call i8* @__cxa_get_exception_ptr(i8* %23)"
  "\n  %25 = getelementptr i8, i8* %24, i64 8"
  "\n  resume { i8*, i32 } %20"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.pool.null"
  "\n  unreachable"
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
  mPoolBuilder = new PoolBuilder(mNodeTypeSpecifier, argumentList, mPoolExpression, 1);

  const char* expected =
  "/tmp/source.yz(1): Error: Object builder argument should start with 'with'. e.g. .withField(value).\n";

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
  mPoolBuilder = new PoolBuilder(mNodeTypeSpecifier, argumentList, mPoolExpression, 3);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mPoolBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Node builder argument value for field mHeight does not match its type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(PoolBuilderTest, allocateNotAllFieldsAreSetDeathTest) {
  string argumentSpecifier1("withWidth");
  BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1, mField1Expression);
  BuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  mPoolBuilder = new PoolBuilder(mNodeTypeSpecifier, argumentList, mPoolExpression, 7);

  const char* expected =
  "/tmp/source.yz(7): Error: Field mHeight of object systems.vos.wisey.compiler.tests.NShape is not initialized.\n";

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mPoolBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ(expected, buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, nullPoolAllocateRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_null_pool_allocate.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_null_pool_allocate.yz:12)\n"
                               "Main thread ended without a result\n");
}
