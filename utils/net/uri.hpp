// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_NET_URI_HPP_
#define UTILS_NET_URI_HPP_

#include <cassert>
#include <string>

#include "abnf.h"


namespace utl {

    inline bool uri_ascii_is_gen_delims(unsigned int c) {
        switch (c) {
        case ':':
        case '/':
        case '?':
        case '#':
        case '[':
        case ']':
        case '@':
            return true;
        default:
            return false;
        }
    }

    inline bool uri_ascii_is_sub_delims(unsigned int c) {
        switch (c) {
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '=':
            return true;
        default:
            return false;
        }
    }

    inline bool uri_ascii_is_reserved(unsigned int c) {
        return uri_ascii_is_gen_delims(c) || uri_ascii_is_sub_delims(c);
    }

    inline bool uri_ascii_is_unreserved(unsigned int c) {
        if (abnf_ascii_is_ALPHA(c) ||
            abnf_ascii_is_DIGIT(c))
        {
            return true;
        }

        switch (c) {
        case '-':
        case '.':
        case '_':
        case '~':
            return true;
        default:
            return false;
        }
    }


    /**
     * URI implemention.
     * https://datatracker.ietf.org/doc/html/rfc3986
     */
    class uri {
    public:
        using Cy = char;
        using Str = std::basic_string<Cy>;

        uri() {}

        bool parse(const Cy* s, const Cy** p = nullptr) {
            const Cy* _p;
            size_t len = std::char_traits<Cy>::length(s);
            bool ret = parseItl(s, len, URI_ORG, &_p);
            if (p) *p = _p;
            return ret;
        }

        bool parseRef(const Cy* s, const Cy** p = nullptr) {
            const Cy* _p;
            size_t len = std::char_traits<Cy>::length(s);
            bool ret = parseItl(s, len, URI_REF, &_p);
            if (p) *p = _p;
            return ret;
        }

        bool parseAbs(const Cy* s, const Cy** p = nullptr) {
            const Cy* _p;
            size_t len = std::char_traits<Cy>::length(s);
            bool ret = parseItl(s, len, URI_ABS, &_p);
            if (p) *p = _p;
            return ret;
        }

        bool parseSuf(const Cy* s, const Cy** p = nullptr) {
            const Cy* _p;
            size_t len = std::char_traits<Cy>::length(s);
            bool ret = parseItl(s, len, URI_SUF, &_p);
            if (p) *p = _p;
            return ret;
        }

        bool resolution(
            const Cy* bs, const Cy* rs,
            const Cy** bs_p = nullptr, const Cy** rs_p = nullptr)
        {
            const Cy* _bs_p;
            size_t bs_len = std::char_traits<Cy>::length(bs);

            uri base;
            if (!base.parseItl(bs, bs_len, URI_ABS, &_bs_p)) {
                *bs_p = bs;
                *rs_p = rs;
                return false;
            }

            const Cy* _rs_p;
            size_t rs_len = std::char_traits<Cy>::length(rs);

            uri ref;
            if (!ref.parseItl(rs, rs_len, URI_REF, &_rs_p)) {
                *bs_p = _bs_p;
                *rs_p = rs;
                return false;
            }

            target(false, base, ref, this);
            *bs_p = _bs_p;
            *rs_p = _rs_p;
            return true;
        }

        void clear() {
            scheme_.clear();
            auth_.user.clear();
            auth_.host.clear();
            auth_.port = 0;
            auth_.type = HOST_NONE;
            path_.clear();
            query_.clear();
            fragment_.clear();
            cdef_ = CD_NONE;
        }

        const Str& getScheme() const { return scheme_; }
        const Str& getUser() const { return auth_.user; }
        const Str& getHost() const { return auth_.host; }
        unsigned int getPort() const { return auth_.port; }
        const Str& getPath() const { return path_; }
        const Str& getQuery() const { return query_; }
        const Str& getFragment() const { return fragment_; }

    private:
        enum URIType {
            URI_ORG,
            URI_REF,
            URI_ABS,
            URI_SUF,
        };

        enum PartType {
            PART_HIER,
            PART_RELA,
            PART_SUFF,
        };

        enum HostType {
            HOST_NONE,
            HOST_IPV4,
            HOST_IPV6,
            HOST_IPVF,
            HOST_NAME,
        };

