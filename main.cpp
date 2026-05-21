#include <ncurses.h>
#include <iostream>
#include <cmath>
using namespace std;

int screen_height = 30;
int screen_width = 80;

float player_x = 8;
float player_y = 8;
float player_a = 0;

int map_height = 16;
int map_width = 16;
float FOV = 3.14159 / 4;
float render_distance = 16;

void initialize_map(string& map){
    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "################";

}

char display_char(float distance_to_wall){
    float normalized = distance_to_wall / render_distance;
    char chars[' ', '.', ':', 'c', 'o', 'P', 'O', '?', '@', 219];
    return chars[(int)(floorf(distance_to_wall  * 10.0f) / 10)];
}


int main(){
    initscr();
    int start_y, start_x;
    start_y = start_x = 10;

    string map;
    initialize_map(map);
    char* screen = new char[screen_height * screen_width];


    WINDOW* win = newwin(screen_height, screen_width, start_y, start_x);
     box(win, 0, 0);
    refresh();
   

    wrefresh(win);

    while (1){
        for (int x = 0; x < screen_width; ++x){
            float ray_angle = (player_a - FOV / 2) + ((float)x / (float)screen_width) * FOV;
            float distance_to_wall = 0;
            bool hit_wall = false;

            float eye_x = sinf(ray_angle);
            float eye_y = cosf(ray_angle);

            while(!hit_wall && distance_to_wall <= render_distance){
                distance_to_wall += 0.1f;
                int ray_x = (int)(player_x + eye_x * distance_to_wall);
                int ray_y = (int)(player_y + eye_y * distance_to_wall);

                if(ray_x < 0 || ray_x > map_width || ray_y < 0 || ray_y > map_height){
                    hit_wall = true;
                    distance_to_wall = render_distance;
                }
                else if (map[ray_y * map_width + ray_x] == '#'){
                    hit_wall = true;
                }
            
            }
            int ceiling = (float)(screen_height / 2.0) - screen_height / ((float)distance_to_wall);
            int floor = screen_height - ceiling;
            for(int y = 0; y < screen_height; y++){
                if(y < ceiling)
                    //screen[y * screen_width + x] = ' ';
                    mvwaddch(win, y, x, ' ');
                else if(y > floor)
                    //screen[y * screen_width + x] = ' ';
                    mvwaddch(win, y, x, ' ');
                else
                    // screen[y * screen_width + x] = '#';
                    mvwaddch(win, y, x, '#');
            }
        }
     
        box(win, 0, 0);
        wrefresh(win);


    }

    endwin();
    return 0;

}

