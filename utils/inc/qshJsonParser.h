/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */
#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#define COMM_TYPE_QMI     0
#define COMM_TYPE_GLINK   1
#define COMM_TYPE_FASTRPC 2

struct comm_config
{
  int comm_type;          // COMM_TYPE_QMI / COMM_TYPE_GLINK / COMM_TYPE_FASTRPC; -1 if not set
  std::string channel_name;  // GLINK channel name or FastRPC URI; empty if unused
  int no_of_channels;        // number of GLINK channels; 0 if unused
  int service_id;            // QMI service id; 0 if unused

  comm_config() : comm_type(-1), no_of_channels(0), service_id(0) {}
};

class qshJsonParser
{
public:
  qshJsonParser();
  ~qshJsonParser(){};

  static qshJsonParser& getInstance()
  {
      static qshJsonParser parserInstance;
      return parserInstance;
  }

  /* reads sensing_hub_config file & starts parsing */
  int loadFile(const char* path);
  /* returns list of supported hub ids */
  std::vector<int> getHubIdList();
  /* gets the hub id for a given hub name */
  int getHubId(std::string hubName);
  /* fetches the hub name for a hub id */
  std::string getHubName(int hubID);
  /* returns if parser state has been updated from config file */
  bool isFileParsed();
  /* fetches the client comm configuration for a hub id */
  comm_config getClientCommConfig(int hubID);
  /* fetches the direct channel configuration for a hub id */
  comm_config getDrcConfig(int hubID);
  void printParserState();

private:
  /* field name -> device's actual value, read once at construction */
  std::unordered_map<std::string, int> mDeviceConfigs;
  /* field name -> list of ids accepted for that field, as declared in the config currently being parsed */
  std::unordered_map<std::string, std::vector<int>> mJsonConfigs;
  int readSocId();
  int readSoftSkuId();
  void resetParserState();

  std::unordered_map<std::string, int> mSensingHubNameIdMap;
  std::unordered_map<int, std::string> mSensingHubIdNameMap;
  std::unordered_map<int, comm_config> mSensingHubClientCommMap;
  std::unordered_map<int, comm_config> mSensingHubDrcMap;

  int mCurrLineNum;
  std::vector<int> mHubIds;

  bool mReadCommAttr;
  bool mIsDrcConfigGrp;
  bool mIsClientCommGrp;
  bool mIsAppsLink;

  int mSensingHubId;
  std::string mPlatformId;

  comm_config mClientCommConfig;
  comm_config mDrcConfig;

  bool mIsFileParsed;
  /* set by parseFile() when the file is valid JSON but its soc_id/sku_id
   * gating doesn't match this device -- lets loadFile() tell that case
   * apart from a real parse error. */
  bool mDeviceConfigMismatch;

  int getWhitespaceLen(char const* str, uint32_t strLen);
  uint32_t parsePair(char* json, uint32_t jsonLen, char** key, char** value);
  uint32_t parseItem(char* itemName, char* itemValue, char* json, uint32_t jsonLen);
  void updateSensingHubInfo(const char* grpName, char* key, char* value);
  int parseGroup(char const* grpName, char* json, uint32_t jsonLen, uint32_t* len);
  int32_t parseConfig(char* json, uint32_t jsonLen);
  int parseConfigItem(char* json, uint32_t jsonLen);
  int parseFile(char* json, uint32_t jsonLen);
  void commitClientCommConfig();
  void commitDrcConfig();
};
