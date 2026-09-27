/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */
#include <sys/stat.h>
#include <cstring>
#include <cstdint>
#include <sstream>

#include "qshJsonParser.h"
#include "qshLog.h"

#define MAX_DATA_VALUE_LEN 256

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

using namespace std;

qshJsonParser::qshJsonParser():
  mDeviceSocId(-1),
  mJsonSocId(-1),
  mReadCommAttr(false),
  mIsDrcConfigGrp(false),
  mIsClientCommGrp(false),
  mIsAppsLink(false),
  mSensingHubId(-1),
  mPlatformId(""),
  mIsFileParsed(false)
{
  mDeviceSocId = readSocId();
  mJsonSocIdList.clear();
  sns_logi("Device soc_id detected = %d", mDeviceSocId);
}

string qshJsonParser::getHubName(int hubID)
{
  if(mSensingHubIdNameMap.count(hubID) == 0) {
    sns_loge("entry does not exists for the hub id : %d", hubID);
    return string();
  }
  return mSensingHubIdNameMap[hubID];
}

int qshJsonParser::readSocId()
{
  const char* socPath = "/sys/devices/soc0/soc_id";
  FILE* fp = fopen(socPath, "r");
  if (!fp)
  {
    sns_loge("Failed to open soc_id file: %s", socPath);
    return -1;
  }

  char buf[16] = {0};
  if (!fgets(buf, sizeof(buf), fp))
  {
    sns_loge("Failed to read soc_id");
    fclose(fp);
    return -1;
  }

  fclose(fp);
  return atoi(buf);
}

void qshJsonParser::resetParserState()
{
  mSensingHubId = -1;
  mPlatformId.clear();
  mClientCommConfig = comm_config();
  mDrcConfig = comm_config();

  mReadCommAttr = false;
  mIsClientCommGrp = false;
  mIsDrcConfigGrp = false;
  mIsAppsLink = false;

  mCurrLineNum = 0;
  mJsonSocId = -1;
  mJsonSocIdList.clear();
  mHubIds.clear();
}

comm_config qshJsonParser::getClientCommConfig(int hubID)
{
  if(0 == mSensingHubClientCommMap.count(hubID))
  {
    sns_loge("getClientCommConfig: hub_id=%d not found", hubID);
    return comm_config();
  }
  return mSensingHubClientCommMap.at(hubID);
}

comm_config qshJsonParser::getDrcConfig(int hubID)
{
  if(0 == mSensingHubDrcMap.count(hubID))
  {
    sns_loge("getDrcConfig: hub_id=%d not found", hubID);
    return comm_config();
  }
  return mSensingHubDrcMap.at(hubID);
}

std::vector<int> qshJsonParser::getHubIdList()
{
  return mHubIds;
}

int qshJsonParser::getHubId(std::string hubName)
{
  if(0 == mSensingHubNameIdMap.count(hubName))
    return -1;
  return mSensingHubNameIdMap.at(hubName);
}

int qshJsonParser::getWhitespaceLen(char const* str, uint32_t strLen)
{
  uint32_t cidx = 0;
  while(cidx < strLen && 0 != isspace(str[cidx]))
  {
    if('\n' == str[cidx])
    {
      mCurrLineNum++;
    }
    if(cidx >= strLen)
    {
      return 0;
    }
    ++cidx;
  }
  return cidx;
}

