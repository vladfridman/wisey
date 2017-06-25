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
  
};
  
} /* namespace wisey */

#endif /* Node_h */
