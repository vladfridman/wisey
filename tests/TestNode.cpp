//
//  TestNode.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Node}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"

#include "wisey/IRWriter.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/Node.hpp"
#include "wisey/NodeTypeSpecifier.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NodeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mComplicatedNode;
  Node* mSimpleNode;
  Node* mSimplerNode;
  Interface* mComplicatedElementInterface;
  Interface* mElementInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
  Method* mMethod;
  StructType* mStructType;
  Field* mLeftField;
  Field* mRightField;
  Field* mAreaField;
  BasicBlock* mBasicBlock;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;

  NodeTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    vector<Type*> elementInterfaceTypes;
    string elementInterfaceFullName = "systems.vos.wisey.compiler.tests.IElement";
    StructType* elementInterfaceStructType = StructType::create(mLLVMContext,
                                                                elementInterfaceFullName);
    elementInterfaceStructType->setBody(elementInterfaceTypes);
    vector<MethodArgument*> elementInterfaceMethodArguments;
    vector<MethodSignature*> elementInterfaceMethods;
    vector<const Model*> elementThrownExceptions;
    MethodSignature* getElementSignature = new MethodSignature("getElement",
                                                               AccessLevel::PUBLIC_ACCESS,
                                                               PrimitiveTypes::INT_TYPE,
                                                               elementInterfaceMethodArguments,
                                                               elementThrownExceptions,
                                                               0);
    elementInterfaceMethods.push_back(getElementSignature);
    vector<Interface*> elementParentInterfaces;
    mElementInterface = new Interface(elementInterfaceFullName, elementInterfaceStructType);
    mElementInterface->setParentInterfacesAndMethodSignatures(elementParentInterfaces,
                                                              elementInterfaceMethods);
    
    vector<Type*> complicatedElementInterfaceTypes;
    string complicatedElementFullName = "systems.vos.wisey.compiler.tests.IComplicatedElement";
    StructType* complicatedElementIinterfaceStructType =
    StructType::create(mLLVMContext, complicatedElementFullName);
    complicatedElementIinterfaceStructType->setBody(complicatedElementInterfaceTypes);
    vector<MethodSignature*> complicatedElementInterfaceMethods;
    complicatedElementInterfaceMethods.push_back(getElementSignature);
    vector<Interface*> complicatedElementParentInterfaces;
    complicatedElementParentInterfaces.push_back(mElementInterface);
    mComplicatedElementInterface = new Interface(complicatedElementFullName,
                                                 complicatedElementIinterfaceStructType);
    mComplicatedElementInterface->
    setParentInterfacesAndMethodSignatures(complicatedElementParentInterfaces,
                                           complicatedElementInterfaceMethods);
    
    vector<Type*> objectInterfaceTypes;
    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    objectInterfaceStructType->setBody(objectInterfaceTypes);
    vector<MethodArgument*> objectInterfaceMethodArguments;
    vector<MethodSignature*> objectInterfaceMethods;
    vector<const Model*> objectThrownExceptions;
    MethodSignature* methodBarSignature = new MethodSignature("foo",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
                                                              objectThrownExceptions,
                                                              0);
    objectInterfaceMethods.push_back(methodBarSignature);
    mObjectInterface = new Interface(objectFullName, objectInterfaceStructType);
    vector<Interface*> objectParentInterfaces;
    mObjectInterface->setParentInterfacesAndMethodSignatures(objectParentInterfaces,
                                                             objectInterfaceMethods);
    
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string complicatedNodeFullName = "systems.vos.wisey.compiler.tests.NComplicatedNode";
    mStructType = StructType::create(mLLVMContext, complicatedNodeFullName);
    mStructType->setBody(types);
    vector<Field*> fixedFields;
    vector<Field*> stateFields;
    ExpressionList fieldArguments;
    mLeftField = new Field(PrimitiveTypes::INT_TYPE, "mLeft", 0, fieldArguments);
    mRightField = new Field(PrimitiveTypes::INT_TYPE, "mRight", 1, fieldArguments);
    fixedFields.push_back(mLeftField);
    fixedFields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method("getElement",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL,
                         0);
    vector<Method*> methods;
    methods.push_back(mMethod);
    Method* fooMethod = new Method("foo",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL,
                                   1);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mComplicatedElementInterface);
    interfaces.push_back(mObjectInterface);
    
    mComplicatedNode = new Node(complicatedNodeFullName, mStructType);
    mComplicatedNode->setFields(fixedFields, stateFields);
    mComplicatedNode->setMethods(methods);
    mComplicatedNode->setInterfaces(interfaces);
    
    vector<Type*> simpleNodeTypes;
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simpleNodeFullName = "systems.vos.wisey.compiler.tests.NSimpleNode";
    StructType* simpleNodeStructType = StructType::create(mLLVMContext, simpleNodeFullName);
    simpleNodeStructType->setBody(simpleNodeTypes);
    vector<Field*> simpleNodeFixedFields;
    vector<Field*> simpleNodeStateFields;
    simpleNodeFixedFields.push_back(new Field(PrimitiveTypes::INT_TYPE,
                                              "mLeft",
                                              0,
                                              fieldArguments));
    simpleNodeFixedFields.push_back(new Field(PrimitiveTypes::INT_TYPE,
                                              "mRight",
                                              1,
                                              fieldArguments));
    mAreaField = new Field(PrimitiveTypes::INT_TYPE, "mArea", 2, fieldArguments);
    simpleNodeStateFields.push_back(mAreaField);
    mSimpleNode = new Node(simpleNodeFullName, simpleNodeStructType);
    mSimpleNode->setFields(simpleNodeFixedFields, simpleNodeStateFields);
    mContext.addNode(mSimpleNode);
    
    vector<Type*> simplerNodeTypes;
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simplerNodeFullName = "systems.vos.wisey.compiler.tests.NSimplerNode";
    StructType* simplerNodeStructType = StructType::create(mLLVMContext, simplerNodeFullName);
    simplerNodeStructType->setBody(simplerNodeTypes);
    vector<Field*> simplerNodeFixedFields;
    vector<Field*> simplerNodeStateFields;
    simplerNodeStateFields.push_back(new Field(PrimitiveTypes::INT_TYPE,
                                               "mLeft",
                                               0,
                                               fieldArguments));
    simplerNodeStateFields.push_back(new Field(PrimitiveTypes::INT_TYPE,
                                               "mRight",
                                               1,
                                               fieldArguments));
    mSimplerNode = new Node(simplerNodeFullName, simplerNodeStructType);
    mSimplerNode->setFields(simplerNodeFixedFields, simplerNodeStateFields);
    mContext.addNode(mSimplerNode);
    
    vector<Type*> vehicleInterfaceTypes;
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vehicleInterfaceStructType->setBody(vehicleInterfaceTypes);
    mVehicleInterface = new Interface(vehicleFullName, vehicleInterfaceStructType);
    
    IConcreteObjectType::generateNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateVTable(mContext, mSimpleNode);
    
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~NodeTest() {
    delete mStringStream;
    delete mField1Expression;
    delete mField2Expression;
  }
};

