/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include "ISession.h"

namespace com {
namespace quic {
namespace sensinghub {
namespace session {
namespace V1_0 {

class sessionFactory {
public:
  typedef ISession* (*getSession_t)();
  sessionFactory();
  ~sessionFactory(){};

  /**
   * @brief create an instance for ISession
   * @return: returns instance to ISession on success , nullptr on failure
   */
  ISession* getSession();
private:
  int loadSymbol();
  void* mLibHandler;
  bool mSymbolLoaded;
  getSession_t getSessionSymbol;
};

}  // namespace V1_0
}  // namespace session
}  // namespace sensinghub
}  // namespace quic
}  // namespace com
