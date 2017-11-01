//
//  TestNodeOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NodeOwner}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/NodeOwner.hpp"
#include "wisey/NodeTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NodeOwnerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mComplicatedNode;
  Node* mSimpleNode;
  Node* mSimplerNode;
  Interface* mComplicatedElementInterface;
  Interface* mElementInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
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
  
  NodeOwnerTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::run(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    string elementInterfaceFullName = "systems.vos.wisey.compiler.tests.IElement";
    StructType* elementInterfaceStructType = StructType::create(mLLVMContext,
                                                                elementInterfaceFullName);
    VariableList elementInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> elementObjectElements;
    vector<ModelTypeSpecifier*> elementThrownExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    IObjectElementDeclaration* getElementSignature =
    new MethodSignatureDeclaration(intSpecifier,
                                   "getElement",
                                   elementInterfaceMethodArguments,
                                   elementThrownExceptions);
    elementObjectElements.push_back(getElementSignature);
    vector<InterfaceTypeSpecifier*> elementParentInterfaces;
    mElementInterface = Interface::newInterface(elementInterfaceFullName,
                                                elementInterfaceStructType,
                                                elementParentInterfaces,
                                                elementObjectElements);
    mContext.addInterface(mElementInterface);
    mElementInterface->buildMethods(mContext);
    
    string complicatedElementFullName = "systems.vos.wisey.compiler.tests.IComplicatedElement";
    StructType* complicatedElementIinterfaceStructType =
    StructType::create(mLLVMContext, complicatedElementFullName);
    vector<IObjectElementDeclaration*> complicatedElementObjectElements;
    complicatedElementObjectElements.push_back(getElementSignature);
    vector<InterfaceTypeSpecifier*> complicatedElementParentInterfaces;
    vector<string> package;
    InterfaceTypeSpecifier* elementInterfaceTypeSpecifier =
    new InterfaceTypeSpecifier(package, "IElement");
    complicatedElementParentInterfaces.push_back(elementInterfaceTypeSpecifier);
    mComplicatedElementInterface = Interface::newInterface(complicatedElementFullName,
                                                           complicatedElementIinterfaceStructType,
                                                           complicatedElementParentInterfaces,
                                                           complicatedElementObjectElements);
    mContext.addInterface(mComplicatedElementInterface);
    mComplicatedElementInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> objectElements;
    vector<ModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceMethodArguments,
                                     objectThrownExceptions);
    objectElements.push_back(methodBarSignature);
    vector<InterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectElements);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string complicatedNodeFullName = "systems.vos.wisey.compiler.tests.NComplicatedNode";
    mStructType = StructType::create(mLLVMContext, complicatedNodeFullName);
    mStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    mLeftField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mLeft", fieldArguments);
    mRightField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mRight", fieldArguments);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    IMethod* method = new Method("getElement",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT_TYPE,
                                 methodArguments,
                                 thrownExceptions,
                                 NULL);
    vector<IMethod*> methods;
    methods.push_back(method);
    IMethod* fooMethod = new Method("foo",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mComplicatedElementInterface);
    interfaces.push_back(mObjectInterface);
    
    mComplicatedNode = Node::newNode(complicatedNodeFullName, mStructType);
    mComplicatedNode->setFields(fields, interfaces.size() + 1);
    mComplicatedNode->setMethods(methods);
    mComplicatedNode->setInterfaces(interfaces);
    
    vector<Type*> simpleNodeTypes;
    simpleNodeTypes.push_back(Type::getInt64Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simpleNodeFullName = "systems.vos.wisey.compiler.tests.NSimpleNode";
    StructType* simpleNodeStructType = StructType::create(mLLVMContext, simpleNodeFullName);
    simpleNodeStructType->setBody(simpleNodeTypes);
    vector<Field*> simpleNodeFields;
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         PrimitiveTypes::INT_TYPE,
                                         "mLeft",
                                          fieldArguments));
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         PrimitiveTypes::INT_TYPE,
                                         "mRight",
                                         fieldArguments));
    mAreaField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mArea", fieldArguments);
    simpleNodeFields.push_back(mAreaField);
    mSimpleNode = Node::newNode(simpleNodeFullName, simpleNodeStructType);
    mSimpleNode->setFields(simpleNodeFields, 1u);
    mContext.addNode(mSimpleNode);
    
    vector<Type*> simplerNodeTypes;
    simplerNodeTypes.push_back(Type::getInt64Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simplerNodeFullName = "systems.vos.wisey.compiler.tests.NSimplerNode";
    StructType* simplerNodeStructType = StructType::create(mLLVMContext, simplerNodeFullName);
    simplerNodeStructType->setBody(simplerNodeTypes);
    vector<Field*> simplerNodeFields;
    simplerNodeFields.push_back(new Field(FIXED_FIELD,
                                          PrimitiveTypes::INT_TYPE,
                                          "mLeft",
                                          fieldArguments));
    simplerNodeFields.push_back(new Field(FIXED_FIELD,
                                          PrimitiveTypes::INT_TYPE,
                                          "mRight",
                                          fieldArguments));
    mSimplerNode = Node::newNode(simplerNodeFullName, simplerNodeStructType);
    mSimplerNode->setFields(simplerNodeFields, 1u);
    mContext.addNode(mSimplerNode);
    
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<InterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDeclaration*> vehicleObjectElements;
    mVehicleInterface = Interface::newInterface(vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleObjectElements);
    
    IConcreteObjectType::generateNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateVTable(mContext, mSimpleNode);
    
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    
    mVehicleInterface->buildMethods(mContext);
    
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
  
  ~NodeOwnerTest() {
    delete mField1Expression;
    delete mField2Expression;
  }
};