TEST_F(NodeTest, testGetOwner) {
  ASSERT_NE(mComplicatedNode->getOwner(), nullptr);
  EXPECT_EQ(mComplicatedNode->getOwner()->getObject(), mComplicatedNode);
}

TEST_F(NodeTest, getNameTest) {
  EXPECT_STREQ(mComplicatedNode->getName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode");
}

TEST_F(NodeTest, getShortNameTest) {
  EXPECT_STREQ(mComplicatedNode->getShortName().c_str(), "NComplicatedNode");
}

TEST_F(NodeTest, getVTableNameTest) {
  EXPECT_STREQ(mComplicatedNode->getVTableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.vtable");
}

TEST_F(NodeTest, getTypeKindTest) {
  EXPECT_EQ(mComplicatedNode->getTypeKind(), NODE_TYPE);
}

TEST_F(NodeTest, getLLVMTypeTest) {
  EXPECT_EQ(mComplicatedNode->getLLVMType(mLLVMContext), mStructType->getPointerTo());
}

TEST_F(NodeTest, getInterfacesTest) {
  EXPECT_EQ(mComplicatedNode->getInterfaces().size(), 2u);
}

TEST_F(NodeTest, getVTableSizeTest) {
  EXPECT_EQ(mComplicatedNode->getVTableSize(), 3u);
}

TEST_F(NodeTest, getFieldsTest) {
  EXPECT_EQ(mComplicatedNode->getFields().size(), 2u);
}

TEST_F(NodeTest, findFeildTest) {
  EXPECT_EQ(mComplicatedNode->findField("mLeft"), mLeftField);
  EXPECT_EQ(mComplicatedNode->findField("mRight"), mRightField);
  EXPECT_EQ(mComplicatedNode->findField("mDepth"), nullptr);
}

TEST_F(NodeTest, findMethodTest) {
  EXPECT_EQ(mComplicatedNode->findMethod("getElement"), mMethod);
  EXPECT_EQ(mComplicatedNode->findMethod("bar"), nullptr);
}

TEST_F(NodeTest, methodIndexesTest) {
  EXPECT_EQ(mComplicatedNode->findMethod("getElement")->getIndex(), 0u);
  EXPECT_EQ(mComplicatedNode->findMethod("foo")->getIndex(), 1u);
}

TEST_F(NodeTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mComplicatedNode->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.name");
}

TEST_F(NodeTest, getTypeTableNameTest) {
  ASSERT_STREQ(mComplicatedNode->getTypeTableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.typetable");
}

TEST_F(NodeTest, canCastToTest) {
  EXPECT_FALSE(mComplicatedNode->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mElementInterface));
  EXPECT_FALSE(mComplicatedNode->canCastTo(NullType::NULL_TYPE));
}

TEST_F(NodeTest, canAutoCastToTest) {
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mElementInterface));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(NullType::NULL_TYPE));
}

