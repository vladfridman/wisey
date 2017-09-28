//
//  Model.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/Field.hpp"
#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {

class Interface;
class ModelOwner;
  
/**
 * Contains information about a model including the llvm::StructType and field information
 */
class Model : public IBuildableConcreteObjectType {
  std::string mName;
  ModelOwner* mModelOwner;
  llvm::StructType* mStructType;
  std::map<std::string, Field*> mFields;
  std::vector<IMethod*> mMethods;
  std::map<std::string, IMethod*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  
public:
  
  Model(std::string name, llvm::StructType* structType);
  
  ~Model();
  
  /**
   * Gets a set of field names and returns the ones that are missing
   */
  std::vector<std::string> getMissingFields(std::set<std::string> givenFields) const;
  
  /**
   * Returns the name of the global variable containing RTTI type representing this model
   */
  std::string getRTTIVariableName() const;
  
  /**
   * Tells whether this model implements a given interface
   */
  bool doesImplmentInterface(Interface* interface) const;
  
  /**
   * Creates a global variable with type description for this model in RTTI format
   */
  void createRTTI(IRGenerationContext& context) const;
  
  /**
   * Returns the size of this object in bytes
   */
  llvm::Value* getSize(IRGenerationContext& context) const;
  
  void setFields(std::vector<Field*> fields) override;
  
  void setInterfaces(std::vector<Interface*> interfaces) override;
  
  void setMethods(std::vector<IMethod*> methods) override;
  
  void setStructBodyTypes(std::vector<llvm::Type*> types) override;
  
  llvm::Instruction* build(IRGenerationContext& context,
                           const ObjectBuilderArgumentList&
                           ObjectBuilderArgumentList) const override;
  
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
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  void addInterfaceAndItsParents(std::vector<Interface*>& result, Interface* interface) const;
  
  void checkArguments(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const;
  
  void checkArgumentsAreWellFormed(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const;
  
  void checkAllFieldsAreSet(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const;
  
  llvm::Instruction* createMalloc(IRGenerationContext& context) const;

  void initializeFields(IRGenerationContext& context,
                        const ObjectBuilderArgumentList& ObjectBuilderArgumentList,
                        llvm::Instruction* malloc) const;
};

} /* namespace wisey */

#endif /* Model_h */
