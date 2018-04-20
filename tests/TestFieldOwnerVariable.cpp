//
//  TestFieldOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldOwnerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldOwnerVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mObject;
  Node* mNode;
  Interface* mInterface;
  BasicBlock* mBasicBlock;
  FieldOwnerVariable* mFieldOwnerVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements,
                                         0);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string modelFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS, modelFullName, modelStructType, 0);
    mNode->setInterfaces(interfaces);
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(mNode->getOwner()->getLLVMType(mContext));
    types.push_back(mInterface->getOwner()->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.NObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mNode->getOwner(), "foo", 0));
    fields.push_back(new StateField(mInterface->getOwner(), "bar", 0));
    mObject = Node::newNode(AccessLevel::PUBLIC_ACCESS, objectFullName, objectStructType, 0);
    mObject->setFields(mContext, fields, 1u);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mFieldOwnerVariable = new FieldOwnerVariable("foo", mObject);
    
    vector<Type*> argumentTypes;
    argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    FunctionType* destructorFunctionType = FunctionType::get(Type::getVoidTy(mLLVMContext),
                                                             argumentTypes,
                                                             false);
    Function::Create(destructorFunctionType,
                     GlobalValue::InternalLinkage,
                     mNode->getTypeName() + ".destructor",
                     mContext.getModule());

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldOwnerVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldOwnerVariable->getName().c_str());
  EXPECT_EQ(mNode->getOwner(), mFieldOwnerVariable->getType());
  EXPECT_TRUE(mFieldOwnerVariable->isField());
  EXPECT_FALSE(mFieldOwnerVariable->isSystem());
}

TEST_F(FieldOwnerVariableTest, generateIdentifierIRTest) {
  mFieldOwnerVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, generateIdentifierReferenceIRTest) {
  mFieldOwnerVariable->generateIdentifierReferenceIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, generateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  llvm::PointerType* llvmType = (llvm::PointerType*) mNode->getOwner()->getLLVMType(mContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  mFieldOwnerVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.NNode* %1 to i8*"
  "\n  call void @__destroyObjectOwnerFunction(i8* %2)"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, "
  "%systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, generateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  llvm::PointerType* llvmType = mNode->getOwner()->getLLVMType(mContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  FieldOwnerVariable* ownerFieldVariable = new FieldOwnerVariable("bar", mObject);
  ownerFieldVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IInterface*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 2"
  "\n  %4 = load %systems.vos.wisey.compiler.tests.IInterface*, %systems.vos.wisey.compiler.tests.IInterface** %3"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %4 to i8*"
  "\n  call void @__destroyObjectOwnerFunction(i8* %5)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %2, %systems.vos.wisey.compiler.tests.IInterface** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, setToNullTest) {
  mFieldOwnerVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, %systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, variableSetToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_object_field_variable_set_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_object_field_variable_set_to_null.yz:20)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, stateOwnerFieldSetToNullTest) {
  runFile("tests/samples/test_state_owner_field_set_to_null.yz", "1");
}

TEST_F(TestFileRunner, objectFieldVariableSetToAnotherTest) {
  runFile("tests/samples/test_object_field_variable_set_to_another.yz", "7");
}

TEST_F(TestFileRunner, assignFieldOwnerToParameterOwnerCompileTest) {
  compileFile("tests/samples/test_assign_field_owner_to_parameter_owner.yz");
}

TEST_F(TestFileRunner, assignFieldOwnerToNullCompileTest) {
  compileFile("tests/samples/test_assign_field_owner_to_null.yz");
}

TEST_F(TestFileRunner, objectFieldVariableNullComplicatedTest) {
  compileAndRunFileCheckOutput("tests/samples/test_object_field_variable_null_complicated.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.getValue(tests/samples/test_object_field_variable_null_complicated.yz:26)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_object_field_variable_null_complicated.yz:36)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, destructorCalledOnAssignFieldOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_destructor_called_on_assign_field_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "car is destoyed\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, fieldPointerOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_field_pointer_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "destructor systems.vos.wisey.compiler.tests.MModel\n",
                                        "");
}
