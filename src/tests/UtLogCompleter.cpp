#include <sstream>

#include <gtest/gtest.h>

#include "Log.hpp"
#include "ut_utils.hpp"


struct LogSequence {
    struct Head {
        struct Next {
            std::stringstream *_stream;

            Next(const Next &next) : _stream(next._stream) {}

            template <class Type>
            explicit Next(std::stringstream *stream, const Type &value) : _stream(stream) {
                (*_stream) << value;
            }

            template <class Type>
            Next operator<<(const Type &value) {
                return Next(_stream, value);
            }
        };

        std::shared_ptr<std::stringstream> _stream;
        std::string _module;

        Head(const Head &head) : _stream(head._stream), _module(head._module) {}

        template <class Type>
        explicit Head(const std::string &module, const Type &value)
            : _stream(new std::stringstream), _module(module) {
            (*_stream) << value;
        }

        ~Head() {
            utils::Singleton<utils::Log>::get()->print(DEBUG, _module, _stream->str().c_str());
        }

        template <class Type>
        Next operator<<(const Type &value) {
            return Next(_stream.get(), value);
        }
    };

    std::string _module;

    LogSequence(const std::string &module) : _module(module) {}

    template <class Type>
    Head operator<<(const Type &value) {
        return Head(_module, value);
    }
};


class TestPkt : public ::testing::Test {
public:
    TestPkt() {}

    ~TestPkt() {}

    void SetUp() {}

    void TearDown() {}
};


TEST_F(TestPkt, test_log_completter) {
    LOG_TO_STDOUT;
    ut_utils::Gout("Begin");
    LogSequence t("test label 1");
    t << 1 << "," << 2 << "," << 3;
    LOG_FLUSH;
    {
        LOG_SIMPLE_TIME_POINT(true);
        LogSequence t("test label 2");
        t << "a"
          << ","
          << "b";
        LOG_FLUSH;
    }
    ut_utils::Gout("End");
}
////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
