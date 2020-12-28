#include <PathIterator.hpp>

using namespace utils;


PathIterator::PathIterator() {}


PathIterator::PathIterator(const char* path_name)
    : _path_name(path_name), _dir(PDir(opendir(path_name), [](DIR* p) { closedir(p); })) {
    if (_dir) {
        _entry = readdir(_dir.get());
    }
}


PathIterator::PathIterator(const PathIterator& pit)
    : _path_name(pit._path_name), _dir(pit._dir), _entry(pit._entry) {}


PathIterator& PathIterator::operator++() {
    _entry = readdir(_dir.get());
    return *this;
}


PathIterator PathIterator::operator++(int) {
    PathIterator tmp(*this);
    ++(*this);
    return tmp;
}


bool PathIterator::operator==(const PathIterator& rhs) const {
    if (_entry and rhs._entry) {
        return _entry->d_name == rhs._entry->d_name;
    }
    return (_entry == rhs._entry);
}


bool PathIterator::operator!=(const PathIterator& rhs) const {
    if (_entry and rhs._entry) {
        return _entry->d_name != rhs._entry->d_name;
    }
    return (_entry != rhs._entry);
}


const char* PathIterator::operator*() {
    if (_entry) {
        return _entry->d_name;
    }
    return "";
}
