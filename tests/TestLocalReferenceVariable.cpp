//
//  TestLocalReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalReferenceVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestPrefix.hpp"
#include "TestFileRunner.hpp"
#include "FakeExpression.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalReferenceVariable.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ThreadExpression.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  Function* mFunction;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
 
public:
  
  LocalReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();

    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "height", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LocalReferenceVariableTest() {
  }
};

TEST_F(LocalReferenceVariableTest, basicFieldsTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalReferenceVariable("foo", mModel, fooValue, 0);

  EXPECT_STREQ("foo", variable->getName().c_str());
  EXPECT_EQ(mModel, variable->getType());
  EXPECT_FALSE(variable->isField());
  EXPECT_FALSE(variable->isStatic());
}

TEST_F(LocalReferenceVariableTest, localReferenceVariableAssignmentTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  
  IVariable* uninitializedHeapVariable = new LocalReferenceVariable("foo", mModel, fooValue, 0);
  mContext.getScopes().setVariable(mContext, uninitializedHeapVariable);
  Value* barValue = ConstantPointerNull::get((llvm::PointerType*) llvmType);
  Value* referenceStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("bar", mModel, referenceStore, 0);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(barValue));
  vector<const IExpression*> arrayIndices;
  
  uninitializedHeapVariable->generateAssignmentIR(mContext, &expression, arrayIndices, 0);

  *mStringStream << *mFunction;
  
  string expected =
  "\ndefine internal i32 @test() {"
  "\ndeclare:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %3 = icmp eq %systems.vos.wisey.compiler.tests.MShape* %2, null"
  "\n  br i1 %3, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %4 = icmp eq %systems.vos.wisey.compiler.tests.MShape* null, null"
  "\n  br i1 %4, label %if.end1, label %if.notnull2"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MShape* %2 to i64*"
  "\n  %6 = getelementptr i64, i64* %5, i64 -1"
  "\n  %7 = atomicrmw add i64* %6, i64 -1 monotonic"
  "\n  br label %if.end"
  "\n"
  "\nif.end1:                                          ; preds = %if.notnull2, %if.end"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n"
  "\nif.notnull2:                                      ; preds = %if.end"
  "\n  %8 = bitcast %systems.vos.wisey.compiler.tests.MShape* null to i64*"
  "\n  %9 = getelementptr i64, i64* %8, i64 -1"
  "\n  %10 = atomicrmw add i64* %9, i64 1 monotonic"
  "\n  br label %if.end1"
  "\n}\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, generateIdentifierIRTest) {
  llvm::PointerType* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("foo", mModel, fooValue, 0);
  llvm::Constant* null = ConstantPointerNull::get(llvmType);
  FakeExpression* fakeExpression = new FakeExpression(null, mModel);
  vector<const IExpression*> arrayIndices;
  localReferenceVariable.generateAssignmentIR(mContext, fakeExpression, arrayIndices, 0);
  
  Value* instruction = localReferenceVariable.generateIdentifierIR(mContext, 0);
  
  *mStringStream << *instruction;
  string expected =
  "  %7 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, generateIdentifierReferenceIRTest) {
  llvm::PointerType* llvmType = mModel->getLLVMType(mContext);
  Value* fooValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("foo", mModel, fooValueStore, 0);
  
  EXPECT_EQ(fooValueStore, localReferenceVariable.generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(LocalReferenceVariableTest, decrementReferenceCounterTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  
  Value* referenceStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("bar", mModel, referenceStore, 0);
  
  localReferenceVariable.decrementReferenceCounter(mContext);
  
  *mStringStream << *mFunction;
  
  string expected =
  "\ndefine internal i32 @test() {"
  "\ndeclare:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %2 = icmp eq %systems.vos.wisey.compiler.tests.MShape* %1, null"
  "\n  br i1 %2, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MShape* %1 to i64*"
  "\n  %4 = getelementptr i64, i64* %3, i64 -1"
  "\n  %5 = atomicrmw add i64* %4, i64 -1 monotonic"
  "\n  br label %if.end"
  "\n}"
  "\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, localReferenceVariableIdentifierUninitializedDeathTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("foo", mModel, fooValue, 0);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(localReferenceVariable.generateIdentifierIR(mContext, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Variable 'foo' is used before it is initialized\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(LocalReferenceVariableTest, setToNullTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("foo", mModel, fooValue, 0);
  localReferenceVariable.setToNull(mContext);
  
  localReferenceVariable.generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, headReferenceVariableAssignmentRunTest) {
  runFile("tests/samples/test_assignment_model_variable.yz", 0);
}

TEST_F(TestFileRunner, interfaceVariableAssignmentRunTest) {
  runFile("tests/samples/test_interface_variable_assignment.yz", 25);
}

TEST_F(TestFileRunner, assignLocalReferenceToFieldOwnerCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_field_owner.yz");
}

TEST_F(TestFileRunner, assignLocalReferenceToFieldReferenceCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_field_reference.yz");
}

TEST_F(TestFileRunner, assignLocalReferenceToLocalOwnerCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_local_owner.yz");
}

TEST_F(TestFileRunner, assignLocalReferenceToLocalReferenceCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_local_reference.yz");
}

TEST_F(TestFileRunner, assignLocalReferenceToNullCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_null.yz");
}

TEST_F(TestFileRunner, usingUninitializedLocalReferenceVariableRunDeathTest) {
  expectFailCompile("tests/samples/test_heap_reference_variable_not_initialized.yz",
                    1,
                    "Error: Variable 'color' is used before it is initialized");
}

TEST_F(TestFileRunner, incompatableHeapVariableTypesInAssignmentRunDeathTest) {
  expectFailCompile("tests/samples/test_incompatible_heap_variable_types_in_assignment.yz",
                    1,
                    "tests/samples/test_incompatible_heap_variable_types_in_assignment.yz\\(17\\): "
                    "Error: Incompatible types: can not cast from type "
                    "systems.vos.wisey.compiler.tests.MShape\\* to "
                    "systems.vos.wisey.compiler.tests.MColor\\*");
}
