//
//  TestIMethodDescriptor.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IMethodDescriptor}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "Argument.hpp"
#include "Method.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IMethodDescriptorTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Method* mMethod;
  Model* mModel;
  Interface* mThreadInterface;
  Controller* mCallStack;
  
public:
  
  IMethodDescriptorTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    wisey::Argument* doubleArgument = new wisey::Argument(PrimitiveTypes::DOUBLE, "argDouble");
    wisey::Argument* charArgument = new wisey::Argument(PrimitiveTypes::CHAR, "argChar");
    std::vector<const wisey::Argument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;

    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* structType = StructType::create(mLLVMContext, "MObject");
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "foo", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "bar", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);

    mMethod = new Method(mModel,
                         "mymethod",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::BOOLEAN,
                         arguments,
                         thrownExceptions,
                         new MethodQualifiers(0),
                         NULL,
                         0);

    mThreadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    mCallStack = mContext.getController(Names::getCallStackControllerFullName(), 0);
  }
};

TEST_F(IMethodDescriptorTest, getLLVMFunctionTypeTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  vector<const wisey::Argument*> arguments;
  vector<const Model*> thrownExceptions;
  arguments.push_back(intArgument);
  MethodSignature method(mModel,
                         "foo",
                         PrimitiveTypes::FLOAT,
                         arguments,
                         thrownExceptions,
                         new MethodQualifiers(0),
                         "systems.vos.wisey.compiler.tests.MObject",
                         0);
  FunctionType* functionType = IMethodDescriptor::getLLVMFunctionType(mContext, &method, mModel, 0);
  
  EXPECT_EQ(functionType->getReturnType(), Type::getFloatTy(mLLVMContext));
  EXPECT_EQ(functionType->getNumParams(), 4u);
  EXPECT_EQ(functionType->getParamType(0), mModel->getLLVMType(mContext));
  EXPECT_EQ(functionType->getParamType(1), mThreadInterface->getLLVMType(mContext));
  EXPECT_EQ(functionType->getParamType(2), mCallStack->getLLVMType(mContext));
  EXPECT_EQ(functionType->getParamType(3), Type::getInt32Ty(mLLVMContext));
}
