//
//  TestInterface.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Interface}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockReferenceVariable.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "AdjustReferenceCountFunction.hpp"
#include "ConstantDefinition.hpp"
#include "ReceivedFieldDefinition.hpp"
#include "GetOriginalObjectFunction.hpp"
#include "Interface.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "Method.hpp"
#include "MethodDefinition.hpp"
#include "MethodSignature.hpp"
#include "MethodSignatureDeclaration.hpp"
#include "Names.hpp"
#include "NullType.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "ReceivedField.hpp"
#include "StaticMethodDefinition.hpp"
#include "ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InterfaceTest : public Test {
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  Interface* mIncompleteInterface;
  InterfaceTypeSpecifier* mObjectInterfaceSpecifier;
  IObjectElementDefinition* mFooMethod;
  IObjectElementDefinition* mBarMethod;
  IObjectElementDefinition* mStaticMethod;
  StructType* mShapeStructType;
  StructType* mIncompleteInterfaceStructType;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  NiceMock<MockExpression>* mMockExpression;
  ConstantDefinition* mConstantDefinition;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  InterfaceTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);

    vector<IModelTypeSpecifier*> exceptions;
    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    VariableList methodArguments;
    mBarMethod = new MethodSignatureDeclaration(intSpecifier,
                                                "bar",
                                                methodArguments,
                                                exceptions,
                                                new MethodQualifiers(0),
                                                0);
    vector<IObjectElementDefinition*> objectElementDeclarations;
    objectElementDeclarations.push_back(mBarMethod);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    vector<MethodSignatureDeclaration*> methodDeclarations;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectStructType,
                                               objectParentInterfaces,
                                               objectElementDeclarations,
                                               mContext.getImportProfile(),
                                               0);
    mContext.addInterface(mObjectInterface, 0);
    mObjectInterface->buildMethods(mContext);

    mObjectInterfaceSpecifier = new InterfaceTypeSpecifier(NULL, "IObject", 0);
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    mShapeStructType = StructType::create(mLLVMContext, shapeFullName);
    mFooMethod = new MethodSignatureDeclaration(intSpecifier,
                                                "foo",
                                                methodArguments,
                                                exceptions,
                                                new MethodQualifiers(0),
                                                0);
    vector<IObjectElementDefinition*> shapeElements;

    intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    VariableList staticArguments;
    vector<IModelTypeSpecifier*> staticMethodExceptions;
    Block* staticMethodBlock = new Block();
    CompoundStatement* staticMethodCompoundStatement = new CompoundStatement(staticMethodBlock, 0);
    mStaticMethod = new StaticMethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                               intSpecifier,
                                               "foostatic",
                                               staticArguments,
                                               staticMethodExceptions,
                                               new MethodQualifiers(0),
                                               staticMethodCompoundStatement,
                                               0);

    mMockExpression = new NiceMock<MockExpression>();
    ON_CALL(*mMockExpression, printToStream(_,_)).WillByDefault(Invoke(printExpression));
    ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    mConstantDefinition = new ConstantDefinition(PUBLIC_ACCESS,
                                                 intSpecifier,
                                                 "MYCONSTANT",
                                                 mMockExpression,
                                                 9);
    shapeElements.push_back(mConstantDefinition);
    shapeElements.push_back(mFooMethod);
    shapeElements.push_back(mStaticMethod);

    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(mObjectInterfaceSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              shapeFullName,
                                              mShapeStructType,
                                              shapeParentInterfaces,
                                              shapeElements,
                                              mContext.getImportProfile(),
                                              5);
    mContext.addInterface(mShapeInterface, 0);
    mShapeInterface->buildMethods(mContext);
    mShapeInterface->defineInjectionFunctionPointer(mContext, 0);

    mIncompleteInterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    mIncompleteInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                   shapeFullName,
                                                   mIncompleteInterfaceStructType,
                                                   shapeParentInterfaces,
                                                   shapeElements,
                                                   mContext.getImportProfile(),
                                                   0);
    
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  mShapeInterface->getTypeName());
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       mShapeInterface->getObjectNameGlobalVariableName());

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);

    vector<Type*> exitFunctionArgumentTypes;
    exitFunctionArgumentTypes.push_back(Type::getInt32Ty(mLLVMContext));
    FunctionType* exitFunctionType = FunctionType::get(Type::getVoidTy(mLLVMContext),
                                                       exitFunctionArgumentTypes,
                                                       false);
    Function::Create(exitFunctionType,
                     GlobalValue::ExternalLinkage,
                     "exit",
                     mContext.getModule());

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~InterfaceTest() {
    delete mMockExpression;
    delete mStringStream;
  }

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(InterfaceTest, interfaceInstantiationTest) {
  EXPECT_TRUE(mShapeInterface->isPublic());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.IShape", mShapeInterface->getTypeName().c_str());
  EXPECT_STREQ("IShape", mShapeInterface->getShortName().c_str());
  EXPECT_EQ(mShapeStructType->getPointerTo(), mShapeInterface->getLLVMType(mContext));
  ASSERT_NE(nullptr, mShapeInterface->getOwner());
  EXPECT_EQ(mShapeInterface, mShapeInterface->getOwner()->getReference());
  EXPECT_EQ(5, mShapeInterface->getLine());
}

