/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_RUNTIME_WAVM_IMPL_WAVM_MEMORY_PROVIDER_HPP
#define KAGOME_CORE_RUNTIME_WAVM_IMPL_WAVM_MEMORY_PROVIDER_HPP

#include "runtime/memory_provider.hpp"

namespace kagome::runtime::wavm {

  class IntrinsicModuleInstance;
  class MemoryImpl;
  class CompartmentWrapper;

  class WavmMemoryProvider final : public MemoryProvider {
   public:
    WavmMemoryProvider(
        std::shared_ptr<IntrinsicModuleInstance> intrinsic_module,
        std::shared_ptr<const CompartmentWrapper> compartment_wrapper);

    boost::optional<runtime::Memory &> getCurrentMemory() const override;
    outcome::result<void> resetMemory(WasmSize heap_base) override;

   private:
    // it contains the memory itself
    std::shared_ptr<IntrinsicModuleInstance> intrinsic_module_;
    std::shared_ptr<Memory> current_memory_;
    std::shared_ptr<const CompartmentWrapper> compartment_;
  };

}  // namespace kagome::runtime::wavm

#endif  // KAGOME_CORE_RUNTIME_WAVM_IMPL_WAVM_MEMORY_PROVIDER_HPP
