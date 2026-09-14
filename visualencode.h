/*
    visualencode
    Copyright (c) 2015-2026, Chris Desjardins
    https://github.com/cdesjardins/ComBomb cjd@chrisd.info

    SPDX-License-Identifier: BSD-3-Clause
    See the LICENSE file at the project root for the full license text.
*/
#ifndef _VISUAL_ENCODE_Hxx
#define _VISUAL_ENCODE_Hxx

#include <string>
#include <sstream>
#include <iomanip>
#include "unparam.h"

class VisualEncode
{
public:
    VisualEncode() = delete;
    VisualEncode(const VisualEncode&) = delete;
    VisualEncode(const char* msg)
    {
        UNREF_PARAM(msg);
    }

    const std::string& getEncoded()
    {
        return _encoded;
    }

protected:
    std::string _encoded;
    virtual void encodeString(const char* msg) = 0;
private:
};

class VisualEncodeSafeOctalNoSlash : public VisualEncode
{
public:
    VisualEncodeSafeOctalNoSlash() = delete;
    VisualEncodeSafeOctalNoSlash(const VisualEncodeSafeOctalNoSlash&) = delete;
    VisualEncodeSafeOctalNoSlash(const char* msg)
        : VisualEncode(msg)
    {
        encodeString(msg);
    }

protected:
    void encodeString(const char* msg)
    {
        for (int i = 0; msg[i]; i++)
        {
            char ch = msg[i];
            if (allow(ch))
            {
                // In the event that there are two newlines in a row
                // assume they meant newline + carrage return
                if ((_encoded.length() > 0) && (ch == 0x0A) && (_encoded.back() == 0x0A))
                {
                    ch = 0x0D;
                }
                _encoded.push_back(ch);
            }
            else
            {
                encodeNonPrint(ch);
            }
        }
    }

    void encodeNonPrint(const unsigned char ch)
    {
        std::stringstream ss;
        _encoded.push_back('\\');
        ss << std::oct << std::setfill('0') << std::setw(3) << (const unsigned int) ch;
        _encoded.append(ss.str());
    }

    bool allow(const char ch) const
    {
        bool ret = false;
        if ((isprint(ch)) ||
            (ch == 0x07) ||
            (ch == 0x08) ||
            (ch == 0x09) ||
            (ch == 0x0A) ||
            (ch == 0x0D))
        {
            ret = true;
        }
        return ret;
    }

private:
};

#endif
