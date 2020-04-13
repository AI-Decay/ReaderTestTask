#include <QCoreApplication>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <stdexcept>
#include <array>
#include "SimpleTimer.hpp"
#include "toml11\\toml.hpp"
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
    if(mapper.GetInfoError() != 0)
       return false;
    std::vector<BYTE> buffer(bufferSize);
    if(size <= readSpeed)
        readSpeed = size;
    timer.Start();
    while (pos < size)
    {
        for(size_t cur = 0; (pos < size) && (cur < readSpeed); cur += bufferSize)
        {
            if(pos + bufferSize > size) {
                if(mapper.ReadFile(size - pos, buffer))
                    return false;
              }
            else {
               if(mapper.ReadFile(pos, buffer))
                 return false;
             }
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

    size_t speed;
    toml::basic_value<toml::discard_comments, std::unordered_map, std::vector> arrayOfPath;
    std::vector<std::wstring> paths;
    std::wstring path;
try {
    const auto file = toml::parse("..\\Info.toml");
    const auto data = toml::find(file, "path");
    arrayOfPath = toml::find(data, "directory");
    speed = toml::find<size_t>(data, "speed");

    for(size_t i = 0; i < arrayOfPath.size(); i++)
    {
        path = toml::find<std::wstring>(arrayOfPath, i);
        if(!std::filesystem::is_directory(path) && std::filesystem::exists(path))
           paths.push_back(path);
    }

  }
    catch(toml::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        return -1;
    }

    size_t size = arrayOfPath.size();



    size_t count = 0;

    while(size/countOfThreads != 0)
    {
        for(auto& th : threads)
        {
            th = std::thread(Read, paths[count++], speed);
        }
        size -= countOfThreads;
    }


    for(auto& x : threads) {
        x.join();
    }

    for(size_t i = 0; i < size; i++)
    {
        threads[i] = std::thread(Read, paths[count++], speed);
    }

    for(auto& x : threads) {
        if(x.joinable())
           x.join();
    }


    return a.exec();
}