uint32_t qshJsonParser::parsePair(char* json, uint32_t jsonlen, char** key,
                           char** value)
{
  uint32_t cidx = 0;
  char* endString;
  bool failed = false;

  *value = NULL;

  if(cidx >= jsonlen || '\"' != json[cidx++])
  {
    sns_logd("parsePair: Missing opening quote line no.: %d ", mCurrLineNum);
    failed = true;
  }
  else if(NULL == (endString = strchr(&json[cidx], '\"')))
  {
    sns_logd("parsePair: Missing closing quote (%i)", mCurrLineNum);
    failed = true;
  }
  else
  {
    *key = &json[cidx];
    size_t keyLen = endString - &json[cidx];
    if(cidx + keyLen >= jsonlen)
    {
      sns_logd("parsePair: Key extends beyond buffer");
      failed = true;
    }
    else
    {
      cidx += keyLen;
      json[cidx++] = '\0'; // We want to safely use this string later
      cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
      if(cidx >= jsonlen || ':' != json[cidx++])
      {
        sns_logd("parsePair: Missing colon Line No.: %d ", mCurrLineNum);
        failed = true;
      }
      else
      {
        cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
        if(cidx < jsonlen && '\"' == json[cidx])
        {
          cidx++;
          *value = &json[cidx];
          endString = strchr(&json[cidx], '\"');
          if(NULL == endString)
          {
            sns_logd("parsePair: Missing closing quote %d ", mCurrLineNum);
            failed = true;
          }
          else
          {
            cidx += (uintptr_t)endString - (uintptr_t)&json[cidx];
            json[cidx++] = '\0';
            cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
          }
        }
        else if(cidx < jsonlen && '{' != json[cidx] && '[' != json[cidx])
        {
          sns_logd("parsePair: Next element is not a quote or opening bracket:%d ", mCurrLineNum);
          failed = true;
        }
      }
      sns_logv("Parsed pair key:%s",*key);
    }
  }
  return failed ? 0 : cidx;
}

uint32_t qshJsonParser::parseItem(char* itemName, char* itemValue, char* json, uint32_t jsonlen)
{
  uint32_t cidx = 0;

  if(cidx >= jsonlen || '{' != json[cidx++])
  {
    sns_logd("parseItem: Missing opening bracket Line no. %d ", mCurrLineNum);
  }
  else
  {
    do
    {
      char* key, *value;
      uint32_t len;

      cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
      len = parsePair(&json[cidx], jsonlen - cidx, &key, &value);
      cidx += len;

      if(0 == len || NULL == key || NULL == value)
      {
        sns_logd("parseItem: Unable to parse pair Line no. %d ", mCurrLineNum);
        return cidx;
      }
      else
      {
        if(0 == strcmp(key, "data"))
        {
          size_t valueLen = strlen(value);
          if(valueLen >= MAX_DATA_VALUE_LEN) {
            sns_loge("Value too long (%zu), truncating to %d", valueLen, MAX_DATA_VALUE_LEN - 1);
          }
          sns_logd("set item value:%s",value);
          strlcpy(itemValue, value, MAX_DATA_VALUE_LEN);
        }
      }
      cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
    } while(cidx < jsonlen && ',' == json[cidx] && cidx++);

    if(cidx >= jsonlen || '}' != json[cidx++])
    {
      sns_logd("parseItem: Missing closing bracket Line no.  %d ", mCurrLineNum);
      cidx = 0;
    }
  }
  return cidx;
}

void qshJsonParser::commitClientCommConfig()
{
  if((-1 == mSensingHubId) || (mClientCommConfig.comm_type == -1))
    return;

  mSensingHubClientCommMap.insert({mSensingHubId, mClientCommConfig});

  sns_logi("commitClientCommConfig: hub_id=%d type=%d channel=%s channels=%d service_id=%d",
           mSensingHubId, mClientCommConfig.comm_type, mClientCommConfig.channel_name.c_str(),
           mClientCommConfig.no_of_channels, mClientCommConfig.service_id);
}

void qshJsonParser::commitDrcConfig()
{
  if((-1 == mSensingHubId) || (mDrcConfig.comm_type == -1))
    return;

  mSensingHubDrcMap.insert({mSensingHubId, mDrcConfig});

  sns_logi("commitDrcConfig: hub_id=%d type=%d channel=%s channels=%d service_id=%d",
           mSensingHubId, mDrcConfig.comm_type, mDrcConfig.channel_name.c_str(),
           mDrcConfig.no_of_channels, mDrcConfig.service_id);
}

