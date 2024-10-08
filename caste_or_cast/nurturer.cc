#include "nurturer.hh"

#include <iostream>

// The Colony class was forward declared in Ant header
// We need to include its header here so we know Colony implementation
#include "colony.hh"
#include "provider.hh"
#include "worker.hh"

void Nurturer::feedLarvae()
{
    if (food_stock_ < 0.5)
        return;
    size_t count = 0;
    std::shared_ptr<Colony> colony = check_colony_access();
    for (auto i = colony->workers_.begin(); i < colony->workers_.end(); i++)
    {
        // if it doesn't have food anymore, can't feed more larvae
        if (food_stock_ < 0.5)
            break;
        std::shared_ptr<Worker> worker = *i;
        // only feed if it is a larvae and if it is not already full (its
        // food_level_ => 4)
        if (DevelopmentStage::LARVA == worker->get_stage()
            && worker->get_food_level() < 4)
        {
            worker->increment_food_level_by(0.5);
            food_stock_ -= 0.5;
            colony->cleanliness -= 0.3;
            food_level_ -= 0.03;
            count++;
        }
    }
    std::cout << count << " larvae were fed by nurturer.\n";
}

void Nurturer::work()
{
    if (stage_ != DevelopmentStage::ADULT)
        return;
    if (food_stock_ > 0.5)
    {
        // eat before working
        food_level_ += 0.5;
        food_stock_ -= 0.5;
        // complete its tasks
        feedLarvae();
        feedQueen();
        cleanNest();
    }
    else
        // make the ant more hungry
        food_level_ -= 0.042;
    //! call base class work() method
    Worker::work();
}

void Nurturer::cleanNest()
{
    std::shared_ptr<Colony> colony = check_colony_access();
    if (colony->cleanliness < 100)
    {
        // clean the nest according to the luck of the Nurturer
        std::cout << "Cleaning nest : gained " << luck_ << " of cleanliness.\n";
        auto cleanliness = colony->cleanliness + luck_;
        colony->cleanliness = (cleanliness > 100) ? 100 : cleanliness;
    }
}

// FIXME : Implements communicate(std::weak_ptr<Ant>) virtual overridden method
// and feedQueen()
void Nurturer::feedQueen()
{
    if (food_stock_ < 1)
        return;
    std::shared_ptr<Colony> colony = check_colony_access();
    std::cout << "Feeding Queen.\n";

    colony->queen_->increment_food_level_by(1.f);
    food_stock_ -= 1.f;

    int whole_part_food = static_cast<int>(colony->queen_->get_food_level());
    if (whole_part_food > 0 && whole_part_food % 6 == 0)
        colony->queen_->layEgg();

    colony->cleanliness -= 0.2;
}

bool Nurturer::communicate(std::weak_ptr<Ant> wk_receiver)
{
    if (Ant::communicate(wk_receiver) == false)
        return false;
    std::cout << "Nurturer initiates communication.\n";

    Provider* cast_provider = dynamic_cast<Provider*>(wk_receiver.lock().get());
    if (cast_provider != nullptr)
        cast_provider->transferFood(*this);
    return true;
}
