//
//  InterfaceDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceDefinition_h
#define InterfaceDefinition_h

#include "wisey/Block.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelFieldDeclaration.hpp"

namespace wisey {
  
/**
 * Represents an INTERFACE definition which is analogous to a pure virtual class in C++
 */
class InterfaceDefinition : public IStatement {
  const std::string mName;
  std::vector<InterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
  std::vector<MethodSignatureDeclaration *> mMethodSignatureDeclarations;
  
public:
  
  InterfaceDefinition(std::string name,
                      std::vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                      std::vector<MethodSignatureDeclaration *> methodSignatureDeclarations) :
  mName(name),
  mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
  mMethodSignatureDeclarations(methodSignatureDeclarations) { }
  
  ~InterfaceDefinition() { };
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  void defineInterfaceTypeName(IRGenerationContext& context, Interface* interface) const;
  
  void defineInstanceOf(IRGenerationContext& context, Interface* interface) const;
  
  llvm::Function* createInstanceOfFunction(IRGenerationContext& context,
                                           Interface* interface) const;
  
  llvm::BitCastInst* composeInstanceOfEntryBlock(IRGenerationContext& context,
                                                 llvm::BasicBlock* entryBlock,
                                                 llvm::BasicBlock* whileCond,
                                                 llvm::Function* function) const;

  llvm::LoadInst* composeInstanceOfWhileConditionBlock(IRGenerationContext& context,
                                                       llvm::BasicBlock* whileCond,
                                                       llvm::BasicBlock* whileBody,
                                                       llvm::BasicBlock* returnFalse,
                                                       llvm::Value* iterator,
                                                       llvm::Value* arrayOfStrings) const;
  
  void composeInstanceOfWhileBodyBlock(IRGenerationContext& context,
                                       llvm::BasicBlock* whileBody,
                                       llvm::BasicBlock* whileCond,
                                       llvm::BasicBlock* returnTrue,
                                       llvm::Value* iterator,
                                       llvm::Value* stringPointer,
                                       llvm::Function* function) const;
  
  void composeReturnFound(IRGenerationContext& context,
                          llvm::BasicBlock* returnFound,
                          llvm::Value* iterator) const;
  
  void composeReturnNotFound(IRGenerationContext& context,
                             llvm::BasicBlock* returnNotFound) const;
};
  
} /* namespace wisey */

#endif /* InterfaceDefinition_h */
