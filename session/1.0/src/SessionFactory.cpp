/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include "SessionFactory.h"
#include <dlfcn.h>

using namespace com::quic::sensinghub::session::V1_0 ;
#ifdef SNS_VERSIONED_LIB_ENABLED
#define SENSING_HUB_INTERFACE_LIB_NAME "libQshSession.so.1"
#else
#define SENSING_HUB_INTERFACE_LIB_NAME "libQshSession.so"
#endif

bool sessionFactory::mSymbolLoaded = false;
sessionFactory::getSession_t sessionFactory::mGetSessionSymbol = nullptr;

ISession* sessionFactory::getSession() {
  if(false == mSymbolLoaded) {
    mSymbolLoaded = (0==loadSymbol())?true:false;
  }
  if(true == mSymbolLoaded &&
     nullptr != mGetSessionSymbol) {
    return mGetSessionSymbol();
  } else {
    return nullptr;
  }
}

int sessionFactory::loadSymbol() {
  void* libHandler = nullptr;
  libHandler = dlopen(SENSING_HUB_INTERFACE_LIB_NAME, RTLD_NOW);
  if(nullptr == libHandler){
    printf("error in loading the lib \n");
    return -1;
  }
  mGetSessionSymbol = (getSession_t)dlsym(libHandler, "getSession");
  if(nullptr == mGetSessionSymbol) {
    printf("error in loading the symbol \n");
    return -1;
  }
  return 0;
}
