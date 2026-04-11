#pragma once
int parse_args(int argc, char** argv, struct PokeAPI &api);
int handle_box(int argc, char* action, char** params, struct PokeAPI &api);
int handle_party(int argc, char* action, char** params, struct PokeAPI &api);
int handle_bag(int argc, char* action, char** params, struct PokeAPI &api);
