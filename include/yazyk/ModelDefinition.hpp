//
//  ModelDefinition.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelDefinition_h
#define ModelDefinition_h

#include "yazyk/Block.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/ModelFieldDeclaration.hpp"

namespace yazyk {
  
/**
 * Represents MODEL definition which is analogous to an immutable class in C++
 */
class ModelDefinition : public IStatement {
  const std::string mName;
  std::vector<ModelFieldDeclaration *>& mFields;
  std::vector<MethodDeclaration *>& mMethods;
  std::vector<std::string>& mInterfaces;
  
public:
  
  ModelDefinition(std::string name,
                  std::vector<ModelFieldDeclaration*>& fields,
                  std::vector<MethodDeclaration *>& methods,
                  std::vector<std::string>& interfaces)
    : mName(name), mFields(fields), mMethods(methods), mInterfaces(interfaces) { }
  
  ~ModelDefinition();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  void processFields(IRGenerationContext& context,
                     Model* model,
                     std::map<std::string, ModelField*>* fields,
                     std::vector<llvm::Type*>& types,
                     int index) const;
  
  void processMethods(IRGenerationContext& context,
                      Model* model,
                      std::map<std::string, Method*>* methods) const;
  
  int processInterfaces(IRGenerationContext& context,
                        std::vector<llvm::Type*>& types) const;
};

} /* namespace yazyk */

#endif /* ModelDefinition_h */
