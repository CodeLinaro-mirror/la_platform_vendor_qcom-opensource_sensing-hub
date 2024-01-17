#ifndef _SNS_DYNAMIC_LOADER_H
#define _SNS_DYNAMIC_LOADER_H
/**
  ============================================================================
  Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause-Clear
  ===========================================================================
 *
 * FastRPC IDL interface to load and unload dynamic libraries by QSH.
 */
#include <AEEStdDef.h>
#include <remote.h>
#include <string.h>
#include <stdlib.h>


#ifndef __QAIC_HEADER
#define __QAIC_HEADER(ff) ff
#endif //__QAIC_HEADER

#ifndef __QAIC_HEADER_EXPORT
#define __QAIC_HEADER_EXPORT
#endif // __QAIC_HEADER_EXPORT

#ifndef __QAIC_HEADER_ATTRIBUTE
#define __QAIC_HEADER_ATTRIBUTE
#endif // __QAIC_HEADER_ATTRIBUTE

#ifndef __QAIC_IMPL
#define __QAIC_IMPL(ff) ff
#endif //__QAIC_IMPL

#ifndef __QAIC_IMPL_EXPORT
#define __QAIC_IMPL_EXPORT
#endif // __QAIC_IMPL_EXPORT

#ifndef __QAIC_IMPL_ATTRIBUTE
#define __QAIC_IMPL_ATTRIBUTE
#endif // __QAIC_IMPL_ATTRIBUTE
#ifdef __cplusplus
extern "C" {
#endif
/** SNS_DL_NAME_LEN
 * Maximum length of the names.
 */
#define SNS_DL_NAME_LEN 128
/** \struct load_library_config
 * Load configuration for dynamic library
 */
typedef struct load_library_config load_library_config;
struct load_library_config {
   char lib_name[128]; /*!< name of the library */
   unsigned char* lib_buffer; /*!< Buffer with library data in binary format */
   int lib_bufferLen;
};
/**  \struct sensor_reg_config
  Default registration configuration for QSH sensors:
  func = sns_register_dynamic_sensors
  count = 1
 */
typedef struct sensor_reg_config sensor_reg_config;
struct sensor_reg_config {
   char func[128]; /*!< Name of the sensor register function. */
   uint8_t count; /*!< Number of instances this sensor will be registered with QSH.
                     The limit is enforced by QSH based on the maximum number
                     of sensors supported per sensor type. */
};
/** \struct sns_dl_entry
 *  Details needed for loading a dynamic library.
 */
typedef struct sns_dl_entry sns_dl_entry;
struct sns_dl_entry {
   load_library_config load_config;
   boolean has_reg_config; /*!< True if registration config is provided using
                                reg_config below. If false, default registration
                                configuration is used. */
   sensor_reg_config reg_config; /*!< Valid when has_reg_config is true. */
};
/** \struct sns_dl_handle
 *  Details needed for unloading dynamic library loaded on DSP.
 */
typedef struct sns_dl_handle sns_dl_handle;
struct sns_dl_handle {
   char lib_name[128]; /*!< name of the library */
};
/**
 * sns dynamic loader APIs
 */
/**
    * Opens the handle in the specified domain.  If this is the first
    * handle, this creates the session.  Typically this means opening
    * the device, aka open("/dev/adsprpc-smd"), then calling ioctl
    * device APIs to create a PD on the DSP to execute our code in,
    * then asking that PD to dlopen the .so and dlsym the skel function.
    *
    * @param uri, <interface>_URI"&_dom=aDSP"
    *    <interface>_URI is a QAIC generated uri, or
    *    "file:///<sofilename>?<interface>_skel_handle_invoke&_modver=1.0"
    *    If the _dom parameter is not present, _dom=DEFAULT is assumed
    *    but not forwarded.
    *    Reserved uri keys:
    *      [0]: first unamed argument is the skel invoke function
    *      _dom: execution domain name, _dom=mDSP/aDSP/DEFAULT
    *      _modver: module version, _modver=1.0
    *      _*: any other key name starting with an _ is reserved
    *    Unknown uri keys/values are forwarded as is.
    * @param h, resulting handle
    * @retval, 0 on success
    */
__QAIC_HEADER_EXPORT int __QAIC_HEADER(sns_dynamic_loader_open)(const char* uri, remote_handle64* h) __QAIC_HEADER_ATTRIBUTE;
/** 
    * Closes a handle.  If this is the last handle to close, the session
    * is closed as well, releasing all the allocated resources.

    * @param h, the handle to close
    * @retval, 0 on success, should always succeed
    */
__QAIC_HEADER_EXPORT int __QAIC_HEADER(sns_dynamic_loader_close)(remote_handle64 h) __QAIC_HEADER_ATTRIBUTE;
/**
    * This is a synchronous API to load the dynamic library on DSP.
    * The client needs to read the contents of the dynamic library into
    * the load_config->lib_buffer and update the load_config->lib_bufferLen.
    *
    * @param [in]  load_config
    *
    * @return AEEResult
    */
__QAIC_HEADER_EXPORT AEEResult __QAIC_HEADER(sns_dynamic_loader_load_library)(remote_handle64 _h, const load_library_config* load_config) __QAIC_HEADER_ATTRIBUTE;
/**
    * This is a synchronous API to load the dynamic library on DSP and
    * register it as a QSH sensor.
    * The client needs to read the contents of the dynamic library into
    * the load_config->lib_buffer and update the load_config->lib_bufferLen.
    *
    * After loading the library, the sensor registration is attempted
    * with the details provided in reg_config. If reg_config is not provided,
    * the default registration configuration is used.
    * If sensor registration fails, the library is unloaded.
    *
    * @param [in]  dl_entry
    *
    * @return AEEResult
    */
__QAIC_HEADER_EXPORT AEEResult __QAIC_HEADER(sns_dynamic_loader_load_sensor)(remote_handle64 _h, const sns_dl_entry* dl_entry) __QAIC_HEADER_ATTRIBUTE;
/**
    * This is a synchronous API to unload the dynamic library loaded on DSP.
    *
    * If a sensor is unloaded, any active datastreams to that sensor will
    * get the sensor invalid state error.
    *
    * @param [in] dl_handle
    *
    * @return  AEEResult
    */
__QAIC_HEADER_EXPORT AEEResult __QAIC_HEADER(sns_dynamic_loader_unload)(remote_handle64 _h, const sns_dl_handle* dl_handle) __QAIC_HEADER_ATTRIBUTE;
#ifndef sns_dynamic_loader_URI
#define sns_dynamic_loader_URI "file:///libsns_dynamic_loader_skel.so?sns_dynamic_loader_skel_handle_invoke&_modver=1.0"
#endif /*sns_dynamic_loader_URI*/
#ifdef __cplusplus
}
#endif
#endif //_SNS_DYNAMIC_LOADER_H