TEST_F(InterfaceTest, findMethodTest) {
  EXPECT_NE(nullptr, mShapeInterface->findMethod("foo"));
  EXPECT_STREQ("foo", mShapeInterface->findMethod("foo")->getName().c_str());
  EXPECT_NE(nullptr, mShapeInterface->findMethod("bar"));
  EXPECT_EQ(nullptr, mShapeInterface->findMethod("zzz"));
}

TEST_F(InterfaceTest, findConstantTest) {
  ASSERT_NE(nullptr, mShapeInterface->findConstant(mContext, "MYCONSTANT", 0));
}

TEST_F(InterfaceTest, findConstantDeathTest) {
  Mock::AllowLeak(mMockExpression);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mShapeInterface->findConstant(mContext, "MYCONSTANT2", 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Interface systems.vos.wisey.compiler.tests.IShape does not have constant named MYCONSTANT2\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InterfaceTest, getMethodIndexTest) {
  EXPECT_EQ(mShapeInterface->getMethodIndex(mContext, mShapeInterface->findMethod("foo"), 0), 0u);
  EXPECT_EQ(mShapeInterface->getMethodIndex(mContext, mShapeInterface->findMethod("bar"), 0), 1u);
}

TEST_F(InterfaceTest, getMethodIndexDeathTest) {
  Mock::AllowLeak(mMockExpression);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mObjectInterface->getMethodIndex(mContext, mShapeInterface->findMethod("foo"), 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Method foo not found in interface systems.vos.wisey.compiler.tests.IObject\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InterfaceTest, getObjectNameGlobalVariableNameTest) {
  EXPECT_STREQ(mShapeInterface->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.IShape.typename");
}

TEST_F(InterfaceTest, getParentInterfacesTest) {
  EXPECT_EQ(mShapeInterface->getParentInterfaces().size(), 1u);
  EXPECT_EQ(mShapeInterface->getParentInterfaces().at(0), mObjectInterface);
  EXPECT_EQ(mObjectInterface->getParentInterfaces().size(), 0u);
}

TEST_F(InterfaceTest, doesExtendInterfaceTest) {
  EXPECT_FALSE(mObjectInterface->doesExtendInterface(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->doesExtendInterface(mObjectInterface));
}

TEST_F(InterfaceTest, isCompleteTest) {
  EXPECT_FALSE(mIncompleteInterface->isComplete());
  mIncompleteInterface->buildMethods(mContext);
  EXPECT_TRUE(mIncompleteInterface->isComplete());
}

TEST_F(InterfaceTest, getCastFunctionNameTest) {
  EXPECT_STREQ(mObjectInterface->getCastFunctionName(mShapeInterface).c_str(),
               "cast.systems.vos.wisey.compiler.tests.IObject."
               "to.systems.vos.wisey.compiler.tests.IShape");
}

TEST_F(InterfaceTest, innerObjectsTest) {
  NiceMock<MockObjectType> innerObject1;
  ON_CALL(innerObject1, getShortName()).WillByDefault(Return("MObject1"));
  NiceMock<MockObjectType> innerObject2;
  ON_CALL(innerObject2, getShortName()).WillByDefault(Return("MObject2"));
  
  mObjectInterface->addInnerObject(&innerObject1);
  mObjectInterface->addInnerObject(&innerObject2);
  
  EXPECT_EQ(mObjectInterface->getInnerObject("MObject1"), &innerObject1);
  EXPECT_EQ(mObjectInterface->getInnerObject("MObject2"), &innerObject2);
  EXPECT_EQ(mObjectInterface->getInnerObjects().size(), 2u);
}

TEST_F(InterfaceTest, markAsInnerTest) {
  EXPECT_FALSE(mObjectInterface->isInner());
  mObjectInterface->markAsInner();
  EXPECT_TRUE(mObjectInterface->isInner());
}

TEST_F(InterfaceTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mObjectInterface->canCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canCastTo(mContext, mObjectInterface));
  EXPECT_FALSE(mObjectInterface->canCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mObjectInterface->canCastTo(mContext, mObjectInterface));
  EXPECT_TRUE(mShapeInterface->canCastTo(mContext, mShapeInterface));
  EXPECT_FALSE(mObjectInterface->canCastTo(mContext, mObjectInterface->getOwner()));
}

TEST_F(InterfaceTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canAutoCastTo(mContext, mObjectInterface));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mObjectInterface->canAutoCastTo(mContext, mObjectInterface));
  EXPECT_TRUE(mShapeInterface->canAutoCastTo(mContext, mShapeInterface));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(mContext, mObjectInterface->getOwner()));
}

