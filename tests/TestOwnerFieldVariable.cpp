//
//  TestOwnerFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link OwnerFieldVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/OwnerFieldVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct OwnerFieldVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mObject;
  Node* mNode;
  Interface* mInterface;
  BasicBlock* mBasicBlock;
  Value* mOwnerFieldValue;
  OwnerFieldVariable* mOwnerFieldVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  OwnerFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<MethodSignatureDeclaration*> interfaceMethods;
    mInterface = Interface::newInterface(interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceMethods);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string modelFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mNode = Node::newNode(modelFullName, modelStructType);
    mNode->setInterfaces(interfaces);
    
    vector<Type*> types;
    types.push_back(mNode->getOwner()->getLLVMType(mLLVMContext));
    types.push_back(mInterface->getOwner()->getLLVMType(mLLVMContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.NObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(STATE_FIELD, mNode->getOwner(), "foo", 0, fieldArguments));
    fields.push_back(new Field(STATE_FIELD, mInterface->getOwner(), "bar", 1, fieldArguments));
    mObject = Node::newNode(objectFullName, objectStructType);
    mObject->setFields(fields);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mLLVMContext));
    IVariable* thisVariable = new HeapReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mOwnerFieldValue = ConstantPointerNull::get(mNode->getLLVMType(mLLVMContext));
    mOwnerFieldVariable = new OwnerFieldVariable("foo", mOwnerFieldValue, mObject);
    
    vector<Type*> argumentTypes;
    argumentTypes.push_back(mNode->getLLVMType(mLLVMContext));
    ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
    FunctionType* destructorFunctionType = FunctionType::get(Type::getVoidTy(mLLVMContext),
                                                             argTypesArray,
                                                             false);
    Function::Create(destructorFunctionType,
                     GlobalValue::InternalLinkage,
                     "destructor." + mNode->getName(),
                     mContext.getModule());

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~OwnerFieldVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(OwnerFieldVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mOwnerFieldVariable->getName().c_str(), "foo");
  EXPECT_EQ(mOwnerFieldVariable->getType(), mNode->getOwner());
  EXPECT_EQ(mOwnerFieldVariable->getValue(), mOwnerFieldValue);
}

TEST_F(OwnerFieldVariableTest, ownerFieldVariableGenerateIdentifierIRTest) {
  mOwnerFieldVariable->generateIdentifierIR(mContext, "test");
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NObject*, "
  "%systems.vos.wisey.compiler.tests.NObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* %0, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, ownerFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = (PointerType*) mNode->getOwner()->getLLVMType(mLLVMContext)
    ->getPointerTo();
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mOwnerFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NObject*, "
  "%systems.vos.wisey.compiler.tests.NObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* %0, i32 0, i32 0"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.NNode("
  "%systems.vos.wisey.compiler.tests.NNode** %1)"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.NNode*, "
  "%systems.vos.wisey.compiler.tests.NNode** null"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* %2, "
  "%systems.vos.wisey.compiler.tests.NNode** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, ownerFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = mNode->getOwner()->getLLVMType(mLLVMContext)->getPointerTo();
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  Value* ownerFieldValue =
    ConstantPointerNull::get(mInterface->getOwner()->getLLVMType(mLLVMContext));
  OwnerFieldVariable* ownerFieldVariable =
    new OwnerFieldVariable("bar", ownerFieldValue, mObject);
  ownerFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NNode** null "
  "to %systems.vos.wisey.compiler.tests.IInterface**"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NObject*, "
  "%systems.vos.wisey.compiler.tests.NObject** null"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* %1, i32 0, i32 1"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.IInterface("
  "%systems.vos.wisey.compiler.tests.IInterface** %2)"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %0"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %3, "
  "%systems.vos.wisey.compiler.tests.IInterface** %2\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, setToNullTest) {
  mOwnerFieldVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NObject*, "
  "%systems.vos.wisey.compiler.tests.NObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* %0, i32 0, i32 0"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, "
  "%systems.vos.wisey.compiler.tests.NNode** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, existsInOuterScopeTest) {
  EXPECT_TRUE(mOwnerFieldVariable->existsInOuterScope());
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToNullTest) {
  expectFailCompile("tests/samples/test_object_field_variable_set_to_null.yz",
                    1,
                    "Error: Variable 'mModel' was previously cleared and can not be used");
}

TEST_F(TestFileSampleRunner, stateOwnerFieldSetToNullTest) {
  runFile("tests/samples/test_state_owner_field_set_to_null.yz", "1");
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToAnotherTest) {
  runFile("tests/samples/test_object_field_variable_set_to_another.yz", "7");
}

TEST_F(TestFileSampleRunner, objectFieldVariableNullComplicatedTest) {
  compileAndRunFileCheckOutput("tests/samples/test_object_field_variable_null_complicated.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.getValue(tests/samples/test_object_field_variable_null_complicated.yz:26)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_object_field_variable_null_complicated.yz:36)\n"
                               "  at wisey.lang.CProgramRunner.run(wisey/lang/CProgramRunner.yz:13)\n");
}

TEST_F(TestFileSampleRunner, destructorCalledOnAssignFieldOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_destructor_called_on_assign_field_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "car is destoyed\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
