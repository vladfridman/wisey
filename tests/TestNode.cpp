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

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/Constant.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/Node.hpp"
#include "wisey/NodeTypeSpecifier.hpp"
#include "wisey/NullType.hpp"
#include "wisey/ObjectKindGlobal.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReceivedField.hpp"
#include "wisey/StateField.hpp"
#include "wisey/ThreadExpression.hpp"
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

struct NodeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mComplicatedNode;
  Node* mSimpleNode;
  Node* mSimplerNode;
  Node* mOwnerNode;
  Model* mReferenceModel;
  Interface* mComplicatedElementInterface;
  Interface* mElementInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
  IMethod* mMethod;
  StructType* mStructType;
  ReceivedField* mLeftField;
  ReceivedField* mRightField;
  BasicBlock* mBasicBlock;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  wisey::Constant* mConstant;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  LLVMFunction* mLLVMFunction;

  NodeTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    string elementInterfaceFullName = "systems.vos.wisey.compiler.tests.IElement";
    StructType* elementInterfaceStructType = StructType::create(mLLVMContext,
                                                                elementInterfaceFullName);
    VariableList elementInterfaceArguments;
    vector<IObjectElementDefinition*> elementInterfaceElements;
    vector<IModelTypeSpecifier*> elementThrownExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    IObjectElementDefinition* getElementSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "getElement",
                                     elementInterfaceArguments,
                                     elementThrownExceptions,
                                     new MethodQualifiers(0),
                                     0);
    elementInterfaceElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> elementParentInterfaces;
    mElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                elementInterfaceFullName,
                                                elementInterfaceStructType,
                                                elementParentInterfaces,
                                                elementInterfaceElements,
                                                mContext.getImportProfile(),
                                                0);
    mContext.addInterface(mElementInterface, 0);
    mElementInterface->buildMethods(mContext);
    
    string complicatedElementFullName = "systems.vos.wisey.compiler.tests.IComplicatedElement";
    StructType* complicatedElementIinterfaceStructType =
    StructType::create(mLLVMContext, complicatedElementFullName);
    vector<IObjectElementDefinition*> complicatedElementInterfaceElements;
    MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
    methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::OVERRIDE);
    getElementSignature = new MethodSignatureDeclaration(intSpecifier,
                                                         "getElement",
                                                         elementInterfaceArguments,
                                                         elementThrownExceptions,
                                                         methodQualifiers,
                                                         0);
    complicatedElementInterfaceElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> complicatedElementParentInterfaces;
    InterfaceTypeSpecifier* elementInterfaceSpecifier =
    new InterfaceTypeSpecifier(NULL, "IElement", 0);
    complicatedElementParentInterfaces.push_back(elementInterfaceSpecifier);
    mComplicatedElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                           complicatedElementFullName,
                                                           complicatedElementIinterfaceStructType,
                                                           complicatedElementParentInterfaces,
                                                           complicatedElementInterfaceElements,
                                                           mContext.getImportProfile(),
                                                           0);
    mContext.addInterface(mComplicatedElementInterface, 0);
    mComplicatedElementInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceArguments;
    vector<IObjectElementDefinition*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDefinition* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceArguments,
                                     objectThrownExceptions,
                                     new MethodQualifiers(0),
                                     0);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements,
                                               mContext.getImportProfile(),
                                               0);
    mContext.addInterface(mObjectInterface, 0);
    mObjectInterface->buildMethods(mContext);

    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string complicatedNodeFullName = "systems.vos.wisey.compiler.tests.NComplicatedNode";
    mStructType = StructType::create(mLLVMContext, complicatedNodeFullName);
    mStructType->setBody(types);
    vector<IField*> fields;
    mComplicatedNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                     complicatedNodeFullName,
                                     mStructType,
                                     mContext.getImportProfile(),
                                     7);
   InjectionArgumentList arguments;
    mLeftField = new ReceivedField(PrimitiveTypes::INT, "mLeft", 0);
    mRightField = new ReceivedField(PrimitiveTypes::INT, "mRight", 0);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<const wisey::Argument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method(mComplicatedNode,
                         "getElement",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT,
                         methodArguments,
                         thrownExceptions,
                         new MethodQualifiers(0),
                         NULL,
                         0);
    vector<IMethod*> methods;
    methods.push_back(mMethod);
    Method* fooMethod = new Method(mComplicatedNode,
                                   "foo",
                                   AccessLevel::PRIVATE_ACCESS,
                                   PrimitiveTypes::INT,
                                   methodArguments,
                                   thrownExceptions,
                                   new MethodQualifiers(0),
                                   NULL,
                                   0);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mComplicatedElementInterface);
    interfaces.push_back(mObjectInterface);
    
    IntConstant* intConstant = new IntConstant(5, 0);
    mConstant = new wisey::Constant(PUBLIC_ACCESS,
                                    PrimitiveTypes::INT,
                                    "MYCONSTANT",
                                    intConstant,
                                    0);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);
    
    vector<const IType*> functionArgumentTypes;
    LLVMFunctionType* llvmFunctionType = LLVMFunctionType::create(LLVMPrimitiveTypes::I8,
                                                                  functionArgumentTypes);
    vector<const wisey::Argument*> llvmFunctionArguments;
    Block* functionBlock = new Block();
    CompoundStatement* functionCompoundStatement = new CompoundStatement(functionBlock, 0);
    mLLVMFunction = new LLVMFunction(mComplicatedNode,
                                     "myfunction",
                                     PUBLIC_ACCESS,
                                     llvmFunctionType,
                                     LLVMPrimitiveTypes::I8,
                                     llvmFunctionArguments,
                                     functionCompoundStatement,
                                     0);
    vector<LLVMFunction*> functions;
    functions.push_back(mLLVMFunction);
    
    mComplicatedNode->setFields(mContext, fields, interfaces.size());
    mComplicatedNode->setMethods(methods);
    mComplicatedNode->setInterfaces(interfaces);
    mComplicatedNode->setConstants(constants);
    mComplicatedNode->setLLVMFunctions(functions);
    
    vector<Type*> ownerTypes;
    ownerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                         ->getPointerTo()->getPointerTo());
    string ownerFullName = "systems.vos.wisey.compiler.tests.NOwner";
    StructType* ownerStructType = StructType::create(mLLVMContext, ownerFullName);
    ownerStructType->setBody(ownerTypes);
    mOwnerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                               ownerFullName,
                               ownerStructType,
                               mContext.getImportProfile(),
                               0);
    mContext.addNode(mOwnerNode, 0);
    
    vector<Type*> referenceTypes;
    referenceTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                             ->getPointerTo()->getPointerTo());
    string referenceFullName = "systems.vos.wisey.compiler.tests.MReference";
    StructType* referenceStructType = StructType::create(mLLVMContext, referenceFullName);
    referenceStructType->setBody(referenceTypes);
    mReferenceModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                      referenceFullName,
                                      referenceStructType,
                                      mContext.getImportProfile(),
                                      0);
    mContext.addModel(mReferenceModel, 0);
    
    vector<Type*> simpleNodeTypes;
    simpleNodeTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                              ->getPointerTo()->getPointerTo());
    simpleNodeTypes.push_back(mOwnerNode->getLLVMType(mContext));
    simpleNodeTypes.push_back(mReferenceModel->getLLVMType(mContext));
    string simpleNodeFullName = "systems.vos.wisey.compiler.tests.NSimpleNode";
    StructType* simpleNodeStructType = StructType::create(mLLVMContext, simpleNodeFullName);
    simpleNodeStructType->setBody(simpleNodeTypes);
    vector<IField*> simpleNodeFields;
    simpleNodeFields.push_back(new StateField(mOwnerNode->getOwner(), "mOwner", 0));
    simpleNodeFields.push_back(new ReceivedField(mReferenceModel, "mReference", 0));
    mSimpleNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                simpleNodeFullName,
                                simpleNodeStructType,
                                mContext.getImportProfile(),
                                0);
    mSimpleNode->setFields(mContext, simpleNodeFields, 1u);
    mContext.addNode(mSimpleNode, 0);
    
    vector<Type*> simplerNodeTypes;
    simplerNodeTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                               ->getPointerTo()->getPointerTo());
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simplerNodeFullName = "systems.vos.wisey.compiler.tests.NSimplerNode";
    StructType* simplerNodeStructType = StructType::create(mLLVMContext, simplerNodeFullName);
    simplerNodeStructType->setBody(simplerNodeTypes);
    vector<IField*> simplerNodeFields;
    simplerNodeFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mLeft", 0));
    simplerNodeFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mRight", 0));
    mSimplerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                 simplerNodeFullName,
                                 simplerNodeStructType,
                                 mContext.getImportProfile(),
                                 0);
    mSimplerNode->setFields(mContext, simplerNodeFields, 1u);
    mContext.addNode(mSimplerNode, 0);
    
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDefinition*> vehicleElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleElements,
                                                mContext.getImportProfile(),
                                                0);
    mContext.addInterface(mVehicleInterface, 0);
    mVehicleInterface->buildMethods(mContext);

    IConcreteObjectType::declareTypeNameGlobal(mContext, mOwnerNode);
    IConcreteObjectType::declareVTable(mContext, mOwnerNode);

    IConcreteObjectType::declareTypeNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::declareVTable(mContext, mSimpleNode);
    
    Value* field1Value = ConstantPointerNull::get(mOwnerNode->getOwner()->getLLVMType(mContext));
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(mOwnerNode->getOwner()));
    Value* field2Value = ConstantPointerNull::get(mReferenceModel->getLLVMType(mContext));
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(mReferenceModel));

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_, _)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mContext, mThreadVariable);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~NodeTest() {
    delete mStringStream;
    delete mField1Expression;
    delete mField2Expression;
    delete mThreadVariable;
  }
};

