#ifndef __UI_H__
#define __UI_H__
#include "circuit.h"
#include "partition.h"
#include <string>
using namespace std;
void ui_init(circuit*, traverser* trav, pnode* (*run_fn)(circuit*,traverser*));
void ui_teardown();

void ui_draw(circuit*, traverser*);
void ui_draw_pnode(pnode* p);
#endif
