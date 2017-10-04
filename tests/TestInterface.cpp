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

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct InterfaceTest : public Test {
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  Interface* mIncompleteInterface;
  InterfaceTypeSpecifier* mObjectInterfaceSpecifier;
  MethodSignatureDeclaration* mFooMethod;
  MethodSignatureDeclaration* mBarMethod;
  StructType* mShapeStructType;
  StructType* mIncompleteInterfaceStructType;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  InterfaceTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::run(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<ModelTypeSpecifier*> exceptions;

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    VariableList methodArguments;
    mBarMethod = new MethodSignatureDeclaration(intSpecifier, "bar", methodArguments, exceptions);
    vector<MethodSignatureDeclaration*> objectMethodSignatures;
    objectMethodSignatures.push_back(mBarMethod);
    vector<InterfaceTypeSpecifier*> objectParentInterfaces;
    vector<MethodSignatureDeclaration*> methodDeclarations;
    mObjectInterface = new Interface(objectFullName,
                                     objectStructType,
                                     objectParentInterfaces,
                                     objectMethodSignatures);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    vector<string> package;
    mObjectInterfaceSpecifier = new InterfaceTypeSpecifier(package, "IObject");
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    mShapeStructType = StructType::create(mLLVMContext, shapeFullName);
    mFooMethod = new MethodSignatureDeclaration(intSpecifier, "foo", methodArguments, exceptions);
    vector<MethodSignatureDeclaration*> shapeMethodSignatures;
    shapeMethodSignatures.push_back(mFooMethod);
    vector<InterfaceTypeSpecifier*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(mObjectInterfaceSpecifier);
    mShapeInterface = new Interface(shapeFullName,
                                    mShapeStructType,
                                    shapeParentInterfaces,
                                    shapeMethodSignatures);
    mContext.addInterface(mShapeInterface);
    mShapeInterface->buildMethods(mContext);

    mIncompleteInterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    mIncompleteInterface = new Interface(shapeFullName,
                                         mIncompleteInterfaceStructType,
                                         shapeParentInterfaces,
                                         shapeMethodSignatures);
    
    Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
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
    
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~InterfaceTest() {
    delete mStringStream;
  }
};

TEST_F(InterfaceTest, interfaceInstantiationTest) {
  EXPECT_STREQ(mShapeInterface->getName().c_str(), "systems.vos.wisey.compiler.tests.IShape");
  EXPECT_STREQ(mShapeInterface->getShortName().c_str(), "IShape");
  EXPECT_EQ(mShapeInterface->getTypeKind(), INTERFACE_TYPE);
  EXPECT_EQ(mShapeInterface->getLLVMType(mLLVMContext),
            mShapeStructType->getPointerTo()->getPointerTo());
  ASSERT_NE(mShapeInterface->getOwner(), nullptr);
  EXPECT_EQ(mShapeInterface->getOwner()->getObject(), mShapeInterface);
}

TEST_F(InterfaceTest, findMethodTest) {
  EXPECT_NE(mShapeInterface->findMethod("foo"), nullptr);
  EXPECT_STREQ(mShapeInterface->findMethod("foo")->getName().c_str(), "foo");
  EXPECT_NE(mShapeInterface->findMethod("bar"), nullptr);
  EXPECT_EQ(mShapeInterface->findMethod("zzz"), nullptr);
}

TEST_F(InterfaceTest, getMethodIndexTest) {
  EXPECT_EQ(mShapeInterface->getMethodIndex(mShapeInterface->findMethod("foo")), 0u);
  EXPECT_EQ(mShapeInterface->getMethodIndex(mShapeInterface->findMethod("bar")), 1u);
}

TEST_F(InterfaceTest, getMethodIndexDeathTest) {
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

TEST_F(InterfaceTest, getOriginalObjectTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)
                                                     ->getPointerTo()->getPointerTo());
  Interface::getOriginalObject(mContext, nullPointerValue);

  *mStringStream << *mBlock;
  string expected =
    "\nentry:"
    "\n  %0 = load i8*, i8** null"
    "\n  %1 = bitcast i8* %0 to i8***"
    "\n  %vtable = load i8**, i8*** %1"
    "\n  %2 = getelementptr i8*, i8** %vtable, i64 0"
    "\n  %unthunkbypointer = load i8*, i8** %2"
    "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
    "\n  %3 = bitcast i8* %0 to i8*"
    "\n  %4 = getelementptr i8, i8* %3, i64 %unthunkby\n";

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
  
  EXPECT_STREQ("interface systems.vos.wisey.compiler.tests.IShape\n"
               "  extends\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "  int foo();\n"
               "}\n",
               stringStream.str().c_str());
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

