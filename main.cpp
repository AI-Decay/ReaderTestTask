#include <QCoreApplication>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <array>
#include "SimpleTimer.hpp"
#include "..\\toml11-3.4.0\\toml.hpp"
#include "FileMapper.hpp"

constexpr auto countOfThreads = 4;

auto FileSize(std::wstring path) {
   return std::filesystem::file_size(path);
}

bool Read(std::wstring path, size_t speed = 1) {

    SimpleTimer timer;
    size_t pos = 0;
    constexpr size_t bufferSize = 1024;
    size_t readSpeed = speed * bufferSize;
    size_t size = FileSize(path);
    FileMapper mapper(path, bufferSize);
    if(size <= readSpeed)
        readSpeed = size;
    timer.Start();
    while (pos < size)
    {
        for(size_t cur = 0; (pos < size) && (cur < readSpeed); cur += bufferSize)
        {
            if(pos + bufferSize > size)
               mapper.ReadFile(size - pos);
            else
               mapper.ReadFile(pos);
            pos += bufferSize;
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
    std::wstring path;

    while(size/countOfThreads != 0)
    {
        for(auto& th : threads)
        {
            path = toml::find<std::wstring>(arrayOfPath, count++);
            th = std::thread(Read, path, speed);
        }
        size -= countOfThreads;
    }


    for(auto& x : threads) {
        x.join();
    }

    for(size_t i = 0; i < size; i++)
    {
        path = toml::find<std::wstring>(arrayOfPath, count++);
        threads[i] = std::thread(Read, path, speed);
    }

    for(auto& x : threads) {
        if(x.joinable())
           x.join();
    }

    return a.exec();
}
