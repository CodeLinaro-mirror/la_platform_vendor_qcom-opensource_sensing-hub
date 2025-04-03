/*
 * Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#pragma once
#include <vector>
#include "ISession.h"

namespace com {
namespace quic {
namespace sensinghub {
namespace session {
namespace V1_0 {

class sessionFactory {
public:
  sessionFactory(){};
  ~sessionFactory(){};

  /**
   * @brief create an instance for ISession
   * @return: returns instance to ISession on success , nullptr on failure
   */

  ISession* getSession(int hub_id = -1);
  std::vector<int> getSensingHubIds();

private:
  typedef ISession* (*getSession_t)(int);
  typedef void*     (*getSensingHubIds_t)();
  int loadSymbol();
  static bool mSymbolLoaded;
  static getSession_t mGetSessionSymbol;
  static getSensingHubIds_t mGetSensingHubIdsSymbol;
};

}  // namespace V1_0
}  // namespace session
}  // namespace sensinghub
}  // namespace quic
}  // namespace com