        enum ComponentDef {
            CD_NONE   = 0,

            CD_SCHEME = 1 << 0,
            CD_AUTH   = 1 << 1,
            CD_QUERY  = 1 << 2,
            CD_FRAG   = 1 << 3,

            CD_USER   = 1 << 4,
            CD_PORT   = 1 << 5,
        };

        struct Authority {
            Str user;
            Str host;
            Str port_str;
            HostType type;
            unsigned int port;
            unsigned int sc_defs;
        };

        bool parseItl(
            const Cy* s, size_t len, URIType type, const Cy** p)
        {
            auto se = s + len;
            auto ss = s;
            int is_ref = 0;

            clear();

            if (type != URI_SUF) {
                // scheme
                if (!parseScheme(ss, se - ss, &scheme_, &ss)) {
                    if (type != URI_REF) {
                        *p = s;
                        return false;
                    }
                    ss = s;
                    is_ref = 1;
                }

                // :
                if (!is_ref) {
                    if (ss >= se || *ss != ':') {
                        if (type != URI_REF) {
                            *p = s;
                            return false;
                        }
                        ss = s;
                        is_ref = 1;
                    } else {
                        cdef_ |= CD_SCHEME;
                        ++ss;
                    }
                }
            }

            // part
            PartType pt;
            if (type == URI_SUF) {
                pt = PART_SUFF;
            } else {
                pt = is_ref ? PART_RELA : PART_HIER;
            }

            if (!parsePathPart(
                ss, se - ss, pt, &cdef_, &auth_, &path_, &ss))
            {
                *p = s;
                return false;
            }

            // query
            if (parseQuery(ss, se - ss, &query_, &ss)) {
                cdef_ |= CD_QUERY;
            }

            // fragment
            if (type != URI_ABS) {
                if (parseFragment(ss, se - ss, &fragment_, &ss)) {
                    cdef_ |= CD_FRAG;
                }
            }

            *p = ss;
            return true;
        }

        static bool parseScheme(
            const Cy* s, size_t len, std::string* out, const Cy** p)
        {
            auto se = s + len;
            if (s >= se || !abnf_ascii_is_ALPHA(*s)) {
                *p = s;
                return false;
            }
            if (out) out->push_back(*s);
            ++s;

            for (; s < se;) {
                auto c = *s;
                if (abnf_ascii_is_ALPHA(c) ||
                    abnf_ascii_is_DIGIT(c) ||
                    c == '+' ||
                    c == '-' ||
                    c == '.')
                {
                    if (out) out->push_back(*s);
                    ++s;
                } else {
                    break;
                }
            }
            *p = s;
            return true;
        }

        static bool parsePctEncoded(
            const Cy* s, size_t len, Cy* val, const Cy** p)
        {
            if (len < 3 || *s != '%') {
                *p = s;
                return false;
            }

            auto _s = s;
            ++s;

            unsigned int v;
            if (abnf_ascii_is_HEXDIG(*s)) {
                v = ascii_ctoi(*s) * 16u;
                ++s;
                if (abnf_ascii_is_HEXDIG(*s)) {
                    *val = (Cy)(v + ascii_ctoi(*s));
                    ++s;
                    *p = s;
                    return true;
                }
            }

            *p = _s;
            return false;
        }

        static bool parseDecOctet(
            const Cy* s, size_t len, unsigned int* val, const Cy** p)
        {
            auto se = s + len;
            if (s < se && abnf_ascii_is_DIGIT(*s)) {
                *val = ascii_ctoi(*s);
                ++s;
                if (s < se && abnf_ascii_is_DIGIT(*s)) {
                    *val *= 10;
                    *val += ascii_ctoi(*s);
                    ++s;
                    if (s < se && abnf_ascii_is_DIGIT(*s)) {
                        *val *= 10;
                        *val += ascii_ctoi(*s);
                        if (*val > 255) {
                            *val /= 10;
                        } else {
                            ++s;
                        }
                    }
                }
            } else {
                *p = s;
                return false;
            }

            *p = s;
            return true;
        }

