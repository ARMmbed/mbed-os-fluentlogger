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

#include "uMP.h"

uMP::uMP() :
_ptr(0), _nbuf(DEFAULT_BUFFSIZE)
{
    _buf = new uint8_t[_nbuf]; 
}

uMP::uMP(uint32_t size) :
_ptr(0), _nbuf(size)
{
  _buf = new uint8_t[_nbuf]; 
}

uMP::~uMP()
{
    delete[] _buf;
}

/* MessagePack funcions (Subset) */
bool uMP::set_nil()
{
    return set_buffer(TAG_NIL);
}

bool uMP::set_true()
{
    return set_buffer(TAG_TRUE);
}

bool uMP::set_false()
{
    return set_buffer(TAG_FALSE);
}

bool uMP::start_array(uint32_t size)
{
    if (size <= 0x0f) {
        return set_buffer((uint8_t)(TAG_FIXARRAY | size));
    }
    return false;
}

bool uMP::start_map(uint32_t size)
{
    if (size <= 0x0f) {
        return set_buffer((uint8_t)(TAG_FIXMAP | size));
    }
#if 0
    if (size <= 0xffff) {
        return cmp_write_map16(ctx, size);
    }
#endif
    return false;
}

bool uMP::set_uint(uint32_t u)
{
    if (u <= 0x7f) {
        return set_buffer((uint8_t)u);
    }
    if (u <= 0xff) {
        return set_u8((uint8_t)u);
    }
    if (u <= 0xffff) {
        return set_u16((uint16_t)u);
    }
    if (u <= 0xffffffff) {
        return set_u32(u);
    }
    return false;
}

bool uMP::set_u8(uint8_t u)
{
    if (!set_buffer((uint8_t)TAG_U8)) {
        return false;
    }
    return set_buffer(u);
}

bool uMP::set_u16(uint16_t u)
{
    if (!set_buffer((uint8_t)TAG_U16)) {
        return false;
    }

    u = to_be16(u);
    return set_buffer((uint8_t*)&u, sizeof(uint16_t));
}

bool uMP::set_u32(uint32_t u)
{
    if (!set_buffer((uint8_t)TAG_U32)) {
        return false;
    }

    u = to_be32(u);
    return set_buffer((uint8_t*)&u, sizeof(uint32_t));
}

bool uMP::set_u64(uint64_t u)
{
    if (!set_buffer((uint8_t)TAG_U64)) {
        return false;
    }

    u = to_be64(u);
    return set_buffer((uint8_t*)&u, sizeof(uint64_t));
}

bool uMP::set_sint(int32_t i)
{
    if (i >=0) {
        return set_uint((uint32_t)i);
    }
    if (i >= -32) {
        return set_buffer((uint8_t)i);
    }
    if (i >= -128) {
        return set_s8(i);
    }
    if (i >= -32768) {
        return set_s16(i);
    }
    if (i >= -2147483648) {
        return set_s32(i);
    }
    return false;
}

bool uMP::set_s8(int8_t i)
{
    if (!set_buffer((uint8_t)TAG_S8)) {
        return false;
    }
    return set_buffer((uint8_t)i);
}

bool uMP::set_s16(int16_t i)
{
    if (!set_buffer((uint8_t)TAG_S16)) {
        return false;
    }

    i = to_be16(i);
    return set_buffer((uint8_t*)&i, sizeof(int16_t));
}

bool uMP::set_s32(int32_t i)
{
    if (!set_buffer((uint8_t)TAG_S32)) {
        return false;
    }

    i = to_be32(i);
    return set_buffer((uint8_t*)&i, sizeof(int32_t));
}

bool uMP::set_s64(int64_t i)
{
    if (!set_buffer((uint8_t)TAG_S64)) {
        return false;
    }

    i = to_be64(i);
    return set_buffer((uint8_t*)&i, sizeof(int64_t));
}

bool uMP::set_str(const char *data, uint32_t size)
{
    if (size <= 0x1f) {
        return set_fixstr(data, size);
    }
    if (size <= 0xff) {
        return set_str8(data, size);
    }
    return false;
}

