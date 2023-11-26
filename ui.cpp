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
    init_world(-1.,circ->get_display_width(),circ->get_display_height(),-1.);
    set_keypress_input(true);
    run_fn = cb;
    //set_mouse_move_input(true);

    set_interval(1000000);
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

void ui_draw_cell_fn(circuit* circ, cell* c) {
    double width = 1.;
    double height = width;
    // center at the cells coords
    double x = 0;
    double y = c->y;

    setlinewidth(2);
    drawarc(x,y,CELL_DIAMETER/2.,0.,360.);
    setlinewidth(1);
}

void ui_draw_net_fn(circuit* circ, net* n) {
    // get all pins on net
    // first approach: draw 0 -> 1, 1 -> 2, ... etc
    cell* c1;
    cell* c2;
    string sp = "";
    for (auto& s : n->get_cell_labels()) {
        if ( sp != "" ) {
            c1 = circ->get_cell(s);
            c2 = circ->get_cell(sp);
            double x0 = 0;  //TODO
            double y0 = 0;
            double x1 = 0;
            double y1 = 0;
            drawline(x0, y0, x1, y1);
            #if 0
            char buff[32] = {'\0'};
            snprintf(buff,32,"%s: %f",n->label.c_str(), n->get_weight());
            drawtext((x0+x1)/2., (y0+y1)/2., buff, 10.0);
            #endif
        }
        sp = s;
    }
}

void ui_draw_cells(circuit* circ){
    setcolor(GREEN);
    setlinestyle(SOLID);
    setlinewidth(1);

    circ->foreach_cell( ui_draw_cell_fn );
}

void ui_draw_rats_nest(circuit* circ){
    setcolor(WHITE);
    setlinestyle(DASHED);
    setlinewidth(1);
    circ->foreach_net( ui_draw_net_fn );
}

void ui_draw_pnode(pnode* p) {
    setcolor(GREEN);
    setlinewidth(2);
    drawarc(p->x,p->y,PNODE_DIAMETER/2.,0.,360.);
    setcolor(WHITE);
    setlinewidth(1);
    if(p->parent != nullptr) {
        drawline(p->x, p->y, p->parent->x, p->parent->y);
    }
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