        static bool parseIPV4(
            const Cy* s, size_t len,
            Str* out, unsigned int v[4], const Cy** p)
        {
            auto _s = s;
            if (!parseDecOctet(s, len, &v[0], &s)) {
                *p = s;
                return false;
            }
            if (*s != '.') {
                *p = s;
                return false;
            }
            ++s;
            if (!parseDecOctet(s, len, &v[1], &s)) {
                *p = s;
                return false;
            }
            if (*s != '.') {
                *p = s;
                return false;
            }
            ++s;
            if (!parseDecOctet(s, len, &v[2], &s)) {
                *p = s;
                return false;
            }
            if (*s != '.') {
                *p = s;
                return false;
            }
            ++s;
            if (!parseDecOctet(s, len, &v[3], &s)) {
                *p = s;
                return false;
            }

            if (out) out->assign(_s, s - _s);

            *p = s;
            return true;
        }

        static bool parseh16(
            const Cy* s, size_t len, unsigned int* val, const Cy** p)
        {
            auto se = s + len;
            // 最少1个，最多4个
            if (s >= se || !abnf_ascii_is_HEXDIG(*s)) {
                *p = s;
                return false;
            }

            *val = ascii_ctoi(*s);
            ++s;

            for (size_t i = 0; i < 3 && s < se; ++i, ++s) {
                if (abnf_ascii_is_HEXDIG(*s)) {
                    *val *= 16;
                    *val += ascii_ctoi(*s);
                } else {
                    break;
                }
            }

            *p = s;
            return true;
        }

        static bool parseIPV6(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            auto se = s + len;
            if (s >= se) {
                *p = s;
                return false;
            }
            auto ss = s;

            int has_d = 0;
            int has_v4 = 0;
            size_t v6a_count = 0;
            size_t v6b_count = 0;
            unsigned int v6[8];
            unsigned int v4[4];

            size_t i = 0;
            for (; i < 8; ++i) {
                if (*ss == ':') {
                    if (ss + 1 >= se) {
                        // 结尾一个 :
                        bool valid = has_d ? true : (v6a_count == 8);
                        if (valid) {
                            break;
                        }
                        *p = s;
                        return false;
                    }
                    ++ss;
                    if (*ss == ':') {
                        if (has_d) {
                            --ss;
                            break;
                        }

                        // ::
                        has_d = 1;
                        ++ss;
                        if (ss >= se) {
                            break;
                        }
                    } else if (ss - s == 1) {
                        // 不允许开头仅一个 :
                        *p = s;
                        return false;
                    }
                }

                if (has_d && v6a_count + v6b_count == 7) {
                    break;
                }

                const Cy* dem;
                if (!parseh16(ss, se - ss, &v6[i], &dem)) {
                    // 后面不是 ipv6 了
                    if (ss - s >= 2) {
                        assert(*(ss - 1) == ':');
                        if (*(ss - 2) == ':') {
                            ss = dem;
                            break;
                        }
                    }
                    *p = s;
                    return false;
                }

                if (dem < se && *dem == '.') {
                    if (has_d) {
                        if (v6a_count > 5 ||
                            v6a_count + v6b_count > 5)
                        {
                            break;
                        }
                    } else {
                        if (v6a_count != 6) {
                            *p = s;
                            return false;
                        }
                    }

                    // ipv4
                    if (!parseIPV4(ss, len, 0, v4, &dem)) {
                        *p = s;
                        return false;
                    }
                    has_v4 = 1;
                    ss = dem;
                    break;
                }

                if (has_d) {
                    ++v6b_count;
                    if (v6a_count + v6b_count == 7) {
                        ss = dem;
                        break;
                    }
                } else {
                    ++v6a_count;
                }

                // h16 后面必须跟一个 : 或者到达尾部
                if (dem >= se || *dem != ':') {
                    // 后面不是 ipv6 了
                    bool valid = has_d ? true : (v6a_count == 8);
                    if (valid) {
                        ss = dem;
                        break;
                    }
                    *p = s;
                    return false;
                }
                ss = dem;
            }

            if (out) out->assign(s, ss - s);

            *p = ss;
            return true;
        }

