#pragma once

#include <vector>

#include "soldier.hh"

class Regiment
{
public:
    Regiment() = default;
    void join_by(Regiment& r);
    size_t count() const;
    void add_soldier(Soldier& s);
    void print_state() const;
    void scream() const;

private:
    std::vector<std::reference_wrapper<Soldier>> soldiers_;
};