void qshJsonParser::updateSensingHubInfo(const char * grpName, char * key, char * value)
{
  if(0 == strcmp(grpName, ".client_comm"))
    mIsClientCommGrp = true;

  if(0 == strcmp(grpName, ".direct_channel"))
    mIsDrcConfigGrp = true;

  if(0 == strcmp(key, "platform_id"))
  {
    mPlatformId = value;
    if(-1 != mSensingHubId)
    {
      mSensingHubNameIdMap.insert({mPlatformId, mSensingHubId});
    }
  }
  else if(0 == strcmp(key, "sensing_hub_id"))
  {
    mSensingHubId = atoi(value);
    mHubIds.push_back(mSensingHubId);
  }
  else if(0 == strcmp(key, "client_comm_type"))
  {
    mClientCommConfig.comm_type = atoi(value);
    sns_logd("updateSensingHubInfo: client_comm_type=%d", mClientCommConfig.comm_type);
  }
  else if(mIsDrcConfigGrp && 0 == strcmp(key, "direct_channel_comm_type"))
  {
    mDrcConfig.comm_type = atoi(value);
    sns_logd("updateSensingHubInfo: drc_comm_type=%d", mDrcConfig.comm_type);
  }
  else if(mIsClientCommGrp && 0 == strcmp(key, "channel_name"))
  {
    mClientCommConfig.channel_name = value;
    sns_logd("updateSensingHubInfo: client_channel_name=%s", mClientCommConfig.channel_name.c_str());
  }
  else if(mIsDrcConfigGrp && 0 == strcmp(key, "channel_name"))
  {
    mDrcConfig.channel_name = value;
    sns_logd("updateSensingHubInfo: drc_channel_name=%s", mDrcConfig.channel_name.c_str());
  }
  else if(mIsClientCommGrp && 0 == strcmp(key, "no_of_channels") && mIsAppsLink)
  {
    mClientCommConfig.no_of_channels = atoi(value);
    sns_logd("updateSensingHubInfo: client_no_of_channels=%d", mClientCommConfig.no_of_channels);
  }
  else if(mIsDrcConfigGrp && 0 == strcmp(key, "no_of_channels"))
  {
    mDrcConfig.no_of_channels = atoi(value);
    sns_logd("updateSensingHubInfo: drc_no_of_channels=%d", mDrcConfig.no_of_channels);
  }
  else if(mIsClientCommGrp && 0 == strcmp(key, "service_id"))
  {
    mClientCommConfig.service_id = atoi(value);
    sns_logd("updateSensingHubInfo: client_service_id=%d", mClientCommConfig.service_id);
  }
  else if(mIsDrcConfigGrp && 0 == strcmp(key, "service_id"))
  {
    mDrcConfig.service_id = atoi(value);
    sns_logd("updateSensingHubInfo: drc_service_id=%d", mDrcConfig.service_id);
  }
  else if(mIsClientCommGrp && 0 == strcmp(key, "link_name") && 0 != strcmp(value, "apps"))
  {
    mIsAppsLink = false;
    if(0 == strcmp(grpName, ".client_comm"))
      mSensingHubIdNameMap[mSensingHubId] = value;
  }
  else if(mIsClientCommGrp && 0 == strcmp(key, "link_name") && 0 == strcmp(value, "apps"))
  {
    mIsAppsLink = true;
  }
}

