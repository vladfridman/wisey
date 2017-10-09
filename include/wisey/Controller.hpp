//
//  Controller.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Controller_h
#define Controller_h

#include <llvm/IR/Instructions.h>

#include "wisey/Field.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/Interface.hpp"

namespace wisey {

class ControllerOwner;
  
/**
 * Contains information about a Controller including its fields and methods
 */
class Controller : public IConcreteObjectType {
  std::string mName;
  llvm::StructType* mStructType;
  bool mIsExternal;
  ControllerOwner* mControllerOwner;
  std::vector<Field*> mReceivedFields;
  std::vector<Field*> mInjectedFields;
  std::vector<Field*> mStateFields;
  std::vector<IMethod*> mMethods;
  std::map<std::string, Field*> mFields;
  std::map<std::string, IMethod*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  
public:
  
  ~Controller();
  
  /**
   * static method for controller instantiation
   */
  static Controller* newController(std::string name, llvm::StructType* structType);
  
  /**
   * static method for external controller instantiation
   */
  static Controller* newExternalController(std::string name, llvm::StructType* structType);

  /**
   * Inject an instance of this controller into LLVM code
   */
  llvm::Instruction* inject(IRGenerationContext& context, ExpressionList expressionList) const;
  
  void setFields(std::vector<Field*> fields) override;
  
  void setInterfaces(std::vector<Interface*> interfaces) override;
  
  void setMethods(std::vector<IMethod*> methods) override;
  
  void setStructBodyTypes(std::vector<llvm::Type*> types) override;
  
  Field* findField(std::string fieldName) const override;
  
  std::map<std::string, Field*> getFields() const override;
  
  IMethod* findMethod(std::string methodName) const override;
  
  std::vector<IMethod*> getMethods() const override;
  
  std::string getObjectNameGlobalVariableName() const override;
  
  std::string getName() const override;
  
  std::string getShortName() const override;
  
  llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;

  std::string getVTableName() const override;

  unsigned long getVTableSize() const override;
  
  std::vector<Interface*> getInterfaces() const override;
  
  std::vector<Interface*> getFlattenedInterfaceHierarchy() const override;
  
  std::string getTypeTableName() const override;
  
  const IObjectOwnerType* getOwner() const override;
  
  bool isExternal() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:

  Controller(std::string name, llvm::StructType* structType, bool isExternal);

  void checkArguments(ExpressionList received) const;
  
  void addInterfaceAndItsParents(std::vector<Interface*>& result, Interface* interface) const;

  llvm::Instruction* createMalloc(IRGenerationContext& context) const;
  
  void initializeReceivedFields(IRGenerationContext& context,
                                ExpressionList& controllerInjectorArguments,
                                llvm::Instruction* malloc) const;
  
  void initializeInjectedFields(IRGenerationContext& context,
                                llvm::Instruction* malloc) const;
  
  void initializeStateFields(IRGenerationContext& context,
                             llvm::Instruction* malloc) const;
  
};
  
} /* namespace wisey */

#endif /* Controller_h */