TEST_F(InterfaceTest, isTypeKindTest) {
  EXPECT_FALSE(mObjectInterface->isPrimitive());
  EXPECT_FALSE(mObjectInterface->isOwner());
  EXPECT_TRUE(mObjectInterface->isReference());
  EXPECT_FALSE(mObjectInterface->isArray());
  EXPECT_FALSE(mObjectInterface->isFunction());
  EXPECT_FALSE(mObjectInterface->isPackage());
  EXPECT_FALSE(mObjectInterface->isNative());
  EXPECT_FALSE(mObjectInterface->isPointer());
  EXPECT_FALSE(mObjectInterface->isImmutable());
}

TEST_F(InterfaceTest, isObjectTest) {
  EXPECT_FALSE(mObjectInterface->isController());
  EXPECT_TRUE(mObjectInterface->isInterface());
  EXPECT_FALSE(mObjectInterface->isModel());
  EXPECT_FALSE(mObjectInterface->isNode());
}

TEST_F(InterfaceTest, isScopeInjectedTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.ITest";
  StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
  vector<Type*> interfaceTypes;
  interfaceTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                           ->getPointerTo()->getPointerTo());
  interfaceStructType->setBody(interfaceTypes);
  vector<IInterfaceTypeSpecifier*> interfaceParentInterfaces;
  vector<IObjectElementDefinition*> interafaceElements;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 interfaceStructType,
                                                 interfaceParentInterfaces,
                                                 interafaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
  mContext.addInterface(interface, 0);
  llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                interface->getTypeName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());
  
  string controllerFullName = "systems.vos.wisey.compiler.tests.CController";
  StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
  vector<Type*> controllerTypes;
  controllerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                            ->getPointerTo()->getPointerTo());
  controllerStructType->setBody(controllerTypes);
  Controller* controller = Controller::newScopedController(AccessLevel::PUBLIC_ACCESS,
                                                           controllerFullName,
                                                           controllerStructType,
                                                           mContext.getImportProfile(),
                                                           0);
  vector<Interface*> controllerParentInterfaces;
  controllerParentInterfaces.push_back(interface);
  controller->setInterfaces(controllerParentInterfaces);
  mContext.addController(controller, 0);
  EXPECT_FALSE(interface->isScopeInjected(mContext));

  mContext.bindInterfaceToController(interface->getTypeName(), controller->getTypeName(), 0);
  EXPECT_TRUE(interface->isScopeInjected(mContext));
}

