/*
    unparam
    Copyright (c) 2015-2026, Chris Desjardins
    https://github.com/cdesjardins/ComBomb cjd@chrisd.info

    SPDX-License-Identifier: BSD-3-Clause
    See the LICENSE file at the project root for the full license text.
*/

#ifndef UNREF_PARAM

#ifndef WIN32
#define UNREF_PARAM(expr) do { (void)(expr); } while (0)
#else
#define UNREF_PARAM(expr) do { (void)(expr); } while (0, 0)
#endif

#endif
