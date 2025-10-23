#include "Serialise.h"

#include <cassert>
#include <string>
#include <sstream>

#include "libtcod.h"

Saver::~Saver()
{
    if (ofile != nullptr && ofile->is_open())
    {
        ofile->close();
    }
}

void Saver::InitForSave(const std::string& filename)
{
    ofile = new std::ofstream(filename);
    if (ofile == nullptr || !ofile->is_open())
    {
        throw std::runtime_error("Cannot open file for saving!");
    }
}

void Saver::PutInt(const int val) const
{
    assert(ofile != nullptr);
    *ofile << val << '\n';
}

void Saver::PutColor(const TCODColor* val) const
{
    assert(ofile != nullptr);
    *ofile << val->r << " " << val->g << " " << val->b << '\n';
}

void Saver::PutString(const std::string& val) const
{
    assert(ofile != nullptr);
    int len = static_cast<int>(val.length());
    *ofile << len << '`' << val << '\n';
}

void Saver::PutFloat(const float val) const
{
    assert(ofile != nullptr);
    *ofile << val << '\n';
}

// Loader implementation
Loader::~Loader()
{
    if (ifile != nullptr && ifile->is_open())
    {
        ifile->close();
    }
}

void Loader::LoadFromFile(const std::string& filename)
{
    ifile = new std::ifstream(filename);
    if (ifile == nullptr || !ifile->is_open())
    {
        throw std::runtime_error("Cannot open file for loading!");
    }
}

int Loader::GetInt() const
{
    assert(ifile != nullptr);
    int val;
    *ifile >> val;
    return val;
}

TCODColor Loader::GetColor() const
{
    assert(ifile != nullptr);
    TCODColor val;
    *ifile >> val.r >> val.g >> val.b;
    return val;
}

std::string Loader::GetString() const
{
    assert(ifile != nullptr);
    int len;
    char separatingChar;
    *ifile >> len;
    *ifile >> separatingChar;

    char* buf = new char[len + 1];
    ifile->read(buf, len);
    buf[len] = '\0';

    return std::string{ buf };
}

float Loader::GetFloat() const
{
    assert(ifile != nullptr);
    float val;
    *ifile >> val;
    return val;
}