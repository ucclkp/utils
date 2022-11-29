// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_NET_ABNF_H_
#define UTILS_NET_ABNF_H_

/**
 * Augmented BNF rules.
 * https://datatracker.ietf.org/doc/html/rfc5234
 */

#define abnf_ascii_is_ALPHA(c)  \
    ((c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A))

#define abnf_ascii_is_CHAR(c)  \
    (c >= 0x01 && c <= 0x7F)

#define abnf_ascii_is_VCHAR(c)  \
    (c >= 0x21 && c <= 0x7E)

#define abnf_ascii_is_DIGIT(c)  \
    (c >= 0x30 && c <= 0x39)

#define abnf_ascii_is_HEXDIG(c)  \
    (abnf_ascii_is_DIGIT(c) || (c >= 0x61 && c <= 0x66) || (c >= 0x41 && c <= 0x46))

#define abnf_ascii_is_OCTET(c)  \
    (c >= 0x00 && c <= 0xFF)

#define abnf_ascii_is_CTL(c)  \
    ((c >= 0x00 && c <= 0x1F) || (c == 0x7F))

#define abnf_ascii_is_CR(c)  \
    (c == 0x0D)

#define abnf_ascii_is_LF(c)  \
    (c == 0x0A)

#define abnf_ascii_is_SP(c)  \
    (c == 0x20)

#define abnf_ascii_is_HTAB(c)  \
    (c == 0x09)

#define abnf_ascii_is_DQUOTE(c)  \
    (c == 0x22)

#define abnf_ascii_IS_WSP(c)  \
    (abnf_ascii_is_SP(c) || abnf_ascii_is_HTAB(c))

#endif  // UTILS_NET_ABNF_H_