#include "test_runner.h"

#include <vector>
#include <memory>
#include "geo2d.h"
#include "game_object.h"

using namespace std;

template <typename T>
struct Collider : GameObject {
    bool Collide(const GameObject& that) const final {
        return that.CollideWith(static_cast<const T&>(*this));
    }
};


class Unit final : public Collider<Unit> {
private:
    geo2d::Point data;
public:
    explicit Unit(geo2d::Point position) : data(position) {

    }
    geo2d::Point GetPoint() const {
        return data;
    }
    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
};

class Building final : public Collider<Building> {
private:
    geo2d::Rectangle data;
public:
    explicit Building(geo2d::Rectangle geometry) : data(geometry) {}
    geo2d::Rectangle GetRectangle() const {
        return data;
    }
    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
};

class Tower final : public Collider<Tower> {
private:
    geo2d::Circle data;
public:
    explicit Tower(geo2d::Circle geometry) : data(geometry) {}
    geo2d::Circle GetCircle() const {
        return data;
    }
    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
};

class Fence final : public Collider<Fence> {
private:
    geo2d::Segment data;
public:
    explicit Fence(geo2d::Segment geometry) : data(geometry) {}
    geo2d::Segment GetSegment() const {
        return data;
    }
    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;
};

bool Collide(const GameObject& first, const GameObject& second);

bool Unit::CollideWith(const Unit& that) const  {
     return geo2d::Collide(data, that.GetPoint());
}

bool Unit::CollideWith(const Building& that) const  {
     return geo2d::Collide(data, that.GetRectangle());
}

bool Unit::CollideWith(const Tower& that) const  {
     return geo2d::Collide(data, that.GetCircle());
}

bool Unit::CollideWith(const Fence& that) const  {
     return geo2d::Collide(data, that.GetSegment());
}

bool Building::CollideWith(const Unit& that) const  {
     return geo2d::Collide(data, that.GetPoint());
}

bool Building::CollideWith(const Building& that) const  {
     return geo2d::Collide(data, that.GetRectangle());
}

bool Building::CollideWith(const Tower& that) const  {
     return geo2d::Collide(data, that.GetCircle());
}

bool Building::CollideWith(const Fence& that) const  {
     return geo2d::Collide(data, that.GetSegment());
}

bool Tower::CollideWith(const Unit& that) const  {
     return geo2d::Collide(data, that.GetPoint());
}

bool Tower::CollideWith(const Building& that) const  {
     return geo2d::Collide(data, that.GetRectangle());
}

bool Tower::CollideWith(const Tower& that) const  {
     return geo2d::Collide(data, that.GetCircle());
}

bool Tower::CollideWith(const Fence& that) const  {
     return geo2d::Collide(data, that.GetSegment());
} 

bool Fence::CollideWith(const Unit& that) const  {
     return geo2d::Collide(data, that.GetPoint());
}

bool Fence::CollideWith(const Building& that) const  {
     return geo2d::Collide(data, that.GetRectangle());
}
bool Fence::CollideWith(const Tower& that) const {
    return geo2d::Collide(data, that.GetCircle());
}

bool Fence::CollideWith(const Fence& that) const {
     return geo2d::Collide(data, that.GetSegment());
}

bool Collide(const GameObject& first, const GameObject& second) {
    return first.Collide(second);
}

void TestAddingNewObjectOnMap() {
    using namespace geo2d;

    const vector<shared_ptr<GameObject>> game_map = {
      make_shared<Unit>(Point{3, 3}),
      make_shared<Unit>(Point{5, 5}),
      make_shared<Unit>(Point{3, 7}),
      make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
      make_shared<Tower>(Circle{Point{9, 4}, 1}),
      make_shared<Tower>(Circle{Point{10, 7}, 1}),
      make_shared<Building>(Rectangle{{11, 4}, {14, 6}})
    };

    for (size_t i = 0; i < game_map.size(); ++i) {
        Assert(
            Collide(*game_map[i], *game_map[i]),
            "An object doesn't collide with itself: " + to_string(i)
        );

        for (size_t j = 0; j < i; ++j) {
            Assert(
                !Collide(*game_map[i], *game_map[j]),
                "Unexpected collision found " + to_string(i) + ' ' + to_string(j)
            );
        }
    }

    auto new_warehouse = make_shared<Building>(Rectangle{ {4, 3}, {9, 6} });
    ASSERT(!Collide(*new_warehouse, *game_map[0]));
    ASSERT(Collide(*new_warehouse, *game_map[1]));
    ASSERT(!Collide(*new_warehouse, *game_map[2]));
    ASSERT(Collide(*new_warehouse, *game_map[3]));
    ASSERT(Collide(*new_warehouse, *game_map[4]));
    ASSERT(!Collide(*new_warehouse, *game_map[5]));
    ASSERT(!Collide(*new_warehouse, *game_map[6]));

    auto new_defense_tower = make_shared<Tower>(Circle{ {8, 2}, 2 });
    ASSERT(!Collide(*new_defense_tower, *game_map[0]));
    ASSERT(!Collide(*new_defense_tower, *game_map[1]));
    ASSERT(!Collide(*new_defense_tower, *game_map[2]));
    ASSERT(Collide(*new_defense_tower, *game_map[3]));
    ASSERT(Collide(*new_defense_tower, *game_map[4]));
    ASSERT(!Collide(*new_defense_tower, *game_map[5]));
    ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestAddingNewObjectOnMap);
    
    return 0;
}