//
//  TestObjectAllocator.cpp
//  runtests
//
//  Created by Vladimir Fridman on 6/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectAllocator}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectAllocator.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ObjectAllocatorTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  ObjectAllocator* mObjectAllocator;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  NiceMock<MockExpression>* mPoolExpression;
  ModelTypeSpecifier* mModelTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  ObjectAllocatorTest() :
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()),
  mPoolExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    mModelTypeSpecifier = new ModelTypeSpecifier(NULL, "MShape", 0);
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(llvmContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType *structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mWidth", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mHeight", 0));
    mModel = Model::newPooledModel(AccessLevel::PUBLIC_ACCESS,
                                   modelFullName,
                                   structType,
                                   mContext.getImportProfile(),
                                   0);
    mModel->setFields(mContext, fields, 1u);
    mContext.addModel(mModel, 0);
    Value* fieldValue1 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(fieldValue1));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mField1Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument1));
    Value* fieldValue2 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(fieldValue2));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mField2Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument2));
    
    IConcreteObjectType::declareTypeNameGlobal(mContext, mModel);
    IConcreteObjectType::declareVTable(mContext, mModel);
    
    const Controller* cMemoryPool = mContext.getController(Names::getCMemoryPoolFullName(), 0);
    Value* pool = ConstantPointerNull::get(cMemoryPool->getLLVMType(mContext));
    ON_CALL(*mPoolExpression, generateIR(_, _)).WillByDefault(Return(pool));
    ON_CALL(*mPoolExpression, getType(_)).WillByDefault(Return(cMemoryPool));
    ON_CALL(*mPoolExpression, printToStream(_, _)).WillByDefault(Invoke(printPoolExpression));

    string argumentSpecifier1("withWidth");
    ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                                 mField1Expression);
    string argumentSpecifier2("withHeight");
    ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                                 mField2Expression);
    ObjectBuilderArgumentList argumentList;
    argumentList.push_back(argument1);
    argumentList.push_back(argument2);
    mObjectAllocator = new ObjectAllocator(mModelTypeSpecifier, argumentList, mPoolExpression, 0);
    
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ObjectAllocatorTest() {
    delete mField1Expression;
    delete mField2Expression;
    delete mPoolExpression;
    delete mStringStream;
  }
  
  static void printBuilderArgument1(IRGenerationContext& context, iostream& stream) {
    stream << "3";
  }
  
  static void printBuilderArgument2(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
  
  static void printPoolExpression(IRGenerationContext& context, iostream& stream) {
    stream << "pool";
  }
};

TEST_F(ObjectAllocatorTest, getTypeTest) {
  ObjectBuilderArgumentList argumentList;
  ObjectAllocator allocator(mModelTypeSpecifier, argumentList, mPoolExpression, 0);
  
  EXPECT_EQ(allocator.getType(mContext), mModel->getOwner());
}

TEST_F(ObjectAllocatorTest, isConstantTest) {
  EXPECT_FALSE(mObjectAllocator->isConstant());
}

TEST_F(ObjectAllocatorTest, isAssignableTest) {
  EXPECT_FALSE(mObjectAllocator->isAssignable());
}

TEST_F(ObjectAllocatorTest, printToStreamTest) {
  stringstream stringStream;
  mObjectAllocator->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("allocator(systems.vos.wisey.compiler.tests.MShape)"
               ".withWidth(3).withHeight(5).onPool(pool)",
               stringStream.str().c_str());
}
TEST_F(TestFileRunner, allocateBuildableDeathRunTest) {
  expectFailCompile("tests/samples/test_allocate_buildable.yz",
                    1,
                    "tests/samples/test_allocate_buildable.yz\\(18\\): Error: "
                    "Object systems.vos.wisey.compiler.tests.MCar can not be allocated on a memory pool beause it is not marked with onPool qualifier, it should be built using builder command");
}
