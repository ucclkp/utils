// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <sstream>

#include "utils/stream_utils.h"
#include "utils/unit_test/test_collector.h"


TEST_CASE(StreamUnitTest) {

    TEST_DEF("StartWith tests.") {
        std::istringstream iss("1", std::ios::binary);
        TEST_E(utl::startWith(iss, "true"), 0);
        TEST_E(utl::startWith(iss, "false"), 0);
        TEST_E(utl::startWith(iss, "123"), 0);
        TEST_E(utl::startWith(iss, "12"), 0);
        TEST_E(utl::startWith(iss, "1"), 1);

        return true;
    };

    TEST_DEF("Read stream tests.") {
        // uint 32
        {
            uint32_t v;
            std::string mem;
            mem.resize(4);
            std::istringstream iss;
            auto buf = reinterpret_cast<unsigned char*>(&mem[0]);

            // BE
            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStream<4, false>(iss, &v)) && v == uint32_t(0x89ABCD00));

            // LE
            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStream<4, true>(iss, &v)) && v == uint32_t(0x00CDAB89));
        }

        // int 32
        {
            int32_t v;
            std::string mem;
            mem.resize(4);
            std::istringstream iss;
            auto buf = reinterpret_cast<unsigned char*>(&mem[0]);

            // BE
            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStream<4, false>(iss, &v)) && v == int32_t(0x89ABCD00));

            // LE
            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStream<4, true>(iss, &v)) && v == int32_t(0x00CDAB89));
        }
        return true;
    };

    TEST_DEF("Read stream ML 4-byte tests.") {
        // uint 32
        {
            uint32_t v;
            std::string mem;
            mem.resize(4);
            std::istringstream iss;
            auto buf = reinterpret_cast<unsigned char*>(&mem[0]);

            // BE
            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<2, false>(iss, &v)) && v == uint32_t(0x000089AB));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<3, false>(iss, &v)) && v == uint32_t(0x0089ABCD));

            // LE
            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<2, true>(iss, &v)) && v == uint32_t(0x0000AB89));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<3, true>(iss, &v)) && v == uint32_t(0x00CDAB89));
        }

        // int 32
        {
            int32_t v;
            std::string mem;
            mem.resize(4);
            std::istringstream iss;
            auto buf = reinterpret_cast<unsigned char*>(&mem[0]);

            // BE
            buf[0] = 0x7F; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<2, false>(iss, &v)) && v == int32_t(0x00007FAB));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<2, false>(iss, &v)) && v == int32_t(0xFFFF89AB));

            buf[0] = 0x7F; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<3, false>(iss, &v)) && v == int32_t(0x007FABCD));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<3, false>(iss, &v)) && v == int32_t(0xFF89ABCD));

            // LE
            buf[0] = 0x89; buf[1] = 0x7F; buf[2] = 0xCD; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<2, true>(iss, &v)) && v == int32_t(0x00007F89));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<2, true>(iss, &v)) && v == int32_t(0xFFFFAB89));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0x7F; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<3, true>(iss, &v)) && v == int32_t(0x007FAB89));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<3, true>(iss, &v)) && v == int32_t(0xFFCDAB89));
        }
        return true;
    };

    TEST_DEF("Read stream ML 8-byte tests.") {
        // uint 64
        {
            uint64_t v = 0;
            std::string mem;
            std::istringstream iss;
            auto buf = reinterpret_cast<unsigned char*>(&mem[0]);

            // BE
            mem.resize(4);
            buf[0] = 0xC1; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<4, false>(iss, &v)) && v == uint64_t(0x00C123'4567));

            mem.resize(5);
            buf[0] = 0xC1; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<5, false>(iss, &v)) && v == uint64_t(0xC123'4567'89));

            mem.resize(6);
            buf[0] = 0xC1; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<6, false>(iss, &v)) && v == uint64_t(0xC123'4567'89AB));

            mem.resize(7);
            buf[0] = 0xC1; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<7, false>(iss, &v)) && v == uint64_t(0xC123'4567'89AB'CD));

            // LE
            mem.resize(4);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x87; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<4, true>(iss, &v)) && v == uint64_t(0x87452301));

            mem.resize(5);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<5, true>(iss, &v)) && v == uint64_t(0x8967452301));

            mem.resize(6);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<6, true>(iss, &v)) && v == uint64_t(0xAB8967452301));

            mem.resize(7);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<7, true>(iss, &v)) && v == uint64_t(0xCDAB8967452301));
        }

        // int 64
        {
            int64_t v = 0;
            std::string mem;
            std::istringstream iss;
            auto buf = reinterpret_cast<unsigned char*>(&mem[0]);

            // BE
            mem.resize(4);
            buf[0] = 0x7F; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<4, false>(iss, &v)) && v == int64_t(0x7F234567));

            buf[0] = 0x80; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<4, false>(iss, &v)) && v == int64_t(0xFFFFFFFF8023'4567));

            mem.resize(5);
            buf[0] = 0x7F; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<5, false>(iss, &v)) && v == int64_t(0x7F23'4567'89));

            buf[0] = 0x80; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<5, false>(iss, &v)) && v == int64_t(0xFFFFFF8023'4567'89));

            mem.resize(6);
            buf[0] = 0x7F; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<6, false>(iss, &v)) && v == int64_t(0x7F23'4567'89AB));

            buf[0] = 0x80; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<6, false>(iss, &v)) && v == int64_t(0xFFFF8023'4567'89AB));

            mem.resize(7);
            buf[0] = 0x7F; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<7, false>(iss, &v)) && v == int64_t(0x7F23'4567'89AB'CD));

            buf[0] = 0x80; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<7, false>(iss, &v)) && v == int64_t(0xFF8023'4567'89AB'CD));

            // LE
            mem.resize(4);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x7F; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<4, true>(iss, &v)) && v == int64_t(0x7F452301));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x80; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<4, true>(iss, &v)) && v == int64_t(0xFFFFFFFF804523'01));

            mem.resize(5);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x7F; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<5, true>(iss, &v)) && v == int64_t(0x7F67452301));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<5, true>(iss, &v)) && v == int64_t(0xFFFFFF8967452301));

            mem.resize(6);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0x7F; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<6, true>(iss, &v)) && v == int64_t(0x7F8967452301));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<6, true>(iss, &v)) && v == int64_t(0xFFFFAB8967452301));

            mem.resize(7);
            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0x7F; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<7, true>(iss, &v)) && v == int64_t(0x7FAB8967452301));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            iss = std::istringstream(mem, std::ios::binary);
            TEST_TRUE((utl::readStreamML<7, true>(iss, &v)) && v == int64_t(0xFFCDAB8967452301));
        }
        return true;
    };

    TEST_DEF("Write stream tests.") {
        // uint 32
        {
            std::string mem;
            std::ostringstream oss;
            unsigned char* buf;

            // BE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStream<4, false>(oss, uint32_t(0x89AB0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x89 && buf[1] == 0xAB && buf[2] == 0x00 && buf[3] == 0x00);

            // LE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStream<4, true>(oss, uint32_t(0x89AB0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0xAB && buf[3] == 0x89);
        }

        // int 32
        {
            std::string mem;
            std::ostringstream oss;
            unsigned char* buf;

            // BE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStream<4, false>(oss, int32_t(0x89ABFFFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x89 && buf[1] == 0xAB && buf[2] == 0xFF && buf[3] == 0xFF);

            // LE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStream<4, true>(oss, int32_t(0x89ABFFFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0xFF && buf[1] == 0xFF && buf[2] == 0xAB && buf[3] == 0x89);
        }

        return true;
    };

    TEST_DEF("Write stream ML 4-byte tests.") {
        // uint 32
        {
            std::string mem;
            std::ostringstream oss;
            unsigned char* buf;

            // BE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<2, false>(oss, uint32_t(0x89AB0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 2 && buf[0] == 0x00 && buf[1] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<3, false>(oss, uint32_t(0x89ABCD00))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 3 && buf[0] == 0xAB && buf[1] == 0xCD && buf[2] == 0x00);

            // LE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<2, true>(oss, uint32_t(0x89AB0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 2 && buf[0] == 0x00 && buf[1] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<3, true>(oss, uint32_t(0x89ABCD00))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 3 && buf[0] == 0x00 && buf[1] == 0xCD && buf[2] == 0xAB);
        }

        // int 32
        {
            std::string mem;
            std::ostringstream oss;
            unsigned char* buf;

            // BE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<2, false>(oss, int32_t(0xFFFF89AB))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 2 && buf[0] == 0x89 && buf[1] == 0xAB);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<2, false>(oss, int32_t(0x0000FFFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 2 && buf[0] == 0xFF && buf[1] == 0xFF);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<3, false>(oss, int32_t(0xFF000000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 3 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<3, false>(oss, int32_t(0x89ABCDFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 3 && buf[0] == 0xAB && buf[1] == 0xCD && buf[2] == 0xFF);

            // LE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<2, true>(oss, int32_t(0xFFFF89AB))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 2 && buf[0] == 0xAB && buf[1] == 0x89);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<2, true>(oss, int32_t(0x0000FFFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 2 && buf[0] == 0xFF && buf[1] == 0xFF);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<3, true>(oss, int32_t(0xFF000000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 3 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<3, true>(oss, int32_t(0x89ABCDFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 3 && buf[0] == 0xFF && buf[1] == 0xCD && buf[2] == 0xAB);
        }

        return true;
    };

    TEST_DEF("Write stream ML 8-byte tests.") {
        // uint 64
        {
            std::string mem;
            std::ostringstream oss;
            unsigned char* buf;

            // BE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, false>(oss, uint64_t(0x0123'4567'0000'0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, false>(oss, uint64_t(0xFFFF'FFFF'0000'0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, false>(oss, uint64_t(0x0000'0000'1122'EE00))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x11 &&
                buf[4] == 0x22 && buf[5] == 0xEE && buf[6] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, false>(oss, uint64_t(0x0123'4567'89AB'CD00))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0x23 && buf[1] == 0x45 && buf[2] == 0x67 && buf[3] == 0x89 &&
                buf[4] == 0xAB && buf[5] == 0xCD && buf[6] == 0x00);

            // LE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, true>(oss, uint64_t(0x0123'4567'0000'0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, true>(oss, uint64_t(0xFFFF'FFFF'0000'0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, true>(oss, uint64_t(0x0000'0000'1122'EE00))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0x00 && buf[1] == 0xEE && buf[2] == 0x22 && buf[3] == 0x11 &&
                buf[4] == 0x00 && buf[5] == 0x00 && buf[6] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, true>(oss, uint64_t(0x0123'4567'89AB'CD00))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0x00 && buf[1] == 0xCD && buf[2] == 0xAB && buf[3] == 0x89 &&
                buf[4] == 0x67 && buf[5] == 0x45 && buf[6] == 0x23);
        }

        // int 64
        {
            std::string mem;
            std::ostringstream oss;
            unsigned char* buf;

            // BE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, false>(oss, int64_t(0xFFFF'0000'FFFF'FFFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0xFF && buf[1] == 0xFF && buf[2] == 0xFF && buf[3] == 0xFF);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, false>(oss, int64_t(0x0123'4567'0000'0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, false>(oss, int64_t(0x0000'0000'1122'EEFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x11 &&
                buf[4] == 0x22 && buf[5] == 0xEE && buf[6] == 0xFF);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, false>(oss, int64_t(0x0123'4567'89AB'CDFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0x23 && buf[1] == 0x45 && buf[2] == 0x67 && buf[3] == 0x89 &&
                buf[4] == 0xAB && buf[5] == 0xCD && buf[6] == 0xFF);

            // LE
            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, true>(oss, int64_t(0xFFFF'0000'FFFF'FFFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0xFF && buf[1] == 0xFF && buf[2] == 0xFF && buf[3] == 0xFF);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<4, true>(oss, int64_t(0x0123'4567'0000'0000))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(mem.size() == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, true>(oss, int64_t(0x0000'0000'1122'EEFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0xFF && buf[1] == 0xEE && buf[2] == 0x22 && buf[3] == 0x11 &&
                buf[4] == 0x00 && buf[5] == 0x00 && buf[6] == 0x00);

            oss = std::ostringstream(std::ios::binary);
            TEST_TRUE((utl::writeStreamML<7, true>(oss, int64_t(0x0123'4567'89AB'CDFF))));
            mem = oss.str(); buf = reinterpret_cast<unsigned char*>(&mem[0]);
            TEST_TRUE(
                mem.size() == 7 &&
                buf[0] == 0xFF && buf[1] == 0xCD && buf[2] == 0xAB && buf[3] == 0x89 &&
                buf[4] == 0x67 && buf[5] == 0x45 && buf[6] == 0x23);
        }
        return true;
    };

}