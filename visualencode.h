/*
    visualencode
    Copyright (C) 2015  Chris Desjardins
    http://blog.chrisd.info cjd@chrisd.info

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
