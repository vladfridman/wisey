//
//  TestInterface.cpp
//  Yazyk
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
#include "yazyk/Interface.hpp"
#include "yazyk/MethodSignature.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct InterfaceTest : public Test {
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  MethodSignature* mMethodSignature;
  StructType* mShapeStructType;
  Field* mWidthField;
  Field* mHeightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  InterfaceTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> objectTypes;
    StructType* objectStructType = StructType::create(mLLVMContext, "IObject");
    objectStructType->setBody(objectTypes);
    vector<MethodSignature*> objectMethodSignatures;
    vector<Interface*> objectParentInterfaces;
    mObjectInterface = new Interface("IObject",
                                     objectStructType,
                                     objectParentInterfaces,
                                     objectMethodSignatures);

    vector<Type*> shapeTypes;
    mShapeStructType = StructType::create(mLLVMContext, "IShape");
    mShapeStructType->setBody(shapeTypes);
    vector<MethodArgument*> shapeMethodArguments;
    mMethodSignature = new MethodSignature("foo",
                                           AccessLevel::PUBLIC_ACCESS,
                                           PrimitiveTypes::INT_TYPE,
                                           shapeMethodArguments,
                                           0);
    vector<MethodSignature*> shapeMethodSignatures;
    shapeMethodSignatures.push_back(mMethodSignature);
    vector<Interface*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(mObjectInterface);
    mShapeInterface = new Interface("IShape",
                                    mShapeStructType,
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

    vector<Type*> argumentTypes;
    argumentTypes.push_back(mObjectInterface->getLLVMType(mLLVMContext));
    argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
    Type* llvmReturnType = PrimitiveTypes::BOOLEAN_TYPE->getLLVMType(mLLVMContext);
    FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          mObjectInterface->getInstanceOfFunctionName(),
                                          mContext.getModule());

    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    function = Function::Create(functionType,
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
  EXPECT_STREQ(mShapeInterface->getName().c_str(), "IShape");
  EXPECT_EQ(mShapeInterface->getTypeKind(), INTERFACE_TYPE);
  EXPECT_EQ(mShapeInterface->getLLVMType(mLLVMContext), mShapeStructType->getPointerTo());
}

TEST_F(InterfaceTest, findMethodTest) {
  EXPECT_EQ(mShapeInterface->findMethod("foo"), mMethodSignature);
  EXPECT_EQ(mShapeInterface->findMethod("bar"), nullptr);
}

TEST_F(InterfaceTest, getObjectNameGlobalVariableNameTest) {
  EXPECT_STREQ(mShapeInterface->getObjectNameGlobalVariableName().c_str(), "interface.IShape.name");
}

TEST_F(InterfaceTest, getInstanceOfFunctionNameTest) {
  EXPECT_STREQ(mShapeInterface->getInstanceOfFunctionName().c_str(), "interface.IShape.instanceof");
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

TEST_F(InterfaceTest, getOriginalObjectTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Interface::getOriginalObject(mContext, nullPointerValue);

  ASSERT_EQ(7ul, mBlock->size());
  *mStringStream << *mBlock;
  string expected =
    "\nentry:"
    "\n  %0 = bitcast i8* null to i8***"
    "\n  %vtable = load i8**, i8*** %0"
    "\n  %unthunkentry = getelementptr i8*, i8** %vtable, i64 0"
    "\n  %unthunkbypointer = load i8*, i8** %unthunkentry"
    "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
    "\n  %1 = bitcast i8* null to i8*"
    "\n  %this.ptr = getelementptr i8, i8* %1, i64 %unthunkby\n";

  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
}

TEST_F(InterfaceTest, callInstanceOfTest) {
  Value* nullPointerValue =
    ConstantPointerNull::get((PointerType*) mObjectInterface->getLLVMType(mLLVMContext));
  mObjectInterface->callInstanceOf(mContext, nullPointerValue, mShapeInterface);
  
  ASSERT_EQ(1ul, mBlock->size());
  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %instanceof = call i1 @interface.IObject.instanceof(%IObject* null, "
  "i8* getelementptr inbounds ([7 x i8], [7 x i8]* @interface.IShape.name, i32 0, i32 0))\n";
  
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
}

TEST_F(InterfaceTest, getCastFunctionNameTest) {
  EXPECT_STREQ(mObjectInterface->getCastFunctionName(mShapeInterface).c_str(),
               "cast.IObject.to.IShape");
}

TEST_F(InterfaceTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->canCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canCastTo(mObjectInterface));
}

TEST_F(InterfaceTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mObjectInterface->canAutoCastTo(mShapeInterface));
  EXPECT_TRUE(mShapeInterface->canAutoCastTo(mObjectInterface));
}

TEST_F(TestFileSampleRunner, interfaceMethodNotImplmentedDeathTest) {
  expectFailIRGeneration("tests/samples/test_interface_method_not_implmented.yz",
                         1,
                         "Error: Method 'getArea' of interface 'IShape' is not "
                         "implemented by object 'MSquare'");
}

TEST_F(TestFileSampleRunner, interfaceMethodDifferentReturnTypeDeathTest) {
  expectFailIRGeneration("tests/samples/test_interface_method_return_type_doesnot_match.yz",
                         1,
                         "Error: Method 'getArea' of interface 'IShape' has different "
                         "return type when implmeneted by object 'MSquare'");
}

TEST_F(TestFileSampleRunner, interfaceMethodDifferentArgumentTypesDeathTest) {
  expectFailIRGeneration("tests/samples/test_interface_method_arguments_dont_match.yz",
                         1,
                         "Error: Method 'getArea' of interface 'IShape' has different "
                         "argument types when implmeneted by object 'MSquare'");
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

