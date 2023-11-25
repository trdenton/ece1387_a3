#ifndef __UI_H__
#define __UI_H__
#include "circuit.h"
#include "partition.h"
#include <string>
using namespace std;
void ui_init(circuit*, traverser* trav);
void ui_teardown();

void ui_draw(circuit*, traverser*);
#endif
