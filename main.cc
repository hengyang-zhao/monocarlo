#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>

namespace monocarlo
{

namespace cell
{
class Base;
}

namespace event
{
class Base;
class AdvanceToName;
class AdvanceToFirstOccurance;
class GoToJail;
class GoBack;
}

class Dice;
class Player;
class World;

class Dice
{
public:
    Dice(int n_dices): dices(n_dices), sum(0), dice_dist(1, 6)
    {
        std::fill(dices.begin(), dices.end(), 0);
    }

    void Roll()
    {
        sum = 0;
        std::cout << "Dice: ";

        bool is_first = true;
        for (auto &d: dices)
        {
            d = dice_dist(rnd_source);
            (is_first ? std::cout : std::cout << " + ") << d;
            sum += d;

            is_first = false;
        }
        (dices.size() > 1 ? std::cout << " = " << sum : std::cout) << std::endl;
    }

    int Get(int i) const
    {
        return dices.at(i);
    }

    int GetSum() const
    {
        return sum;
    }

private:
    std::vector<int> dices;
    int sum;

    // TODO: Set seed.
    std::mt19937 rnd_source;
    std::uniform_int_distribution<int> dice_dist;
};

// Simulates a minimum unit of the World
namespace cell
{
class Base
{
public:
    void PassBy(Player *p) const;
    void Stay(Player *p) const;
    void Leave(Player *p) const;

    int64_t GetStays() const { return total_stays; }

    virtual const std::string &Name() const = 0;

    virtual ~Base() {}

protected:
    virtual void PassByImpl(Player *p) const {}
    virtual void StayImpl(Player *p) const {}
    virtual void LeaveImpl(Player *p) const {}

private:
    mutable int64_t total_stays = 0;
};

class Chance: public Base
{
public:
    Chance();

    const std::string &Name() const override
    {
        static const auto name = new std::string("CHANCE");
        return *name;
    }

protected:
    void StayImpl(Player *p) const override;

private:
    // TODO: Is this per-object? Make it shared.
    std::vector<std::unique_ptr<event::Base>> events;
    //
    // TODO: Set seed.
    mutable std::mt19937 rnd_source;
    mutable std::uniform_int_distribution<int> uniform_dist;
};

class Chest: public Base
{
public:
    const std::string &Name() const override
    {
        static const auto name = new std::string("COMMUNITY CHEST");
        return *name;
    }

protected:
    // All consequences are merely related to $$$.
    // Skipped implementation.
};

class Utility: public Base
{
public:
    Utility(const std::string &name): name(name) {}
    const std::string &Name() const override { return name; }

protected:
    // All consequences are merely related to $$$.
    // Skipped implementation.

private:
    const std::string name;
};

class Railroad: public Base
{
public:
    Railroad(const std::string &name): name(name) {}
    const std::string &Name() const override { return name; }

protected:
    // All consequences are merely related to $$$.
    // Skipped implementation.

private:
    const std::string name;
};

class Jail: public Base
{
public:
    const std::string &Name() const override
    {
        static const auto name = new std::string("THE JAIL");
        return *name;
    }

protected:
    void LeaveImpl(Player *p) const override
    {
        // TODO
    }
};

class Free: public Base
{
public:
    const std::string &Name() const override
    {
        static const auto name = new std::string("FREE PARKING");
        return *name;
    }

protected:
    // No consequence.
};

class Start: public Base
{
public:
    const std::string &Name() const override
    {
        static const auto name = new std::string("GO");
        return *name;
    }

protected:
    // All consequences are merely related to $$$.
    // Skipped implementation.
};

class GoToJail: public Base
{
public:
    const std::string &Name() const override
    {
        static const auto name = new std::string("GO TO JAIL");
        return *name;
    }

protected:
    void StayImpl(Player *p)
    {
        // TODO
    }
};

class Property: public Base
{
public:
    Property(const std::string &name): name(name) {}
    const std::string &Name() const override { return name; }

protected:
    // All consequences are merely related to $$$.
    // Skipped implementation.

private:
    const std::string name;
};

class LuxuryTax: public Base
{
public:
    const std::string &Name() const override
    {
        static const auto name = new std::string("LUXURY TAX");
        return *name;
    }

protected:
    // All consequences are merely related to $$$.
    // Skipped implementation.
};

class IncomeTax: public Base
{
public:
    const std::string &Name() const override
    {
        static const auto name = new std::string("INCOME TAX");
        return *name;
    }

protected:
    // All consequences are merely related to $$$.
    // Skipped implementation.
};
} // namespace cell

class Player
{
public:
    Player(const std::string &name, World *world):
        name(name), world(world)
    {
    }

public:
    void Go();
    const std::string &Name() const { return name; }

private:
    friend class World;
    friend class event::Base;
    friend class event::AdvanceToName;
    friend class event::AdvanceToFirstOccurance;
    friend class event::GoToJail;
    friend class event::GoBack;