        static bool parseIPvf(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            auto se = s + len;
            // "v" 在上层查看

            // hex dig
            auto ss = s;
            if (ss >= se || !abnf_ascii_is_HEXDIG(*ss)) {
                *p = s;
                return false;
            }
            ++ss;

            // hex digs
            for (; ss < se; ++ss) {
                if (!abnf_ascii_is_HEXDIG(*ss)) {
                    break;
                }
            }

            // .
            if (ss >= se || *ss != '.') {
                *p = s;
                return false;
            }
            ++ss;

            if (ss >= se ||
                (!uri_ascii_is_unreserved(*ss) &&
                !uri_ascii_is_sub_delims(*ss) &&
                *ss != ':'))
            {
                *p = s;
                return false;
            }
            ++ss;

            for (; ss < se; ++ss) {
                if (!uri_ascii_is_unreserved(*ss) &&
                    !uri_ascii_is_sub_delims(*ss) &&
                    *ss != ':')
                {
                    break;
                }
            }

            if (out) {
                out->push_back('v');
                out->append(s, ss - s);
            }

            *p = ss;
            return true;
        }

        static bool parseIPLiteral(
            const Cy* s, size_t len,
            HostType* type, Str* out, const Cy** p)
        {
            auto se = s + len;
            // "[" 在上层查看

            auto ss = s;
            if (ss >= se) {
                *p = s;
                return false;
            }

            if (*ss == 'v' || *ss == 'V') {
                // IPvFuture
                ++ss;
                if (!parseIPvf(ss, se - ss, out, &ss)) {
                    *p = s;
                    return false;
                }
                *type = HOST_IPVF;
            } else {
                // IPv6address
                if (!parseIPV6(ss, se - ss, out, &ss)) {
                    *p = s;
                    return false;
                }
                *type = HOST_IPV6;
            }

            // ]
            if (ss >= se || *ss != ']') {
                *p = s;
                return false;
            }
            ++ss;

            *p = ss;
            return true;
        }

        static bool parseRegName(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            auto se = s + len;
            if (s >= se) {
                *p = s;
                return true;
            }

            for (; s < se;) {
                if (!uri_ascii_is_unreserved(*s) &&
                    !uri_ascii_is_sub_delims(*s))
                {
                    Cy val;
                    if (parsePctEncoded(s, se - s, &val, &s)) {
                        out->push_back(val);
                    } else {
                        break;
                    }
                } else {
                    out->push_back(*s);
                    ++s;
                }
            }

            *p = s;
            return true;
        }

        static bool parseUserInfo(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            auto ss = s;
            auto se = ss + len;
            for (; ss < se;) {
                auto c = *ss;
                if (c == ':' ||
                    uri_ascii_is_unreserved(c) ||
                    uri_ascii_is_sub_delims(c))
                {
                    out->push_back(c);
                    ++ss;
                } else {
                    Cy oct;
                    if (!parsePctEncoded(ss, se - ss, &oct, &ss)) {
                        break;
                    }
                    out->push_back(oct);
                }
            }

            // @
            if (ss >= se || *ss != '@') {
                *p = s;
                return false;
            }
            ++ss;

            *p = ss;
            return true;
        }

        static bool parseHost(
            const Cy* s, size_t abs_len,
            HostType* type, Str* out, const Cy** p)
        {
            auto se = s + abs_len;
            auto ss = s;

            if (ss < se && *ss == '[') {
                // IP-literal
                ++ss;
                if (!parseIPLiteral(ss, se - ss, type, out, &ss)) {
                    *p = s;
                    return false;
                }
            } else {
                unsigned int v[4];
                const Cy* _s;
                if (parseIPV4(ss, abs_len, out, v, &_s) &&
                    (_s == se || *_s == ':'))
                {
                    // IPv4address
                    ss = _s;
                    *type = HOST_IPV4;
                } else {
                    // reg-name
                    if (!parseRegName(ss, abs_len, out, &ss) ||
                        (ss != se && *ss != ':'))
                    {
                        *p = s;
                        return false;
                    }
                    *type = HOST_NAME;
                }
            }

            *p = ss;
            return true;
        }

        static bool parsePort(
            const Cy* s, size_t len,
            Str* port_str, unsigned int* port, const Cy** p)
        {
            auto se = s + len;
            if (s >= se || *s != ':') {
                *p = s;
                return false;
            }
            ++s;

            *port = 0;
            auto ss = s;
            for (; s < se; ++s) {
                if (abnf_ascii_is_DIGIT(*s)) {
                    *port *= 10;
                    *port += ascii_ctoi(*s);
                } else {
                    break;
                }
            }

            if (port_str) port_str->assign(ss, s - ss);

            *p = s;
            return true;
        }

