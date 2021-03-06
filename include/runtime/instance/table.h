// SPDX-License-Identifier: Apache-2.0
//===-- ssvm/runtime/instance/table.h - Table Instance definition ---------===//
//
// Part of the SSVM Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the table instance definition in store manager.
///
//===----------------------------------------------------------------------===//
#pragma once

#include "common/ast/type.h"
#include "common/errcode.h"
#include "common/types.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace SSVM {
namespace Runtime {
namespace Instance {

class TableInstance {
public:
  TableInstance() = delete;
  TableInstance(const ElemType &Elem, const AST::Limit &Lim)
      : Type(Elem), HasMaxSize(Lim.hasMax()), MinSize(Lim.getMin()),
        MaxSize(Lim.getMax()), FuncElem(MinSize), FuncElemInit(MinSize, false) {
  }
  virtual ~TableInstance() = default;

  /// Getter of element type.
  ElemType getElementType() const { return Type; }

  /// Getter of limit definition.
  bool getHasMax() const { return HasMaxSize; }

  /// Getter of limit definition.
  uint32_t getMin() const { return MinSize; }

  /// Getter of limit definition.
  uint32_t getMax() const { return MaxSize; }

  /// Set the function index initialization list.
  Expect<void> setInitList(const uint32_t Offset,
                           const std::vector<uint32_t> &Addrs) {
    if (Offset + Addrs.size() > MinSize) {
      return Unexpect(ErrCode::UndefinedElement);
    }
    std::copy(Addrs.begin(), Addrs.end(), FuncElem.begin() + Offset);
    std::fill(FuncElemInit.begin() + Offset,
              FuncElemInit.begin() + Offset + Addrs.size(), true);
    return {};
  }

  /// Check is out of bound.
  bool checkAccessBound(const uint32_t Offset) {
    return (Offset > MinSize) ? false : true;
  }

  /// Get the elem address.
  Expect<uint32_t> getElemAddr(const uint32_t Idx) const {
    if (Idx >= FuncElem.size()) {
      return Unexpect(ErrCode::UndefinedElement);
    }
    if (Symbol) {
      return Symbol[Idx];
    } else {
      if (FuncElemInit[Idx]) {
        return FuncElem[Idx];
      } else {
        return Unexpect(ErrCode::UninitializedElement);
      }
    }
  }

  /// Getter of symbol
  void *getSymbol() const { return Symbol; }
  /// Setter of symbol
  void setSymbol(void *S) { Symbol = reinterpret_cast<uint32_t *>(S); }

private:
  /// \name Data of table instance.
  /// @{
  const ElemType Type;
  const bool HasMaxSize;
  const uint32_t MinSize = 0;
  const uint32_t MaxSize = 0;
  std::vector<uint32_t> FuncElem;
  std::vector<bool> FuncElemInit;
  uint32_t *Symbol = nullptr;
  /// @}
};

} // namespace Instance
} // namespace Runtime
} // namespace SSVM