    const std::string name;
    World *world = nullptr;
    int location = 0;
    bool is_in_jail = false;
};

// Simulates the Monopoly map/world.
class World
{
public:
    World(): dice(2)
    {
        using namespace cell;
        cells.push_back(std::make_unique<Start>());
        cells.push_back(std::make_unique<Property>("MEDITERRANEAN AVENUE"));
        cells.push_back(std::make_unique<Chest>());
        cells.push_back(std::make_unique<Property>("BALTIC AVENUE"));
        cells.push_back(std::make_unique<IncomeTax>());
        cells.push_back(std::make_unique<Railroad>("READING RAILROAD"));
        cells.push_back(std::make_unique<Property>("ORIENTAL AVENUE"));
        cells.push_back(std::make_unique<Chance>());
        cells.push_back(std::make_unique<Property>("VERMONT AVENUE"));
        cells.push_back(std::make_unique<Property>("CONNECTICUT AVENUE"));
        cells.push_back(std::make_unique<Jail>());
        cells.push_back(std::make_unique<Property>("ST. CHARLES PLACE"));
        cells.push_back(std::make_unique<Utility>("UTILITY COMPANY"));
        cells.push_back(std::make_unique<Property>("STATES AVENUE"));
        cells.push_back(std::make_unique<Property>("VIRGINIA AVENUE"));
        cells.push_back(std::make_unique<Railroad>("PENNSYLVANIA RAILROAD"));
        cells.push_back(std::make_unique<Property>("ST. JAMES PLACE"));
        cells.push_back(std::make_unique<Chest>());
        cells.push_back(std::make_unique<Property>("TENNESSEE AVENUE"));
        cells.push_back(std::make_unique<Property>("NEW YORK AVENUE"));
        cells.push_back(std::make_unique<Free>());
        cells.push_back(std::make_unique<Property>("KENTUCKY AVENUE"));
        cells.push_back(std::make_unique<Chance>());
        cells.push_back(std::make_unique<Property>("INDIANA AVENUE"));
        cells.push_back(std::make_unique<Property>("ILLINOIS AVENUE"));
        cells.push_back(std::make_unique<Railroad>("B. & O. RAILROAD"));
        cells.push_back(std::make_unique<Property>("ATLANTIC AVENUE"));
        cells.push_back(std::make_unique<Property>("VENTNOR AVENUE"));
        cells.push_back(std::make_unique<Utility>("WATER WORKS"));
        cells.push_back(std::make_unique<Property>("MARVIN GARDENS"));
        cells.push_back(std::make_unique<GoToJail>());
        cells.push_back(std::make_unique<Property>("PACIFIC AVENUE"));
        cells.push_back(std::make_unique<Property>("NORTH CAROLINA AVENUE"));
        cells.push_back(std::make_unique<Chest>());
        cells.push_back(std::make_unique<Property>("PENNSYLVANIA AVENUE"));
        cells.push_back(std::make_unique<Railroad>("SHORT LINE"));
        cells.push_back(std::make_unique<Chance>());
        cells.push_back(std::make_unique<Property>("PARK PLACE"));
        cells.push_back(std::make_unique<LuxuryTax>());
        cells.push_back(std::make_unique<Property>("BOARDWALK"));

        for (int i = 0; i < cells.size(); ++i)
        {
            cells_by_name[cells[i]->Name()] = i;
            std::cout << "Added " << cells[i]->Name() << std::endl;
        }
    }

public:
    Player *GetPlayer(const std::string &name)
    {
        auto &p = players[name];
        if (p == nullptr)
        {
            p.reset(new Player(name, this));
        }
        return p.get();
    }
    void PrintStats() const
    {
        int64_t total = 0;
        for (auto &c: cells) total += c->GetStays();
        for (auto &c: cells)
        {
            std::clog << c->Name() << ": " << c->GetStays() << " ("
                << c->GetStays() * 100.0 / total << "%) hits" << std::endl;
        }
    }

private:
    friend class Player;
    friend class event::AdvanceToName;
    friend class event::AdvanceToFirstOccurance;
    friend class event::GoToJail;
    friend class event::GoBack;

    std::vector<std::unique_ptr<cell::Base>> cells;
    std::map<std::string, int> cells_by_name;

    std::map<std::string, std::unique_ptr<Player>> players;

    Dice dice;
};

// Simulates event cards.
namespace event
{
class Base
{
public:
    virtual void Apply(Player *) const = 0;
};

class AdvanceToName: public Base
{
public:
    AdvanceToName(const std::string &destination): destination(destination) {}

    void Apply(Player *p) const override
    {
        std::cout << "Event: ADVANCE TO EXACT (" << destination << ")" << std::endl;
        World *world = p->world;
        world->cells[p->location]->Leave(p);

        int n_steps = world->dice.GetSum();
        while (world->cells[p->location]->Name() != destination)
        {
            p->location = (p->location + 1) % world->cells.size();
            world->cells[p->location]->PassBy(p);
        }
        world->cells[p->location]->Stay(p);
    }

private:
    const std::string destination;
};

class AdvanceToFirstOccurance: public Base
{
public:
    AdvanceToFirstOccurance(std::initializer_list<std::string> dests): destinations(dests.begin(), dests.end()) {}

