//
//  ModelBuilderArgument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelBuilderArgument_h
#define ModelBuilderArgument_h

namespace wisey {

class IExpression;
class Model;
  
/**
 * Represents one argument in the model builder pattern.
 *
 * The model builder is of the form:
 *
 * ModelType modelA = builder(ModelType).withField(1).builder();
 *
 * withField(1) represents one model builder argument
 */
class ModelBuilderArgument {
  
  std::string mFieldSpecifier;
  IExpression* mFieldExpression;
  
public:
  
  ModelBuilderArgument(std::string fieldSpecifier, IExpression* fieldExpression)
    : mFieldSpecifier(fieldSpecifier), mFieldExpression(fieldExpression) { }
  
  ~ModelBuilderArgument();
  
  bool checkArgument(const Model* model);
  
  /**
   * Derives field name from builder argument by converting 'widthFieldA' to 'fieldA'
   */
  std::string deriveFieldName() const;
  
  /**
   * Computes argument value
   */
  llvm::Value* getValue(IRGenerationContext& context) const;
  
  /**
   * Tells argument type
   */
  const IType* getType(IRGenerationContext& context) const;
  
private:
  
};
  
/**
 * Represents a list of model builder arguments
 */
typedef std::vector<ModelBuilderArgument*> ModelBuilderArgumentList;

} /* namespace wisey */

#endif /* ModelBuilderArgument_h */