TEST_F(NodeTest, isPublicTest) {
  EXPECT_TRUE(mComplicatedNode->isPublic());
}

TEST_F(NodeTest, getLineTest) {
  EXPECT_EQ(7, mComplicatedNode->getLine());
}

TEST_F(NodeTest, getOwnerTest) {
  ASSERT_NE(nullptr, mComplicatedNode->getOwner());
  EXPECT_EQ(mComplicatedNode, mComplicatedNode->getOwner()->getReference());
}

TEST_F(NodeTest, getNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.NComplicatedNode",
               mComplicatedNode->getTypeName().c_str());
}

TEST_F(NodeTest, getShortNameTest) {
  EXPECT_STREQ("NComplicatedNode", mComplicatedNode->getShortName().c_str());
}

TEST_F(NodeTest, getVTableNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.NComplicatedNode.vtable",
               mComplicatedNode->getVTableName().c_str());
}

TEST_F(NodeTest, getLLVMTypeTest) {
  EXPECT_EQ(mStructType->getPointerTo(), mComplicatedNode->getLLVMType(mContext));
}

TEST_F(NodeTest, getInterfacesTest) {
  EXPECT_EQ(2u, mComplicatedNode->getInterfaces().size());
}

TEST_F(NodeTest, getFieldsTest) {
  EXPECT_EQ(2u, mComplicatedNode->getFields().size());
}

