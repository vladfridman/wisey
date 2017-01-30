//
//  ModelBuilderArgument.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelBuilderArgument_h
#define ModelBuilderArgument_h

#include "yazyk/IExpression.hpp"

namespace yazyk {

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
  
  std::string& mFieldSpecifier;
  IExpression& mFieldValue;
  
public:
  
  ModelBuilderArgument(std::string& fieldSpecifier, IExpression& fieldValue)
  : mFieldSpecifier(fieldSpecifier), mFieldValue(fieldValue) { }
  
  ~ModelBuilderArgument() { }
  
  bool checkArgument();
};
  
/**
 * Represents a list of model builder arguments
 */
typedef std::vector<ModelBuilderArgument*> ModelBuilderArgumentList;

} /* namespace yazyk */

#endif /* ModelBuilderArgument_h */
