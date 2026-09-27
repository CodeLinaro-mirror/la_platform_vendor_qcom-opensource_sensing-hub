#pragma once
/** ============================================================================
 * @file
 *
 * @brief SessionFactory provides simple and flexible way for creating ISession
 * instances.
 *
 * @copyright 
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 *
 * ===========================================================================*/

#include <vector>
#include "ISession.h"
#include "SessionConfig.h"

namespace com {
namespace quic {
namespace sensinghub {
namespace session {
namespace V1_0 {

/*==============================================================================
  Type Definitions
  ============================================================================*/

/**
 * @class sessionFactory
 * @brief SessionFactory is a runtime interface that provides simple and flexible
 * way for creating instances of ISession, functioning as factory class.
 * Users can create an ISession only through sessionFactory instance.
 *
 */
class sessionFactory {
public:
  /**
   * @brief Constructor for sessionFactory, creates singleton object
   *
   */
  sessionFactory(){};

  /**
   * @brief Destructor for sessionFactory.
   * Once the usecase is done, to avoid the memory leaks,
   * clients are expected to delete the sessionFactory instance.
   *
   */
  ~sessionFactory(){};

  /**
   * @brief Creates ISession instance using the default sensing-hub ID (-1).
   *
   * @return
   *   - Pointer to ISession object  Success.
   *   - Nullptr                     Failure.
   */
  ISession* getSession();

  /**
   * @brief Creates ISession instance for the specified sensing-hub ID.
   *
   * @param [in] hub_id  Hub ID of the desired sensing-hub.
   * @return
   *   - Pointer to ISession object  Success.
   *   - Nullptr                     Failure.
   */
  ISession* getSession(int hub_id );

  /**
   * @brief Creates ISession instance from a configured SessionConfig for the specified sensing-hub ID.
   *
   * @param [in] hub_id  Hub ID of the desired sensing-hub.
   * @param [in] config  SessionConfig instance obtained from
   * getSessionConfig().
   *
   * @return
   *   - Pointer to ISession object  Success.
   *   - Nullptr                     Failure.
   *
   */
  ISession *getSession(int hub_id, const SessionConfig *config);

  /**
   * @brief Retrieve the IDs of supported Sensing Hubs.
   *
   */
  std::vector<int> getSensingHubIds();

  /**
   * @brief Requests a new SessionConfig instance.
   * Clients populate it via setters and pass it to
   * getSession(int hub_id, const SessionConfig*)
   *
   * @return Pointer to a new SessionConfig instance.
   *
   */
  SessionConfig *getSessionConfig();

  /**
   * @brief Sets an optional direct-channel create request on a SessionConfig
   * This is used for direct channel requests.
   *
   * @param [in] config          SessionConfig instance to modify.
   * @param [in] encodedRequest  Proto-encoded sns_direct_channel_create_msg.
   * @param [in] requestLen      Length of encodedRequest in bytes.
   *
   */
  void setDirectChannelRequest(SessionConfig* config, const char* encodedRequest, int requestLen);


  ISession* getSession(int hub_id, bool direct_channel); // Compatibility function remove this

private:
  /**
   * @brief Pointer to ISession object.
   *
   */
  typedef ISession* (*getSession_t)(int);

  /**
   * @brief Pointer to ISession object, created from an encoded SessionConfig.
   *
   */
  typedef ISession* (*getSessionFromConfig_t)(const char*, int);

  /**
   * @brief Pointer to vector containing Sensing Hub IDs.
   *
   */
  typedef void* (*getSensingHubIds_t)();

  /**
   * @brief Dynamically loads the proprietary library.
   *
   * @return
   *    -  true  Success.
   *    - false  Failure.
   */
  bool loadSymbol();

  static bool mSymbolLoaded;  /*!< true if loadSymbol() returns 0, else false*/
  static getSession_t mGetSessionSymbol;  /*!< stores pointer to ISession */
  static getSessionFromConfig_t mgetSessionFromConfigSymbol;  /*!< stores pointer to config-based ISession creation */
  static getSensingHubIds_t mGetSensingHubIdsSymbol;  /*!< stores pointer to vector containing Hub IDs */
};

}  // namespace V1_0
}  // namespace session
}  // namespace sensinghub
}  // namespace quic
}  // namespace com
