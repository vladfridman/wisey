//
//  Controller.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Controller_h
#define Controller_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/Field.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IInjectable.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/Interface.hpp"

namespace wisey {

class ControllerOwner;
  
/**
 * Contains information about a Controller including its fields and methods
 */
class Controller : public IConcreteObjectType, public IInjectable {
  std::string mName;
  llvm::StructType* mStructType;
  bool mIsExternal;
  ControllerOwner* mControllerOwner;
  std::vector<Field*> mReceivedFields;
  std::vector<Field*> mInjectedFields;
  std::vector<Field*> mStateFields;
  std::vector<IMethod*> mMethods;
  std::map<std::string, Field*> mFields;
  std::vector<Field*> mFieldsOrdered;
  std::map<Field*, unsigned long> mFieldIndexes;
  std::map<std::string, IMethod*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  std::vector<Constant*> mConstants;
  std::map<std::string, Constant*> mNameToConstantMap;
  ImportProfile* mImportProfile;
  
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

  llvm::Instruction* inject(IRGenerationContext& context,
                            const InjectionArgumentList injectionArgumentList,
                            int line) const override;
  
  void setFields(std::vector<Field*> fields, unsigned long startIndex) override;
  
  void setInterfaces(std::vector<Interface*> interfaces) override;
  
  void setMethods(std::vector<IMethod*> methods) override;
  
  void setStructBodyTypes(std::vector<llvm::Type*> types) override;

  void setConstants(std::vector<Constant*> constants) override;
  
  std::vector<Constant*> getConstants() const override;

  Field* findField(std::string fieldName) const override;
  
  unsigned long getFieldIndex(Field* field) const override;
  
  std::vector<Field*> getFields() const override;
  
  IMethod* findMethod(std::string methodName) const override;
  
  Constant* findConstant(std::string constantName) const override;
  
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
                      const IType* toType,
                      int line) const override;

  std::string getVTableName() const override;

  unsigned long getVTableSize() const override;
  
  std::vector<Interface*> getInterfaces() const override;
  
  std::vector<Interface*> getFlattenedInterfaceHierarchy() const override;
  
  std::string getTypeTableName() const override;
  
  const IObjectOwnerType* getOwner() const override;
  
  bool isExternal() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  void incremenetReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;

  void decremenetReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;

  llvm::Value* getReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
  
  void setImportProfile(ImportProfile* importProfile) override;
  
  ImportProfile* getImportProfile() const override;

private:

  Controller(std::string name, llvm::StructType* structType, bool isExternal);

  void checkArguments(const InjectionArgumentList& injectionArgumentList) const;

  void checkArgumentsAreWellFormed(const InjectionArgumentList& injectionArgumentList) const;
  
  void checkAllFieldsAreSet(const InjectionArgumentList& injectionArgumentList) const;

  std::vector<std::string> getMissingReceivedFields(std::set<std::string> givenFields) const;

  llvm::Instruction* createMalloc(IRGenerationContext& context) const;
  
  void initializeReceivedFields(IRGenerationContext& context,
                                const InjectionArgumentList& controllerInjectorArguments,
                                llvm::Instruction* malloc,
                                int line) const;
  
  void initializeInjectedFields(IRGenerationContext& context,
                                llvm::Instruction* malloc,
                                int line) const;
  
  void initializeStateFields(IRGenerationContext& context,
                             llvm::Instruction* malloc) const;
  
};
  
} /* namespace wisey */

#endif /* Controller_h */
