// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_ARG_PARSER_HPP_
#define UTILS_ARG_PARSER_HPP_

#include <cassert>
#include <cstring>
#include <cstdarg>
#include <map>
#include <string>
#include <vector>


namespace utl {

    // https://pubs.opengroup.org/onlinepubs/9799919799/
    class opt_parser {
    public:
        enum opt_arg_type {
            oat_none = 0,
            oat_string,
            oat_int,
            oat_float,
        };

        enum ret_code : int {
            rc_par_arg = -3,
            rc_req_arg = -2,
            rc_unk_opt = -1,
            rc_ok      =  0,
            rc_req_opt =  1,
            rc_min_opr =  2,
            rc_max_opr =  3,
        };

        enum conf_resolv : unsigned int {
            crv_def    = 0,
            crv_ow     = 1 << 0,
            crv_fail   = 1 << 1,  // Let reg_opt() return false when conflict
            crv_assert = 1 << 2,
            crv_log    = 1 << 3,
        };

        struct opt_arg {
            opt_arg_type type = oat_none;
            std::string str;
            union {
                double num_f;
                long long num_i;
            };

            long long as_int() const {
                assert(type == oat_int);
                return num_i;
            }
            double as_float() const {
                assert(type == oat_float);
                return num_f;
            }
            const std::string& as_str() const {
                assert(type == oat_string);
                return str;
            }
        };

        class opt_result {
        public:
            struct opt_r {
                std::string name;
                char short_name = 0;
                std::vector<opt_arg> args;
            };
            using opt_map = std::map<std::string, opt_r>;

            bool has_opt(const char* name) const {
                return find_opt(name) != opts_.end();
            }
            bool has_opt(char short_name) const {
                return find_opt(short_name) != opts_.end();
            }

            const opt_r& opt(const char* name) const {
                auto it = find_opt(name);
                assert(it != opts_.end());
                return it->second;
            }
            const opt_r& opt(char short_name) const {
                auto it = find_opt(short_name);
                assert(it != opts_.end());
                return it->second;
            }
            const opt_map& opts() const {
                return opts_;
            }

            const opt_arg& operand(size_t index) const {
                assert(index < operands_.size());
                return operands_[index];
            }
            const std::vector<opt_arg>& operands() const {
                return operands_;
            }

        private:
            friend class opt_parser;

            opt_map::const_iterator find_opt(const char* name) const {
                for (auto it = opts_.cbegin(); it != opts_.cend(); ++it) {
                    if (it->second.name == name) return it;
                }
                return opts_.cend();
            }

            opt_map::const_iterator find_opt(char short_name) const {
                if (short_name <= 0) return opts_.cend();
                for (auto it = opts_.cbegin(); it != opts_.cend(); ++it) {
                    if (it->second.short_name == short_name) return it;
                }
                return opts_.cend();
            }

            opt_map opts_;
            std::vector<opt_arg> operands_;
        };

    private:
        using log_callback = void (*)(const char*);

        static const size_t kMaxNameLengthInHelp = 128u;

        struct opt_info {
            std::string name;
            char short_name = 0;
            std::string desc;
            bool required = false;
            opt_arg_type arg_type = oat_none;
            bool arg_required = false;
            bool multiple = false;
        };

        struct operand_info {
            size_t min_count = 0;
            size_t max_count = SIZE_MAX;
            std::vector<opt_arg_type> types;
        };

        using opt_info_map = std::map<std::string, opt_info>;

        opt_info_map::iterator find_by_name(const char* name) {
            for (auto it = opts_.begin(); it != opts_.end(); ++it) {
                if (it->second.name == name) return it;
            }
            return opts_.end();
        }

        opt_info_map::iterator find_by_short_name(char name) {
            for (auto it = opts_.begin(); it != opts_.end(); ++it) {
                if (it->second.short_name == name) return it;
            }
            return opts_.end();
        }

        opt_info_map::iterator find_conf_by_short_name(const char* name, char short_name) {
            if (short_name <= 0) return opts_.end();

            for (auto it = opts_.begin(); it != opts_.end(); ++it) {
                if (it->second.short_name == short_name) {
                    if (it->second.name == name) {
                        return opts_.end();
                    }
                    return it;
                }
            }
            return opts_.end();
        }

        static int find_next_opt_arg_from_argv(int start, int argc, const char* const* argv) {
            for (int j = start; j < argc; ++j) {
                const char* arg = argv[j];
                if (!arg) continue;

                size_t arg_len = strlen(arg);
                if (!arg_len) continue;

                if (arg[0] != '-') {
                    return j;
                }
                break;
            }
            return -1;
        }

