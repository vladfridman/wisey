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

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
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
#include "wisey/WiseyModelOwnerType.hpp"
#include "wisey/WiseyModelType.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"
#include "wisey/WiseyObjectType.hpp"

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
  FixedField* mLeftField;
  FixedField* mRightField;
  FixedField* mAreaField;
  BasicBlock* mBasicBlock;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  NodeOwnerTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);

    string elementInterfaceFullName = "systems.vos.wisey.compiler.tests.IElement";
    StructType* elementInterfaceStructType = StructType::create(mLLVMContext,
                                                                elementInterfaceFullName);
    VariableList elementInterfaceArguments;
    vector<IObjectElementDefinition*> elementObjectElements;
    vector<IModelTypeSpecifier*> elementThrownExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
    IObjectElementDefinition* getElementSignature =
    new MethodSignatureDeclaration(intSpecifier,
                                   "getElement",
                                   elementInterfaceArguments,
                                   elementThrownExceptions,
                                   new MethodQualifiers(0),
                                   0);
    elementObjectElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> elementParentInterfaces;
    mElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                elementInterfaceFullName,
                                                elementInterfaceStructType,
                                                elementParentInterfaces,
                                                elementObjectElements,
                                                0);
    mContext.addInterface(mElementInterface);
    mElementInterface->buildMethods(mContext);
    
    string complicatedElementFullName = "systems.vos.wisey.compiler.tests.IComplicatedElement";
    StructType* complicatedElementIinterfaceStructType =
    StructType::create(mLLVMContext, complicatedElementFullName);
    vector<IObjectElementDefinition*> complicatedElementObjectElements;
    MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
    methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::OVERRIDE);
    getElementSignature = new MethodSignatureDeclaration(intSpecifier,
                                                         "getElement",
                                                         elementInterfaceArguments,
                                                         elementThrownExceptions,
                                                         methodQualifiers,
                                                         0);
    complicatedElementObjectElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> complicatedElementParentInterfaces;
    InterfaceTypeSpecifier* elementInterfaceTypeSpecifier = new InterfaceTypeSpecifier(NULL,
                                                                                       "IElement",
                                                                                       0);
    complicatedElementParentInterfaces.push_back(elementInterfaceTypeSpecifier);
    mComplicatedElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                           complicatedElementFullName,
                                                           complicatedElementIinterfaceStructType,
                                                           complicatedElementParentInterfaces,
                                                           complicatedElementObjectElements,
                                                           0);
    mContext.addInterface(mComplicatedElementInterface);
    mComplicatedElementInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceArguments;
    vector<IObjectElementDefinition*> objectElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDefinition* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceArguments,
                                     objectThrownExceptions,
                                     new MethodQualifiers(0),
                                     0);
    objectElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectElements,
                                               0);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string complicatedNodeFullName = "systems.vos.wisey.compiler.tests.NComplicatedNode";
    mStructType = StructType::create(mLLVMContext, complicatedNodeFullName);
    mStructType->setBody(types);
    vector<IField*> fields;
    mLeftField = new FixedField(PrimitiveTypes::INT_TYPE, "mLeft", 0);
    mRightField = new FixedField(PrimitiveTypes::INT_TYPE, "mRight", 0);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<const wisey::Argument*> methodArguments;
    vector<const Model*> thrownExceptions;
    IMethod* method = new Method(mComplicatedNode,
                                 "getElement",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT_TYPE,
                                 methodArguments,
                                 thrownExceptions,
                                 new MethodQualifiers(0),
                                 NULL,
                                 0);
    vector<IMethod*> methods;
    methods.push_back(method);
    IMethod* fooMethod = new Method(mComplicatedNode,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mComplicatedElementInterface);
    interfaces.push_back(mObjectInterface);
    
    mComplicatedNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                     complicatedNodeFullName,
                                     mStructType,
                                     0);
    mComplicatedNode->setFields(mContext, fields, interfaces.size());
    mComplicatedNode->setMethods(methods);
    mComplicatedNode->setInterfaces(interfaces);
    
    vector<Type*> simpleNodeTypes;
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simpleNodeFullName = "systems.vos.wisey.compiler.tests.NSimpleNode";
    StructType* simpleNodeStructType = StructType::create(mLLVMContext, simpleNodeFullName);
    simpleNodeStructType->setBody(simpleNodeTypes);
    vector<IField*> simpleNodeFields;
    simpleNodeFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mLeft", 0));
    simpleNodeFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mRight", 0));
    mAreaField = new FixedField(PrimitiveTypes::INT_TYPE, "mArea", 0);
    simpleNodeFields.push_back(mAreaField);
    mSimpleNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                simpleNodeFullName,
                                simpleNodeStructType,
                                0);
    mSimpleNode->setFields(mContext, simpleNodeFields, 1u);
    mContext.addNode(mSimpleNode);
    
    vector<Type*> simplerNodeTypes;
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simplerNodeFullName = "systems.vos.wisey.compiler.tests.NSimplerNode";
    StructType* simplerNodeStructType = StructType::create(mLLVMContext, simplerNodeFullName);
    simplerNodeStructType->setBody(simplerNodeTypes);
    vector<IField*> simplerNodeFields;
    simplerNodeFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mLeft", 0));
    simplerNodeFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mRight", 0));
    mSimplerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                 simplerNodeFullName,
                                 simplerNodeStructType,
                                 0);
    mSimplerNode->setFields(mContext, simplerNodeFields, 1u);
    mContext.addNode(mSimplerNode);
    IConcreteObjectType::generateNameGlobal(mContext, mSimplerNode);
    IConcreteObjectType::generateShortNameGlobal(mContext, mSimplerNode);
    IConcreteObjectType::generateVTable(mContext, mSimplerNode);

    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDefinition*> vehicleObjectElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleObjectElements,
                                                0);
    
    IConcreteObjectType::generateNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateShortNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateVTable(mContext, mSimpleNode);
    
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
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
  EXPECT_EQ(mComplicatedNode, mComplicatedNode->getOwner()->getReference());
}

