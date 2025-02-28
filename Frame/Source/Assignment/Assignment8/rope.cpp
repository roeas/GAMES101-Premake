#include <cassert>
#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL
{

Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
{
    masses.reserve(num_nodes);
    springs.reserve(num_nodes - 1);

    for (size_t i = 0; i < num_nodes; ++i)
    {
        Vector2D pos = start + (end - start) * ((float)i / ((float)num_nodes - 1.0f));
        masses.emplace_back(new Mass{ pos, node_mass, false });
    }

    for (size_t i = 0; i < num_nodes - 1; ++i)
    {
        springs.emplace_back(new Spring{ masses[i], masses[i + 1], k });
    }

    for (const auto &i : pinned_nodes)
    {
        assert(i < masses.size());
        masses[i]->pinned = true;
    }
}

void Rope::simulateEuler(float delta_t, Vector2D gravity)
{
    for (const auto &s : springs)
    {
        float length = (s->m1->position - s->m2->position).norm();
        s->m1->forces += -s->k * (s->m1->position - s->m2->position) / length * (length - s->rest_length);
        s->m2->forces += -s->k * (s->m2->position - s->m1->position) / length * (length - s->rest_length);
    }

    for (auto &m : masses)
    {
        if (!m->pinned)
        {
            m->velocity += (m->forces / m->mass + gravity) * delta_t;
            m->position += m->velocity * delta_t;
        }

        m->forces = Vector2D{ 0, 0 };
    }
}

void Rope::simulateVerlet(float delta_t, Vector2D gravity)
{
    for (auto &s : springs)
    {
        auto length = (s->m1->position - s->m2->position).norm();
        s->m1->forces += -s->k * (s->m1->position - s->m2->position) / length * (length - s->rest_length);
        s->m2->forces += -s->k * (s->m2->position - s->m1->position) / length * (length - s->rest_length);
    }

    for (auto &m : masses)
    {
        if (!m->pinned)
        {
            constexpr float friction = 0.00001;

            Vector2D position = m->position;
            m->position = position + (1.0f - friction) * (position - m->last_position) + (m->forces / m->mass + gravity) * delta_t * delta_t;
            m->last_position = position;
        }

        m->forces = Vector2D{ 0, 0 };
    }
}

}
