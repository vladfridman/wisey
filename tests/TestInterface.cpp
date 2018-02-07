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

#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockReferenceVariable.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForInterfaceFunction.hpp"
#include "wisey/ConstantDeclaration.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/StaticMethodDeclaration.hpp"
#include "wisey/ThreadExpression.hpp"

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
  IObjectElementDeclaration* mFooMethod;
  IObjectElementDeclaration* mBarMethod;
  IObjectElementDeclaration* mStaticMethod;
  StructType* mShapeStructType;
  StructType* mIncompleteInterfaceStructType;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  NiceMock<MockExpression>* mMockExpression;
  ConstantDeclaration* mConstantDeclaration;
  NiceMock<MockReferenceVariable>* mThreadVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  InterfaceTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    vector<IModelTypeSpecifier*> exceptions;
    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    VariableList methodArguments;
    mBarMethod = new MethodSignatureDeclaration(intSpecifier, "bar", methodArguments, exceptions);
    vector<IObjectElementDeclaration*> objectElementDeclarations;
    objectElementDeclarations.push_back(mBarMethod);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    vector<MethodSignatureDeclaration*> methodDeclarations;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectStructType,
                                               objectParentInterfaces,
                                               objectElementDeclarations);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    mObjectInterfaceSpecifier = new InterfaceTypeSpecifier(NULL, "IObject");
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    mShapeStructType = StructType::create(mLLVMContext, shapeFullName);
    mFooMethod = new MethodSignatureDeclaration(intSpecifier, "foo", methodArguments, exceptions);
    vector<IObjectElementDeclaration*> shapeElements;

    intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    VariableList staticMethodArguments;
    vector<IModelTypeSpecifier*> staticMethodExceptions;
    Block* staticMethodBlock = new Block();
    CompoundStatement* staticMethodCompoundStatement = new CompoundStatement(staticMethodBlock, 0);
    mStaticMethod = new StaticMethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                                intSpecifier,
                                                "foostatic",
                                                staticMethodArguments,
                                                staticMethodExceptions,
                                                staticMethodCompoundStatement,
                                                0);
    
    mMockExpression = new NiceMock<MockExpression>();
    ON_CALL(*mMockExpression, printToStream(_,_)).WillByDefault(Invoke(printExpression));
    intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    mConstantDeclaration = new ConstantDeclaration(PUBLIC_ACCESS,
                                                   intSpecifier,
                                                   "MYCONSTANT",
                                                   mMockExpression);
    shapeElements.push_back(mConstantDeclaration);
    shapeElements.push_back(mFooMethod);
    shapeElements.push_back(mStaticMethod);

    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(mObjectInterfaceSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              shapeFullName,
                                              mShapeStructType,
                                              shapeParentInterfaces,
                                              shapeElements);
    mContext.addInterface(mShapeInterface);
    mShapeInterface->buildMethods(mContext);

    mIncompleteInterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    mIncompleteInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                   shapeFullName,
                                                   mIncompleteInterfaceStructType,
                                                   shapeParentInterfaces,
                                                   shapeElements);
    
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
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    Controller* threadController = mContext.getController(Names::getThreadControllerFullName());
    Value* threadObject = ConstantPointerNull::get(threadController->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockReferenceVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadController));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~InterfaceTest() {
    delete mMockExpression;
    delete mThreadVariable;
    delete mStringStream;
  }

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(InterfaceTest, interfaceInstantiationTest) {
  EXPECT_EQ(mShapeInterface->getAccessLevel(), AccessLevel::PUBLIC_ACCESS);
  EXPECT_STREQ(mShapeInterface->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.IShape");
  EXPECT_STREQ(mShapeInterface->getShortName().c_str(), "IShape");
  EXPECT_EQ(mShapeInterface->getTypeKind(), INTERFACE_TYPE);
  EXPECT_EQ(mShapeInterface->getLLVMType(mContext), mShapeStructType->getPointerTo());
  ASSERT_NE(mShapeInterface->getOwner(), nullptr);
  EXPECT_EQ(mShapeInterface->getOwner()->getObject(), mShapeInterface);
}

TEST_F(InterfaceTest, findMethodTest) {
  EXPECT_NE(mShapeInterface->findMethod("foo"), nullptr);
  EXPECT_STREQ(mShapeInterface->findMethod("foo")->getName().c_str(), "foo");
  EXPECT_NE(mShapeInterface->findMethod("bar"), nullptr);
  EXPECT_EQ(mShapeInterface->findMethod("zzz"), nullptr);
}

TEST_F(InterfaceTest, findConstantTest) {
  ASSERT_NE(mShapeInterface->findConstant("MYCONSTANT"), nullptr);
}

TEST_F(InterfaceTest, findConstantDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(mShapeInterface->findConstant("MYCONSTANT2"),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IShape "
              "does not have constant named MYCONSTANT2");
}

TEST_F(InterfaceTest, getMethodIndexTest) {
  EXPECT_EQ(mShapeInterface->getMethodIndex(mShapeInterface->findMethod("foo")), 0u);
  EXPECT_EQ(mShapeInterface->getMethodIndex(mShapeInterface->findMethod("bar")), 1u);
}

TEST_F(InterfaceTest, getMethodIndexDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(mObjectInterface->getMethodIndex(mShapeInterface->findMethod("foo")),
              ::testing::ExitedWithCode(1),
              "Error: Method foo not found in interface systems.vos.wisey.compiler.tests.IObject");
}

TEST_F(InterfaceTest, getObjectNameGlobalVariableNameTest) {
  EXPECT_STREQ(mShapeInterface->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.IShape.name");
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

TEST_F(InterfaceTest, getReferenceAdjustmentFunctionTest) {
  Function* result = mObjectInterface->getReferenceAdjustmentFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(result, AdjustReferenceCounterForInterfaceFunction::get(mContext));
}

TEST_F(InterfaceTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->canCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canCastTo(mObjectInterface));
  EXPECT_FALSE(mObjectInterface->canCastTo(NullType::NULL_TYPE));
  EXPECT_TRUE(mObjectInterface->canCastTo(mObjectInterface));
  EXPECT_TRUE(mShapeInterface->canCastTo(mShapeInterface));
}

TEST_F(InterfaceTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canAutoCastTo(mObjectInterface));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(NullType::NULL_TYPE));
  EXPECT_TRUE(mObjectInterface->canAutoCastTo(mObjectInterface));
  EXPECT_TRUE(mShapeInterface->canAutoCastTo(mShapeInterface));
}