bool uMP::set_str(const std::string& str)
{
    return set_str(str.c_str(), (uint32_t)str.size());
}

bool uMP::set_fixstr(const char *data, uint8_t size)
{
    if (size > 0x1f) {
        return false;
    }
    if (!set_buffer((uint8_t)(TAG_FIXSTR | size))) {
        return false;
    }
    if (!set_buffer((uint8_t*)data, size)) {
        return false;
    }
    return true;
}

bool uMP::set_str8(const char *data, uint8_t size)
{
    if (size > 0xff) {
        return false;
    }
    if (!set_buffer((uint8_t)TAG_STR8)) {
        return false;
    }
    if (!set_buffer((uint8_t)size)) {
        return false;
    }
    if (!set_buffer((uint8_t*)data, size)) {
        return false;
    }
    return true;
}

bool uMP::set_raw(const char *data, uint8_t size)
{
    if (!set_buffer((uint8_t*)data, size)) {
        return false;
    }
    return true;
}

bool uMP::set_float(float f)
{
    if (!set_buffer((uint8_t)TAG_FLOAT32)) {
        return false;
    }
    f = to_be32(f);
    if (!set_buffer((uint8_t*)&f, sizeof(float))) {
        return false;
    }
    return true;
}

bool uMP::set_double(double d)
{
    if (!set_buffer((uint8_t)TAG_FLOAT64)) {
        return false;
    }
    d = to_be64(d);
    if (!set_buffer((uint8_t*)&d, sizeof(double))) {
        return false;
    }
    return true;
}

bool uMP::set_buffer(const uint8_t c)
{
    //buffer overflow?
    if ( _ptr == _nbuf) {
        return false;
    }
    *(_buf+_ptr) = c;
    _ptr++;
    return true;
}

bool uMP::set_buffer(const uint8_t *c, size_t size)
{
    //buffer overflow?
    if ( (_ptr+size) > _nbuf) {
        return false;
    }
    while (size--) {
        *(_buf+_ptr) = *c++;
        _ptr++;
    }
    return true;
}

//ByteOrder
template<typename T> T uMP::to_be16(T t)
{
    uint16_t *x = (uint16_t *)&t;
    *x = __REV16(*x);
    return t;
}

template<typename T> T uMP::to_be32(T t)
{
    uint32_t *x = (uint32_t *)&t;
    *x = __REV(*x);
    return t;
}

template<typename T> T uMP::to_be64(T t)
{
    uint32_t *x = (uint32_t *)&t;
    uint32_t h = __REV(*x);
    uint32_t l = __REV(*(x+1));
    //swap
    *x     = l;
    *(x+1) = h;
    return t;
}

// map functions
bool uMP::map(const std::string& k, uint8_t v)
{
    if( set_str(k) == false )
        return false;
    return set_u8(v);
}

bool uMP::map(const std::string& k, uint16_t v)
{
    if( set_str(k) == false )
        return false;
    return set_u16(v);
}

bool uMP::map(const std::string& k, uint32_t v)
{
    if( set_str(k) == false )
        return false;
    return set_uint(v);
}

bool uMP::map(const std::string& k, int8_t v)
{
    if( set_str(k) == false )
        return false;
    return set_s8(v);
}

bool uMP::map(const std::string& k, int16_t v)
{
    if( set_str(k) == false )
        return false;
    return set_s16(v);
}

bool uMP::map(const std::string& k, int32_t v)
{
    if( set_str(k) == false )
        return false;
    return set_sint(v);
}

bool uMP::map(const std::string& k, float v)
{
    if( set_str(k) == false )
        return false;
    return set_float(v);
}

bool uMP::map(const std::string& k, double v)
{
    if( set_str(k) == false )
        return false;
    return set_double(v);
}

bool uMP::map(const std::string& k, const char *v)
{
    if( set_str(k) == false )
        return false;
    return set_str(v);
}

bool uMP::map(const std::string& k, const std::string& v)
{
    if( set_str(k) == false )
        return false;
    return set_str(v);
}
