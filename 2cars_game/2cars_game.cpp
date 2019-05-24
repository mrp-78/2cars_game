// 2cars_game.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "SBDL.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <queue>
#include <fstream>

struct point_car {
	int x, y;
	char location;
};
struct obstacle {
	int line, type, x;
	double y;
};
using namespace std;

Texture background, red_car, blue_car, name_texture, programmer_texture, play0, music, sound, music_disabled, sound_disabled, pause, score_texture , score_texture2;
Texture red_rectangle, blue_rectangle, red_circle, blue_circle, play, home , gameover_texture , scoretext_texture , besttext_texture , try_again , pausetext_texture;
Texture best_score_texture;
Music *musicbkg;
Sound *score_sound, *die1_sound, *die2_sound;
Font *font, *font1, *font2 , *font3;
const int window_width = 438, window_height = 780;
int wait1, wait2, score, best_score;
int angle_blue, angle_red, angle_change1 = -4, angle_change2 = 4, show_num_b, show_num_r, wait = 700, wait_pause;
double v_ob = 2, wait_ob_r, wait_ob_b;
bool blue_go_left, blue_go_right, red_go_left, red_go_right, start_menue = true, play_music = true, play_sound = true;
bool pause_menu , game_over , paus , show_b = true , show_r = true , v_change;
obstacle ob_r, ob_b ;
point_car red, blue;
deque <obstacle> redobstacles;
deque <obstacle> blueobstacles;
SDL_Rect bluecar_rect, redcar_rect , red_ob_rect , blue_ob_rect;
ofstream best_score_w;
ifstream best_score_r;

void load_assets();
void go_left(int &x, int &angle, int check, bool &go);
void go_right(int &x, int &angle, int check, bool &go);
void left_key();
void right_key();
void start_menu();
void pause_menue();
void game_over_menu();
void red_obstacle();
void blue_obstacle();
void intersections();
void show_last_ob(int size, obstacle last, Texture circle, Texture rectangle, bool &show);

