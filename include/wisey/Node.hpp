//
//  Node.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Node_h
#define Node_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/BuilderArgument.hpp"
#include "wisey/Field.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/Method.hpp"

namespace wisey {
  
class Interface;
class NodeOwner;
  
/**
 * Contains information about a node including the llvm::StructType and field information
 */
class Node : public IConcreteObjectType {
  std::string mName;
  NodeOwner* mNodeOwner;
  llvm::StructType* mStructType;
  std::vector<Field*> mFixedFields;
  std::vector<Field*> mStateFields;
  std::map<std::string, Field*> mFields;
  std::vector<Method*> mMethods;
  std::map<std::string, Method*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  
public:
  
  Node(std::string name, llvm::StructType* structType);
  
  ~Node();
  
  /**
   * Set fixed and state fields to the given lists of fields
   */
  void setFields(std::vector<Field*> fixedFields, std::vector<Field*> stateFields);

  /**
   * Set interfaces for this node
   */
  void setInterfaces(std::vector<Interface*> interfaces);
  
  /**
   * Set methods for this controller
   */
  void setMethods(std::vector<Method*> methods);
  
  /**
   * Set the struct field types for the struct that represents this controller
   */
  void setStructBodyTypes(std::vector<llvm::Type*> types);

  /**
   * Returns the difference beteween the set of fixed fields and the fields given as argument
   */
  std::vector<std::string> getMissingFields(std::set<std::string> givenFields) const;

  /**
   * Builds an instance of this node and initializes all fields
   */
  llvm::Instruction* build(IRGenerationContext& context,
                           const BuilderArgumentList& builderArgumentList) const;

  Field* findField(std::string fieldName) const override;
  
  std::map<std::string, Field*> getFields() const override;
  
  Method* findMethod(std::string methodName) const override;
  
  std::vector<Method*> getMethods() const override;
  
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
  
  IObjectOwnerType* getOwner() const override;
  
private:
  
  void addInterfaceAndItsParents(std::vector<Interface*>& result, Interface* interface) const;
  
  void checkArguments(const BuilderArgumentList& builderArgumentList) const;

  void checkArgumentsAreWellFormed(const BuilderArgumentList& builderArgumentList) const;

  void checkAllFieldsAreSet(const BuilderArgumentList& builderArgumentList) const;

  llvm::Instruction* createMalloc(IRGenerationContext& context) const;

  void initializeFixedFields(IRGenerationContext& context,
                             const BuilderArgumentList& builderArgumentList,
                             llvm::Instruction* malloc) const;

  void initializeStateFields(IRGenerationContext& context,
                             llvm::Instruction* malloc) const;

};
  
} /* namespace wisey */

#endif /* Node_h */