        int resolve_conflict(const char* name, char short_name) {
            bool has_conflict = false;
            auto nc_it = find_by_name(name);
            if (nc_it != opts_.end()) {
                if (conf_resolv_ & crv_log) {
                    log_to_cb("Conflict when registry option '%s'!\n", name);
                }
                if (conf_resolv_ & crv_assert) {
                    assert(false && "Conflict when registry a option!");
                }
                if ((conf_resolv_ & crv_fail) && !(conf_resolv_ & crv_ow)) {
                    return -1;
                }
                has_conflict = true;
            }

            auto snc_it = find_conf_by_short_name(name, short_name);
            if (snc_it != opts_.end()) {
                if (conf_resolv_ & crv_log) {
                    log_to_cb("Conflict when registry option(short) '%c'!\n", snc_it->second.short_name);
                }
                if (conf_resolv_ & crv_assert) {
                    assert(false && "Conflict when registry a option(short name)!");
                }
                if ((conf_resolv_ & crv_fail) && !(conf_resolv_ & crv_ow)) {
                    return -1;
                }
                snc_it->second.short_name = 0;
                has_conflict = true;
            }

            if (has_conflict && (conf_resolv_ & crv_fail)) {
                return 1;
            }
            return 0;
        }

        static bool from_str(opt_arg_type type, const char* str, opt_arg* out) {
            opt_arg val;
            val.type = type;

            switch (type) {
            case oat_string:
                val.str = str;
                break;
            case oat_int:
                try { val.num_i = std::stoll(str); } catch (...) { return false; }
                break;
            case oat_float:
                try { val.num_f = std::stod(str); } catch (...) { return false; }
                break;
            default:
                assert(false && "Unknown value type!");
                return false;
            }
            *out = std::move(val);
            return true;
        }

        static void put_opt_to_r(opt_info_map::iterator it, opt_result& result) {
            auto& opt_info = it->second;
            auto& r = result.opts_[opt_info.name];
            r.name = opt_info.name;
            r.short_name = opt_info.short_name;
            if (!opt_info.multiple) {
                r.args.clear();
            }
        }

        static bool put_opt_arg_to_r(
            opt_info_map::iterator it,
            const char* val,
            opt_result& result)
        {
            auto& opt_info = it->second;
            auto& r = result.opts_[opt_info.name];
            r.name = opt_info.name;
            r.short_name = opt_info.short_name;

            opt_arg oarg;
            if (!from_str(opt_info.arg_type, val, &oarg)) {
                return false;
            }
            if (!opt_info.multiple) {
                r.args.clear();
            }
            r.args.push_back(std::move(oarg));
            return true;
        }

        static const char* oat2str(opt_arg_type type) {
            switch (type) {
            case oat_string: return "str";
            case oat_int:    return "int";
            case oat_float:  return "float";
            default:         return "none";
            }
        }

        void log_to_cb(const char* fmt, ...) const {
            va_list args;
            va_start(args, fmt);

            if (log_callback_) {
                char buf[512];
                int ret = vsnprintf(buf, 512, fmt, args);
                if (ret > 0) {
                    log_callback_(buf);
                }
            } else {
                vprintf(fmt, args);
            }

            va_end(args);
        }

        static const char* gen_print_arg_type(const opt_info& info) {
            const char* arg;
            switch (info.arg_type) {
            case oat_none:   arg = ""; break;
            case oat_int:    arg = info.arg_required ? " <int>"    : "<int>";    break;
            case oat_float:  arg = info.arg_required ? " <float>"  : "<float>";  break;
            case oat_string: arg = info.arg_required ? " <string>" : "<string>"; break;
            default: arg = ""; break;
            }
            return arg;
        }

    public:
        opt_parser(
            std::string name,
            std::string desc,
            unsigned int conf_resolv)
            : name_(std::move(name)),
              desc_(std::move(desc)),
              conf_resolv_(conf_resolv) {}

        bool reg_opt(std::string name) {
            return reg_opt(std::move(name), 0);
        }

        bool reg_opt(
            std::string name,
            char short_name)
        {
            return reg_opt(std::move(name), short_name, "");
        }

        bool reg_opt(
            std::string name,
            char short_name,
            std::string desc)
        {
            return reg_opt(
                std::move(name),
                short_name,
                desc,
                false);
        }

        bool reg_opt(
            std::string name,
            char short_name,
            std::string desc,
            bool required)
        {
            return reg_opt(
                std::move(name),
                short_name,
                desc,
                required,
                oat_none,
                false,
                false);
        }