int main(int argc, char *argv[])
{
	SBDL::InitEngine("2 Cars", window_width, window_height);
	const int FPS = 200; //frame per second
	const int delay = 1000 / FPS; //delay we need at each frame

	srand(time(NULL));
	load_assets();
	blue = { window_width - blue_car.width - 21, window_height - blue_car.height - 70 , 'R' };
	red = { 21 , window_height - red_car.height - 70 ,'L' };
	name_texture = SBDL::createFontTexture(font, "2 CARS", 250, 250, 250);
	gameover_texture = SBDL::createFontTexture(font, "Game Over", 250, 250, 250);
	pausetext_texture = SBDL::createFontTexture(font, "Pause", 250, 250, 250);
	programmer_texture = SBDL::createFontTexture(font1, "programmer : MRP", 250, 250, 250);
	score_texture = SBDL::createFontTexture(font2, to_string(score), 250, 250, 250);
	scoretext_texture = SBDL::createFontTexture(font3, "score", 250, 250, 250);
	besttext_texture = SBDL::createFontTexture(font3, "best", 250, 250, 250);
	
	SBDL::playMusic(musicbkg, -1);
	while (SBDL::isRunning()) {
		int startTime = SBDL::getTime();
		SBDL::updateEvents();
		SBDL::clearRenderScreen();

		SBDL::showTexture(background, 0, 0);
		SBDL::showTexture(red_car, red.x, red.y, angle_red);
		SBDL::showTexture(blue_car, blue.x, blue.y, angle_blue);
		if (start_menue)
			start_menu();
		else if (game_over) {
			game_over_menu();
		}
		else if (pause_menu)
			pause_menue();
		else {
			SDL_Rect pause_rect = { window_width - 43 , 15 , 28 , 42 };
			SBDL::showTexture(background, 0, 0);
			red_obstacle();
			blue_obstacle();
			SBDL::showTexture(red_car, red.x, red.y, angle_red);
			SBDL::showTexture(blue_car, blue.x, blue.y, angle_blue);
			SBDL::showTexture(pause, window_width - pause.width - 15, 15);
			SBDL::freeTexture(score_texture);
			score_texture = SBDL::createFontTexture(font2, to_string(score), 250, 250, 250);
			SBDL::showTexture(score_texture, 10, 5);
			if (SBDL::key(SDL_SCANCODE_RIGHT) && wait1 <= 0)
				right_key();
			if (SBDL::key(SDL_SCANCODE_LEFT) && wait2 <= 0)
				left_key();
			if ((SBDL::key(SDL_SCANCODE_SPACE) || (SBDL::mouseInRect(pause_rect) && SBDL::Mouse.clicked())) && wait_pause <= 0) {
				wait_pause = 300;
				pause_menu = !pause_menu;
			}
			if (blue_go_left)
				go_left(blue.x, angle_blue, window_width - blue_car.width - 134, blue_go_left);
			if (red_go_left)
				go_left(red.x, angle_red, 21, red_go_left);
			if (blue_go_right)
				go_right(blue.x, angle_blue, window_width - blue_car.width - 21, blue_go_right);
			if (red_go_right)
				go_right(red.x, angle_red, 134, red_go_right);
			bluecar_rect = { blue.x + 10, blue.y + 5, blue_car.width - 15, blue_car.height - 10};
			redcar_rect = { red.x + 10 , red.y + 5, red_car.width - 15, red_car.height - 10};
			intersections();
		}
		SBDL::updateRenderScreen();
		int elapsedTime = SBDL::getTime() - startTime;
		if (score % 5 == 0 && v_change) {
			v_change = false;
			v_ob += 0.05;
		}
		if (elapsedTime < delay)
			SBDL::delay(delay - elapsedTime);
		if (wait1 > 0)
			wait1 -= delay - elapsedTime;
		if (wait2 > 0)
			wait2 -= delay - elapsedTime;
		if (wait_pause > 0)
			wait_pause -= delay - elapsedTime;
		if (wait_ob_r > 0 && !pause_menu && !paus)
			wait_ob_r -= 2 * v_ob;
		if (wait_ob_b > 0 && !pause_menu && !paus)
			wait_ob_b -= 2 * v_ob;
		if (paus && show_num_b > 1) {
			show_num_b--;
			if (show_num_b % 50 == 0)
				show_b = !show_b;
			if (show_num_b == 1) {
				paus = false;
				game_over = true;
			}
		}
		if (paus && show_num_r > 1) {
			show_num_r--;
			if (show_num_r % 50 == 0)
				show_r = !show_r;
			if (show_num_r == 1) {
				paus = false;
				game_over = true;
			}
		}
	}
}

void load_assets() {
	background = SBDL::loadTexture("assets/Background2.png");
	red_car = SBDL::loadTexture("assets/Cars/RedCar.png");
	blue_car = SBDL::loadTexture("assets/Cars/BlueCar.png");
	musicbkg = SBDL::loadMusic("assets/Sounds/music.wav");
	font = SBDL::loadFont("assets/Font/gobold.ttf" , 70);
	font1 = SBDL::loadFont("assets/Font/gobold.ttf", 15);
	font2 = SBDL::loadFont("assets/Font/gobold.ttf", 45);
	font3 = SBDL::loadFont("assets/Font/gobold.ttf", 35);
	play0 = SBDL::loadTexture("assets/Button/Play0.png");
	play = SBDL::loadTexture("assets/Button/Play.png");
	home = SBDL::loadTexture("assets/Button/Home.png");
	music = SBDL::loadTexture("assets/Button/Music.png");
	sound = SBDL::loadTexture("assets/Button/Sound.png");
	music_disabled = SBDL::loadTexture("assets/Button/MusicDisabled.png");
	sound_disabled = SBDL::loadTexture("assets/Button/SoundDisabled.png");
	pause = SBDL::loadTexture("assets/Pause.png");
	blue_circle = SBDL::loadTexture("assets/Obstacle/BlueCircle1.png");
	red_circle = SBDL::loadTexture("assets/Obstacle/RedCircle1.png");
	blue_rectangle = SBDL::loadTexture("assets/Obstacle/BlueRectangle1.png");
	red_rectangle = SBDL::loadTexture("assets/Obstacle/RedRectangle1.png");
	try_again = SBDL::loadTexture("assets/TryAgain.png");
	score_sound = SBDL::loadSound("assets/Sounds/score.wav");
	die1_sound = SBDL::loadSound("assets/Sounds/die1.wav");
	die2_sound = SBDL::loadSound("assets/Sounds/die2.wav");
}

