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

#ifndef FLUENT_LOGGER_MBED_H
#define FLUENT_LOGGER_MBED_H
#include "mbed.h"
#include "TCPSocket.h"
#include "TLSSocket.h"
#include "uMP.h"

/** Fluent Logger for mbed
 *
 */
class FluentLogger {
public:
    /** Create a FluentLogger instance with TCP Socket
     *
     * @param host fluentd server hostname/ipaddress
     * @param port fluentd server port (default: 24224)
     * @param bufsize message buffer length (default: 128)
     */
    FluentLogger(NetworkInterface* aNetwork, const char *host, const int port = 24224, uint32_t bufsize = 128);

    /** Create a FluentLogger instance with TLS socket
     *
     * @param host fluentd server hostname/ipaddress
     * @param port fluentd server port (default: 24224)
     * @param bufsize message buffer length (default: 128)
     */
    FluentLogger(NetworkInterface* aNetwork, const char* ssl_ca_pem, const char *host, const int port = 24224, uint32_t bufsize = 128);

    /** Open connection (automatically called on log)
     *
     * @retval 0 Success
     * @retval -1 Failure
     */
    int open();

    /** Close connection (if you want)
     *
     * @retval 0 Success
     * @retval -1 Failure
     */
    int close();

    /** Send simple string message to fluent server with tag.
     *
     * @param tag tag
     * @param msg null terminated string
     * @retval 0 Success
     * @retval -1 Failure
     */
    int log(const char *tag, const char *msg);

    /** Send MassagePacked message to fluent server with tag.
     *
     * @param tag tag
     * @param msg MessagePacked message
     * @retval 0 Success
     * @retval -1 Failure
     */
    int log(const char *tag, uMP &msg);

private:
    /** FluentLogger
     */
    FluentLogger();
    /** send message via TCP
     * @retval 0 Success
     * @retval -1 Failure
     */
    int send();

    NetworkInterface *_net;
    Socket* _sock;
    nsapi_error_t _rt;
    bool _socket_is_tls;
    const char *_host;
    const int  _port;
    int        _timeout;
    uMP        *_mp;
};

#endif // FLUENT_LOGGER_MBED_H