TEST_F(NodeOwnerTest, getObjectTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getObject(), mComplicatedNode);
}

TEST_F(NodeOwnerTest, getNameTest) {
  EXPECT_STREQ(mComplicatedNode->getOwner()->getName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode*");
}

TEST_F(NodeOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getLLVMType(mLLVMContext),
            mComplicatedNode->getLLVMType(mLLVMContext)->getPointerElementType());
}

TEST_F(NodeOwnerTest, getTypeKindTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getTypeKind(), NODE_OWNER_TYPE);
}

TEST_F(NodeOwnerTest, canCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mElementInterface));
}

TEST_F(NodeOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mElementInterface));
}

TEST_F(NodeOwnerTest, castToFirstInterfaceTest) {
  PointerType* type = mComplicatedNode->getOwner()->getLLVMType(mLLVMContext)->getPointerTo();
  ConstantPointerNull* pointer = ConstantPointerNull::get(type);
  mComplicatedNode->getOwner()->castTo(mContext, pointer, mComplicatedElementInterface->getOwner());

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NComplicatedNode*, "
  "%systems.vos.wisey.compiler.tests.NComplicatedNode** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* %0 to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 8"
  "\n  %3 = alloca %systems.vos.wisey.compiler.tests.IComplicatedElement*"
  "\n  %4 = bitcast i8* %2 to %systems.vos.wisey.compiler.tests.IComplicatedElement*"
  "\n  store %systems.vos.wisey.compiler.tests.IComplicatedElement* %4, "
  "%systems.vos.wisey.compiler.tests.IComplicatedElement** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mComplicatedNode->getLLVMType(mLLVMContext));

  mComplicatedNode->getOwner()->castTo(mContext, pointer, mElementInterface->getOwner());
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NComplicatedNode*, "
  "%systems.vos.wisey.compiler.tests.NComplicatedNode** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* %0 to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 16"
  "\n  %3 = alloca %systems.vos.wisey.compiler.tests.IElement*"
  "\n  %4 = bitcast i8* %2 to %systems.vos.wisey.compiler.tests.IElement*"
  "\n  store %systems.vos.wisey.compiler.tests.IElement* %4, "
  "%systems.vos.wisey.compiler.tests.IElement** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