void go_left(int &x, int &angle, int check, bool &go ) {
	if (!paus) {
		if (x <= check) {
			x = check;
			angle = 0;
			angle_change1 = -1;
			go = false;
		}
		else {
			x -= 2;
			if (angle <= -32)
				angle_change1 = 1;
			if (angle > 0)
				angle = 0;
			angle += angle_change1;
		}
	}
}

void go_right(int &x, int &angle, int check, bool &go) {
	if (!paus) {
		if (x >= check) {
			x = check;
			angle_change2 = 1;
			angle = 0;
			go = false;
		}
		else {
			x += 2;
			if (angle >= 32)
				angle_change2 = -1;
			if (angle < 0)
				angle = 0;
			angle += angle_change2;
		}
	}
}

void right_key() {
	wait1 = 300;
	if (blue.location == 'R') {
		blue_go_left = true;
		blue.location = 'L';
	}
	else {
		blue_go_right = true;
		blue.location = 'R';
	}
}

void left_key() {
	wait2 = 300;
	if (red.location == 'L') {
		red_go_right = true;
		red.location = 'R';
	}
	else {
		red_go_left = true;
		red.location = 'L';
	}
}

void start_menu(){
	SDL_Rect rec = { 0 , 0 , window_width , window_height };
	SDL_Rect play0_rect = { (window_width / 2) - (play0.width / 2), (window_height / 2) - (play0.height / 2) - 50 , play0.width , play0.height };
	SDL_Rect music_rect = { (window_width / 2) - (music.width) - 10, (window_height / 2) + (play0.height / 2) - 20 , music.width , music.height };
	SDL_Rect sound_rect = { (window_width / 2) + 10, (window_height / 2) + (play0.height / 2) - 20 , sound.width , sound.height };
	SBDL::showTexture(pause, window_width - pause.width - 15, 15);
	SBDL::showTexture(score_texture, 10, 5);
	SBDL::drawRectangle(rec , 0 , 0 , 0 , 200);
	SBDL::showTexture(name_texture, (window_width / 2) - (name_texture.width / 2), 150);
	SBDL::showTexture(play0, play0_rect);
	SBDL::showTexture(programmer_texture, (window_width / 2) - (programmer_texture.width / 2), 550);

	if (play_music)
		SBDL::showTexture(music, music_rect);
	else
		SBDL::showTexture(music_disabled, music_rect);
	if (play_sound)
		SBDL::showTexture(sound, sound_rect);
	else
		SBDL::showTexture(sound_disabled, sound_rect);

	if (SBDL::key(SDL_SCANCODE_SPACE) || (SBDL::mouseInRect(play0_rect) && SBDL::Mouse.clicked()) && wait_pause <= 0) {
		v_ob = 1.7; 
		wait_pause = 300;
		start_menue = false;
	}
	if (SBDL::mouseInRect(music_rect) && SBDL::Mouse.clicked()) {
		if (play_music) {
			SBDL::stopMusic();
			play_music = false;
		}
		else {
			SBDL::playMusic(musicbkg, -1);
			play_music = true;
		}
	}
	if (SBDL::mouseInRect(sound_rect) && SBDL::Mouse.clicked()) {
		if (play_sound) {
			SBDL::stopAllSounds();
			play_sound = false;
		}
		else
			play_sound = true;
	}
}