TEST_F(InterfaceTest, isOwnerTest) {
  EXPECT_FALSE(mObjectInterface->isOwner());
}

TEST_F(InterfaceTest, printToStreamTest) {
  stringstream stringStream;
  Model* innerPublicModel = Model::newModel(PUBLIC_ACCESS, "MInnerPublicModel", NULL);
  vector<Field*> fields;
  InjectionArgumentList arguments;
  Field* field1 = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mField1", arguments);
  Field* field2 = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mField2", arguments);
  fields.push_back(field1);
  fields.push_back(field2);
  innerPublicModel->setFields(fields, 0);
  
  vector<MethodArgument*> methodArguments;
  vector<const Model*> thrownExceptions;
  Method* method = new Method(innerPublicModel,
                              "bar",
                              AccessLevel::PUBLIC_ACCESS,
                              PrimitiveTypes::INT_TYPE,
                              methodArguments,
                              thrownExceptions,
                              NULL,
                              0);
  vector<IMethod*> methods;
  methods.push_back(method);
  innerPublicModel->setMethods(methods);
  
  Model* innerPrivateModel = Model::newModel(PRIVATE_ACCESS, "MInnerPrivateModel", NULL);
  innerPrivateModel->setFields(fields, 0);
  
  mShapeInterface->addInnerObject(innerPublicModel);
  mShapeInterface->addInnerObject(innerPrivateModel);
  mShapeInterface->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external interface systems.vos.wisey.compiler.tests.IShape\n"
               "  extends\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  int foo();\n"
               "  static int foostatic();\n"
               "\n"
               "external model MInnerPrivateModel {\n"
               "}\n"
               "\n"
               "external model MInnerPublicModel {\n"
               "\n"
               "  fixed int mField1;\n"
               "  fixed int mField2;\n"
               "\n"
               "  int bar();\n"
               "}\n"
               "\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(InterfaceTest, fieldDefinitionDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Mock::AllowLeak(mThreadVariable);

  PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  InjectionArgumentList arguments;
  FieldDeclaration* fieldDeclaration = new FieldDeclaration(FieldKind::FIXED_FIELD,
                                                            intSpecifier,
                                                            "mField",
                                                            arguments);
  
  
  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDeclaration*> elements;
  elements.push_back(fieldDeclaration);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 name,
                                                 structType,
                                                 parentInterfaces,
                                                 elements);
  
  EXPECT_EXIT(interface->buildMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interfaces can not contain fields");
}

