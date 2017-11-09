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
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ConstantDeclaration.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
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
  StructType* mShapeStructType;
  StructType* mIncompleteInterfaceStructType;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  NiceMock<MockExpression>* mMockExpression;
  ConstantDeclaration* mConstantDeclaration;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  InterfaceTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()) {
    TestPrefix::run(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<ModelTypeSpecifier*> exceptions;

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    VariableList methodArguments;
    mBarMethod = new MethodSignatureDeclaration(intSpecifier, "bar", methodArguments, exceptions);
    vector<IObjectElementDeclaration*> objectElementDeclarations;
    objectElementDeclarations.push_back(mBarMethod);
    vector<InterfaceTypeSpecifier*> objectParentInterfaces;
    vector<MethodSignatureDeclaration*> methodDeclarations;
    mObjectInterface = Interface::newInterface(objectFullName,
                                               objectStructType,
                                               objectParentInterfaces,
                                               objectElementDeclarations);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    vector<string> package;
    mObjectInterfaceSpecifier = new InterfaceTypeSpecifier(package, "IObject");
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    mShapeStructType = StructType::create(mLLVMContext, shapeFullName);
    mFooMethod = new MethodSignatureDeclaration(intSpecifier, "foo", methodArguments, exceptions);
    vector<IObjectElementDeclaration*> shapeElements;

    mMockExpression = new NiceMock<MockExpression>();
    ON_CALL(*mMockExpression, printToStream(_,_)).WillByDefault(Invoke(printExpression));
    intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    mConstantDeclaration = new ConstantDeclaration(PUBLIC_ACCESS,
                                                   intSpecifier,
                                                   "MYCONSTANT",
                                                   mMockExpression);
    shapeElements.push_back(mConstantDeclaration);
    shapeElements.push_back(mFooMethod);

    vector<InterfaceTypeSpecifier*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(mObjectInterfaceSpecifier);
    mShapeInterface = Interface::newInterface(shapeFullName,
                                              mShapeStructType,
                                              shapeParentInterfaces,
                                              shapeElements);
    mContext.addInterface(mShapeInterface);
    mShapeInterface->buildMethods(mContext);

    mIncompleteInterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    mIncompleteInterface = Interface::newInterface(shapeFullName,
                                                   mIncompleteInterfaceStructType,
                                                   shapeParentInterfaces,
                                                   shapeElements);
    
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  mShapeInterface->getName());
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
    Value* threadObject = ConstantPointerNull::get(threadController->getLLVMType(mLLVMContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadController));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);
    
    vector<Catch*> catchList;
    FinallyBlock* emptyBlock = new FinallyBlock();
    TryCatchInfo* tryCatchInfo = new TryCatchInfo(mBasicBlock, mBasicBlock, emptyBlock, catchList);
    mContext.getScopes().setTryCatchInfo(tryCatchInfo);
    
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
  EXPECT_STREQ(mShapeInterface->getName().c_str(), "systems.vos.wisey.compiler.tests.IShape");
  EXPECT_STREQ(mShapeInterface->getShortName().c_str(), "IShape");
  EXPECT_EQ(mShapeInterface->getTypeKind(), INTERFACE_TYPE);
  EXPECT_EQ(mShapeInterface->getLLVMType(mLLVMContext), mShapeStructType->getPointerTo());
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

TEST_F(InterfaceTest, getOriginalObjectVTableTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Interface::getOriginalObjectVTable(mContext, nullPointerValue);

  *mStringStream << *mBasicBlock;
  string expected =
    "\nentry:"
    "\n  %0 = bitcast i8* null to i8***"
    "\n  %vtable = load i8**, i8*** %0"
    "\n  %1 = getelementptr i8*, i8** %vtable, i64 0"
    "\n  %unthunkbypointer = load i8*, i8** %1"
    "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
    "\n  %2 = bitcast i8* null to i8*"
    "\n  %3 = getelementptr i8, i8* %2, i64 %unthunkby\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(InterfaceTest, getCastFunctionNameTest) {
  EXPECT_STREQ(mObjectInterface->getCastFunctionName(mShapeInterface).c_str(),
               "cast.systems.vos.wisey.compiler.tests.IObject."
               "to.systems.vos.wisey.compiler.tests.IShape");
}

