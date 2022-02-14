// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/json/json_structs.h"


namespace utl {
namespace json {

    IntegerValue::IntegerValue(int64_t val)
        : val_(val) {}

    DoubleValue::DoubleValue(double val)
        : val_(val) {}

    StringValue::StringValue(const std::string_view& val)
        : val_(val) {}

    BoolValue::BoolValue(bool val)
        : val_(val) {}

    ArrayValue::ArrayValue() {}

    ArrayValue::~ArrayValue() {
        for (auto v : values_) {
            delete v;
        }
    }

    size_t ArrayValue::getCount() const {
        return values_.size();
    }

    void ArrayValue::put(Value* v) {
        values_.push_back(v);
    }

    void ArrayValue::put(int64_t val) {
        values_.push_back(new IntegerValue(val));
    }

    void ArrayValue::put(double val) {
        values_.push_back(new DoubleValue(val));
    }

    void ArrayValue::put(const std::string_view& val) {
        values_.push_back(new StringValue(val));
    }

    void ArrayValue::put(bool val) {
        values_.push_back(new BoolValue(val));
    }

    void ArrayValue::put(ArrayValue* val) {
        values_.push_back(val);
    }

    void ArrayValue::put(ObjectValue* val) {
        values_.push_back(val);
    }

    bool ArrayValue::get(size_t index, Value** v) const {
        if (index >= values_.size() || !v) {
            return false;
        }

        *v = values_[index];
        return true;
    }

    bool ArrayValue::getInteger(size_t index, int64_t* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != JT_INTEGER) {
            return false;
        }

        *out = val->asInteger()->getValue();
        return true;
    }

    bool ArrayValue::getDouble(size_t index, double* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != JT_DOUBLE) {
            return false;
        }