TEST_F(InterfaceTest, methodDeclarationDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Mock::AllowLeak(mThreadVariable);

  const PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableList arguments;
  vector<IModelTypeSpecifier*> thrownExceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDeclaration* methodDeclaration =
  new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                        intSpecifier,
                        "foo",
                        arguments,
                        thrownExceptions,
                        compoundStatement,
                        0);

  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDeclaration*> elements;
  elements.push_back(methodDeclaration);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 name,
                                                 structType,
                                                 parentInterfaces,
                                                 elements);
  
  EXPECT_EXIT(interface->buildMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interfaces can not contain method implmentations");
}

TEST_F(InterfaceTest, constantsAfterMethodSignaturesDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Mock::AllowLeak(mThreadVariable);

  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDeclaration*> elements;
  elements.push_back(mBarMethod);
  elements.push_back(mConstantDeclaration);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 name,
                                                 structType,
                                                 parentInterfaces,
                                                 elements);
  
  EXPECT_EXIT(interface->buildMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: In interfaces constants should be declared before methods");
}

TEST_F(InterfaceTest, incrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mContext));
  mShapeInterface->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %0, i64 1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, decrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mContext));
  mShapeInterface->decrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %0, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mContext));
  mShapeInterface->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  %1 = call i8* @__getOriginalObject(i8* %0, i64 -8)"
  "\n  %2 = bitcast i8* %1 to i64*"
  "\n  %refCounter = load i64, i64* %2\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, getDestructorFunctionNameTest) {
  EXPECT_STREQ("destructor.systems.vos.wisey.compiler.tests.IShape",
               mShapeInterface->getDestructorFunctionName().c_str());
}

TEST_F(InterfaceTest, defineDestructorFunctionTest) {
  string functionName = mShapeInterface->getDestructorFunctionName();
  EXPECT_EQ(mContext.getModule()->getFunction(functionName), nullptr);
  
  mShapeInterface->defineDestructorFunction(mContext);
  
  EXPECT_NE(mContext.getModule()->getFunction(functionName), nullptr);
}

TEST_F(InterfaceTest, composeDestructorFunctionBodyTest) {
  string functionName = mShapeInterface->getDestructorFunctionName();
  mShapeInterface->defineDestructorFunction(mContext);
  Function* destructor = mContext.getModule()->getFunction(functionName);
  
  EXPECT_EQ(destructor->getBasicBlockList().size(), 0u);
  
  mShapeInterface->composeDestructorFunctionBody(mContext);
  
  EXPECT_GT(destructor->getBasicBlockList().size(), 0u);
  *mStringStream << *destructor;
}

TEST_F(InterfaceTest, circularDependencyDeathTest) {
  Mock::AllowLeak(mThreadVariable);
  Mock::AllowLeak(mMockExpression);
  
  InterfaceTypeSpecifier* parentInterfaceSpecifier = new InterfaceTypeSpecifier(NULL, "IParent");
  InterfaceTypeSpecifier* childInterfaceSpecifier = new InterfaceTypeSpecifier(NULL, "IChild");

  string childFullName = "systems.vos.wisey.compiler.tests.IChild";
  StructType* childStructType = StructType::create(mLLVMContext, childFullName);
  vector<IInterfaceTypeSpecifier*> childParentInterfaces;
  childParentInterfaces.push_back(parentInterfaceSpecifier);
  vector<IObjectElementDeclaration*> childElements;
  Interface* child = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                             childFullName,
                                             childStructType,
                                             childParentInterfaces,
                                             childElements);
  mContext.addInterface(child);
  
  string parentFullName = "systems.vos.wisey.compiler.tests.IParent";
  StructType* parentStructType = StructType::create(mLLVMContext, parentFullName);
  vector<IInterfaceTypeSpecifier*> parentParentInterfaces;
  parentParentInterfaces.push_back(childInterfaceSpecifier);
  vector<IObjectElementDeclaration*> parentElements;
  Interface* parent = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              parentFullName,
                                              parentStructType,
                                              parentParentInterfaces,
                                              parentElements);
  mContext.addInterface(parent);

  EXPECT_EXIT(child->buildMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Circular interface dependency between interfaces "
              "systems.vos.wisey.compiler.tests.IChild and "
              "systems.vos.wisey.compiler.tests.IParent");
}