TEST_F(InterfaceTest, printToStreamTest) {
  stringstream stringStream;
  mShapeInterface->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external interface systems.vos.wisey.compiler.tests.IShape\n"
               "  extends\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  constant int MYCONSTANT = 5;\n"
               "\n"
               "  int foo();\n"
               "  static int foostatic();\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(InterfaceTest, fieldDefinitionDeathTest) {
  Mock::AllowLeak(mMockExpression);

  const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
  ReceivedFieldDefinition* fieldDeclaration = ReceivedFieldDefinition::create(intSpecifier, "mField", 3);
  
  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDefinition*> elements;
  elements.push_back(fieldDeclaration);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 name,
                                                 structType,
                                                 parentInterfaces,
                                                 elements,
                                                 mContext.getImportProfile(),
                                                 0);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(interface->buildMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Interfaces can not contain fields\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InterfaceTest, methodDeclarationDeathTest) {
  Mock::AllowLeak(mMockExpression);

  const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
  VariableList arguments;
  vector<IModelTypeSpecifier*> thrownExceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* methodDeclaration = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                             intSpecifier,
                                                             "foo",
                                                             arguments,
                                                             thrownExceptions,
                                                             new MethodQualifiers(0),
                                                             compoundStatement,
                                                             5);

  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDefinition*> elements;
  elements.push_back(methodDeclaration);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 name,
                                                 structType,
                                                 parentInterfaces,
                                                 elements,
                                                 mContext.getImportProfile(),
                                                 0);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(interface->buildMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Interfaces can not contain method implmentations\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InterfaceTest, constantsAfterMethodSignaturesDeathTest) {
  Mock::AllowLeak(mMockExpression);

  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDefinition*> elements;
  elements.push_back(mBarMethod);
  elements.push_back(mConstantDefinition);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 name,
                                                 structType,
                                                 parentInterfaces,
                                                 elements,
                                                 mContext.getImportProfile(),
                                                 0);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(interface->buildMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(9): Error: In interfaces constants should be declared before methods\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InterfaceTest, incrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mContext));
  mShapeInterface->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mEntryBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  call void @__adjustReferenceCounter(i8* %0, i64 1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, decrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mContext));
  mShapeInterface->decrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mEntryBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  call void @__adjustReferenceCounter(i8* %0, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mContext));
  mShapeInterface->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mEntryBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  %1 = call i8* @__getOriginalObject(i8* %0)"
  "\n  %2 = bitcast i8* %1 to i64*"
  "\n  %3 = getelementptr i64, i64* %2, i64 -1"
  "\n  %refCounter = load i64, i64* %3\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, circularDependencyDeathTest) {
  Mock::AllowLeak(mMockExpression);

  InterfaceTypeSpecifier* parentInterfaceSpecifier = new InterfaceTypeSpecifier(NULL, "IParent", 0);
  InterfaceTypeSpecifier* childInterfaceSpecifier = new InterfaceTypeSpecifier(NULL, "IChild", 0);

  string childFullName = "systems.vos.wisey.compiler.tests.IChild";
  StructType* childStructType = StructType::create(mLLVMContext, childFullName);
  vector<IInterfaceTypeSpecifier*> childParentInterfaces;
  childParentInterfaces.push_back(parentInterfaceSpecifier);
  vector<IObjectElementDefinition*> childElements;
  Interface* child = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                             childFullName,
                                             childStructType,
                                             childParentInterfaces,
                                             childElements,
                                             mContext.getImportProfile(),
                                             3);
  mContext.addInterface(child, 0);
  
  string parentFullName = "systems.vos.wisey.compiler.tests.IParent";
  StructType* parentStructType = StructType::create(mLLVMContext, parentFullName);
  vector<IInterfaceTypeSpecifier*> parentParentInterfaces;
  parentParentInterfaces.push_back(childInterfaceSpecifier);
  vector<IObjectElementDefinition*> parentElements;
  Interface* parent = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              parentFullName,
                                              parentStructType,
                                              parentParentInterfaces,
                                              parentElements,
                                              mContext.getImportProfile(),
                                              1);
  mContext.addInterface(parent, 0);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(child->buildMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Circular interface dependency between interfaces systems.vos.wisey.compiler.tests.IChild and systems.vos.wisey.compiler.tests.IParent\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InterfaceTest, createLocalVariableTest) {
  mShapeInterface->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.IShape*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store %systems.vos.wisey.compiler.tests.IShape* null, %systems.vos.wisey.compiler.tests.IShape** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  InjectionArgumentList injectionArgumentList;
  IField* field = new ReceivedField(mShapeInterface, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mShapeInterface->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(InterfaceTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mShapeInterface->getLLVMType(mContext));
  mShapeInterface->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  call void @__adjustReferenceCounter(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, injectWrapperFunctionTest) {
  InjectionArgumentList injectionArgumentList;
  mShapeInterface->inject(mContext, injectionArgumentList, 0);

  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->getFunction(mShapeInterface->getTypeName() +
                                                         ".inject");
  *mStringStream << *function;
  
  string expected =
  "\ndefine %systems.vos.wisey.compiler.tests.IShape* @systems.vos.wisey.compiler.tests.IShape.inject(%wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.IShape* (%wisey.threads.IThread*, %wisey.threads.CCallStack*)*, %systems.vos.wisey.compiler.tests.IShape* (%wisey.threads.IThread*, %wisey.threads.CCallStack*)** @systems.vos.wisey.compiler.tests.IShape.inject.pointer"
  "\n  %1 = icmp eq %systems.vos.wisey.compiler.tests.IShape* (%wisey.threads.IThread*, %wisey.threads.CCallStack*)* %0, null"
  "\n  br i1 %1, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MInterfaceNotBoundException.refCounter* getelementptr (%wisey.lang.MInterfaceNotBoundException.refCounter, %wisey.lang.MInterfaceNotBoundException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MInterfaceNotBoundException.refCounter*"
  "\n  %2 = bitcast %wisey.lang.MInterfaceNotBoundException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 ptrtoint (%wisey.lang.MInterfaceNotBoundException.refCounter* getelementptr (%wisey.lang.MInterfaceNotBoundException.refCounter, %wisey.lang.MInterfaceNotBoundException.refCounter* null, i32 1) to i64), i1 false)"
  "\n  %3 = getelementptr %wisey.lang.MInterfaceNotBoundException.refCounter, %wisey.lang.MInterfaceNotBoundException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %4 = getelementptr %wisey.lang.MInterfaceNotBoundException, %wisey.lang.MInterfaceNotBoundException* %3, i32 0, i32 1"
  "\n  store i8* getelementptr inbounds ([40 x i8], [40 x i8]* @systems.vos.wisey.compiler.tests.IShape.typename, i32 0, i32 0), i8** %4"
  "\n  %5 = bitcast %wisey.lang.MInterfaceNotBoundException* %3 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 0"
  "\n  %7 = bitcast i8* %6 to i32 (...)***"
  "\n  %8 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MInterfaceNotBoundException.vtable, i32 0, i32 0, i32 0"
  "\n  %9 = bitcast i8** %8 to i32 (...)**"
  "\n  store i32 (...)** %9, i32 (...)*** %7"
  "\n  %10 = bitcast { i8*, i8* }* @wisey.lang.MInterfaceNotBoundException.rtti to i8*"
  "\n  %11 = bitcast %wisey.lang.MInterfaceNotBoundException* %3 to i8*"
  "\n  %12 = getelementptr i8, i8* %11, i64 -8"
  "\n  %13 = call i8* @__cxa_allocate_exception(i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 2), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %13, i8* %12, i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 2), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(i8* %12)"
  "\n  invoke void @__cxa_throw(i8* %13, i8* %10, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %14 = call %systems.vos.wisey.compiler.tests.IShape* %0(%wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack)"
  "\n  ret %systems.vos.wisey.compiler.tests.IShape* %14"
  "\n"
  "\ncleanup:                                          ; preds = %if.null"
  "\n  %15 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %16 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %15, { i8*, i32 }* %16"
  "\n  %17 = getelementptr { i8*, i32 }, { i8*, i32 }* %16, i32 0, i32 0"
  "\n  %18 = load i8*, i8** %17"
  "\n  %19 = call i8* @__cxa_get_exception_ptr(i8* %18)"
  "\n  %20 = getelementptr i8, i8* %19, i64 8"
  "\n  resume { i8*, i32 } %15"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.null"
  "\n  unreachable"
  "\n}"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, injectTest) {
  InjectionArgumentList injectionArgumentList;
  mShapeInterface->inject(mContext, injectionArgumentList, 0);

  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  mContext.runComposingCallbacks();

  *mStringStream << *mEntryBlock;
  
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call %systems.vos.wisey.compiler.tests.IShape* @systems.vos.wisey.compiler.tests.IShape.inject(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, composeInjectFunctionWithControllerTest) {
  mContext.getScopes().popScope(mContext, 0);

  string interfaceFullName = "systems.vos.wisey.compiler.tests.ITest";
  StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
  vector<Type*> interfaceTypes;
  interfaceTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                           ->getPointerTo()->getPointerTo());
  interfaceStructType->setBody(interfaceTypes);
  vector<IInterfaceTypeSpecifier*> interfaceParentInterfaces;
  vector<IObjectElementDefinition*> interafaceElements;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 interfaceStructType,
                                                 interfaceParentInterfaces,
                                                 interafaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
  mContext.addInterface(interface, 0);
  llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                interface->getTypeName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());

  string controllerFullName = "systems.vos.wisey.compiler.tests.CController";
  StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
  vector<Type*> controllerTypes;
  controllerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                  ->getPointerTo()->getPointerTo());
  controllerStructType->setBody(controllerTypes);
  Controller* controller = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                     controllerFullName,
                                                     controllerStructType,
                                                     mContext.getImportProfile(),
                                                     0);
  vector<Interface*> controllerParentInterfaces;
  controllerParentInterfaces.push_back(interface);
  controller->setInterfaces(controllerParentInterfaces);
  
  mContext.addController(controller, 0);
  mContext.bindInterfaceToController(interface->getTypeName(), controller->getTypeName(), 0);

  IConcreteObjectType::declareTypeNameGlobal(mContext, controller);
  IConcreteObjectType::declareVTable(mContext, controller);

  interface->defineInjectionFunctionPointer(mContext, 0);
  controller->declareInjectFunction(mContext, 0);
  
  InjectionArgumentList arguments;
  interface->composeInjectFunctionWithController(mContext, controller, 0);
  mContext.runComposingCallbacks();

  Function* function = mContext.getModule()->getFunction(interface->getTypeName() +
                                                         ".inject.function");
  *mStringStream << *function;

  string expected =
  "\ndefine %systems.vos.wisey.compiler.tests.ITest* @systems.vos.wisey.compiler.tests.ITest.inject.function(%wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack) {"
  "\nentry:"
  "\n  %0 = call %systems.vos.wisey.compiler.tests.CController* @systems.vos.wisey.compiler.tests.CController.inject(%wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack)"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.CController* %0 to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 0"
  "\n  %3 = bitcast i8* %2 to %systems.vos.wisey.compiler.tests.ITest*"
  "\n  ret %systems.vos.wisey.compiler.tests.ITest* %3"
  "\n}"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, interfaceMethodNotImplmentedDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_not_implmented.yz",
                    1,
                    "Error: Method getArea of interface systems.vos.wisey.compiler.tests.IShape is not implemented by object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileRunner, interfaceInheritedMethodNotImplmentedDeathTest) {
  expectFailCompile("tests/samples/test_interface_inherited_method_not_implemented.yz",
                    1,
                    "tests/samples/test_interface_inherited_method_not_implemented.yz\\(19\\): "
                    "Error: Method getArea of interface systems.vos.wisey.compiler.tests.IShape is not implemented by object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileRunner, interfaceMethodDifferentReturnTypeDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_return_type_doesnot_match.yz",
                    1,
                    "Error: Method getArea of interface systems.vos.wisey.compiler.tests.IShape has different return type when implmeneted by object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileRunner, interfaceMethodDifferentArgumentTypesDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_arguments_dont_match.yz",
                    1,
                    "Error: Method getArea of interface systems.vos.wisey.compiler.tests.IShape has different argument types when implmeneted by object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileRunner, interfaceExceptionsDoNotReconcileDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_exceptions_dont_reconcile.yz",
                    1,
                    "tests/samples/test_interface_method_exceptions_dont_reconcile.yz\\(13\\): "
                    "Error: Method getArea of object systems.vos.wisey.compiler.tests.MSquare throws an unexpected exception of type systems.vos.wisey.compiler.tests.MException\n"
                    "tests/samples/test_interface_method_exceptions_dont_reconcile.yz\\(13\\): "
                    "Error: Exceptions thrown by method getArea of interface systems.vos.wisey.compiler.tests.IShape do not reconcile with exceptions thrown by its implementation in object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileRunner, objectMethodNotMarkedOverrideDeathTest) {
  expectFailCompile("tests/samples/test_object_method_not_marked_override.yz",
                    1,
                    "tests/samples/test_object_method_not_marked_override.yz\\(9\\): "
                    "Error: Object systems.vos.wisey.compiler.tests.MMyModel should mark method 'getValue' with 'override' qualifier because it overrides the method defined in the parent interface systems.vos.wisey.compiler.tests.IMyInterface");
}