TEST_F(NodeTest, findFeildTest) {
  EXPECT_EQ(mLeftField, mComplicatedNode->findField("mLeft"));
  EXPECT_EQ(mRightField, mComplicatedNode->findField("mRight"));
  EXPECT_EQ(nullptr, mComplicatedNode->findField("mDepth"));
}

TEST_F(NodeTest, findConstantTest) {
  EXPECT_EQ(mConstant, mComplicatedNode->findConstant(mContext, "MYCONSTANT", 0));
}

TEST_F(NodeTest, findConstantDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mComplicatedNode->findConstant(mContext, "MYCONSTANT2", 7));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Node systems.vos.wisey.compiler.tests.NComplicatedNode does not have constant named MYCONSTANT2\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(NodeTest, findLLVMFunctionTest) {
  EXPECT_EQ(mLLVMFunction, mComplicatedNode->findLLVMFunction("myfunction"));
}

TEST_F(NodeTest, getFieldIndexTest) {
  EXPECT_EQ(mComplicatedNode->getFieldIndex(mLeftField), 2u);
  EXPECT_EQ(mComplicatedNode->getFieldIndex(mRightField), 3u);
}

TEST_F(NodeTest, findMethodTest) {
  EXPECT_EQ(mComplicatedNode->findMethod("getElement"), mMethod);
  EXPECT_EQ(mComplicatedNode->findMethod("bar"), nullptr);
}

