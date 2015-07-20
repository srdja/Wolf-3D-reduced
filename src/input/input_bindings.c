#include <SDL2/SDL.h>
#include <stdbool.h>
#include "../game/wolf_player.h"

#include "input.h"
#include "input_bindings.h"
#include "../game/client.h"

void move_fw() {
    player_move(0, 1);
}

void move_fw_stop() {
    player_move(0, -1);
}

void move_bw() {
    player_move(0, -1);
}

void move_bw_stop() {
    player_move(0, 1);
}

void move_sl() {
    player_move(-1, 0);
}

void move_sl_stop() {
    player_move(1, 0);
}

void move_sr() {
    player_move(1, 0);
}

void move_sr_stop()
{
    player_move(-1, 0);
}

void turn_left() {
    player_turn(1);
}

void turn_left_stop() {
    player_turn(0);
}

void turn_right() {
    player_turn(-1);
}

void turn_right_stop() {
    player_turn(0);
}

void use() {
    ClientStatic.player.is_using = true;
}

void use_stop() {
    ClientStatic.player.is_using = false;
}

void attack() {
    ClientStatic.player.is_attacking = true;
}

void attack_stop() {
    ClientStatic.player.is_attacking = false;
}

static ButtonMap *forward;
static ButtonMap *backward;
static ButtonMap *strafe_l;
static ButtonMap *strafe_r;

static ButtonMap *turn_l;
static ButtonMap *turn_r;

static ButtonMap *pl_use;
static ButtonMap *pl_attack;

void input_bindings_init()
{
    InputContext *game = icontext_new(true);

    forward   = button_map_new(SDL_SCANCODE_W, false, move_fw, move_fw_stop);
    backward  = button_map_new(SDL_SCANCODE_S, false, move_bw, move_bw_stop);
    strafe_l  = button_map_new(SDL_SCANCODE_A, false, move_sl, move_sl_stop);
    strafe_r  = button_map_new(SDL_SCANCODE_D, false, move_sr, move_sr_stop);

    turn_l    = button_map_new(SDL_SCANCODE_LEFT,  false, turn_left,  turn_left_stop);
    turn_r    = button_map_new(SDL_SCANCODE_RIGHT, false, turn_right, turn_right_stop);

    pl_use    = button_map_new(SDL_SCANCODE_SPACE, false, use, use_stop);
    pl_attack = button_map_new(SDL_SCANCODE_LCTRL, false, attack, attack_stop);

    icontext_add_key_map(game, forward);
    icontext_add_key_map(game, backward);
    icontext_add_key_map(game, strafe_l);
    icontext_add_key_map(game, strafe_r);
    icontext_add_key_map(game, turn_l);
    icontext_add_key_map(game, turn_r);

    icontext_add_key_map(game, pl_use);
    icontext_add_key_map(game, pl_attack);

    input_add_context(game, "game");
    input_set_context("game");
}