TEST_F(TestFileRunner, modelImplmenetingInterfaceDefinitionRunTest) {
  runFile("tests/samples/test_interface_implementation.yz", 90);
}

TEST_F(TestFileRunner, interfaceInheritanceRunTest) {
  runFile("tests/samples/test_level2_inheritance.yz", 235);
}

TEST_F(TestFileRunner, modelWithMultipleInterfaceInheritanceRunTest) {
  runFile("tests/samples/test_model_multiple_interface_inheritance.yz", 5);
}

TEST_F(TestFileRunner, interfaceWithMultipleInterfaceInheritanceRunTest) {
  runFile("tests/samples/test_interface_multiple_interface_inheritance.yz", 15);
}

TEST_F(TestFileRunner, interfaceCastToModelRunTest) {
  runFile("tests/samples/test_interface_cast_to_model.yz", 5);
}

TEST_F(TestFileRunner, interfaceCastToInterfaceRunTest) {
  runFile("tests/samples/test_interface_cast_to_interface.yz", 5);
}

TEST_F(TestFileRunner, interfaceCastToAnotherInterfaceRunTest) {
  runFile("tests/samples/test_interface_cast_to_another_interface.yz", 5);
}

TEST_F(TestFileRunner, interfaceAutoCastRunTest) {
  runFile("tests/samples/test_interface_autocast.yz", 5);
}

