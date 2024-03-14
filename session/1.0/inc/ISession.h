/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#pragma once
#include <string>
#include <functional>
#include <memory>
#include "suid.h"

namespace com {
namespace quic {
namespace sensinghub {
namespace session {
namespace V1_0 {

using namespace ::com::quic::sensinghub;
/*
 * ISession is an interface to interact with QSH
 * */
class ISession {
public:
  /*
   * error type if any from ISession & QSH
   * */
  enum error
  {
    RESET
  };
  /**
   * type alias for ISession respCallBack, errorCallBack and event eventCallBack respectively.
   */
  using respCallBack = std::function<void(const uint32_t respValue, uint64_t clientConnectID)>;
  using errorCallBack = std::function<void(error errorValue)>;
  using eventCallBack = std::function<void(const uint8_t *sensorData, size_t sensorDataSize, uint64_t sensorDataTimeStamp)>;

  /**
   * @brief opens the session for this instance
   **        Client needs to call only once per given session.
   *
   * @out: 0 upon success & -1 upon failure.
   */
  virtual int open() = 0;

  /**
   * @brief close the session for this instance
   *
   * @out: none
   */
  virtual void close() = 0;

  /**
   * @brief set the callbacks for the given suid.
   *        Client needs to call only once per given suid.
   *
   * @param: callback pointers, respCallBack, errorCallBack and eventCallBack.
   *         All are mandatory. Client can pass nullptr in case do not need any.
   *
   *         if callback pointers are already registered with the given suid,
   *         it will be updated with new callbacks.
   *
   *         if all callback pointers are null and the given suid is already registered,
   *         it will be unregistered.
   *
   * @out: 0 upon success & -1 upon failure.
   */
  virtual int setCallBacks(suid suid, respCallBack respCB, errorCallBack errorCB, eventCallBack eventCB) = 0;

  /**
   * @brief send proto encoded message to QSH.
   *
   * @param: suid of the given sensor
   *
   * @param: encodedMessage: proto encoded request message.
   *
   * @out: 0 upon success & -1 upon failure.
   */
  virtual int sendRequest(suid suid, std::string message) = 0;
  virtual ~ISession(){};
protected:
};

}  // namespace V1_0
}  // namespace session
}  // namespace sensinghub
}  // namespace quic
}  // namespace com
