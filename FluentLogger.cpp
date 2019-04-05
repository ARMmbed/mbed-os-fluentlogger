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
#include "mbed_trace.h"
#ifdef USE_NTP
#include <time.h>
#endif

#define TRACE_GROUP "FLUENTLOGGER"

FluentLogger::FluentLogger(NetworkInterface* aNetwork, const char *host, const int port, uint32_t bufsize) :
_sock(), _host(host), _port(port), _timeout(1000)
{
    _mp = new uMP(bufsize);
    _sock = new TCPSocket();
    _net = aNetwork;
    _socket_is_tls = false;
}

FluentLogger::FluentLogger(NetworkInterface* aNetwork, const char* ssl_ca_pem, const char *host, const int port, uint32_t bufsize) :
_sock(), _host(host), _port(port), _timeout(1000)
{
    _mp = new uMP(bufsize);
    _sock = new TLSSocket();
    _net = aNetwork;
    _socket_is_tls = true;
    ((TLSSocket*)_sock)->set_root_ca_cert(ssl_ca_pem);
}

int FluentLogger::close()
{

    if(_socket_is_tls){
        _rt = ((TLSSocket*)_sock)->close();
        if (_rt != NSAPI_ERROR_OK) {
            tr_debug("Could not close() TLS Socket (%d)", _rt);
            return _rt;
        }
    }
    else{
        _rt = ((TCPSocket*)_sock)->close();
        if (_rt != NSAPI_ERROR_OK) {
            tr_debug("Could not close() TCP Socket (%d)", _rt);
            return _rt;
        }
    }

    // delete _sock;
    // _sock = NULL;
    return _rt;
}

int FluentLogger::log(const char *tag, const char *msg)
{
    // if(_socket_is_tls){
    //     nsapi_error_t rt = ((TLSSocket*)_sock)->open(_net);
    //     if (rt != NSAPI_ERROR_OK) {
    //         tr_debug("\r\nCould not open() TCP Socket (%d)\r\n", rt);
    //         return rt;
    //     }
    //     rt = ((TLSSocket*)_sock)->connect(_host, _port);
    //     if (rt != NSAPI_ERROR_OK) {
    //         tr_debug("\r\nCould not connect() TCP socket (%d)\r\n", rt);
    //         return rt;
    //     }
    // }
    // else{
    //     nsapi_error_t rt = ((TCPSocket*)_sock)->open(_net);
    //     if (rt != NSAPI_ERROR_OK) {
    //         tr_debug("\r\nCould not open() TCP Socket (%d)\r\n", rt);
    //         return rt;
    //     }
    //     rt = ((TCPSocket*)_sock)->connect(_host, _port);
    //     if (rt != NSAPI_ERROR_OK) {
    //         tr_debug("\r\nCould not connect() TCP socket (%d)\r\n", rt);
    //         return rt;
    //     }
    // }

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

    // nsapi_error_t rt = _sock->open(_net);
    // if (rt != NSAPI_ERROR_OK) {
    //     tr_debug("\r\nCould not open TCP Socket (%d)\r\n", rt);
    //     return -1;
    // }
    // rt = _sock->connect(_host, _port);
    // if (rt != NSAPI_ERROR_OK) {
    //     tr_debug("Could not connect TCP socket (%d)\r\n", rt);
    //     return -1;
    // }

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
    if(_socket_is_tls){
        tr_debug("Socket Open TLS");
        _rt = ((TLSSocket*)_sock)->open(_net);
        if (_rt != NSAPI_ERROR_OK) {
            tr_debug("Could not open() TLS Socket (%d)", _rt);
            // _rt = close();
            // return _rt;
        }
        tr_debug("Socket Connect TLS");
        _rt = ((TLSSocket*)_sock)->connect(_host, _port);
        if (_rt != NSAPI_ERROR_OK) {
            tr_debug("Could not connect() TLS socket (%d)", _rt);
            // _rt = close();
            // return _rt;
        }
        tr_debug("Socket Send TLS");
        // _sock->set_blocking(false);
        _rt = ((TLSSocket*)_sock)->send((char*)_mp->get_buffer(), (int)_mp->get_size());
        // if ( _rt != NSAPI_ERROR_OK ) {//fail
        //     tr_debug("\r\nCould not send() TLS socket (%d)\r\n", _rt);
        //     _rt = close();
        //     return(_rt);
        // }
    }
    else{
        tr_debug("Socket Open TCP");
        _rt = ((TCPSocket*)_sock)->open(_net);
        if (_rt != NSAPI_ERROR_OK) {
            tr_debug("Could not open() TCP Socket (%d)", _rt);
            return _rt;
        }
        tr_debug("Socket Connect TCP");
        _rt = ((TCPSocket*)_sock)->connect(_host, _port);
        if (_rt != NSAPI_ERROR_OK) {
            tr_debug("Could not connect() TCP socket (%d)", _rt);
            return _rt;
        }
        tr_debug("Socket Send TCP");
        // _sock->set_blocking(false);
        _rt = ((TCPSocket*)_sock)->send((char*)_mp->get_buffer(), (int)_mp->get_size());
        if ( _rt<0 && _rt != NSAPI_ERROR_OK ) {//fail
            tr_debug("Socket Send failed, closing TLS socket");
            _rt = close();
            return(_rt);
        }
        _rt = close();
    }
    // _rt = close();
    return(_rt);
}
