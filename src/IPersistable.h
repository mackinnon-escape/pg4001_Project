#ifndef IPERSISTABLE_H
#define IPERSISTABLE_H

class Loader;
class Saver;

class IPersistable
{
public:
    virtual ~IPersistable() = default;
    virtual void Load(Loader& loader) = 0;
    virtual void Save(Saver& saver) const = 0;
};

#endif // IPERSISTABLE_H