TEST_F(TestFileRunner, interfaceStaticMethodRunTest) {
  runFile("tests/samples/test_interface_static_method.yz", 5);
}

TEST_F(TestFileRunner, interfaceStaticMethodComplicatedRunTest) {
  runFile("tests/samples/test_interface_static_method_complicated.yz", 7);
}

TEST_F(TestFileRunner, interfaceObjectAddReferenceForModelRunTest) {
  runFile("tests/samples/test_interface_object_add_reference_for_model.yz", 5);
}

TEST_F(TestFileRunner, interfaceObjectAddReferenceForNodeRunTest) {
  runFile("tests/samples/test_interface_object_add_reference_for_node.yz", 7);
}

TEST_F(TestFileRunner, interfaceBridgeToPrivateStaticMethodRunTest) {
  runFile("tests/samples/test_interface_bridge_to_private_static_method.yz", 5);
}

TEST_F(TestFileRunner, concreteObjectOverloadInterfaceReturnTypeRunTest) {
  runFile("tests/samples/test_concrete_object_overload_interface_return_type.yz", 1);
}

TEST_F(TestFileRunner, concreteObjectOverloadInterfaceArgumentTypeRunTest) {
  runFile("tests/samples/test_concrete_object_overload_interface_argument_type.yz", 3);
}