int qshJsonParser::parseGroup(char const* grpName, char* json,
                            uint32_t jsonlen, uint32_t* len)
{
  uint32_t cidx = 0;

  *len = 0;

  if(cidx >= jsonlen || '{' != json[cidx++])
  {
    sns_loge("parseGroup: Missing opening bracket Line no. %d ", mCurrLineNum);
    return -1;
  }
  {
    do
    {
      char *key, *value;
      uint32_t len;

      cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
      len = parsePair(&json[cidx], jsonlen - cidx, &key, &value);
      cidx += len;

      if(0 == len || NULL == key)
      {
        sns_loge("%s: Unable to parse pair on line No %d ", __func__, mCurrLineNum);
        return -1;
      }
      else if(NULL != value)
      {
        if(0 == strcmp(key, "owner"))
        {
          sns_logd("Owner of group:  %s ", value);
        }
        else
        {
          sns_loge("Owner of the group not found");
        }
      }
      else
      {
        if('.' == key[0])
        {
          if(0 != parseGroup(key, &json[cidx], jsonlen - cidx, &len))
          {
            sns_loge("parseGroup: Error parsing nested group:%s %d ", key, mCurrLineNum);
            return -1;
          }
          cidx += len;
          if(0 == strcmp(key, ".client_comm")) {
            commitClientCommConfig();
            mIsClientCommGrp = false;
            mClientCommConfig = comm_config();
          } else if(0 == strcmp(key, ".direct_channel")) {
            commitDrcConfig();
            mIsDrcConfigGrp = false;
            mDrcConfig = comm_config();
          }
        }
        else
        {
          char localValue[MAX_DATA_VALUE_LEN];
          len = parseItem(key, &localValue[0], &json[cidx], jsonlen - cidx);
          sns_logv("local key:value %s:%s", key, localValue);
          updateSensingHubInfo(grpName, key, localValue);
          cidx += len;
        }
        if(0 == len)
        {
          sns_loge("parseGroup: Error parsing item:%s %d ", key, mCurrLineNum);
          return -1;
        }
      }
      cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
    } while(',' == json[cidx] && cidx++);

    if(cidx >= jsonlen || '}' != json[cidx++])
    {
      sns_loge("parseGroup: Missing closing bracket Line No. %d ", mCurrLineNum);
      return -1;
    }
  }
  *len = cidx;
  return 0;
}

int qshJsonParser::parseConfigItem(char* json, uint32_t jsonLen)
{
    uint32_t cidx = 0;
    char *key, *value;
    uint32_t len;

    if (json[cidx++] != '{')
        return 0;

    while (cidx < jsonLen) {
        cidx += getWhitespaceLen(&json[cidx], jsonLen - cidx);

        len = parsePair(&json[cidx], jsonLen - cidx, &key, &value);
        if (len == 0 || key == NULL)
            break;

        cidx += len;

        if (strcmp(key, "soc_id") == 0 && value != NULL) {
            sns_logi("config: found list soc_id=%s", value);

            std::stringstream ss(value);

            std::string token;
            while (std::getline(ss, token, ',')) {
                mJsonSocIdList.push_back(std::stoi(token));
            }
        }

        cidx += getWhitespaceLen(&json[cidx], jsonLen - cidx);
        if (json[cidx] != ',')
            break;

        cidx++;
    }

    return cidx;
}

int32_t qshJsonParser::parseConfig(char* json, uint32_t jsonlen)
{
    int32_t cidx = 0;

    cidx += getWhitespaceLen(json, jsonlen);

    if (json[cidx] != '{') {
        sns_loge("parse_config: Missing open bracket");
        return -1;
    }

    cidx += parseConfigItem(&json[cidx], jsonlen - cidx);

    cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
    if (json[cidx++] != '}') {
        sns_loge("parse_config: Missing closing bracket");
        return -1;
    }

    return cidx;
}

int qshJsonParser::parseFile(char* json, uint32_t jsonlen)
{
  uint32_t cidx = 0;
  uint32_t len;
  int32_t ret;
  char *key, *value;

  mCurrLineNum = 0;
  sns_logi("%s: started parsing sensing hub info json file", __func__);
  cidx = getWhitespaceLen(json, jsonlen);
  if(cidx >= jsonlen || '{' != json[cidx++])
  {
    sns_loge("Missing open bracket in config file");
    return -1;
  }

  do {
    cidx += getWhitespaceLen(&json[cidx], jsonlen - cidx);
    len = parsePair(&json[cidx], jsonlen - cidx, &key, &value);

    if(0 == len || NULL == key || NULL != value)
    {
        sns_loge("Pair parsing failed");
        return -1;
    }
    cidx += len;
    if(0 == strcmp(key, "config"))
    {
      ret = parseConfig(&json[cidx], jsonlen - cidx);
      if(0 == ret) {
        sns_loge("Error: parsing config failed");
        return -1;
      }
      cidx += ret;

      if(mJsonSocIdList.size() < 1){
        sns_logi("json soc_id does not exist, continue to parse the rest of json");
      } else {
        for(int socId: mJsonSocIdList){
          if(socId == mDeviceSocId){
            mJsonSocId = socId;
            sns_logi("soc_id=%d match target soc_id=%d",
                    mJsonSocId, mDeviceSocId);
            break;
          }
        }
        if(mJsonSocId == -1){
          sns_logi("Skipping JSON: soc_id=%d does not match target soc_id=%d",
            mJsonSocId, mDeviceSocId);
          return 0;
        }
      }
    }
    else if(0 != parseGroup(key, &json[cidx], jsonlen - cidx, &len))
    {
      sns_loge("Error parsing group: key=%s, %d", key, mCurrLineNum);
      return -1;
    }
    else
    {
      cidx += len;
    }
    sns_logd("Parsing next group");
  } while(cidx < jsonlen && ',' == json[cidx] && cidx++);
  sns_logi("Parsing Complete");
  return 0;
}

