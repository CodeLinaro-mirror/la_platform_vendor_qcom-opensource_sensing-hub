/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include "sessionFactory.h"
#include <dlfcn.h>

using namespace com::quic::sensinghub::session::V1_0 ;
#define SENSING_HUB_INTERFACE_LIB_NAME "libQshSession.so"

sessionFactory::sessionFactory():
    mLibHandler(nullptr),
    mSymbolLoaded(false),
    getSessionSymbol(nullptr)
{
}

ISession* sessionFactory::getSession() {
  if(false == mSymbolLoaded) {
    mSymbolLoaded = (0==loadSymbol())?true:false;
  }
  if(true == mSymbolLoaded) {
    return getSessionSymbol();
  } else {
    return nullptr;
  }
}

int sessionFactory::loadSymbol() {
  mLibHandler = dlopen(SENSING_HUB_INTERFACE_LIB_NAME, RTLD_NOW);
  if(nullptr == mLibHandler){
    printf("error in loading the lib \n");
    return -1;
  }
  getSessionSymbol = (getSession_t)dlsym(mLibHandler, "getSession");
  if(nullptr == getSessionSymbol) {
    printf("error in loading the lib \n");
    return -1;
  }
  return 0;
}