TEST_F(InterfaceTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->canCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canCastTo(mObjectInterface));
  EXPECT_FALSE(mObjectInterface->canCastTo(NullType::NULL_TYPE));
}

TEST_F(InterfaceTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canAutoCastTo(mObjectInterface));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(NullType::NULL_TYPE));
}

TEST_F(InterfaceTest, printToStreamTest) {
  stringstream stringStream;
  mShapeInterface->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external interface systems.vos.wisey.compiler.tests.IShape\n"
               "  extends\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  int foo();\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(InterfaceTest, fieldDefinitionDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Mock::AllowLeak(mThreadVariable);

  PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  ExpressionList arguments;
  FieldDeclaration* fieldDeclaration = new FieldDeclaration(FieldKind::FIXED_FIELD,
                                                            intSpecifier,
                                                            "mField",
                                                            arguments);
  
  
  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDeclaration*> elements;
  elements.push_back(fieldDeclaration);
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(name, structType, parentInterfaces, elements);
  
  EXPECT_EXIT(interface->buildMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interfaces can not contain fields");
}

TEST_F(InterfaceTest, methodDeclarationDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Mock::AllowLeak(mThreadVariable);

  const PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableList arguments;
  vector<ModelTypeSpecifier*> thrownExceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block);
  MethodDeclaration* methodDeclaration =
  new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                        intSpecifier,
                        "foo",
                        arguments,
                        thrownExceptions,
                        compoundStatement);

  string name = "systems.vos.wisey.compiler.tests.IInterface";
  StructType* structType = StructType::create(mLLVMContext, name);
  vector<IObjectElementDeclaration*> elements;
  elements.push_back(methodDeclaration);
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(name, structType, parentInterfaces, elements);
  
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
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(name, structType, parentInterfaces, elements);
  
  EXPECT_EXIT(interface->buildMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: In interfaces constants should be declared before method signatures");
}

TEST_F(InterfaceTest, incremenetReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mLLVMContext));
  mShapeInterface->incremenetReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8***"
  "\n  %vtable = load i8**, i8*** %0"
  "\n  %1 = getelementptr i8*, i8** %vtable, i64 0"
  "\n  %unthunkbypointer = load i8*, i8** %1"
  "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
  "\n  %2 = sub i64 %unthunkby, 8"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  %4 = getelementptr i8, i8* %3, i64 %2"
  "\n  %5 = bitcast i8* %4 to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %5, i64 1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, decremenetReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mLLVMContext));
  mShapeInterface->decremenetReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8***"
  "\n  %vtable = load i8**, i8*** %0"
  "\n  %1 = getelementptr i8*, i8** %vtable, i64 0"
  "\n  %unthunkbypointer = load i8*, i8** %1"
  "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
  "\n  %2 = sub i64 %unthunkby, 8"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  %4 = getelementptr i8, i8* %3, i64 %2"
  "\n  %5 = bitcast i8* %4 to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %5, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mShapeInterface->getLLVMType(mLLVMContext));
  mShapeInterface->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8***"
  "\n  %vtable = load i8**, i8*** %0"
  "\n  %1 = getelementptr i8*, i8** %vtable, i64 0"
  "\n  %unthunkbypointer = load i8*, i8** %1"
  "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
  "\n  %2 = sub i64 %unthunkby, 8"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.IShape* null to i8*"
  "\n  %4 = getelementptr i8, i8* %3, i64 %2"
  "\n  %5 = bitcast i8* %4 to i64*"
  "\n  %refCounter = load i64, i64* %5\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
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