        static bool parseAuthority(
            const Cy* s, size_t len,
            Authority* auth, const Cy** p)
        {
            auto se = s + len;
            auto ss = s;

            auth->sc_defs = CD_NONE;

            // 找结尾
            auto st = ss;
            const Cy* sa = 0;
            int found = 0;
            for (; st < se; ++st) {
                switch (*st) {
                case '/':
                case '?':
                case '#': found = 1; break;
                case '@': sa = st;   break;
                default: break;
                }
                if (found) break;
            }
            if (!found) {
                st = se;
            }

            // userinfo
            if (sa) {
                if (!parseUserInfo(
                    ss, sa + 1 - ss, &auth->user, &ss))
                {
                    *p = s;
                    return false;
                }
                auth->sc_defs |= CD_USER;
            }

            // host
            if (!parseHost(
                ss, st - ss, &auth->type, &auth->host, &ss))
            {
                *p = s;
                return false;
            }

            if (ss >= se) {
                *p = ss;
                return true;
            }

            // port
            if (parsePort(
                ss, se - ss, &auth->port_str, &auth->port, &ss))
            {
                auth->sc_defs |= CD_PORT;
            }

            *p = ss;
            return true;
        }

        static bool parsePChar(
            const Cy* s, size_t len, Cy* out, const Cy** p)
        {
            auto se = s + len;
            if (s >= se) {
                *p = s;
                return false;
            }

            if (uri_ascii_is_unreserved(*s) ||
                uri_ascii_is_sub_delims(*s) ||
                *s == '@' ||
                *s == ':')
            {
                *out = *s;
                ++s;
                *p = s;
                return true;
            }

            const Cy* _p;
            if (!parsePctEncoded(s, se - s, out, &_p)) {
                *p = s;
                return false;
            }

            *p = _p;
            return true;
        }

        static void parsePathAbempty(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            auto se = s + len;
            for (; s < se;) {
                if (*s != '/') {
                    break;
                }
                if (out) out->push_back(*s);
                ++s;
                for (;;) {
                    Cy v;
                    if (!parsePChar(s, se - s, &v, &s)) {
                        break;
                    }
                    if (out) out->push_back(v);
                }
            }
            *p = s;
        }

        static void parsePathRootless(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            Cy v;
            auto se = s + len;
            if (!parsePChar(s, se - s, &v, &s)) {
                *p = s;
                return;
            }
            if (out) out->push_back(v);
            for (;;) {
                if (!parsePChar(s, se - s, &v, &s)) {
                    break;
                }
                if (out) out->push_back(v);
            }
            parsePathAbempty(s, se - s, out, p);
        }

        static void parsePathNoScheme(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            Cy v;
            auto se = s + len;
            if (s >= se) {
                *p = s;
                return;
            }
            if (!uri_ascii_is_unreserved(*s) &&
                !uri_ascii_is_sub_delims(*s) &&
                *s != '@')
            {
                if (!parsePctEncoded(s, se - s, &v, &s)) {
                    *p = s;
                    return;
                }
                if (out) out->push_back(v);
            } else {
                if (out) out->push_back(*s);
                ++s;
            }

            for (;;) {
                if (!uri_ascii_is_unreserved(*s) &&
                    !uri_ascii_is_sub_delims(*s) &&
                    *s != '@')
                {
                    if (!parsePctEncoded(s, se - s, &v, &s)) {
                        *p = s;
                        break;
                    }
                    if (out) out->push_back(v);
                } else {
                    if (out) out->push_back(*s);
                    ++s;
                }
            }
            parsePathAbempty(s, se - s, out, p);
        }

