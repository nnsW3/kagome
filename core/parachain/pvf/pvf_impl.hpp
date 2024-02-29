/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "parachain/pvf/pvf.hpp"

#include <thread>

#include "crypto/sr25519_provider.hpp"
#include "log/logger.hpp"
#include "runtime/runtime_api/parachain_host.hpp"
#include "runtime/runtime_properties_cache.hpp"

namespace boost::asio {
  class io_context;
}  // namespace boost::asio

namespace libp2p::basic {
  class Scheduler;
}  // namespace libp2p::basic

namespace kagome::application {
  class AppConfiguration;
  class AppStateManager;
}  // namespace kagome::application

namespace kagome::blockchain {
  class BlockTree;
}

namespace kagome::runtime {
  class ModuleInstance;
  class ModuleFactory;
  class Executor;
  class RuntimeContextFactory;
  class RuntimeInstancesPool;
}  // namespace kagome::runtime

namespace kagome::parachain {
  enum class PvfError {
    // NO_DATA conflicted with <netdb.h>
    NO_PERSISTED_DATA = 1,
    POV_SIZE,
    POV_HASH,
    CODE_HASH,
    SIGNATURE,
    HEAD_HASH,
    COMMITMENTS_HASH,
    OUTPUTS,
    PERSISTED_DATA_HASH,
    NO_CODE,
  };
}  // namespace kagome::parachain

OUTCOME_HPP_DECLARE_ERROR(kagome::parachain, PvfError)

namespace kagome::parachain {
  class ModulePrecompiler;

  struct ValidationParams;

  struct ValidationResult {
    SCALE_TIE(6);

    HeadData head_data;
    std::optional<ParachainRuntime> new_validation_code;
    std::vector<UpwardMessage> upward_messages;
    std::vector<network::OutboundHorizontal> horizontal_messages;
    uint32_t processed_downward_messages;
    BlockNumber hrmp_watermark;
  };

  class PvfImpl : public Pvf, public std::enable_shared_from_this<PvfImpl> {
   public:
    struct Config {
      bool precompile_modules;
      size_t runtime_instance_cache_size{16};
      size_t max_stack_depth{};
      unsigned precompile_threads_num{1};
    };

    PvfImpl(const Config &config,
            std::shared_ptr<boost::asio::io_context> io_context,
            std::shared_ptr<libp2p::basic::Scheduler> scheduler,
            std::shared_ptr<crypto::Hasher> hasher,
            std::unique_ptr<runtime::RuntimeInstancesPool> instance_pool,
            std::shared_ptr<runtime::RuntimePropertiesCache>
                runtime_properties_cache,
            std::shared_ptr<blockchain::BlockTree> block_tree,
            std::shared_ptr<crypto::Sr25519Provider> sr25519_provider,
            std::shared_ptr<runtime::ParachainHost> parachain_api,
            std::shared_ptr<runtime::Executor> executor,
            std::shared_ptr<runtime::RuntimeContextFactory> ctx_factory,
            std::shared_ptr<application::AppStateManager> app_state_manager,
            std::shared_ptr<application::AppConfiguration> app_configuration);

    ~PvfImpl() override;

    bool prepare();

    outcome::result<Result> pvfSync(
        const CandidateReceipt &receipt,
        const ParachainBlock &pov,
        const runtime::PersistedValidationData &pvd) const override;
    outcome::result<Result> pvfValidate(
        const PersistedValidationData &data,
        const ParachainBlock &pov,
        const CandidateReceipt &receipt,
        const ParachainRuntime &code) const override;

   private:
    using CandidateDescriptor = network::CandidateDescriptor;
    using ParachainRuntime = network::ParachainRuntime;

    outcome::result<ParachainRuntime> getCode(
        const CandidateDescriptor &descriptor) const;
    outcome::result<ValidationResult> callWasm(
        const CandidateReceipt &receipt,
        const common::Hash256 &code_hash,
        const ParachainRuntime &code_zstd,
        const ValidationParams &params) const;

    outcome::result<CandidateCommitments> fromOutputs(
        const CandidateReceipt &receipt, ValidationResult &&result) const;

    Config config_;
    std::shared_ptr<boost::asio::io_context> io_context_;
    std::shared_ptr<libp2p::basic::Scheduler> scheduler_;
    std::shared_ptr<crypto::Hasher> hasher_;
    std::shared_ptr<runtime::RuntimePropertiesCache> runtime_properties_cache_;
    std::shared_ptr<blockchain::BlockTree> block_tree_;
    std::shared_ptr<crypto::Sr25519Provider> sr25519_provider_;
    std::shared_ptr<runtime::ParachainHost> parachain_api_;
    std::shared_ptr<runtime::Executor> executor_;
    std::shared_ptr<runtime::RuntimeContextFactory> ctx_factory_;
    log::Logger log_;

    std::shared_ptr<runtime::RuntimeInstancesPool> runtime_cache_;
    std::shared_ptr<ModulePrecompiler> precompiler_;
    std::shared_ptr<application::AppConfiguration> app_configuration_;

    std::unique_ptr<std::thread> precompiler_thread_;
  };
}  // namespace kagome::parachain
