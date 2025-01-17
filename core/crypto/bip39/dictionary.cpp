/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include "crypto/bip39/dictionary.hpp"

#include "crypto/bip39/entropy_accumulator.hpp"
#include "crypto/bip39/wordlist/english.hpp"

namespace kagome::crypto::bip39 {

  void Dictionary::initialize() {
    size_t i = 0;
    for (const auto &word : english::dictionary) {
      entropy_map_[word] = EntropyToken(i++);
    }
  }

  outcome::result<EntropyToken> Dictionary::findValue(
      std::string_view word) const {
    auto loc = entropy_map_.find(word);
    if (entropy_map_.end() != loc) {
      return loc->second;
    }

    return DictionaryError::ENTRY_NOT_FOUND;
  }

}  // namespace kagome::crypto::bip39

OUTCOME_CPP_DEFINE_CATEGORY(kagome::crypto::bip39, DictionaryError, error) {
  using E = kagome::crypto::bip39::DictionaryError;
  switch (error) {
    case E::ENTRY_NOT_FOUND:
      return "word not found";
  }
  return "unknown DictionaryError error";
}
