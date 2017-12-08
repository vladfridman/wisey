//
//  TestInstanceOf.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InstanceOf}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/InstanceOf.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct InstanceOfTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;

  InstanceOfTest() : mLLVMContext(mContext.getLLVMContext()) {
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* shapeStructType = StructType::create(mLLVMContext, shapeFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              shapeFullName,
                                              shapeStructType,
                                              parentInterfaces,
                                              interfaceElements);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectStructType,
                                               parentInterfaces,
                                               interfaceElements);

    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  mShapeInterface->getName());
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       mShapeInterface->getObjectNameGlobalVariableName());

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());

    mBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(InstanceOfTest, callTest) {
  Value* nullPointerValue =
    ConstantPointerNull::get(mObjectInterface->getLLVMType(mLLVMContext));
  InstanceOf::call(mContext, mObjectInterface, nullPointerValue, mShapeInterface);
  
  ASSERT_EQ(1ul, mBlock->size());
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %instanceof = invoke i32 @systems.vos.wisey.compiler.tests.IObject.instanceof("
  "%systems.vos.wisey.compiler.tests.IObject* null, "
  "i8* getelementptr inbounds ([40 x i8], [40 x i8]* "
  "@systems.vos.wisey.compiler.tests.IShape.name, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %0 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %0"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
