#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/TypeBuilder.h>

#include "yazyk/Block.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/node.hpp"
#include "yazyk/log.hpp"
#include "yazyk/TypeIdentifier.hpp"
#include "y.tab.h"

using namespace llvm;
using namespace std;

namespace yazyk {

/* -- Code Generation -- */

Value* Integer::generateIR(IRGenerationContext& context) {
  return ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), value, true);
}
  
} /* namespace yazyk */