TEST_F(TestFileRunner, interfaceOverridesInterfaceMethodReturnTypeRunTest) {
  runFile("tests/samples/test_interface_overrides_interface_method_return_type.yz", 5);
}

TEST_F(TestFileRunner, interfaceOverridesInterfaceMethodArgumentTypeRunTest) {
  runFile("tests/samples/test_interface_overrides_interface_method_argument_type.yz", 7);
}

TEST_F(TestFileRunner, interfaceOverridesInterfaceMethodReturnTypeFailRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_overrides_interface_method_return_type_fail.yz",
                    1,
                    "tests/samples/test_interface_overrides_interface_method_return_type_fail.yz\\(20\\): Error: "
                    "Interface systems.vos.wisey.compiler.tests.IReturnsHasValue overrides method 'getHasValue' of parent interface with a different return type");
}

TEST_F(TestFileRunner, interfaceOverridesInterfaceMethodArgumentTypeFailRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_overrides_interface_method_argument_type_fail.yz",
                    1,
                    "tests/samples/test_interface_overrides_interface_method_argument_type_fail.yz\\(20\\): Error: "
                    "Interface systems.vos.wisey.compiler.tests.IProcessesHasValue overrides method 'processHasValue' of parent interface with different argument types");
}

TEST_F(TestFileRunner, interfaceStaticMethodThrowsNpeDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_static_method_throws_npe.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.IShape.getValue(tests/samples/test_interface_static_method_throws_npe.yz:7)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_static_method_throws_npe.yz:24)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, interfaceCastToModelCastExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_cast_to_model_cast_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_cast_to_model_cast_exception.yz:47)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.MFridge to systems.vos.wisey.compiler.tests.MFoo\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, interfaceCastToInterfaceCastExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_cast_to_interface_cast_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_cast_to_interface_cast_exception.yz:47)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.MFoo to systems.vos.wisey.compiler.tests.IDevice\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, interfaceCircularDependencyRunDeathTest) {
  expectFailCompile("tests/samples/test_circular_dependency_loop.yz",
                    1,
                    "Error: Circular interface dependency between interfaces "
                    "systems.vos.wisey.compiler.tests.IParent and "
                    "systems.vos.wisey.compiler.tests.IChild");
}

