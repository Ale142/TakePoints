#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <signal.h>

#define KEY_SEEN 1
#define KEY_RELEASED 2
#define TIMER 10 // seconds timer

volatile bool end = false;

typedef struct player {
    float x;
    float y;
    float points;
}player;

typedef struct points {
    float x;
    float y;
}points;

bool collide(int ax1, int ay1, int ax2, int ay2,int bx1, int by1, int bx2, int by2) {
    if(ax1 > bx2) return false;
    if(ax2 < bx1) return false;
    if(ay1 > by2) return false;
    if(ay2 < by1) return false; 

    return true;
}

void init(bool test, const char* descr) {
    if(test) return;

    printf("Error %s inizialization\n", descr);
    exit(1);
}


   

void redraw_points(points* points) {
    srand(time(0));

    points->x = 1 + rand() % 630;
    points->y = 1 + rand() % 470;
    al_draw_filled_rectangle(points->x, points->y, points->x + 10, points->y + 10, al_map_rgb(255, 255, 0));

}

void check_player_position(player* player) {
    if(player->x + 20 > 640)
        player->x = 0;
    if(player->y + 20 > 480)
        player->y = 0;
    if(player->x < 0)
        player->x = 640 - 20;
    if(player->y < 0)
        player->y = 480 - 20;
}

void end_game() {
    end = true;
}

int main() {
    init(al_init(), "allegro");
    init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(640,480);
    init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    init(font, "font");

    init(al_init_primitives_addon(), "primitives");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    

    player player;
    player.x = 100;
    player.y = 100;
    player.points = 0;

    points points;


    
    bool done = false;
    bool redraw = true;
    bool ouch = false;
    bool start = true;
    ALLEGRO_EVENT event;

    float x,y;
    x = 100;
    y = 100;

    al_start_timer(timer);

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));
   
    // init
    points.x = 120;
    points.y = 120;
    al_draw_filled_rectangle(points.x, points.y, points.x + 10, points.y + 10, al_map_rgb(255, 255, 0));

    signal(SIGALRM, end_game);
    alarm(TIMER);

    while(!end) {

        al_wait_for_event(queue, &event); 

        switch(event.type) {
            case ALLEGRO_EVENT_TIMER:
             
             if(collide(player.x,player.y,player.x + 20,player.y + 20, points.x,points.y,points.x + 10,points.y+10)) {
                
                if(start) start = false;
                redraw_points(&points);
                ouch = true;
                player.points += 1;
            }
               
                if(key[ALLEGRO_KEY_UP]) 
                    player.y -= 2 + 0.1 * player.points;
                    
                if(key[ALLEGRO_KEY_DOWN]) 
                    player.y += 2 + 0.1 * player.points;
                    
                if(key[ALLEGRO_KEY_LEFT]) 
                    player.x -= 2 + 0.1 * player.points;
                    
                if(key[ALLEGRO_KEY_RIGHT]) 
                    player.x += 2 + 0.1 * player.points;
                    
                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;
                for(int i = 0; i < ALLEGRO_KEY_MAX; i++) 
                    key[i] &= KEY_SEEN;
                

            redraw = true;
            break;
            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
           
        }
        
        if(done) break;

        if(redraw && al_is_event_queue_empty(queue)) {

            check_player_position(&player);
        
            al_clear_to_color(al_map_rgb(0,0,0));
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %.1f Y: %.1f POINTS:%.1f TIMER:%f", player.x, player.y,player.points,al_get_time());
            al_draw_filled_rectangle(player.x, player.y, player.x +20, player.y + 20, al_map_rgb(255, 0, 0));
            al_draw_filled_rectangle(points.x, points.y, points.x + 10, points.y + 10, al_map_rgb(255, 255, 0));

            al_flip_display();

            redraw = false;
        }

    }

    // done = false;
    // al_flush_event_queue(queue);

    // ALLEGRO_EVENT_QUEUE* queue_2 = al_create_event_queue();
    // init(queue_2, "queue");

    // al_register_event_source(queue_2, al_get_keyboard_event_source());
    // al_register_event_source(queue_2, al_get_display_event_source(disp));
    // al_register_event_source(queue_2, al_get_timer_event_source(timer));
    // while(1) {
    //     al_wait_for_event(queue_2, &event); 
    //     switch(event.type) {
    //         case ALLEGRO_EVENT_TIMER:
    //             if(key[ALLEGRO_KEY_ESCAPE])
    //                 done = true;
    //             break;
    //         case ALLEGRO_EVENT_DISPLAY_CLOSE:
    //             done = true;
    //             break;
    //     }
    //     if(done) break;

    //     if(al_is_event_queue_empty(queue_2)) {
    //         al_clear_to_color(al_map_rgb(0,0,0));
    //         al_draw_textf(font, al_map_rgb(255,255,255),320,240,0, "END GAME. TOTAL POINTS : %.1f \n PRESS ANY KET TO QUIT", player.points);
    //     }


    // }


    al_destroy_display(disp);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;

}
