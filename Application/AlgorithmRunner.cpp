#include <sstream>
#include <thread>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional/optional_io.hpp>
#include <QString>
#include "AlgorithmRunner.h"

AlgorithmRunner::AlgorithmRunner() :
    mLastGeneration(0), mStep(0), mMap(""), mStopSent(false)
{

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

void AlgorithmRunner::setup(QList<QString> files, QString lastGeneration, QString step, QString map) {
    mMap = map.toStdString();
    mLastGeneration = atoi(lastGeneration.toStdString().c_str());
    mStep = atoi(step.toStdString().c_str());
    std::cout << "TestRunner: map = " << map.toStdString() << std::endl;

    for (auto it = files.begin(); it != files.end(); it++) {
        mFiles.push_back(it->toStdString());
        std::cout << "TestRunner: file = " << it->toStdString() << std::endl;
    }
}

void AlgorithmRunner::stop()
{
    mStopSent = true;
}

void AlgorithmRunner::run(const QJSValue &callback) {
    auto runTests = [&](const QJSValue &callback) {
        QJSValue cbCopy(callback);
        std::stringstream command;
        int status = 0, time = 0;

        for (int generation = 0; generation <= mLastGeneration; generation += mStep) {

            std::cout << "AlgorithmRunner: generation: " << generation << std::endl;

            for (auto &file: mFiles) {
                // Testing was stopped, terminate execution of execs
                if (mStopSent) {
                    mStopSent = false;
                    return;
                }

                command.str("");

                std::cout << "AlgorithmRunner: file: " << file << std::endl;

                // check if this executable must be run by 'mpirun'
                if (file.find("mpi") != std::string::npos)
                    command << "mpirun ";

                command << "" << file << " -f " << mMap << " -n " << generation;

                std::cout << "AlgorithmRunner: Command: " << command.str() << std::endl;

                auto returned_str = execCommand(command.str(), status);

                std::cout << "AlgorithmRunner: Returned string: " << returned_str << std::endl;

                time = parseTime(returned_str);
                std::cout << "AlgorithmRunner: callback(" << file.c_str()
                          << ", " << time
                          << ", " << generation
                          << std::endl;

                // update chart by calling callback function
                cbCopy.call(QJSValueList {file.c_str(), time, generation});
            }
        }
    };

    std::thread t(runTests, callback);
    t.detach();
}