        static bool parsePathPart(
            const Cy* s, size_t len, PartType type,
            unsigned int* defs, Authority* auth, Str* path, const Cy** p)
        {
            auto ss = s;
            auto se = ss + len;
            if (type == PART_SUFF) {
                // authority
                if (!parseAuthority(
                    ss, se - ss, auth, &ss))
                {
                    *p = s;
                    return false;
                }
                //  path-abempty
                parsePathAbempty(ss, se - ss, path, p);

                *defs |= CD_AUTH;
                return true;
            }

            if (ss >= se) {
                // path-empty
                *p = ss;
                return true;
            }

            if (*ss == '/') {
                ++ss;
                if (ss >= se) {
                    // path-absolute (empty)
                    *p = ss;
                    return true;
                }
                if (*ss == '/') {
                    ++ss;
                    // authority
                    if (!parseAuthority(
                        ss, se - ss, auth, &ss))
                    {
                        *p = s;
                        return false;
                    }
                    //  path-abempty
                    parsePathAbempty(ss, se - ss, path, p);

                    *defs |= CD_AUTH;
                } else {
                    // path-absolute
                    parsePathRootless(ss, se - ss, path, p);
                }
            } else {
                if (type == PART_RELA) {
                    // path-noscheme
                    parsePathNoScheme(ss, se - ss, path, p);
                } else if (type == PART_HIER) {
                    // path-rootless
                    parsePathRootless(ss, se - ss, path, p);
                } else {
                    return false;
                }
            }

            return true;
        }

        static bool parseQuery(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            auto se = s + len;
            if (s >= se || *s != '?') {
                *p = s;
                return false;
            }
            ++s;

            for (; s < se;) {
                if (*s != '/' && *s != '?') {
                    Cy v;
                    if (!parsePChar(s, se - s, &v, &s)) {
                        break;
                    }
                    if (out) out->push_back(v);
                } else {
                    if (out) out->push_back(*s);
                    ++s;
                }
            }

            *p = s;
            return true;
        }

        static bool parseFragment(
            const Cy* s, size_t len, Str* out, const Cy** p)
        {
            auto se = s + len;
            if (s >= se || *s != '#') {
                *p = s;
                return false;
            }
            ++s;

            for (; s < se;) {
                if (*s != '/' && *s != '?') {
                    Cy v;
                    if (!parsePChar(s, se - s, &v, &s)) {
                        break;
                    }
                    if (out) out->push_back(v);
                } else {
                    if (out) out->push_back(*s);
                    ++s;
                }
            }

            *p = s;
            return true;
        }

        static void target(
            int is_strict, const uri& b, const uri& r, uri* t)
        {
            int r_scheme_def;
            if (!is_strict && r.scheme_ == b.scheme_) {
                r_scheme_def = 0;
            } else {
                r_scheme_def = (r.cdef_ & CD_SCHEME) ? 1 : 0;
            }

            if (r_scheme_def) {
                t->scheme_ = r.scheme_;
                t->auth_ = r.auth_;
                removeDotSegs(r.path_, &t->path_);
                t->query_ = r.query_;
            } else {
                if (r.cdef_ & CD_AUTH) {
                    t->auth_ = r.auth_;
                    removeDotSegs(r.path_, &t->path_);
                    t->query_ = r.query_;
                } else {
                    if (r.path_.empty()) {
                        t->path_ = b.path_;
                        if (r.cdef_ & CD_QUERY) {
                            t->query_ = r.query_;
                        } else {
                            t->query_ = b.query_;
                        }
                    } else {
                        if (r.path_[0] == '/') {
                            removeDotSegs(r.path_, &t->path_);
                        } else {
                            mergePath(b, r, &t->path_);
                            removeDotSegs(t->path_, &t->path_);
                        }
                        t->query_ = r.query_;
                    }
                    t->auth_ = b.auth_;
                }
                t->scheme_ = b.scheme_;
            }
            t->fragment_ = r.fragment_;
        }

        static void composition(const uri& u, Str* out) {
            if (u.cdef_ & CD_SCHEME) {
                out->append(u.scheme_);
                out->push_back(':');
            }

            if (u.cdef_ & CD_AUTH) {
                out->push_back('/');
                out->push_back('/');
                if (u.auth_.sc_defs & CD_USER) {
                    out->append(u.auth_.user);
                    out->push_back('@');
                }
                out->append(u.auth_.host);
                if (u.auth_.sc_defs & CD_PORT) {
                    out->push_back(':');
                    out->append(u.auth_.port_str);
                }
            }

            out->append(u.path_);

            if (u.cdef_ & CD_QUERY) {
                out->push_back('?');
                out->append(u.query_);
            }

            if (u.cdef_ & CD_FRAG) {
                out->push_back('#');
                out->append(u.fragment_);
            }
        }