TEST_F(TestFileRunner, interfaceMethodOverridesAnotherMethodRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_overrides_another_method.yz",
                    1,
                    "tests/samples/test_interface_method_overrides_another_method.yz\\(8\\): "
                    "Error: Method 'getValue' in interface systems.vos.wisey.compiler.tests.IParent"
                    " must be marked override because it overrides a method of the same name from "
                    "systems.vos.wisey.compiler.tests.IChild");
}

TEST_F(TestFileRunner, interfaceMethodOverridesNothingRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_overrides_nothing.yz",
                    1,
                    "tests/samples/test_interface_method_overrides_nothing.yz\\(8\\): Error: "
                    "Method signature 'getSomething' is marked override but it does not override "
                    "any signatures from parent interfaces");
}

TEST_F(TestFileRunner, injectInterfaceNotBoundInInjectFieldRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_interface_not_bound_in_inject_field.yz",
                    1,
                    "tests/samples/test_inject_interface_not_bound_in_inject_field.yz\\(29\\): "
                    "Error: Arguments are not allowed for injection of interfaces that are not bound to controllers");
}

TEST_F(TestFileRunner, injectInterfaceNotBoundInlineRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_interface_not_bound_inline.yz",
                    1,
                    "tests/samples/test_inject_interface_not_bound_inline.yz\\(31\\): "
                    "Error: Arguments are not allowed for injection of interfaces that are not bound to controllers");
}

TEST_F(TestFileRunner, interfaceRedefineConstantRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_redefine_constant.yz",
                    1,
                    "tests/samples/test_interface_redefine_constant.yz\\(7\\): Error: "
                    "Constant named 'CONSTANT' was already defined on line 6");
}

TEST_F(TestFileRunner, interfaceMethodSignatureOverloadRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_signature_overload.yz",
                    1,
                    "tests/samples/test_interface_method_signature_overload.yz\\(7\\): Error: "
                    "Method signature 'getValue' was already defined on line 6");
}

TEST_F(TestFileRunner, interfaceStaticMethodOverloadRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_static_method_overload.yz",
                    1,
                    "tests/samples/test_interface_static_method_overload.yz\\(12\\): Error: "
                    "Static method 'getYear' was already defined on line 8");
}

TEST_F(TestFileRunner, interfaceFunctionOverloadRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_function_overload.yz",
                    1,
                    "tests/samples/test_interface_function_overload.yz\\(12\\): Error: "
                    "Function or method 'getYear' was already defined on line 8");
}

TEST_F(TestFileRunner, interfacePrivateStaticMethodRunDeathTest) {
  expectFailCompile("tests/samples/test_interface_private_static_method.yz",
                    1,
                    "tests/samples/test_interface_private_static_method.yz\\(14\\): Error: "
                    "Static method 'getValue' of object systems.vos.wisey.compiler.tests.IShape is private");
}