TEST_F(NodeTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mLLVMContext));
  mComplicatedNode->castTo(mContext, pointer, mComplicatedElementInterface);
  ASSERT_EQ(mBasicBlock->size(), 1u);
  
  *mStringStream << *mBasicBlock->begin();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to "
               "%systems.vos.wisey.compiler.tests.IComplicatedElement*");
  mStringBuffer.clear();
}

TEST_F(NodeTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mLLVMContext));
  mComplicatedNode->castTo(mContext, pointer, mElementInterface);
  ASSERT_EQ(mBasicBlock->size(), 3u);
  
  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %1 = getelementptr i8, i8* %0, i64 8");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IElement*");
  mStringBuffer.clear();
}

TEST_F(NodeTest, getFlattenedInterfaceHierarchyTest) {
  vector<Interface*> allInterfaces = mComplicatedNode->getFlattenedInterfaceHierarchy();
  
  EXPECT_EQ(allInterfaces.size(), 3u);
  EXPECT_EQ(allInterfaces.at(0), mComplicatedElementInterface);
  EXPECT_EQ(allInterfaces.at(1), mElementInterface);
  EXPECT_EQ(allInterfaces.at(2), mObjectInterface);
}

TEST_F(NodeTest, getMissingFieldsTest) {
  set<string> givenFields;
  givenFields.insert("mLeft");
  
  vector<string> missingFields = mComplicatedNode->getMissingFields(givenFields);
  
  ASSERT_EQ(missingFields.size(), 1u);
  EXPECT_EQ(missingFields.at(0), "mRight");
}

TEST_F(NodeTest, buildTest) {
  string argumentSpecifier1("withLeft");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withRight");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  Value* result = mSimpleNode->build(mContext, argumentList);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  ASSERT_EQ(8ul, mBasicBlock->size());
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %malloccall = tail call i8* @malloc(i64 mul nuw (i64 ptrtoint "
  "(i32* getelementptr (i32, i32* null, i32 1) to i64), i64 3))"
  "\n  %buildervar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.NSimpleNode*"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode, "
  "%systems.vos.wisey.compiler.tests.NSimpleNode* %buildervar, i32 0, i32 0"
  "\n  store i32 3, i32* %0"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode, "
  "%systems.vos.wisey.compiler.tests.NSimpleNode* %buildervar, i32 0, i32 1"
  "\n  store i32 5, i32* %1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode, "
  "%systems.vos.wisey.compiler.tests.NSimpleNode* %buildervar, i32 0, i32 2"
  "\n  store i32 0, i32* %2\n";
  
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, buildInvalidObjectBuilderArgumentsDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  
  string argumentSpecifier1("left");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withRight");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  const char *expected =
  "Error: Object builder argument should start with 'with'. e.g. .withField\\(value\\)."
  "\nError: Some arguments for the node systems.vos.wisey.compiler.tests.NSimpleNode "
  "builder are not well formed";
  
  EXPECT_EXIT(mSimpleNode->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(NodeTest, buildIncorrectArgumentTypeDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  
  Value* fieldValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
  ON_CALL(*mField2Expression, generateIR(_)).WillByDefault(Return(fieldValue));
  ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));

  string argumentSpecifier1("withLeft");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withRight");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  EXPECT_EXIT(mSimpleNode->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              "Error: Node builder argument value for field mRight does not match its type");
}

TEST_F(NodeTest, buildNotAllFieldsAreSetDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  
  string argumentSpecifier1("withRight");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  
  const char *expected =
  "Error: Field mLeft of the node systems.vos.wisey.compiler.tests.NSimpleNode is not initialized."
  "\nError: Some fields of the node systems.vos.wisey.compiler.tests.NSimpleNode "
  "are not initialized.";
  
  EXPECT_EXIT(mSimpleNode->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(TestFileSampleRunner, linkListRunTest) {
  runFile("tests/samples/test_linklist.yz", "9");
}

TEST_F(TestFileSampleRunner, linkListReverseRunTest) {
  runFile("tests/samples/test_linklist_reverse.yz", "7");
}