void pause_menue() {
	SDL_Rect rec = { 0 , 0 , window_width , window_height };
	SDL_Rect home_rect = { (window_width / 2) - (home.width/2) , (window_height / 2) + 55 , home.width , home.height };
	SDL_Rect play_rect = { (window_width / 2) - (play0.width/2) , (window_height / 2) - (play0.height) , play0.width , play0.height };
	SDL_Rect music_rect = { (window_width / 2) - 105 , (window_height / 2) + 55 , music.width , music.height };
	SDL_Rect sound_rect = { (window_width / 2) + 45 , (window_height / 2) + 55 , sound.width , sound.height };
	red_obstacle();
	blue_obstacle();
	SBDL::showTexture(pause, window_width - pause.width - 15, 15);
	SBDL::showTexture(score_texture, 10, 5);
	SBDL::drawRectangle(rec, 0, 0, 0, 200);
	SBDL::showTexture(home, home_rect);
	SBDL::showTexture(play0, play_rect);
	SBDL::showTexture(pausetext_texture, (window_width / 2) - (pausetext_texture.width / 2), 100);
	if ((SBDL::key(SDL_SCANCODE_SPACE)) && wait_pause <= 0) {
		wait_pause = 300;
		pause_menu = !pause_menu;
	}
	if (SBDL::mouseInRect(play_rect) && SBDL::Mouse.clicked()) {
		wait_pause = 300;
		pause_menu = !pause_menu;
	}
	if (SBDL::mouseInRect(home_rect) && SBDL::Mouse.clicked()) {
		start_menue = true;
		score = 0;
		redobstacles.clear();
		blueobstacles.clear();
		wait_ob_b = 0;
		wait_ob_r = 0;
		pause_menu = false;
	}
	if (play_music)
		SBDL::showTexture(music, music_rect);
	else
		SBDL::showTexture(music_disabled, music_rect);
	if (play_sound)
		SBDL::showTexture(sound, sound_rect);
	else
		SBDL::showTexture(sound_disabled, sound_rect);
	if (SBDL::mouseInRect(music_rect) && SBDL::Mouse.clicked()) {
		if (play_music) {
			SBDL::stopMusic();
			play_music = false;
		}
		else {
			SBDL::playMusic(musicbkg, -1);
			play_music = true;
		}
	}
	if (SBDL::mouseInRect(sound_rect) && SBDL::Mouse.clicked()) {
		if (play_sound) {
			SBDL::stopAllSounds();
			play_sound = false;
		}
		else
			play_sound = true;
	}
}

