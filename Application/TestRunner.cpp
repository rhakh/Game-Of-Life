#include "TestRunner.h"
#include <QString>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional/optional_io.hpp>

TestRunner::TestRunner() {

}

std::string execCommand(const std::string cmd, int& out_exitStatus) {
    std::array<char, 256> buffer;
    std::string result;

    out_exitStatus = 0;
    auto pPipe = ::popen(cmd.c_str(), "r");
    // TODO: check files for existance and delete trow
    if(pPipe == nullptr)
        throw std::runtime_error("Cannot open pipe");

    while(not std::feof(pPipe)) {
        auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
        result.append(buffer.data(), bytes);
    }

    auto rc = ::pclose(pPipe);

    if(WIFEXITED(rc))
        out_exitStatus = WEXITSTATUS(rc);

    return result;
}

int parseTime(std::string &spendTime) {
    namespace qi     = boost::spirit::qi;
    namespace phx    = boost::phoenix;

    auto begin = spendTime.begin();
    auto end = spendTime.end();
    float time;
    bool ok = qi::phrase_parse
                 (
                     begin,
                     end,
                         "Spend time:" >>
                         qi::float_[phx::ref(time) = qi::_1] >>
                         "ms",
                     qi::space
                 );

    if (!ok)
        return -1;
    return time;
}

void TestRunner::setup(QList<QString> files, QList<QString> generations, QString map) {
    mMap = map.toStdString();
    std::cout << "TestRunner: map = " << map.toStdString() << std::endl;
    for (auto it = files.begin(); it != files.end(); it++) {
        mFiles.push_back(it->toStdString());
        std::cout << "TestRunner: file = " << it->toStdString() << std::endl;
    }
    for (auto it = generations.begin(); it != generations.end(); it++) {
        mGenerations.push_back(it->toStdString());
        std::cout << "TestRunner: generation = " << it->toStdString() << std::endl;
    }
}

void TestRunner::run(const QJSValue &callback) {
    QJSValue cbCopy(callback);
    // QJSEngine *engine = qjsEngine(this);
    std::stringstream command;
    int status = 0, time = 0;

    std::cout << "TestRunner: enter run" << std::endl;

    for (auto &generation: mGenerations) {

        std::cout << "TestRunner: generation: " << generation << std::endl;

        command.str("");
        for (auto &file: mFiles) {

            std::cout << "TestRunner: file: " << file << std::endl;

            // if this executable must be run by 'mpirun'
            if (file.find("mpi") != std::string::npos)
                command << "mpirun ";

            command << "" << file << " -f " << mMap << " -n " << generation;

            std::cout << "TestRunner: Command: " << command.str() << std::endl;

            auto returned_str = execCommand(command.str(), status);

            std::cout << "TestRunner: Returned string: " << returned_str << std::endl;

            time = parseTime(returned_str);
            // cbCopy.call(QJSValueList { engine->toScriptValue(file), time });
            cbCopy.call(QJSValueList {file.c_str(), time, generation.c_str()});
        }
    }

    std::cout << "TestRunner: return run" << std::endl;
}
