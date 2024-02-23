/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

namespace com {
namespace quic {
namespace sensinghub {

/*
 * @brief Struct to represent sensor's unique ID (128-bit)
 */
struct suid
{
  suid() : low(0), high(0) {}
  suid(uint64_t low, uint64_t high): low(low), high(high) {}
  bool operator==(const suid& rhs) const
  {
    return (low == rhs.low && high == rhs.high);
  }
  uint64_t low, high;
};

}  // namespace sensinghub
}  // namespace quic
}  // namespace com