bool qshJsonParser::isFileParsed(){
  return mIsFileParsed;
}

void qshJsonParser::printParserState()
{
  if (!mIsFileParsed) {
    sns_logi("printParserState: file not yet parsed");
    return;
  }

  sns_logi("=== qshJsonParser state ===");
  sns_logi("  device_soc_id : %d", mDeviceSocId);
  sns_logi("  json_soc_id   : %d", mJsonSocId);

  sns_logi("  hub_ids (%zu):", mHubIds.size());
  for (int id : mHubIds)
    sns_logi("    [%d]", id);

  sns_logi("  name->id map (%zu):", mSensingHubNameIdMap.size());
  for (const auto& kv : mSensingHubNameIdMap)
    sns_logi("    '%s' -> %d", kv.first.c_str(), kv.second);

  sns_logi("  id->name map (%zu):", mSensingHubIdNameMap.size());
  for (const auto& kv : mSensingHubIdNameMap)
    sns_logi("    %d -> '%s'", kv.first, kv.second.c_str());

  sns_logi("  client comm map (%zu):", mSensingHubClientCommMap.size());
  for (const auto& kv : mSensingHubClientCommMap) {
    const comm_config& c = kv.second;
    sns_logi("    hub_id=%d  comm_type=%d  channel='%s'  channels=%d  service_id=%d",
             kv.first, c.comm_type, c.channel_name.c_str(),
             c.no_of_channels, c.service_id);
  }

  sns_logi("  drc map (%zu):", mSensingHubDrcMap.size());
  for (const auto& kv : mSensingHubDrcMap) {
    const comm_config& d = kv.second;
    sns_logi("    hub_id=%d  comm_type=%d  channel='%s'  channels=%d  service_id=%d",
             kv.first, d.comm_type, d.channel_name.c_str(),
             d.no_of_channels, d.service_id);
  }
  sns_logi("===========================");
}

/* call this function to start parsing */
int qshJsonParser::loadFile(const char* path)
{
  resetParserState();

  struct stat buf;
  if(0 != stat(path, &buf))
  {
    sns_loge("stat failed for config file");
    return -1;
  }
  char* fileContent = (char*)malloc(buf.st_size + 1);
  if(nullptr == fileContent)
  {
    sns_loge("failed to allocate memory for reading file content");
    return -1;
  }
  FILE* fptr = NULL;
  fptr = fopen(path, "r");
  if(NULL == fptr)
  {
    sns_loge("failed to open sensing_hub_config file");
    free(fileContent);
    return -1;
  }
  if(fread(fileContent, sizeof(char), buf.st_size, fptr) < buf.st_size)
  {
    sns_loge("failed to read sensing_hub_config");
    free(fileContent);
    fclose(fptr);
    return -1;
  }
  fileContent[buf.st_size] = '\0';
  fclose(fptr);
  sns_logi("parsing the contents of the file %s",path);
  if(0 != parseFile(fileContent, buf.st_size))
  {
    free(fileContent);
    return -1;
  }
  free(fileContent);
  mIsFileParsed = true;
  printParserState();
  return 0;
}