    void Apply(Player *p) const override
    {
        {
            std::cout << "Event: ADVANCE TO FIRST OCCURANCE (";
            bool is_first = true;
            for (const auto &d: destinations)
            {
                (is_first ? std::cout : std::cout << " or ") << d;
                is_first = false;
            }
            std::cout << ")" << std::endl;
        }
        World *world = p->world;
        world->cells[p->location]->Leave(p);

        int n_steps = world->dice.GetSum();
        while (destinations.count(world->cells[p->location]->Name()) == 0)
        {
            p->location = (p->location + 1) % world->cells.size();
            world->cells[p->location]->PassBy(p);
        }
        world->cells[p->location]->Stay(p);
    }

private:
    const std::set<std::string> destinations;
};

class GoToJail: public Base
{
public:
    void Apply(Player *p) const override
    {
        std::cout << "Event: GO TO JAIL" << std::endl;
        World *world = p->world;

        world->cells[p->location]->Leave(p);

        p->location = world->cells_by_name.at("THE JAIL");

        world->cells[p->location]->PassBy(p);
        world->cells[p->location]->Stay(p);
    }
};

class GoBack: public Base
{
public:
    GoBack(int n_steps): n_steps(n_steps) {}
    void Apply(Player *p) const override
    {
        std::cout << "Event: GO BACK " << n_steps << " SPACES" << std::endl;
        World *world = p->world;
        world->cells[p->location]->Leave(p);

        int n_steps = world->dice.GetSum();
        for (int i = 0; i < n_steps; ++i)
        {
            p->location = (p->location - 1) % world->cells.size();
            world->cells[p->location]->PassBy(p);
        }
        world->cells[p->location]->Stay(p);
    }

private:
    int n_steps;
};

} // namespace event

namespace cell {
void Base::PassBy(Player *p) const
{
    std::cout << "Player " << p->Name() << " PASSING " << Name() << std::endl;
    PassByImpl(p);
}
void Base::Stay(Player *p) const
{
    std::cout << "Player " << p->Name() << " STAYED AT " << Name() << std::endl;
    StayImpl(p);
    total_stays += 1;
}
void Base::Leave(Player *p) const
{
    std::cout << "Player " << p->Name() << " LEFT " << Name() << std::endl;
    LeaveImpl(p);
}

Chance::Chance()
{
    using namespace event;
    using event::GoToJail;

    events.push_back(std::make_unique<AdvanceToName>("READING RAILROAD"));
    events.push_back(std::make_unique<AdvanceToName>("GO"));
    events.push_back(std::make_unique<AdvanceToName>("BOARDWALK"));
    events.push_back(std::make_unique<AdvanceToName>("ST. CHARLES PLACE"));
    events.push_back(std::make_unique<AdvanceToName>("ILLINOIS AVENUE"));
    events.push_back(std::make_unique<AdvanceToFirstOccurance,
            std::initializer_list<std::string>>({
                "WATER WORKS",
                "UTILITY COMPANY",
                }));
    events.push_back(std::make_unique<AdvanceToFirstOccurance,
            std::initializer_list<std::string>>({
                "READING RAILROAD",
                "PENNSYLVANIA RAILROAD",
                "B. & O. RAILROAD",
                "SHORT LINE",
                }));
    // TODO: Implement AdvanceToNearest.
    events.push_back(std::make_unique<AdvanceToFirstOccurance,
            std::initializer_list<std::string>>({
                "READING RAILROAD",
                "PENNSYLVANIA RAILROAD",
                "B. & O. RAILROAD",
                "SHORT LINE",
                }));
    events.push_back(std::make_unique<GoToJail>());
    events.push_back(std::make_unique<GoBack>(3));

    uniform_dist = std::uniform_int_distribution<int>(0, events.size() - 1);
}
} // namespace cell

void Player::Go()
{
    world->dice.Roll();

    world->cells[location]->Leave(this);

    int n_steps = world->dice.GetSum();
    for (int i = 0; i < n_steps; ++i)
    {
        location = (location + 1) % world->cells.size();
        world->cells[location]->PassBy(this);
    }
    world->cells[location]->Stay(this);
}

namespace cell
{
void Chance::StayImpl(Player *p) const
{
    auto event = events.at(uniform_dist(rnd_source)).get();
    event->Apply(p);
}
} // namespace cell

} // namespace monocarlo

int main(int argc, char **argv)
{
    using namespace monocarlo;

    World world;
    Player *p = world.GetPlayer("lao-zhao");

    for (int i = 0; i < 1000000; ++i) p->Go();

    world.PrintStats();
    return 0;
}
