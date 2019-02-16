#include "Controller.h"
#include <cmath>
#include <float.h>
#include <iostream>
namespace mrsd {
Controller::~Controller()
{
    delete[] dangerZone;
}

void Controller::control(const mrsd::Game& g, float t)
{
    if (p != 0) {
        std::list<Projectile> proj = g.getProjectiles();
        for (std::list<Projectile>::iterator it = proj.begin(); it != proj.end(); it++) {
            Prediction pred = trackProjectile(*it, g, t);
            if (exp_pred[pred.x] == 0) {
                exp_pred[pred.x] = pred.t;
            }
        }
        currentTime += t;
        determineSafeSpots(g, t);
        pickSafeSpot(g, t);
    } else {
        currentTime = 0;
        dangerZone = new int[g.getWidth() + 1];
        for (int i = 0; i < g.getWidth() + 1; ++i)
            dangerZone[i] = 0;
    }
}

void Controller::createPlayer(Game& g)
{
    if (p == 0) {
        p = new Player();
        p->dead = true;
    }
    if (p->dead) {
        p->dead = false;
        p->x = g.getWidth() / 2;
        g.newPlayer(p);
    }
    /*int timesteps = 10;
		occupancyGrid = new int*[timesteps];
		for(int i = 0; i < timesteps; i++) {
			occupancyGrid[i] = new int[g.getWidth()+1];
			for(int j = 0; j < g.getWidth()+1; j++) {
				occupancyGrid[i][j] = 0;
			}
		}*/
}

Prediction Controller::trackProjectile(const Projectile& p, const Game& g, float t)
{
    Prediction pred;
    Projectile temp_p = p;
    float gravity = g.getGravity();
    float time_elaspsed = 0;
    while (temp_p.y >= 0) {
        temp_p.x += temp_p.vx * t;
        temp_p.y += temp_p.vy * t + .5f * t * t * gravity;
        temp_p.vy += t * gravity;
        time_elaspsed = time_elaspsed + t;
    }
    pred.t = currentTime + time_elaspsed;
    pred.x = temp_p.x;
    return pred;
}

void Controller::determineSafeSpots(const Game& g, float t)
{
    for (std::unordered_map<float, float>::iterator it = exp_pred.begin(); it != exp_pred.end(); it++) {
        if (it->second < currentTime + explosion_thres_time) {
            explode(g, it->first);
            it->second = FLT_MAX;
        }
    }
    for (std::list<Explosion>::iterator it = future_explosions.begin(); it != future_explosions.end();) {
        if (!tickExplosion(g, *it, t))
            it = future_explosions.erase(it);
        else
            ++it;
    }
}

void Controller::explode(const Game& g, float x)
{
    const float expSize = g.explosionSize;
    future_explosions.push_back(Explosion());
    Explosion& exp = future_explosions.back();
    exp.time = -explosion_thres_time;
    exp.x = x;
    exp.y = 0;
    for (int i = std::floor(exp.x - expSize - explosion_thres_size);
         i <= std::ceil(exp.x + expSize + explosion_thres_size);
         ++i) {
        if (i >= 0 && i <= g.getWidth())
            dangerZone[i]++;
    }
}

bool Controller::tickExplosion(const Game& g, Explosion& e, float t)
{
    e.time += t;
    if (e.time > g.explosionTime) {
        for (int i = std::floor(e.x - g.explosionSize - explosion_thres_size);
             i <= std::ceil(e.x + g.explosionSize + explosion_thres_size);
             ++i) {
            if (i >= 0 && i <= g.getWidth())
                dangerZone[i]--;
        }
        return false;
    }
    return true;
}

int Controller::pickSafeSpot(const Game& g, float t)
{
    int min_index = std::floor(p->x);
    //int max_index = std::ceil(p->x);
    /*while(dangerZone[min_index] == 1) 
		{
			p->x += g.playerSpeed * t;
			min_index = std::floor(p->x);
		} */
    for (int i = 0; i < g.getWidth() + 1; i++) {
        std::cout << dangerZone[i] << " ";
    }
    std::cout << '\n';
    if (dangerZone[min_index] == 1) {
        int i = min_index, j = min_index;
        while (dangerZone[i] != 0)
            i = i + 3;
        while (dangerZone[j] != 0)
            j = j - 3;
        if (min_index - j < i - min_index)
            p->x -= g.playerSpeed;
        else
            p->x += g.playerSpeed;
        std::cout << p->x << "\n \n \n";
    }
    return 0;
}
}
