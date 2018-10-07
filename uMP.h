/* uMP - micro MessagePack class
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

#ifndef MBED_UMP_H
#define MBED_UMP_H

#include "mbed.h"
#include <stdint.h>
#include <string.h>
#include <string>

/** Subset of MessagePack implementation.
 *
 */
class uMP {
public:
    /** uMP
     */
    uMP();
    /** uMP
     *
     * @param size buffer size
     */
    explicit uMP(uint32_t size);
    ~uMP();

    /** Initialize buffer pointer
     */
    void init(){ _ptr = 0; }

    /** Get message size
     *
     * @return message size(bytes)
     */
    inline uint32_t get_size(){ return _ptr; }

    /** Get message buffer pointer
     *
     * @return Pointer of message buffer
     */
    inline uint8_t *get_buffer(){ return _buf; }

    /** Start array format
     *
     * @param size Number of array elements
     * @retval true Success
     * @retval false Failure
     */
    bool start_array(uint32_t size);

    /** Start map format
     *
     * @param size Number of map pairs
     * @retval true Success
     * @retval false Failure
     */
    bool start_map(uint32_t size);

    /** Set NIL message
     *
     * @retval true Success
     * @retval false Failure
     */
    bool set_nil();

    /** Set TRUE message (bool family)
     *
     * @retval true Success
     * @retval false Failure
     */
    bool set_true();

    /** Set FALSE message (bool family)
     *
     * @retval true Success
     * @retval false Failure
     */
    bool set_false();

    /** Set unsigned int message
     *
     * Auto route the optimal function.
     *
     * @param u unsigned int value(max 32bit)
     * @retval true Success
     * @retval false Failure
     */
    bool set_uint(uint32_t u); //max 32bit

    /** Set uint8 message
     *
     * @param u uint8_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_u8(uint8_t u);

    /** Set uint16 message
     *
     * @param u uint16_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_u16(uint16_t u);

    /** Set uint32 message
     *
     * @param u uint32_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_u32(uint32_t u);

    /** Set uint64 message
     *
     * @param u uint64_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_u64(uint64_t u);

    /** Set signed int message
     *
     * Auto route the optimal function.
     *
     * @param s signed int value(max 32bit)
     * @retval true Success
     * @retval false Failure
     */
    bool set_sint(int32_t i);  //max 32bit

    /** Set int8 message
     *
     * @param s int8_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_s8(int8_t i);

    /** Set int16 message
     *
     * @param s int16_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_s16(int16_t i);

    /** Set int32 message
     *
     * @param s int32_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_s32(int32_t i);

    /** Set int64 message
     *
     * @param s int64_t value
     * @retval true Success
     * @retval false Failure
     */
    bool set_s64(int64_t i);

    /** Set float(32bit) message
     *
     * @param f float value
     * @retval true Success
     * @retval false Failure
     */
    bool set_float(float f);

    /** Set double(64bit) message
     *
     * @param d double value
     * @retval true Success
     * @retval false Failure
     */
    bool set_double(double d);

    /** Set string message
     *
     * Auto route the optimal function.
     *
     * @param data Pointer of message string
     * @param size Size of message string (max 255 bytes)
     * @retval true Success
     * @retval false Failure
     */
    bool set_str(const char *data, uint32_t size);

    /** Set string message
     *
     * Auto route the optimal function.
     *
     * @param str string of message string
     * @retval true Success
     * @retval false Failure
     */
    bool set_str(const std::string& str);

    /** Set string message (upto 31 bytes)
     *
     * @param data Pointer of message string
     * @param size Size of message string (max 31 bytes)
     * @retval true Success
     * @retval false Failure
     */
    bool set_fixstr(const char *data, uint8_t size);

    /** Set string message (upto 256 bytes)
     *
     * @param data Pointer of message string
     * @param size Size of message string (max 255 bytes)
     * @retval true Success
     * @retval false Failure
     */
    bool set_str8(const char *data, uint8_t size);

