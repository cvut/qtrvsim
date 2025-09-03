#ifndef TLB_POLICY_H
#define TLB_POLICY_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace machine {

// Abstract TLB replacement policy interface & implementations (RAND, LRU, LFU, PLRU) for set-associative tables.
class TLBPolicy {
public:
  virtual size_t select_way(size_t set) const = 0;

  virtual void notify_access(size_t set, size_t way, bool valid) = 0;

  virtual ~TLBPolicy() = default;
};

class TLBPolicyRAND final : public TLBPolicy {
  size_t associativity;

public:
  explicit TLBPolicyRAND(size_t assoc);
  size_t select_way(size_t set) const override;
  void notify_access(size_t set, size_t way, bool valid) override;
};


class TLBPolicyLRU final : public TLBPolicy {
  size_t associativity;
  size_t set_count;
  std::vector<std::vector<uint32_t>> stats;
public:
  TLBPolicyLRU(size_t assoc, size_t sets);
  size_t select_way(size_t set) const override;
  void notify_access(size_t set, size_t way, bool valid) override;
};

class TLBPolicyLFU final : public TLBPolicy {
  size_t associativity;
  size_t set_count;
  std::vector<std::vector<uint32_t>> stats;
public:
  TLBPolicyLFU(size_t assoc, size_t sets);
  size_t select_way(size_t set) const override;
  void notify_access(size_t set, size_t way, bool valid) override;
};

class TLBPolicyPLRU final : public TLBPolicy {
  size_t associativity;
  size_t set_count;
  size_t c_log2;
  std::vector<std::vector<uint8_t>> tree;
public:
  TLBPolicyPLRU(size_t assoc, size_t sets);
  size_t select_way(size_t set) const override;
  void notify_access(size_t set, size_t way, bool valid) override;
};

enum class TLBPolicyKind { RAND, LRU, LFU, PLRU };
std::unique_ptr<TLBPolicy> make_tlb_policy(
  TLBPolicyKind kind,
  size_t associativity,
  size_t set_count
);

}

#endif // TLB_POLICY_H
