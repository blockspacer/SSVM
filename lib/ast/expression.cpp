// SPDX-License-Identifier: Apache-2.0
#include "common/ast/expression.h"

namespace SSVM {
namespace AST {

/// Load to construct Expression node. See "include/common/ast/expression.h".
Expect<void> Expression::loadBinary(FileMgr &Mgr) {
  /// Read opcode until the End code.
  while (true) {
    Instruction::OpCode Code;

    /// Read the opcode and check if error.
    if (auto Res = Mgr.readByte()) {
      Code = static_cast<Instruction::OpCode>(*Res);
    } else {
      return Unexpect(Res);
    }

    /// When reach end, this expression is ended.
    if (Code == Instruction::OpCode::End)
      break;

    /// Create the instruction node and load contents.
    std::unique_ptr<Instruction> NewInst;
    if (auto Res = makeInstructionNode(Code)) {
      NewInst = std::move(*Res);
    } else {
      return Unexpect(Res);
    }
    if (auto Res = NewInst->loadBinary(Mgr)) {
      Instrs.push_back(std::move(NewInst));
    } else {
      return Unexpect(Res);
    }
  }

  return {};
}

} // namespace AST
} // namespace SSVM
