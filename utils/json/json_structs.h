// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_JSON_JSON_STRUCTS_H_
#define UTILS_JSON_JSON_STRUCTS_H_

#include <map>
#include <string>
#include <vector>


namespace utl {
namespace json {

    class IntegerValue;
    class DoubleValue;
    class StringValue;
    class BoolValue;
    class NullValue;
    class ArrayValue;
    class ObjectValue;

    class Value {
    public:
        enum Type {
            JT_OBJECT,
            JT_ARRAY,
            JT_INTEGER,
            JT_DOUBLE,
            JT_STRING,
            JT_BOOL,
            JT_NULL,
        };

        virtual ~Value() = default;

        virtual Type getType() const = 0;

        IntegerValue* asInteger() {
            if (getType() != JT_INTEGER) { return nullptr; }
            return reinterpret_cast<IntegerValue*>(this);
        }
        DoubleValue* asDouble() {
            if (getType() != JT_DOUBLE) { return nullptr; }
            return reinterpret_cast<DoubleValue*>(this);
        }
        StringValue* asString() {
            if (getType() != JT_STRING) { return nullptr; }
            return reinterpret_cast<StringValue*>(this);
        }
        BoolValue* asBool() {
            if (getType() != JT_BOOL) { return nullptr; }
            return reinterpret_cast<BoolValue*>(this);
        }
        NullValue* asNull() {
            if (getType() != JT_NULL) { return nullptr; }
            return reinterpret_cast<NullValue*>(this);
        }
        ArrayValue* asArray() {
            if (getType() != JT_ARRAY) { return nullptr; }
            return reinterpret_cast<ArrayValue*>(this);
        }
        ObjectValue* asObject() {
            if (getType() != JT_OBJECT) { return nullptr; }
            return reinterpret_cast<ObjectValue*>(this);
        }
    };


    class IntegerValue : public Value {
    public:
        explicit IntegerValue(int64_t val);

        Type getType() const override { return JT_INTEGER; }

        int64_t getValue() const { return val_; }

    private:
        int64_t val_;
    };


    class DoubleValue : public Value {
    public:
      explicit DoubleValue(double val);

      Type getType() const override { return JT_DOUBLE; }

      double getValue() const { return val_; }

    private:
      double val_;
    };


    class StringValue : public Value {
    public:
        explicit StringValue(const std::string_view& val);

        Type getType() const override { return JT_STRING; }

        void getValue(std::string* val) const { *val = val_; }

    private:
        std::string val_;
    };


    class BoolValue : public Value {
    public:
        explicit BoolValue(bool val);

        Type getType() const override { return JT_BOOL; }

        bool getValue() const { return val_; }

    private:
        bool val_;
    };


    class NullValue : public Value {
    public:
        NullValue() = default;

        Type getType() const override { return JT_NULL; }
    };


    class ArrayValue : public Value {
    public:
        ArrayValue();
        ~ArrayValue();

        Type getType() const override { return JT_ARRAY; }

        size_t getCount() const;

        void put(Value* v);
        void put(int64_t val);
        void put(double val);
        void put(const std::string_view& val);
        void put(bool val);
        void put(ArrayValue* val);
        void put(ObjectValue* val);

        bool get(size_t index, Value** v) const;
        bool getInteger(size_t index, int64_t* out) const;
        bool getDouble(size_t index, double* out) const;
        bool getString(size_t index, std::string* out) const;
        bool getBoolean(size_t index, bool* out) const;
        bool getArray(size_t index, ArrayValue** out) const;
        bool getObject(size_t index, ObjectValue** out) const;

        Value* get(size_t index) const;
        int64_t getInteger(size_t index, int64_t def_val = 0) const;
        double getDouble(size_t index, double def_val = 0.0) const;
        std::string getString(size_t index, const std::string_view& def_val = {}) const;
        bool getBoolean(size_t index, bool def_val = false) const;
        ArrayValue* getArray(size_t index) const;
        ObjectValue* getObject(size_t index) const;

    private:
        std::vector<Value*> values_;
    };


    class ObjectValue : public Value {
    public:
        ObjectValue();
        ~ObjectValue();

        Type getType() const override { return JT_OBJECT; }

        void put(const std::string_view& path, Value* v);
        void put(const std::string_view& path, int64_t val);
        void put(const std::string_view& path, double val);
        void put(const std::string_view& path, const std::string& val);
        void put(const std::string_view& path, bool val);
        void put(const std::string_view& path, ArrayValue* val);
        void put(const std::string_view& path, ObjectValue* val);

        bool get(const std::string_view& path, Value** out) const;
        bool getInteger(const std::string_view& path, int64_t* out) const;
        bool getDouble(const std::string_view& path, double* out) const;
        bool getString(const std::string_view& path, std::string* out) const;
        bool getBoolean(const std::string_view& path, bool* out) const;
        bool getArray(const std::string_view& path, ArrayValue** out) const;
        bool getObject(const std::string_view& path, ObjectValue** out) const;

        Value* get(const std::string_view& path) const;
        int64_t getInteger(const std::string_view& path, int64_t def_val = 0) const;
        double getDouble(const std::string_view& path, double def_val = 0.0) const;
        std::string getString(const std::string_view& path, const std::string& def_val = {}) const;
        bool getBoolean(const std::string_view& path, bool def_val = false) const;
        ArrayValue* getArray(const std::string_view& path) const;
        ObjectValue* getObject(const std::string_view& path) const;

    private:
        using ValMap = std::map<std::string, Value*, std::less<>>;

        void putWne(const std::string_view& key, Value* val);
        bool getWne(const std::string_view& key, Value** out) const;

        ValMap map_;
    };

}
}

#endif  // UTILS_JSON_JSON_STRUCTS_H_