        bool reg_opt(
            std::string name,
            char short_name,
            std::string desc,
            bool required,
            opt_arg_type arg_type,
            bool arg_required,
            bool multiple)
        {
            int rc = resolve_conflict(name.c_str(), short_name);
            if (rc == -1) {
                return false;
            }

            auto& info = opts_[name];

            info.name = std::move(name);
            info.short_name = short_name;
            info.desc = std::move(desc);
            info.required = required;
            info.arg_type = arg_type;
            info.arg_required = arg_required;
            info.multiple = multiple;

            return rc == 0;
        }

        bool unreg_opt(const std::string& name) {
            auto it = find_by_name(name.c_str());
            if (it != opts_.end()) {
                opts_.erase(it);
                return true;
            }
            return false;
        }

        bool unreg_opt(char short_name, bool entire = true) {
            if (short_name <= 0) return false;

            auto it = find_by_short_name(short_name);
            if (it != opts_.end()) {
                if (entire) {
                    opts_.erase(it);
                } else {
                    it->second.short_name = 0;
                }
                return true;
            }
            return false;
        }

        void clear_opts() {
            opts_.clear();
        }
        void clear_operands() {
            operands_.min_count = 0;
            operands_.max_count = SIZE_MAX;
            operands_.types.clear();
        }
        void clear_result() {
            result_.opts_.clear();
            result_.operands_.clear();

        }
        void clear_all() {
            clear_opts();
            clear_operands();
            clear_result();
        }

        void set_operands(size_t min_count, size_t max_count) {
            set_operands(min_count, max_count, {});
        }

        void set_operands(
            size_t min_count,
            size_t max_count,
            std::initializer_list<opt_arg_type> types)
        {
            operands_.min_count = (std::min)(min_count, max_count);
            operands_.max_count = (std::max)(min_count, max_count);
            operands_.types = types;
        }

        void set_err_log_callback(log_callback cb) {
            log_callback_ = cb;
        }

        bool is_opt_reg(const std::string& name) const {
            for (const auto& pair : opts_) {
                if (pair.second.name == name) return true;
            }
            return false;
        }

        bool is_opt_reg(char short_name) const {
            if (short_name <= 0) return false;

            for (const auto& pair : opts_) {
                if (pair.second.short_name == short_name) return true;
            }
            return false;
        }

#ifdef _WIN32
        int parse_win();
#endif

