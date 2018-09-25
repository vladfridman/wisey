//
//  EssentialFunctions.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "AdjustReferenceCountFunction.hpp"
#include "CastObjectFunction.hpp"
#include "CheckArrayNotReferencedFunction.hpp"
#include "CheckCastToObjectFunction.hpp"
#include "DestroyNativeObjectFunction.hpp"
#include "DestroyOwnerArrayFunction.hpp"
#include "DestroyObjectOwnerFunction.hpp"
#include "DestroyPrimitiveArrayFunction.hpp"
#include "DestroyReferenceArrayFunction.hpp"
#include "GetOriginalObjectFunction.hpp"
#include "GetOriginalObjectNameFunction.hpp"
#include "IRGenerationContext.hpp"
#include "InstanceOfFunction.hpp"
#include "IsObjectFunction.hpp"
#include "EssentialFunctions.hpp"
#include "ReturnStatement.hpp"
#include "ThrowArrayIndexExceptionFunction.hpp"
#include "ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

EssentialFunctions::~EssentialFunctions() {
}

void EssentialFunctions::generateIR(IRGenerationContext& context) const {
  AdjustReferenceCountFunction::get(context);
  CastObjectFunction::get(context);
  CheckArrayNotReferencedFunction::get(context);
  CheckCastToObjectFunction::get(context);
  DestroyNativeObjectFunction::get(context);
  DestroyOwnerArrayFunction::get(context);
  DestroyObjectOwnerFunction::get(context);
  DestroyPrimitiveArrayFunction::get(context);
  DestroyReferenceArrayFunction::get(context);
  GetOriginalObjectFunction::get(context);
  GetOriginalObjectNameFunction::get(context);
  InstanceOfFunction::get(context);
  IsObjectFunction::get(context);
  ThrowArrayIndexExceptionFunction::get(context);
  ThrowReferenceCountExceptionFunction::get(context);
}
