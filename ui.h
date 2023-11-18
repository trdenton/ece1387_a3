#ifndef __UI_H__
#define __UI_H__
#include "circuit.h"
#include <string>
using namespace std;
void ui_init(circuit*);
void ui_teardown();

void ui_draw(circuit*);
#endif
