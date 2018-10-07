/* fluent-logger-mbed 
 * Copyright (c) 2014 Yuuichi Akagawa
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "FluentLogger.h"
#ifdef USE_NTP
#include <time.h>
#endif

FluentLogger::FluentLogger(NetworkInterface* aNetwork, const char *host, const int port, uint32_t bufsize) :
_sock(), _host(host), _port(port), _timeout(1000)
{
    _mp = new uMP(bufsize);
    _sock = new TCPSocket();
    _net = aNetwork;
}

int FluentLogger::close()
{
    nsapi_error_t rt = _sock->close();
    // delete _sock;
    // _sock = NULL;
    return rt;
}

int FluentLogger::log(const char *tag, const char *msg)
{
    nsapi_error_t rt = _sock->open(_net);
    if (rt != NSAPI_ERROR_OK) {
        printf("\r\nCould not open() TCP Socket (%d)\r\n", rt);
        return rt;
    }
    rt = _sock->connect(_host, _port);
    if (rt != NSAPI_ERROR_OK) {
        printf("\r\nCould not connect() TCP socket (%d)\r\n", rt);
        return rt;
    }

    _mp->init();

//     // tag, timestamp, message
//     if (!_mp->start_array(3)) {
//         return -1;
//     }
//     if (!_mp->set_str(tag, strlen(tag))) {
//         return -1;
//     }
// #ifdef USE_NTP  
//     if (!_mp->set_u32(time(NULL))) {
//         return -1;
//     }
// #else
//     if (!_mp->set_u32(0)) {
//         return -1;
//     }
// #endif
//     if (!_mp->set_str(msg, strlen(msg))) {
//         return -1;
//     }
        // tag, timestamp, message
    if (!_mp->start_array(3)) {
        return -1;
    }
    if (!_mp->set_str(tag, strlen(tag))) {
        return -1;
    }
    if (!_mp->set_u32(0)) {
        return -1;
    }
    if (!_mp->set_str(msg, strlen(msg))) {
        
        return -1;
    }
    return(send());
}

int FluentLogger::log(const char *tag, uMP &mpmsg)
{

    nsapi_error_t rt = _sock->open(_net);
    if (rt != NSAPI_ERROR_OK) {
        printf("\r\nCould not open TCP Socket (%d)\r\n", rt);
        return -1;
    }
    rt = _sock->connect(_host, _port);
    if (rt != NSAPI_ERROR_OK) {
        printf("Could not connect TCP socket (%d)\r\n", rt);
        return -1;
    }

    _mp->init();

    // tag, timestamp, message
    if (!_mp->start_array(3)) {
        return -1;
    }
    if (!_mp->set_str(tag, strlen(tag))) {
        return -1;
    }
#ifdef USE_NTP  
    if (!_mp->set_u32(time(NULL))) {
        return -1;
    }
#else
    if (!_mp->set_u32(0)) {
        return -1;
    }
#endif
    if (!_mp->set_raw((const char*)mpmsg.get_buffer(), mpmsg.get_size())) {
        return -1;
    }
    return(send());
}

int FluentLogger::send()
{
    // _sock->set_blocking(false);
    nsapi_error_t rt = _sock->send((char*)_mp->get_buffer(), (int)_mp->get_size());
    if ( rt != NSAPI_ERROR_OK ) {//fail
        rt = close();
        return(rt);
    }
    return(rt);
}
