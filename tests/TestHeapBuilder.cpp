//
//  TestHeapBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link HeapBuilder}
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
#include "wisey/HeapBuilder.hpp"
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

struct HeapBuilderTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  HeapBuilder* mHeapBuilder;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  ModelTypeSpecifier* mModelTypeSpecifier;
  BasicBlock *mEntryBlock;
  BasicBlock *mDeclareBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";

  HeapBuilderTest() :
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    TestPrefix::generateIR(mContext);

    mModelTypeSpecifier = new ModelTypeSpecifier(NULL, "MShape", 0);
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(llvmContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType *structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mWidth", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mHeight", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);
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

    string argumentSpecifier1("withWidth");
    BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1, mField1Expression);
    string argumentSpecifier2("withHeight");
    BuilderArgument *argument2 = new BuilderArgument(argumentSpecifier2, mField2Expression);
    BuilderArgumentList argumentList;
    argumentList.push_back(argument1);
    argumentList.push_back(argument2);
    mHeapBuilder = new HeapBuilder(mModelTypeSpecifier, argumentList, 0);

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());

    mDeclareBlock = BasicBlock::Create(llvmContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(llvmContext, "entry", mFunction);
    mContext.setBasicBlock(mEntryBlock);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~HeapBuilderTest() {
    delete mField1Expression;
    delete mField2Expression;
    delete mStringStream;
  }
  
  static void printBuilderArgument1(IRGenerationContext& context, iostream& stream) {
    stream << "3";
  }
  
  static void printBuilderArgument2(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(HeapBuilderTest, getTypeTest) {
  BuilderArgumentList argumentList;
  HeapBuilder HeapBuilder(mModelTypeSpecifier, argumentList, 0);

  EXPECT_EQ(HeapBuilder.getType(mContext), mModel->getOwner());
}

TEST_F(HeapBuilderTest, isConstantTest) {
  EXPECT_FALSE(mHeapBuilder->isConstant());
}

TEST_F(HeapBuilderTest, isAssignableTest) {
  EXPECT_FALSE(mHeapBuilder->isAssignable());
}

TEST_F(HeapBuilderTest, printToStreamTest) {
  stringstream stringStream;
  mHeapBuilder->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("builder(systems.vos.wisey.compiler.tests.MShape)"
               ".withWidth(3).withHeight(5).build()",
               stringStream.str().c_str());
}

TEST_F(HeapBuilderTest, generateIRTest) {
  mHeapBuilder->generateIR(mContext, PrimitiveTypes::VOID);
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
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.MShape.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.MShape.refCounter, %systems.vos.wisey.compiler.tests.MShape.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.MShape.refCounter*"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MShape.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.MShape.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.MShape.refCounter, %systems.vos.wisey.compiler.tests.MShape.refCounter* null, i32 1) to i64), i1 false)"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MShape.refCounter, %systems.vos.wisey.compiler.tests.MShape.refCounter* %buildervar, i32 0, i32 1"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.MShape, %systems.vos.wisey.compiler.tests.MShape* %2, i32 0, i32 1"
  "\n  store i32 3, i32* %3"
  "\n  %4 = getelementptr %systems.vos.wisey.compiler.tests.MShape, %systems.vos.wisey.compiler.tests.MShape* %2, i32 0, i32 2"
  "\n  store i32 5, i32* %4"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MShape* %2 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 0"
  "\n  %7 = bitcast i8* %6 to i32 (...)***"
  "\n  %8 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.MShape.vtable, i32 0, i32 0, i32 0"
  "\n  %9 = bitcast i8** %8 to i32 (...)**"
  "\n  store i32 (...)** %9, i32 (...)*** %7"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* %2, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n}"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(HeapBuilderTest, allocateInvalidBuilderArgumentsDeathTest) {
  string argumentSpecifier1("width");
  BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withHeight");
  BuilderArgument *argument2 = new BuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  BuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  mHeapBuilder = new HeapBuilder(mModelTypeSpecifier, argumentList, 1);

  const char* expected =
  "/tmp/source.yz(1): Error: Object builder argument should start with 'with'. e.g. .withField(value).\n";
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mHeapBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ(expected, buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(HeapBuilderTest, allocateIncorrectArgumentTypeDeathTest) {
  Value* fieldValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
  ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(fieldValue));
  ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  string argumentSpecifier1("withWidth");
  BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withHeight");
  BuilderArgument *argument2 = new BuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  BuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  mHeapBuilder = new HeapBuilder(mModelTypeSpecifier, argumentList, 3);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mHeapBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Model builder argument value for field mHeight does not match its type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(HeapBuilderTest, allocateNotAllFieldsAreSetDeathTest) {
  string argumentSpecifier1("withWidth");
  BuilderArgument *argument1 = new BuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  BuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  mHeapBuilder = new HeapBuilder(mModelTypeSpecifier, argumentList, 7);

  const char* expected =
  "/tmp/source.yz(7): Error: Field mHeight of object systems.vos.wisey.compiler.tests.MShape is not initialized.\n";
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mHeapBuilder->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ(expected, buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
