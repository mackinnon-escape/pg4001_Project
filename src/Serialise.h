#pragma once

#include <fstream>
#include <string>
class TCODColor;


class Saver
{
public:
    ~Saver();
    void InitForSave(const std::string& filename);
    void PutInt(int val) const;
    void PutColor(const TCODColor* val) const;
    void PutString(const std::string& val) const;
    void PutFloat(const float val) const;

private:
    std::ofstream* ofile{ nullptr };
};

class Loader
{
public:
    Loader() : ifile(nullptr) {}
    ~Loader();
    void LoadFromFile(const std::string& filename);
    int GetInt() const;
    TCODColor GetColor() const;
    std::string GetString() const;
    float GetFloat() const;
private:
    std::ifstream* ifile{ nullptr };
};