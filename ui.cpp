#include <stdio.h>
#include <string>
#include "easygl/graphics.h"
#include "spdlog/spdlog.h"
#include "circuit.h"
#include "ui.h"
#include <unistd.h>
#include <sys/time.h>
using namespace std;

// Callbacks for event-driven window handling.
void ui_drawscreen();
void ui_click_handler (float x, float y);
void ui_mouse_handler (float x, float y);
void ui_key_handler(char c);

float logic_cell_width = 10.0;

circuit* circ;
traverser* trav;

pnode* (*run_fn)(circuit*, traverser*);

bool draw_cells = true;
bool draw_rats_nest = true;


void ui_toggle_rat(void(*draw)()) {
    draw_rats_nest = !draw_rats_nest;
    draw();
}

void ui_toggle_cell(void(*draw)()) {
    draw_cells = !draw_cells;
    draw();
}

void ui_init(circuit* c, traverser* t, pnode* (*cb)(circuit*, traverser*)) {
    circ = c;
    trav = t;
    spdlog::info("Init UI");
    init_graphics("A1", BLACK);
    create_button("Proceed","TOGGLE RAT", ui_toggle_rat);
    create_button("TOGGLE RAT","TOGGLE CELL", ui_toggle_cell);
    init_world(0.,circ->get_display_width(),circ->get_display_height(),0.);
    set_keypress_input(true);
    run_fn = cb;
    //set_mouse_move_input(true);

    set_interval(200000);
    event_loop(ui_click_handler, ui_mouse_handler, ui_key_handler, ui_drawscreen);
}

void ui_teardown() {
    close_graphics ();
}

void ui_drawscreen() {
    set_draw_mode (DRAW_NORMAL);  // Should set this if your program does any XOR drawing in callbacks.
    pnode* p = run_fn(circ, trav);
    //if (p != nullptr)
    //    ui_draw_pnode(p);
    ui_draw(circ, trav);
}

void ui_click_handler (float x, float y) {
	spdlog::debug("user clicked at {},{}",x,y);
}

void ui_mouse_handler (float x, float y) {
	spdlog::debug("mouse move {},{}",x,y);
}

void ui_key_handler(char c) {
	spdlog::debug("keypress {}",c);
    if (c=='n') {
        spdlog::debug("NEXT",c);
        clearscreen();
        ui_drawscreen();
    }
}

void ui_draw_pnode(pnode* p) {
    setcolor(GREEN);
    setlinewidth(2);
    drawarc(p->x,p->y,PNODE_DIAMETER,0.,360.);
    setcolor(WHITE);
    setlinewidth(1);
    if(p->parent != nullptr) {
        drawline(p->x, p->y, p->parent->x, p->parent->y);
    }
    char buf[32] = {0};
    snprintf(buf,32,"%d [%d]",p->level, p->p->cost());
    drawtext(p->x, p->y, buf, 2*PNODE_DIAMETER);
}

void ui_draw_traverser(traverser* t) {
    for(auto& it : t->pnodes) {
        ui_draw_pnode(it);
    }
}

void ui_draw(circuit* circ, traverser* t) {
    static int count = 0;
    count++;
    //if (count%100 == 0)
        clearscreen();
    ui_draw_traverser(t);
}