void game_over_menu() {
	best_score_r.open("best_scor.txt");
	best_score_r >> best_score;
	best_score_r.close();
	if (score > best_score) {
		best_score = score;
		best_score_w.open("best_scor.txt");
		best_score_w << best_score << endl;
		best_score_w.close();
		best_score_texture = SBDL::createFontTexture(font3, to_string(best_score), 250, 250, 250);

	}
	pause_menu = true;
	SDL_Rect rec = { 0 , 0 , window_width , window_height };
	SDL_Rect tryagain_rect = { (window_width / 2) - (try_again.width / 2), 340 , try_again.width , try_again.height };
	SDL_Rect home_rect = { (window_width / 2) - (home.width / 2), 485 , home.width , home.height };
	SDL_Rect music_rect = { (window_width / 2) - 105 , 485 , music.width , music.height };
	SDL_Rect sound_rect = { (window_width / 2) + 45 , 485 , sound.width , sound.height };
	red_obstacle();
	blue_obstacle();
	SBDL::showTexture(pause, window_width - pause.width - 15, 15);
	SBDL::showTexture(score_texture, 10, 5);
	SBDL::drawRectangle(rec, 0, 0, 0, 200);
	SBDL::showTexture(gameover_texture, (window_width / 2) - (gameover_texture.width / 2), 80);
	SBDL::showTexture(scoretext_texture, (window_width / 2) - (scoretext_texture.width) - 15, 200);
	SBDL::showTexture(besttext_texture, (window_width / 2) - (scoretext_texture.width) - 15, 255);
	SBDL::showTexture(try_again, tryagain_rect);
	SBDL::showTexture(home, home_rect);
	SBDL::freeTexture(score_texture2);
	score_texture2 = SBDL::createFontTexture(font3, to_string(score), 250, 250, 250);
	SBDL::showTexture(score_texture2, (window_width) - score_texture2.width - 120, 200);
	SBDL::freeTexture(best_score_texture);
	best_score_texture = SBDL::createFontTexture(font3, to_string(best_score), 250, 250, 250);
	SBDL::showTexture(best_score_texture, (window_width) - best_score_texture.width - 120, 255);
	if (play_music)
		SBDL::showTexture(music, music_rect);
	else
		SBDL::showTexture(music_disabled, music_rect);
	if (play_sound)
		SBDL::showTexture(sound, sound_rect);
	else
		SBDL::showTexture(sound_disabled, sound_rect);
	if (SBDL::key(SDL_SCANCODE_SPACE) || (SBDL::mouseInRect(tryagain_rect) && SBDL::Mouse.clicked()) && wait_pause <= 0) {
		wait_pause = 300;
		score = 0;
		redobstacles.clear();
		blueobstacles.clear();
		wait_ob_b = 0;
		wait_ob_r = 0;
		v_ob = 1.7;
		pause_menu = false;
		game_over = false;
	}
	if (SBDL::mouseInRect(home_rect) && SBDL::Mouse.clicked()) {
		start_menue = true;
		score = 0;
		redobstacles.clear();
		blueobstacles.clear();
		wait_ob_b = 0;
		wait_ob_r = 0;
		v_ob = 1.7;
		pause_menu = false;
		game_over = false;
	}
	if (SBDL::mouseInRect(music_rect) && SBDL::Mouse.clicked()) {
		if (play_music) {
			SBDL::stopMusic();
			play_music = false;
		}
		else {
			SBDL::playMusic(musicbkg, -1);
			play_music = true;
		}
	}
	if (SBDL::mouseInRect(sound_rect) && SBDL::Mouse.clicked()) {
		if (play_sound) {
			SBDL::stopAllSounds();
			play_sound = false;
		}
		else
			play_sound = true;
	}
}

void red_obstacle() {
	if (wait_ob_r <= 0) {
		ob_r = { rand() % 2 , rand() % 2 , 26 , -50 };
		if (ob_r.line == 1)
			ob_r.x = 139;
		redobstacles.push_back(ob_r);
		wait_ob_r = wait ;
	}
	if (redobstacles.front().y  > 780)
		redobstacles.pop_front();
	show_last_ob(redobstacles.size(), redobstacles[0] , red_circle , red_rectangle , show_r);
	for (int i = 1; i < redobstacles.size(); i++) {
		if (redobstacles[i].line == 0) {
			if (redobstacles[i].type == 0)
				SBDL::showTexture(red_circle, 26, redobstacles[i].y);
			else
				SBDL::showTexture(red_rectangle, 26, redobstacles[i].y);
		}
		else {
			if (redobstacles[i].type == 0) 
				SBDL::showTexture(red_circle, 139, redobstacles[i].y);
			else 
				SBDL::showTexture(red_rectangle, 139, redobstacles[i].y);
		}
	}
	if (!pause_menu && !paus)
		for (int i = 0; i < redobstacles.size(); i++)
			redobstacles[i].y += v_ob;
	red_ob_rect = { redobstacles[0].x  , int(redobstacles[0].y) , 50 , 50 };
}

