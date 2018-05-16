//
//  TestLocalOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalOwnerVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
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
    fields.push_back(new FixedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT, "height", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);
    
    IConcreteObjectType::declareTypeNameGlobal(mContext, mModel);
    IConcreteObjectType::declareVTable(mContext, mModel);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(LocalOwnerVariableTest, basicFieldsTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalOwnerVariable("foo", mModel->getOwner(), fooValue, 0);

  EXPECT_STREQ("foo", variable->getName().c_str());
  EXPECT_EQ(mModel->getOwner(), variable->getType());
  EXPECT_FALSE(variable->isField());
  EXPECT_FALSE(variable->isSystem());
}

TEST_F(LocalOwnerVariableTest, generateAssignmentIRTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");

  IVariable* uninitializedHeapVariable =
    new LocalOwnerVariable("foo", mModel->getOwner(), fooValue, 0);
  mContext.getScopes().setVariable(mContext, uninitializedHeapVariable);
  Value* barValue = ConstantPointerNull::get((llvm::PointerType*) llvmType);
  Value* ownerStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalOwnerVariable heapOwnerVariable("bar", mModel->getOwner(), ownerStore, 0);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(barValue));
  vector<const IExpression*> arrayIndices;

  uninitializedHeapVariable->generateAssignmentIR(mContext, &expression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MShape* %2 to i8*"
  "\n  call void @__destroyObjectOwnerFunction(i8* %3)"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, %systems.vos.wisey.compiler.tests.MShape** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalOwnerVariableTest, setToNullTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue, 0);
  
  heapOwnerVariable.setToNull(mContext, 0);

  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, %systems.vos.wisey.compiler.tests.MShape** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalOwnerVariableTest, generateIdentifierIRTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValueStore, 0);
  
  heapOwnerVariable.setToNull(mContext, 0);
  heapOwnerVariable.generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalOwnerVariableTest, generateIdentifierReferenceIRTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValueStore, 0);
  
  heapOwnerVariable.setToNull(mContext, 0);

  EXPECT_EQ(fooValueStore, heapOwnerVariable.generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(LocalOwnerVariableTest, generateIdentifierIRDeathTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue, 0);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(heapOwnerVariable.generateIdentifierIR(mContext, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Variable 'foo' is used before it is initialized\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(LocalOwnerVariableTest, freeTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue, 0);
  
  heapOwnerVariable.free(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MShape* %1 to i8*"
  "\n  call void @__destroyObjectOwnerFunction(i8* %2)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, assignLocalOwnerToFieldOwnerCompileTest) {
  compileFile("tests/samples/test_assign_local_owner_to_field_owner.yz");
}

TEST_F(TestFileRunner, assignLocalOwnerToLocalOwnerCompileTest) {
  compileFile("tests/samples/test_assign_local_owner_to_local_owner.yz");
}

TEST_F(TestFileRunner, assignLocalOwnerToNullCompileTest) {
  compileFile("tests/samples/test_assign_local_owner_to_null.yz");
}

TEST_F(TestFileRunner, usingUninitializedLocalOwnerVariableRunDeathTest) {
  expectFailCompile("tests/samples/test_heap_owner_variable_not_initialized.yz",
                    1,
                    "Error: Variable 'color' is used before it is initialized");
}

TEST_F(TestFileRunner, destructorCalledOnAssignLocalOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_destructor_called_on_assign_heap_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "car is destoyed\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, stringArrayRunTest) {
  runFileCheckOutput("tests/samples/test_string_array.yz",
                     "element 0: zero\n"
                     "element 1: one\n"
                     "element 2: two\n"
                     "element 3: three\n"
                     "element 4: four\n",
                     "");
}

TEST_F(TestFileRunner, localPointerOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_local_pointer_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MModel\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
