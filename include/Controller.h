#pragma once
#include "Player.h"
#include "Game.h"
#include <map>
#include <vector>
#include <unordered_map>
namespace mrsd
{
	struct Prediction
	{
		float t, x;
	};
	class Controller
	{
		public:
			Controller(): p(0){}
			/*
			 * Deconstructor for the controller class
			 */
			~Controller();
			/*
			 * Control AI based on state of game. "t" time has passed since last update.
			 */
			void control(const Game& g, float t);
			/*
			 * Construct Player and add to game.
			 */
			void createPlayer(Game& g);
		private:
			Player* p;
			float currentTime;
			int *dangerZone;
			int future_duration = 5;
			std::list<Explosion> future_explosions;
			std::unordered_map<float, float> exp_pred;
			float explosion_thres_time = 2.f;
			float explosion_thres_size = 1.f;
			/*
			 * Track projectile and determine where and when it will hit ground.
			 */
			Prediction trackProjectile(const Projectile& p, const Game& g, float t);

			/*
			 * Determine safe spots at time t based on state of all observed projectiles.
			 */
			void determineSafeSpots(const Game& g, float t);

			/*
			 * Pick safe spot from determined safe spots.
			 */
			int pickSafeSpot(const Game& g, float t);

			/*
			 * Function for starting explosion
			 */
			void explode(const Game& g, float x);

			bool tickExplosion(const Game& g, Explosion& e, float t);
	};
}