    /** Set raw message
     *
     * Insert the pre build message into buffer.
     * This function is not MessagePack standard.
     *
     * @param data Pointer of message string
     * @param size Size of message string
     * @retval true Success
     * @retval false Failure
     */
    bool set_raw(const char *data, uint8_t size);

    /** associate a key with value (bool)
     *
     * @param k key string
     * @param v bool value(true/false)
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, bool v);

    /** associate a key with value (uint8_t)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, uint8_t v);

    /** associate a key with value (uint16_t)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, uint16_t v);

    /** associate a key with value (uint32_t)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, uint32_t v);

    /** associate a key with value (int8_t)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, int8_t v);

    /** associate a key with value (int16_t)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, int16_t v);

    /** associate a key with value (int32_t)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, int32_t v);

    /** associate a key with value (float)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, float v);

    /** associate a key with value (double)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, double v);

    /** associate a key with value (char * string)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, const char *v);

    /** associate a key with value (string)
     *
     * @param k key string
     * @param v value
     * @retval true Success
     * @retval false Failure
     */
    bool map(const std::string& k, const std::string& v);

private:
    enum MpTag{
        TAG_POSITIVE_FIXNUM = 0x00,
        TAG_FIXMAP          = 0x80,
        TAG_FIXARRAY        = 0x90,
        TAG_FIXSTR          = 0xa0,
        TAG_NIL             = 0xc0,
        TAG_FALSE           = 0xc2,
        TAG_TRUE            = 0xc3,
//      TAG_BIN8            = 0xc4,
//      TAG_BIN16           = 0xc5,
//      TAG_BIN32           = 0xc6,
//      TAG_EXT8            = 0xc7,
//      TAG_EXT16           = 0xc8,
//      TAG_EXT32           = 0xc9,
        TAG_FLOAT32         = 0xca,
        TAG_FLOAT64         = 0xcb,
        TAG_U8              = 0xcc,
        TAG_U16             = 0xcd,
        TAG_U32             = 0xce,
        TAG_U64             = 0xcf,
        TAG_S8              = 0xd0,
        TAG_S16             = 0xd1,
        TAG_S32             = 0xd2,
        TAG_S64             = 0xd3,
//      TAG_FIXEXT1         = 0xd4,
//      TAG_FIXEXT2         = 0xd5,
//      TAG_FIXEXT16        = 0xd8,
        TAG_STR8            = 0xd9,
//      TAG_STR16           = 0xda,
//      TAG_STR32           = 0xdb,
//      TAG_ARRAY16         = 0xdc,
//      TAG_ARRAY32         = 0xdd,
//      TAG_MAP16           = 0xde,
//      TAG_MAP32           = 0xdf,
        TAG_NEGATIVE_FIXNUM = 0xe0
    };

    static const uint16_t DEFAULT_BUFFSIZE = 128;
    uint8_t   *_buf;
    uint32_t  _ptr;
    uint32_t  _nbuf;

    /** Insert sigle byte fomrat message
     *
     * @param c single byte message format data
     * @retval true Success
     * @retval false Failure
     */
    bool set_buffer(const uint8_t c);

    /** Insert multi byte fomrat message
     *
     * @param c Pointer of message data
     * @param sise Size of message data
     * @retval true Success
     * @retval false Failure
     */
    bool set_buffer(const uint8_t *c, size_t size);

    /** Endian converter - 16bit data
     *
     * @param t 16bit data
     * @return converted 16bit data
     */
    template<typename T> T to_be16(T t);

    /** Endian converter - 32bit data
     *
     * @param t 32bit data
     * @return converted 32bit data
     */
    template<typename T> T to_be32(T t);

    /** Endian converter - 64bit data
     *
     * @param t 64bit data
     * @return converted 64bit data
     */
    template<typename T> T to_be64(T t);
};

#endif