void blue_obstacle() {
	if (wait_ob_b <= 0) {
		ob_b = { rand() % 2 , rand() % 2 , window_width - 76 , -50 };
		if (ob_b.line == 1)
			ob_b.x = window_width - 189;
		blueobstacles.push_back(ob_b);
		wait_ob_b = wait + 1;
	}
	if (blueobstacles.front().y > 780)
		blueobstacles.pop_front();
	show_last_ob(blueobstacles.size(), blueobstacles[0] , blue_circle , blue_rectangle , show_b);
	for (int i = 1; i < blueobstacles.size(); i++) {
		if (blueobstacles[i].line == 0) {
			if (blueobstacles[i].type == 0) 
				SBDL::showTexture(blue_circle, window_width - 76, blueobstacles[i].y);
			else 
				SBDL::showTexture(blue_rectangle, window_width - 76, blueobstacles[i].y);
		}
		else {
			if (blueobstacles[i].type == 0) 
				SBDL::showTexture(blue_circle, window_width - 189, blueobstacles[i].y);
			else 
				SBDL::showTexture(blue_rectangle, window_width - 189, blueobstacles[i].y);
		}
	}
	if (!pause_menu && !paus)
		for (int i = 0; i < blueobstacles.size(); i++)
			blueobstacles[i].y += v_ob;
	blue_ob_rect = { blueobstacles[0].x , int(blueobstacles[0].y) , 50 , 50 };
}

void intersections() {
	if (SBDL::hasIntersectionRect(redcar_rect, red_ob_rect) && !paus) {
		if (redobstacles[0].type == 0) {
			score++;
			v_change = true;
			redobstacles.pop_front();
			if (play_sound)
				SBDL::playSound(score_sound, 1);
		}
		else{
			paus = true;
			show_num_r = 250;
			if (play_sound)
				SBDL::playSound(die1_sound, 1);
		}
	}
	if (redobstacles[0].type == 0)
		if (redobstacles[0].y >= (window_height - 50) && !paus) {
			paus = true;
			show_num_r = 250;
			if (play_sound)
				SBDL::playSound(die2_sound, 1);
		}
	if (SBDL::hasIntersectionRect(bluecar_rect, blue_ob_rect) && !paus) {
		if (blueobstacles[0].type == 0) {
			score++;
			v_change = true;
			blueobstacles.pop_front();
			if (play_sound)
				SBDL::playSound(score_sound, 1);
		}
		else {
			paus = true;
			show_num_b = 250;
			if (play_sound)
				SBDL::playSound(die1_sound, 1);
		}
	}
	if (blueobstacles[0].type == 0)
		if (blueobstacles[0].y >= (window_height - 50) && !paus){
			paus = true;
			show_num_b = 250;
			if (play_sound)
				SBDL::playSound(die2_sound, 1);
		}
}

void show_last_ob(int size, obstacle last , Texture circle , Texture rectangle , bool &show) {
	if (size > 0) {
		if (last.line == 0) {
			if (last.type == 0) {
				if (paus) {
					if (show)
						SBDL::showTexture(circle, last.x, last.y);
				}
				else
					SBDL::showTexture(circle, last.x, last.y);
			}
			else {
				if (paus) {
					if (show)
						SBDL::showTexture(rectangle, last.x, last.y);
				}
				else
					SBDL::showTexture(rectangle, last.x, last.y);
			}
		}
		else {
			if (last.type == 0) {
				if (paus) {
					if (show)
						SBDL::showTexture(circle, last.x, last.y);
				}
				else
					SBDL::showTexture(circle, last.x, last.y);
			}
			else {
				if (paus) {
					if (show)
						SBDL::showTexture(rectangle, last.x, last.y);
				}
				else
					SBDL::showTexture(rectangle, last.x, last.y);
			}
		}
	}
}
