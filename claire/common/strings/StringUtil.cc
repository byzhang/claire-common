// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/strings/StringUtil.h>

#include <stdio.h>
#include <assert.h>

#include <algorithm>

#include <boost/type_traits/is_arithmetic.hpp>

namespace claire {

#pragma GCC diagnostic ignored "-Wtype-limits"
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "digits size not equal 20");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof(digitsHex) == 17, "digitsHex size not equal 17");

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t IntToBuffer(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t HexToBuffer(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;

    do
    {
        int lsd = i % 16;
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template size_t IntToBuffer(char buf[], char);
template size_t IntToBuffer(char buf[], short);
template size_t IntToBuffer(char buf[], unsigned short);
template size_t IntToBuffer(char buf[], int);
template size_t IntToBuffer(char buf[], unsigned int);
template size_t IntToBuffer(char buf[], long);
template size_t IntToBuffer(char buf[], unsigned long);
template size_t IntToBuffer(char buf[], long long);
template size_t IntToBuffer(char buf[], unsigned long long);

size_t HexString(char buff[], size_t bufflen,
                 const char *str, size_t strlen)
{
    size_t currlen = 0;
    for (size_t i = 0;i < strlen; i++)
    {
        int len = 0;
        if (i % 16 == 0)
        {
            len = snprintf(buff + currlen, bufflen - currlen, "\n0x%04x:", static_cast<unsigned int>(i));
            if (len < 0 || len > static_cast<int>(bufflen - currlen))
            {
                break;
            }
            currlen += len;
        }

        if (i % 2 == 0)
        {
            len = snprintf(buff + currlen, bufflen - currlen, " ");
            if (len < 0 || len > static_cast<int>(bufflen - currlen))
            {
                break;
            }
            currlen += len;
        }

        len = snprintf(buff + currlen,  bufflen - currlen, "%02x", static_cast<uint8_t>(str[i]));
        if (len < 0 || len > static_cast<int>(bufflen - currlen))
        {
            break;
        }
        currlen += len;
    }

    return currlen;
}

} // namespace claire
