#include <QCoreApplication>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <array>
#include "SimpleTimer.hpp"
#include "..\\toml11-3.4.0\\toml.hpp"

constexpr auto countOfThreads = 4;

auto FileSize(std::string path) {
    std::ifstream file( path, std::ios::binary | std::ios::ate);
    return file.tellg();
}

bool Read(std::string path, size_t speed = 1) {
    SimpleTimer timer;
    size_t pos = 0;
    constexpr size_t bufferSize = 1024;
    size_t readSpeed = speed * bufferSize;
    std::ifstream file{ path };
    if(!file)
        return false;
    size_t size = FileSize(path);
    std::array<char, bufferSize>  buffer;
    if(size <= readSpeed)
        readSpeed = size;
    timer.Start();
    while (pos < size)
    {
        for(size_t cur = 0; (pos < size) && (cur < readSpeed); cur += bufferSize)
        {
            if(pos + bufferSize > size)
               file.read(&buffer[0], size - pos);
            else
               file.read(&buffer[0], bufferSize);
            pos += bufferSize;
            file.seekg(pos);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 - timer.Duration()));
        timer.ResetTime();
    }

    std::cout << std::filesystem::path(path).filename() << "  has been read" << std::endl;
    return true;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::array<std::thread, countOfThreads> threads;

    const auto file = toml::parse("..\\Info.toml");
    const auto data = toml::find(file, "path");
    const auto arrayOfPath = toml::find(data, "directory");
    const auto speed = toml::find<size_t>(data, "speed");
    size_t size = arrayOfPath.size();
    size_t count = 0;
    std::string path;

    while(size/countOfThreads != 0)
    {
        for(auto& th : threads)
        {
            path = toml::find<std::string>(arrayOfPath, count++);
            th = std::thread(Read, path, speed);
        }
        size -= 4;
    }


    for(auto& x : threads) {
        x.join();
    }

    for(size_t i = 0; i < size; i++)
    {
        path = toml::find<std::string>(arrayOfPath, count++);
        threads[i] = std::thread(Read, path, speed);
    }

    for(auto& x : threads) {
        if(x.joinable())
           x.join();
    }

    return a.exec();
}