TEST_F(NodeOwnerTest, getNameTest) {
  EXPECT_STREQ(mComplicatedNode->getOwner()->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode*");
}

TEST_F(NodeOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getLLVMType(mContext),
            mComplicatedNode->getLLVMType(mContext));
}

TEST_F(NodeOwnerTest, getDestructorFunctionTest) {
  Function* result = mSimplerNode->getOwner()->getDestructorFunction(mContext, 0);
  
  ASSERT_NE(nullptr, result);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  EXPECT_EQ(functionType, result->getFunctionType());
}

TEST_F(NodeOwnerTest, canCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mElementInterface));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->
               canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mComplicatedNode->getOwner()->
              canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(NodeOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mElementInterface));
  EXPECT_FALSE(mComplicatedNode->getOwner()->
               canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->
               canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mComplicatedNode->getOwner()->
              canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mComplicatedNode->getOwner()->
              canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(NodeOwnerTest, castToFirstInterfaceTest) {
  llvm::PointerType* type = mComplicatedNode->getOwner()->getLLVMType(mContext);
  ConstantPointerNull* pointer = ConstantPointerNull::get(type);
  mComplicatedNode->getOwner()->castTo(mContext,
                                       pointer,
                                       mComplicatedElementInterface->getOwner(),
                                       0);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IComplicatedElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));

  mComplicatedNode->getOwner()->castTo(mContext, pointer, mElementInterface->getOwner(), 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, isTypeKindTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isPrimitive());
  EXPECT_TRUE(mSimpleNode->getOwner()->isOwner());
  EXPECT_FALSE(mSimpleNode->getOwner()->isReference());
  EXPECT_FALSE(mSimpleNode->getOwner()->isArray());
  EXPECT_FALSE(mSimpleNode->getOwner()->isFunction());
  EXPECT_FALSE(mSimpleNode->getOwner()->isPackage());
  EXPECT_FALSE(mSimpleNode->getOwner()->isNative());
  EXPECT_FALSE(mSimpleNode->getOwner()->isPointer());
}

TEST_F(NodeOwnerTest, isObjectTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isController());
  EXPECT_FALSE(mSimpleNode->getOwner()->isInterface());
  EXPECT_FALSE(mSimpleNode->getOwner()->isModel());
  EXPECT_TRUE(mSimpleNode->getOwner()->isNode());
  EXPECT_FALSE(mSimpleNode->getOwner()->isThread());
}

TEST_F(NodeOwnerTest, createLocalVariableTest) {
  mComplicatedNode->getOwner()->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.NComplicatedNode*"
  "\n  store %systems.vos.wisey.compiler.tests.NComplicatedNode* null, %systems.vos.wisey.compiler.tests.NComplicatedNode** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mComplicatedNode->getOwner(), "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mComplicatedNode->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(NodeOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mComplicatedNode->getOwner()->getLLVMType(mContext));
  mComplicatedNode->getOwner()->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %var = alloca %systems.vos.wisey.compiler.tests.NComplicatedNode*"
  "\n  store %systems.vos.wisey.compiler.tests.NComplicatedNode* null, %systems.vos.wisey.compiler.tests.NComplicatedNode** %var\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, injectDeathTest) {
  ::Mock::AllowLeak(mField1Expression);
  ::Mock::AllowLeak(mField2Expression);
  InjectionArgumentList arguments;
  EXPECT_EXIT(mComplicatedNode->getOwner()->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type "
              "systems.vos.wisey.compiler.tests.NComplicatedNode\\* is not injectable");
}
