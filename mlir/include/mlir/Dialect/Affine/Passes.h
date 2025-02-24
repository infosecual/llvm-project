//===- Passes.h - Pass Entrypoints ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This header file defines a set of transforms specific for the AffineOps
// dialect.
//
//===----------------------------------------------------------------------===//

#ifndef MLIR_DIALECT_AFFINE_PASSES_H
#define MLIR_DIALECT_AFFINE_PASSES_H

#include "mlir/Pass/Pass.h"
#include <limits>

namespace mlir {

class AffineForOp;

/// Creates a simplification pass for affine structures (maps and sets). In
/// addition, this pass also normalizes memrefs to have the trivial (identity)
/// layout map.
std::unique_ptr<OperationPass<FuncOp>> createSimplifyAffineStructuresPass();

/// Creates a loop invariant code motion pass that hoists loop invariant
/// operations out of affine loops.
std::unique_ptr<OperationPass<FuncOp>>
createAffineLoopInvariantCodeMotionPass();

/// Creates a pass to convert all parallel affine.for's into 1-d affine.parallel
/// ops.
std::unique_ptr<OperationPass<FuncOp>> createAffineParallelizePass();

/// Apply normalization transformations to affine loop-like ops.
std::unique_ptr<OperationPass<FuncOp>> createAffineLoopNormalizePass();

/// Performs packing (or explicit copying) of accessed memref regions into
/// buffers in the specified faster memory space through either pointwise copies
/// or DMA operations.
std::unique_ptr<OperationPass<FuncOp>> createAffineDataCopyGenerationPass(
    unsigned slowMemorySpace, unsigned fastMemorySpace,
    unsigned tagMemorySpace = 0, int minDmaTransferSize = 1024,
    uint64_t fastMemCapacityBytes = std::numeric_limits<uint64_t>::max());
/// Overload relying on pass options for initialization.
std::unique_ptr<OperationPass<FuncOp>> createAffineDataCopyGenerationPass();

/// Creates a pass to replace affine memref accesses by scalars using store to
/// load forwarding and redundant load elimination; consequently also eliminate
/// dead allocs.
std::unique_ptr<OperationPass<FuncOp>> createAffineScalarReplacementPass();

/// Creates a pass to perform tiling on loop nests.
std::unique_ptr<OperationPass<FuncOp>>
createLoopTilingPass(uint64_t cacheSizeBytes);
/// Overload relying on pass options for initialization.
std::unique_ptr<OperationPass<FuncOp>> createLoopTilingPass();

/// Creates a loop unrolling pass with the provided parameters.
/// 'getUnrollFactor' is a function callback for clients to supply a function
/// that computes an unroll factor - the callback takes precedence over unroll
/// factors supplied through other means. If -1 is passed as the unrollFactor
/// and no callback is provided, anything passed from the command-line (if at
/// all) or the default unroll factor is used (LoopUnroll:kDefaultUnrollFactor).
std::unique_ptr<OperationPass<FuncOp>> createLoopUnrollPass(
    int unrollFactor = -1, bool unrollUpToFactor = false,
    bool unrollFull = false,
    const std::function<unsigned(AffineForOp)> &getUnrollFactor = nullptr);

/// Creates a loop unroll jam pass to unroll jam by the specified factor. A
/// factor of -1 lets the pass use the default factor or the one on the command
/// line if provided.
std::unique_ptr<OperationPass<FuncOp>>
createLoopUnrollAndJamPass(int unrollJamFactor = -1);

/// Creates a pass to vectorize loops, operations and data types using a
/// target-independent, n-D super-vector abstraction.
std::unique_ptr<OperationPass<FuncOp>>
createSuperVectorizePass(ArrayRef<int64_t> virtualVectorSize);
/// Overload relying on pass options for initialization.
std::unique_ptr<OperationPass<FuncOp>> createSuperVectorizePass();

//===----------------------------------------------------------------------===//
// Registration
//===----------------------------------------------------------------------===//

/// Generate the code for registering passes.
#define GEN_PASS_REGISTRATION
#include "mlir/Dialect/Affine/Passes.h.inc"

} // namespace mlir

#endif // MLIR_DIALECT_AFFINE_PASSES_H