TEST_F(TestFileSampleRunner, interfaceMethodNotImplmentedDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_not_implmented.yz",
                    1,
                    "Error: Method getArea "
                    "of interface systems.vos.wisey.compiler.tests.IShape is not "
                    "implemented by object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileSampleRunner, interfaceMethodDifferentReturnTypeDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_return_type_doesnot_match.yz",
                    1,
                    "Error: Method getArea "
                    "of interface systems.vos.wisey.compiler.tests.IShape has different "
                    "return type when implmeneted by "
                    "object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileSampleRunner, interfaceMethodDifferentArgumentTypesDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_arguments_dont_match.yz",
                    1,
                    "Error: Method getArea "
                    "of interface systems.vos.wisey.compiler.tests.IShape has different "
                    "argument types when implmeneted by "
                    "object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileSampleRunner, interfaceExceptionsDoNotReconcileDeathTest) {
  expectFailCompile("tests/samples/test_interface_method_exceptions_dont_reconcile.yz",
                    1,
                    "Error: Method getArea of "
                    "object systems.vos.wisey.compiler.tests.MSquare throws an unexpected "
                    "exception of type systems.vos.wisey.compiler.tests.MException\n"
                    "Error: Exceptions thrown by method getArea "
                    "of interface systems.vos.wisey.compiler.tests.IShape "
                    "do not reconcile with exceptions thrown by its implementation "
                    "in object systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileSampleRunner, modelImplmenetingInterfaceDefinitionRunTest) {
  runFile("tests/samples/test_interface_implementation.yz", "90");
}

TEST_F(TestFileSampleRunner, interfaceInheritanceRunTest) {
  runFile("tests/samples/test_level2_inheritance.yz", "235");
}

TEST_F(TestFileSampleRunner, modelWithMultipleInterfaceInheritanceRunTest) {
  runFile("tests/samples/test_model_multiple_interface_inheritance.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceWithMultipleInterfaceInheritanceRunTest) {
  runFile("tests/samples/test_interface_multiple_interface_inheritance.yz", "15");
}

TEST_F(TestFileSampleRunner, interfaceCastToModelRunTest) {
  runFile("tests/samples/test_interface_cast_to_model.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceCastToInterfaceRunTest) {
  runFile("tests/samples/test_interface_cast_to_interface.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceCastToAnotherInterfaceRunTest) {
  runFile("tests/samples/test_interface_cast_to_another_interface.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceAutoCastRunTest) {
  runFile("tests/samples/test_interface_autocast.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceStaticMethodRunTest) {
  runFile("tests/samples/test_interface_static_method.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceStaticMethodComplicatedRunTest) {
  runFile("tests/samples/test_interface_static_method_complicated.yz", "7");
}

TEST_F(TestFileSampleRunner, interfaceObjectAddReferenceForModelRunTest) {
  runFile("tests/samples/test_interface_object_add_reference_for_model.yz", "5");
}

TEST_F(TestFileSampleRunner, interfaceObjectAddReferenceForNodeRunTest) {
  runFile("tests/samples/test_interface_object_add_reference_for_node.yz", "7");
}

TEST_F(TestFileSampleRunner, interfaceStaticMethodThrowsNpeDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_static_method_throws_npe.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.IShape.getValue(tests/samples/test_interface_static_method_throws_npe.yz:7)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_static_method_throws_npe.yz:24)\n");
}

TEST_F(TestFileSampleRunner, interfaceCastToModelCastExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_cast_to_model_cast_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_cast_to_model_cast_exception.yz:47)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.IObject to systems.vos.wisey.compiler.tests.MFoo\n");
}

TEST_F(TestFileSampleRunner, interfaceCastToInterfaceCastExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_cast_to_interface_cast_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MCastException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_cast_to_interface_cast_exception.yz:47)\n"
                               "Details: Can not cast from systems.vos.wisey.compiler.tests.IObject to systems.vos.wisey.compiler.tests.IDevice\n");
}

TEST_F(TestFileSampleRunner, interfaceCircularDependencyRunDeathTest) {
  expectFailCompile("tests/samples/test_circular_dependency_loop.yz",
                    1,
                    "Error: Circular interface dependency between interfaces "
                    "systems.vos.wisey.compiler.tests.IParent and "
                    "systems.vos.wisey.compiler.tests.IChild");
}