        *out = val->asDouble()->getValue();
        return true;
    }

    bool ArrayValue::getString(size_t index, std::string* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != JT_STRING) {
            return false;
        }

        val->asString()->getValue(out);
        return true;
    }

    bool ArrayValue::getBoolean(size_t index, bool* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != JT_BOOL) {
            return false;
        }

        *out = val->asBool()->getValue();
        return true;
    }

    bool ArrayValue::getArray(size_t index, ArrayValue** out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != JT_ARRAY) {
            return false;
        }

        *out = val->asArray();
        return true;
    }

    bool ArrayValue::getObject(size_t index, ObjectValue** out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != JT_OBJECT) {
            return false;
        }

        *out = val->asObject();
        return true;
    }

    Value* ArrayValue::get(size_t index) const {
        Value* out;
        if (get(index, &out)) {
            return out;
        }
        return nullptr;
    }

    int64_t ArrayValue::getInteger(size_t index, int64_t def_val) const {
        int64_t out;
        if (getInteger(index, &out)) {
            return out;
        }
        return def_val;
    }

    double ArrayValue::getDouble(size_t index, double def_val) const {
        double out;
        if (getDouble(index, &out)) {
            return out;
        }
        return def_val;
    }

    std::string ArrayValue::getString(size_t index, const std::string_view& def_val) const {
        std::string out;
        if (getString(index, &out)) {
            return out;
        }
        return std::string(def_val);
    }

    bool ArrayValue::getBoolean(size_t index, bool def_val) const {
        bool out;
        if (getBoolean(index, &out)) {
            return out;
        }
        return def_val;
    }

    ArrayValue* ArrayValue::getArray(size_t index) const {
        ArrayValue* out;
        if (getArray(index, &out)) {
            return out;
        }
        return nullptr;
    }

    ObjectValue* ArrayValue::getObject(size_t index) const {
        ObjectValue* out;
        if (getObject(index, &out)) {
            return out;
        }
        return nullptr;
    }


    ObjectValue::ObjectValue() {}

    ObjectValue::~ObjectValue() {
        for (auto pair : map_) {
            delete pair.second;
        }
    }

    void ObjectValue::put(const std::string_view& path, Value* v) {
        size_t idx = 0;
        Value* value = nullptr;
        ObjectValue* obj_val = this;
        for (;;) {
            auto dot_idx = path.find(".", idx);
            if (dot_idx != std::string::npos) {
                auto cur_key = path.substr(idx, dot_idx - idx);
                idx = dot_idx + 1;
                bool ret = obj_val->getWne(cur_key, &value);
                if (!ret || value->getType() != JT_OBJECT) {
                    auto obj = new ObjectValue();
                    obj_val->putWne(cur_key, obj);
                    obj_val = obj;
                } else {
                    obj_val = value->asObject();
                }
            } else {
                auto cur_key = path.substr(idx);
                obj_val->putWne(cur_key, v);
                break;
            }
        }
    }

    void ObjectValue::put(const std::string_view& path, int64_t val) {
        put(path, new IntegerValue(val));
    }

    void ObjectValue::put(const std::string_view& path, double val) {
        put(path, new DoubleValue(val));
    }

    void ObjectValue::put(const std::string_view& path, const std::string& val) {
        put(path, new StringValue(val));
    }

    void ObjectValue::put(const std::string_view& path, bool val) {
        put(path, new BoolValue(val));
    }

    void ObjectValue::put(const std::string_view& path, ArrayValue* val) {
        put(path, static_cast<Value*>(val));
    }

    void ObjectValue::put(const std::string_view& path, ObjectValue* val) {
        put(path, static_cast<Value*>(val));
    }

    bool ObjectValue::get(const std::string_view& path, Value** out) const {
        if (!out) {
            return false;
        }

        size_t idx = 0;
        Value* val;
        const ObjectValue* obj_val = this;
        for (;;) {
            auto dot_idx = path.find(".", idx);
            if (dot_idx != std::string::npos) {
                auto cur_key = path.substr(idx, dot_idx - idx);
                idx = dot_idx + 1;
                if (!obj_val->getWne(cur_key, &val)) {
                    return false;
                }
                if (val->getType() != JT_OBJECT) {
                    return false;
                }
                obj_val = val->asObject();
            } else {
                auto cur_key = path.substr(idx);
                if (!obj_val->getWne(cur_key, out)) {
                    return false;
                }
                break;
            }
        }
        return true;
    }

    bool ObjectValue::getInteger(const std::string_view& path, int64_t* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != JT_INTEGER) {
            return false;
        }
        *out = val->asInteger()->getValue();
        return true;
    }

    bool ObjectValue::getDouble(const std::string_view& path, double* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != JT_DOUBLE) {
            return false;
        }
        *out = val->asDouble()->getValue();
        return true;
    }

    bool ObjectValue::getString(const std::string_view& path, std::string* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != JT_STRING) {
            return false;
        }
        val->asString()->getValue(out);
        return true;
    }

    bool ObjectValue::getBoolean(const std::string_view& path, bool* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != JT_BOOL) {
            return false;
        }
        *out = val->asBool()->getValue();
        return true;
    }

    bool ObjectValue::getArray(const std::string_view& path, ArrayValue** out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != JT_ARRAY) {
            return false;
        }
        *out = val->asArray();
        return true;
    }

    bool ObjectValue::getObject(const std::string_view& path, ObjectValue** out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != JT_OBJECT) {
            return false;
        }
        *out = val->asObject();
        return true;
    }

    Value* ObjectValue::get(const std::string_view& path) const {
        Value* out;
        if (get(path, &out)) {
            return out;
        }
        return nullptr;
    }

    int64_t ObjectValue::getInteger(const std::string_view& path, int64_t def_val) const {
        int64_t out;
        if (getInteger(path, &out)) {
            return out;
        }
        return def_val;
    }

    double ObjectValue::getDouble(const std::string_view& path, double def_val) const {
        double out;
        if (getDouble(path, &out)) {
            return out;
        }
        return def_val;
    }

    std::string ObjectValue::getString(const std::string_view& path, const std::string& def_val) const {
        std::string out;
        if (getString(path, &out)) {
            return out;
        }
        return def_val;
    }

    bool ObjectValue::getBoolean(const std::string_view& path, bool def_val) const {
        bool out;
        if (getBoolean(path, &out)) {
            return out;
        }
        return def_val;
    }

    ArrayValue* ObjectValue::getArray(const std::string_view& path) const {
        ArrayValue* out;
        if (getArray(path, &out)) {
            return out;
        }
        return nullptr;
    }

    ObjectValue* ObjectValue::getObject(const std::string_view& path) const {
        ObjectValue* out;
        if (getObject(path, &out)) {
            return out;
        }
        return nullptr;
    }

    void ObjectValue::putWne(const std::string_view& key, Value* val) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            delete it->second;
            it->second = val;
        } else {
            map_[std::string(key)] = val;
        }
    }

    bool ObjectValue::getWne(const std::string_view& key, Value** out) const {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }

        *out = it->second;
        return true;
    }

}
}