        static void mergePath(const uri& b, const uri& r, Str* out) {
            if ((b.cdef_ & CD_AUTH) && b.path_.empty()) {
                out->push_back('/');
            } else {
                for (size_t i = b.path_.size(); i-- > 0;) {
                    if (b.path_[i] == '/') {
                        out->append(b.path_.begin(), b.path_.begin() + i + 1);
                        break;
                    }
                }
            }
            out->append(r.path_);
        }

        static void normalize(uri* u) {
            ascii_lower(&u->scheme_);
            ascii_lower(&u->auth_.host);
            removeDotSegs(u->path_, &u->path_);
        }

    public:
        static void removeDotSegs(const Str& path, Str* out) {
            auto _path(path);

            auto s = &_path[0];
            auto se = s + _path.size();
            for (; s < se;) {
                if (*s == '.') {
                    ++s;
                    // [.]
                    if (s >= se) break;
                    if (*s == '.') {
                        ++s;
                        // [..]
                        if (s >= se) break;
                        if (*s == '/') {
                            // [../]
                            ++s;
                        } else if (*s == '.') {
                            // [...]
                            continue;
                        }
                        // [..?]
                    } else if (*s == '/') {
                        // [./]
                        ++s;
                        continue;
                    }
                    // [.?]
                } else if (*s == '/') {
                    ++s;
                    if (s >= se) {
                        // [/]
                        out->push_back('/');
                        break;
                    }
                    if (*s == '.') {
                        if (s + 1 >= se) {
                            // [/.]
                            *s = '/';
                            continue;
                        }
                        ++s;
                        if (*s == '/') {
                            // [/./]
                            continue;
                        }
                        if (*s == '.') {
                            if (s + 1 >= se) {
                                // [/..]
                                *s = '/';
                                for (size_t j = out->size(); j-- > 0;) {
                                    if (j == 0 || out->at(j) == '/') {
                                        out->erase(j);
                                        break;
                                    }
                                }
                                continue;
                            }
                            ++s;
                            for (size_t j = out->size(); j-- > 0;) {
                                if (j == 0 || out->at(j) == '/') {
                                    out->erase(j);
                                    break;
                                }
                            }
                            if (*s == '/') {
                                // [/../]
                                continue;
                            }
                            // [/..?]
                        }
                        // [/.?]
                        --s;
                        *s = '/';
                        continue;
                    }
                    // [//]
                    --s;
                }

                auto _s = s;
                ++s;
                for (; s < se; ++s) {
                    if (*s == '/') break;
                }
                out->append(_s, s - _s);
            }
        }

    private:
        static unsigned int ascii_ctoi(unsigned int c) noexcept {
            // 0 - 9
            if (c >= 0x30 && c <= 0x39) {
                return c - 0x30;
            }
            // A - F
            if (c >= 0x41 && c <= 0x46) {
                return c - 'A' + 10u;
            }
            // a - f
            if (c >= 0x61 && c <= 0x66) {
                return c - 'a' + 10u;
            }
            return ~0u;
        }

        static void ascii_upper(Str* str) noexcept {
            for (size_t i = 0; i < str->size(); ++i) {
                auto& c = (*str)[i];
                // a - f
                if (c >= 0x61 && c <= 0x7A) {
                    c -= 0x20;
                }
            }
        }

        static unsigned int ascii_upper(unsigned int c) noexcept {
            // a - f
            if (c >= 0x61 && c <= 0x7A) {
                return c - 0x20;
            }
            return c;
        }

        static void ascii_lower(Str* str) noexcept {
            for (size_t i = 0; i < str->size(); ++i) {
                auto& c = (*str)[i];
                // A - F
                if (c >= 0x41 && c <= 0x5A) {
                    c += 0x20;
                }
            }
        }

        static unsigned int ascii_lower(unsigned int c) noexcept {
            // A - F
            if (c >= 0x41 && c <= 0x5A) {
                return c + 0x20;
            }
            return c;
        }

        Str scheme_;
        Authority auth_;
        Str path_;
        Str query_;
        Str fragment_;
        unsigned int cdef_;
    };

}

#endif  // UTILS_NET_URI_HPP_