        int parse(int argc, const char* const* argv) {
            assert(argv);

            int cur_code = rc_ok;
            opt_result result;

            int delimiter = 0;
            auto cur = opts_.end();
            for (int i = 1; i < argc; ++i) {
                const char* arg = argv[i];
                if (!arg) continue;

                size_t arg_len = strlen(arg);
                if (!arg_len) continue;

                if (!delimiter && arg[0] == '-') {
                    // opt
                    cur = opts_.end();

                    if (arg_len == 1) continue; // ignore "-"

                    if (arg[1] == '-') {
                        // "--"
                        if (arg_len == 2) {
                            delimiter = 1;
                            continue;
                        }
                        cur = find_by_name(arg + 2);
                        if (cur == opts_.end()) {
                            // unknown opt
                            log_to_cb("Unknown option '%s'.\n", arg + 2);
                            return rc_unk_opt;
                        }

                        if (cur->second.arg_type == oat_none) {
                            put_opt_to_r(cur, result);
                        } else {
                            // check next args
                            int j = find_next_opt_arg_from_argv(i + 1, argc, argv);
                            if (cur->second.arg_required && j < 0) {
                                // need opt_arg!
                                log_to_cb("Option '%s' need a argument.\n", arg + 2);
                                return rc_req_arg;
                            }
                            if (j >= 0) {
                                // opt_arg
                                if (!put_opt_arg_to_r(cur, argv[j], result)) {
                                    log_to_cb("Parsing argument '%s'(to %s) of option '%s' failed.\n",
                                        argv[j], oat2str(cur->second.arg_type), arg + 2);
                                    return rc_par_arg;
                                }
                                i = j;
                            } else {
                                put_opt_to_r(cur, result);
                            }
                        }
                    } else {
                        // "-", may grouped
                        for (size_t j = 1; j < arg_len; ++j) {
                            cur = find_by_short_name(arg[j]);
                            if (cur == opts_.end()) {
                                // unknown opt
                                log_to_cb("Unknown option (short name) '%c'.\n", arg[j]);
                                return rc_unk_opt;
                            }

                            if (cur->second.arg_type == oat_none) {
                                put_opt_to_r(cur, result);
                                continue;
                            }
                            if (cur->second.arg_required && j + 1 >= arg_len) {
                                int k = find_next_opt_arg_from_argv(i + 1, argc, argv);
                                if (k < 0) {
                                    log_to_cb("Option (short name) '%c' need a argument.\n", arg[j]);
                                    return rc_req_arg;
                                }
                                // opt_arg
                                if (!put_opt_arg_to_r(cur, argv[k], result)) {
                                    log_to_cb("Parsing argument '%s'(to %s) of option (short name) '%c' failed.\n",
                                        argv[k], oat2str(cur->second.arg_type), arg[j]);
                                    return rc_par_arg;
                                }
                                i = k;
                            } else {
                                // opt_arg
                                if (!put_opt_arg_to_r(cur, arg + j + 1, result)) {
                                    log_to_cb("Parsing argument '%s'(to %s) of option (short name) '%c' failed.\n",
                                        arg + j + 1, oat2str(cur->second.arg_type), arg[j]);
                                    return rc_par_arg;
                                }
                            }
                            break;
                        }
                    }
                } else {
                    // operands
                    size_t cur_idx = result.operands_.size();
                    if (cur_idx >= operands_.max_count) {
                        log_to_cb("Too many operands (%zu/%zu).\n", cur_idx + 1, operands_.max_count);
                        cur_code = rc_max_opr;
                        continue;
                    }

                    opt_arg_type vt;
                    if (operands_.types.size() > cur_idx) {
                        vt = operands_.types[cur_idx];
                    } else if (!operands_.types.empty()) {
                        vt = operands_.types.back();
                    } else {
                        vt = oat_string;
                    }

                    opt_arg oarg;
                    if (!from_str(vt, arg, &oarg)) {
                        log_to_cb("Parsing operand '%s'(to %s)' failed.\n", arg, oat2str(vt));
                        return rc_par_arg;
                    }

                    result.operands_.push_back(std::move(oarg));
                }
            }

            // checking required opts
            if (cur_code == rc_ok) {
                for (const auto& pair : opts_) {
                    if (pair.second.required &&
                        result.opts_.find(pair.second.name) == result.opts_.end())
                    {
                        log_to_cb("Option '%s' must be presented.\n", pair.second.name.c_str());
                        cur_code = rc_req_opt;
                        break;
                    }
                }
            }

            if (cur_code == rc_ok) {
                if (result.operands_.size() < operands_.min_count) {
                    log_to_cb("Too little operands (%zu/%zu).\n",
                        result.operands_.size(), operands_.min_count);
                    cur_code = rc_min_opr;
                }
            }

            result_ = std::move(result);
            return cur_code;
        }

        const opt_result& result() const {
            return result_;
        }

        void print_help_msg() const {
            log_to_cb("%s, %s\n", name_.c_str(), desc_.c_str());
            if (operands_.max_count > 0) {
                if (operands_.min_count > 0) {
                    log_to_cb("Usage: %s [option]... operand...\n\n", name_.c_str());
                } else {
                    log_to_cb("Usage: %s [option]... [operand]...\n\n", name_.c_str());
                }
            } else {
                log_to_cb("Usage: %s [option]...\n\n", name_.c_str());
            }

            char spaces[kMaxNameLengthInHelp];
            memset(spaces, ' ', kMaxNameLengthInHelp);

            size_t filled_space;
            size_t max_name_len = 0;
            for (const auto& opt : opts_) {
                if (opt.second.name.size() > max_name_len) {
                    max_name_len = opt.second.name.size() +
                        strlen(gen_print_arg_type(opt.second));
                }
            }
            if (max_name_len > kMaxNameLengthInHelp) {
                max_name_len = kMaxNameLengthInHelp;
            }

            for (const auto& opt : opts_) {
                auto& info = opt.second;
                const char* arg = gen_print_arg_type(info);
                size_t name_region_len = info.name.size() + strlen(arg);
                if (name_region_len >= max_name_len) {
                    filled_space = 0;
                } else {
                    filled_space = max_name_len - name_region_len;
                }
                spaces[filled_space] = '\0';

                if (info.short_name > 0) {
                    log_to_cb("  -%c,  --%s%s %s%s\n", info.short_name, info.name.c_str(), arg, spaces, info.desc.c_str());
                } else {
                    log_to_cb("        --%s%s %s%s\n", info.name.c_str(), arg, spaces, info.desc.c_str());
                }

                spaces[filled_space] = ' ';
            }
        }

    private:
        std::string name_;
        std::string desc_;
        unsigned int conf_resolv_ = 0;

        opt_info_map opts_;
        operand_info operands_;
        log_callback log_callback_ = nullptr;

        opt_result result_;
    };

}

#endif  // UTILS_ARG_PARSER_HPP_