TEST_F(NodeTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mComplicatedNode->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.typename");
}

TEST_F(NodeTest, getTypeTableNameTest) {
  ASSERT_STREQ(mComplicatedNode->getTypeTableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.typetable");
}

TEST_F(NodeTest, innerObjectsTest) {
  NiceMock<MockObjectType> innerObject1;
  ON_CALL(innerObject1, getShortName()).WillByDefault(Return("MObject1"));
  NiceMock<MockObjectType> innerObject2;
  ON_CALL(innerObject2, getShortName()).WillByDefault(Return("MObject2"));
  
  mComplicatedNode->addInnerObject(&innerObject1);
  mComplicatedNode->addInnerObject(&innerObject2);
  
  EXPECT_EQ(mComplicatedNode->getInnerObject("MObject1"), &innerObject1);
  EXPECT_EQ(mComplicatedNode->getInnerObject("MObject2"), &innerObject2);
  EXPECT_EQ(mComplicatedNode->getInnerObjects().size(), 2u);
}

TEST_F(NodeTest, markAsInnerTest) {
  EXPECT_FALSE(mComplicatedNode->isInner());
  mComplicatedNode->markAsInner();
  EXPECT_TRUE(mComplicatedNode->isInner());
}

TEST_F(NodeTest, canCastToTest) {
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mContext, mElementInterface));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mContext, mComplicatedNode));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mComplicatedNode->
               canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mComplicatedNode->
               canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(NodeTest, canAutoCastToTest) {
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mContext, mElementInterface));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mContext, mComplicatedNode));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mComplicatedNode->
               canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mComplicatedNode->
               canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(NodeTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->castTo(mContext, pointer, mComplicatedElementInterface, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IComplicatedElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->castTo(mContext, pointer, mElementInterface, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, isTypeKindTest) {
  EXPECT_FALSE(mSimpleNode->isPrimitive());
  EXPECT_FALSE(mSimpleNode->isOwner());
  EXPECT_TRUE(mSimpleNode->isReference());
  EXPECT_FALSE(mSimpleNode->isArray());
  EXPECT_FALSE(mSimpleNode->isFunction());
  EXPECT_FALSE(mSimpleNode->isPackage());
  EXPECT_FALSE(mSimpleNode->isNative());
  EXPECT_FALSE(mSimpleNode->isPointer());
  EXPECT_FALSE(mSimpleNode->isImmutable());
}

TEST_F(NodeTest, isObjectTest) {
  EXPECT_FALSE(mSimpleNode->isController());
  EXPECT_FALSE(mSimpleNode->isInterface());
  EXPECT_FALSE(mSimpleNode->isModel());
  EXPECT_TRUE(mSimpleNode->isNode());
}

TEST_F(NodeTest, isScopeInjectedTest) {
  EXPECT_FALSE(mSimpleNode->isScopeInjected(mContext));
}

TEST_F(NodeTest, incrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, decrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->decrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i64*"
  "\n  %1 = getelementptr i64, i64* %0, i64 -1"
  "\n  %refCounter = load i64, i64* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
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
  string argumentSpecifier1("withOwner");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withReference");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  mSimpleNode->declareBuildFunction(mContext);
  
  Value* result = mSimpleNode->build(mContext, argumentList, 0);
  
  EXPECT_NE(result, nullptr);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = call %systems.vos.wisey.compiler.tests.NSimpleNode* @systems.vos.wisey.compiler.tests.NSimpleNode.build(%systems.vos.wisey.compiler.tests.NOwner* null, %systems.vos.wisey.compiler.tests.MReference* null)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, declareBuildFunctionTest) {
  Function* buildFunction = mSimpleNode->declareBuildFunction(mContext);
  
  EXPECT_NE(buildFunction, nullptr);
  
  *mStringStream << *buildFunction;
  string expected =
  "\ndeclare %systems.vos.wisey.compiler.tests.NSimpleNode* @systems.vos.wisey.compiler.tests.NSimpleNode.build(%systems.vos.wisey.compiler.tests.NOwner*, %systems.vos.wisey.compiler.tests.MReference*)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, defineBuildFunctionTest) {
  Function* buildFunction = mSimpleNode->defineBuildFunction(mContext);
  mContext.runComposingCallbacks();
  
  EXPECT_NE(buildFunction, nullptr);
  
  *mStringStream << *buildFunction;
  string expected = string() +
  "\ndefine %systems.vos.wisey.compiler.tests.NSimpleNode* @systems.vos.wisey.compiler.tests.NSimpleNode.build(%systems.vos.wisey.compiler.tests.NOwner* %mOwner, %systems.vos.wisey.compiler.tests.MReference* %mReference) {"
  "\nentry:" +
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.NSimpleNode.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.NSimpleNode.refCounter, %systems.vos.wisey.compiler.tests.NSimpleNode.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.NSimpleNode.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NSimpleNode.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.NSimpleNode.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.NSimpleNode.refCounter, %systems.vos.wisey.compiler.tests.NSimpleNode.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode.refCounter, %systems.vos.wisey.compiler.tests.NSimpleNode.refCounter* %buildervar, i32 0, i32 1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode, %systems.vos.wisey.compiler.tests.NSimpleNode* %1, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.NOwner* %mOwner, %systems.vos.wisey.compiler.tests.NOwner** %2"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode, %systems.vos.wisey.compiler.tests.NSimpleNode* %1, i32 0, i32 2"
  "\n  store %systems.vos.wisey.compiler.tests.MReference* %mReference, %systems.vos.wisey.compiler.tests.MReference** %3"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MReference* %mReference to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %4, i64 1)"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.NSimpleNode* %1 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 0"
  "\n  %7 = bitcast i8* %6 to i32 (...)***"
  "\n  %8 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.NSimpleNode.vtable, i32 0, i32 0, i32 0"
  "\n  %9 = bitcast i8** %8 to i32 (...)**"
  "\n  store i32 (...)** %9, i32 (...)*** %7"
  "\n  ret %systems.vos.wisey.compiler.tests.NSimpleNode* %1"
  "\n}"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, buildInvalidObjectBuilderArgumentsDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  string argumentSpecifier1("owner");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withReference");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  mSimpleNode->declareBuildFunction(mContext);

  const char* expected =
  "/tmp/source.yz(1): Error: Object builder argument should start with 'with'. e.g. .withField(value).\n";
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mSimpleNode->build(mContext, argumentList, 1));
  EXPECT_STREQ(expected, buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(NodeTest, buildIncorrectArgumentTypeDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  Value* fieldValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
  ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(fieldValue));
  ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));

  string argumentSpecifier1("withOwner");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withReference");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  mSimpleNode->declareBuildFunction(mContext);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mSimpleNode->build(mContext, argumentList, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Node builder argument value for field mReference does not match its type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(NodeTest, buildNotAllFieldsAreSetDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  ObjectBuilderArgumentList argumentList;
  
  const char* expected =
  "/tmp/source.yz(7): Error: Field mReference of the node systems.vos.wisey.compiler.tests.NSimpleNode is not initialized.\n";
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mSimpleNode->build(mContext, argumentList, 7));
  EXPECT_STREQ(expected, buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(NodeTest, printToStreamTest) {
  stringstream stringStream;

  mComplicatedNode->printToStream(mContext, stringStream);

  EXPECT_STREQ("external node systems.vos.wisey.compiler.tests.NComplicatedNode\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IComplicatedElement,\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  constant int MYCONSTANT;\n"
               "\n"
               "  receive int mLeft;\n"
               "  receive int mRight;\n"
               "\n"
               "  int getElement();\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(NodeTest, createLocalVariableTest) {
  mComplicatedNode->createLocalVariable(mContext, "temp", 0);
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

TEST_F(NodeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  InjectionArgumentList injectionArgumentList;
  IField* field = new ReceivedField(mComplicatedNode, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mComplicatedNode->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(NodeTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, injectDeathTest) {
  ::Mock::AllowLeak(mField1Expression);
  ::Mock::AllowLeak(mField2Expression);
  ::Mock::AllowLeak(mThreadVariable);
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mComplicatedNode->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type systems.vos.wisey.compiler.tests.NComplicatedNode is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(NodeTest, getObjectTypeNameGlobalTest) {
  EXPECT_EQ(ObjectKindGlobal::getNode(mContext),
            mSimpleNode->getObjectTypeNameGlobal(mContext));
}

TEST_F(TestFileRunner, linkListRunTest) {
  runFile("tests/samples/test_linklist.yz", 9);
}

TEST_F(TestFileRunner, linkListReverseRunTest) {
  runFile("tests/samples/test_linklist_reverse.yz", 7);
}

TEST_F(TestFileRunner, nodeBuilderObjectArgumentAutocast) {
  runFile("tests/samples/test_node_builder_object_argument_autocast.yz", 2018);
}

TEST_F(TestFileRunner, nodeBuilderPrimitiveArgumentAutocast) {
  runFile("tests/samples/test_node_builder_primitive_argument_autocast.yz", 0);
}

TEST_F(TestFileRunner, nodeBuilderSetStateFieldsRunTest) {
  runFile("tests/samples/test_node_builder_set_state_fields.yz", 2018);
}

TEST_F(TestFileRunner, nodeWithModelFieldCompileTest) {
  compileFile("tests/samples/test_node_with_model_field.yz");
}

TEST_F(TestFileRunner, nodeWithNodeReferenceStateFieldRunTest) {
  runFile("tests/samples/test_node_reference_node_state_field.yz", 3);
}

TEST_F(TestFileRunner, nodeInitStateFieldWithNodeRunTest) {
  runFile("tests/samples/test_node_init_state_field_with_node.yz", 1);
}

TEST_F(TestFileRunner, nodeInitWithControllerRunTest) {
  runFile("tests/samples/test_node_init_with_controller.yz", 1);
}

TEST_F(TestFileRunner, nodeInitReceivedFieldWithInterfaceThatsControllerRunTest) {
  runFile("tests/samples/test_node_init_received_field_with_interface_thats_controller.yz", 1);
}

TEST_F(TestFileRunner, nodeInitStateFieldWithControllerDeathRunTest) {
  expectFailCompile("tests/samples/test_node_init_state_field_with_controller.yz",
                    1,
                    "tests/samples/test_node_init_state_field_with_controller.yz\\(30\\): Error: "
                    "Trying to initialize a node state field with object that is not a node. Node state fields can only be of node type");
}

TEST_F(TestFileRunner, nodeInitStateFieldWithInterfaceThatsControllerDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_node_init_state_field_with_interface_thats_controller.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_node_init_state_field_with_interface_thats_controller.yz:30)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.CService to node\n"
                               "Main thread ended without a result\n");
}
