#include <ncurses.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <vector>
using namespace std;

int screen_height = 30 * 1.5f;
int screen_width = 80 * 2.0f;

float player_x = 2;
float player_y = 1;
float player_a = 0;

int map_height = 16;
int map_width = 16;
float FOV = 3.14159 / 4.0f;
float render_distance = 16;


struct Enemy {
    float x;
    float y;
    char symbol;      
    int state;        
    float speed;
    bool is_alive;
};

void initialize_map(string& map){
    map += "################";
    map += "#.........#....#";
    map += "########..#....#";
    map += "#.........#....#";
    map += "#..#...#.......#";
    map += "#..#...#########";
    map += "#..#...######..#";
    map += "####...........#";
    map += "#..########....#";
    map += "#.........#....#";
    map += "#....######....#";
    map += "#..............#";
    map += "###########....#";
    map += "#X.....####....#";
    map += "###............#";
    map += "################";

}

char wall_char(float distance_to_wall){
    int index = (int)((distance_to_wall / render_distance) * 10.0f);
    
   
    if (index < 0) index = 0;
    if (index > 9) index = 9;
    
   
    char chars[] = {'$', '@', '%', 'O', 'P', 'o', 'c', ':', '.', ' '};
    return chars[index];
}
char floor_char(float distance_to_wall){
    int index = (int)((distance_to_wall / render_distance) * 3.0f);
    
   
    if (index < 0) index = 0;
    if (index > 2) index = 2;
    
   
    char chars[] = {'\'', '.', '-'};
    return chars[index];
}


int main(){
    initscr(); 
    noecho(); 
    cbreak();             
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK); 
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    vector<Enemy> enemies;
    enemies.push_back({11.0f, 11.0f, 'D', 0, 1.5f, true});
    int start_y, start_x;
    start_y = start_x = 10;

    string map;
    initialize_map(map);
    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();


    char* screen = new char[screen_height * screen_width];


    WINDOW* win = newwin(screen_height, screen_width, start_y, start_x);
    keypad(win, TRUE);
    box(win, 0, 0);
    
    refresh();
   

    wrefresh(win);
    float* depth_buffer = new float[screen_width];
    while (1){
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsed = tp2 - tp1;
        tp1 = tp2;
        float delta = elapsed.count();

        int choice = wgetch(win);
        switch(choice){
            case KEY_UP:
                player_y += cosf(player_a) * delta * 2.0f;
                player_x += sinf(player_a) * delta * 2.0f;
                if(map[(int)player_y * map_width + (int)player_x] == '#'){
                    player_y -= cosf(player_a) * delta * 2.0f;
                    player_x -= sinf(player_a) * delta * 2.0f;
                }
                break;
            case KEY_DOWN:
                player_y -= cosf(player_a) * delta * 2.0f;
                player_x -= sinf(player_a) * delta * 2.0f;
                if(map[(int)player_y * map_width + (int)player_x] == '#'){
                    player_y += cosf(player_a) * delta * 2.0f;
                    player_x += sinf(player_a) * delta * 2.0f;
                }
                break;
            case KEY_RIGHT:
                player_a += 0.2f * delta;
                break;
            case KEY_LEFT:
                player_a -= 0.2f * delta;
                break;

        }   
        wattron(win, COLOR_PAIR(2));
        
        for (int x = 0; x < screen_width; ++x){
            float ray_angle = (player_a - FOV / 2) + ((float)x / (float)screen_width) * FOV;
            float distance_to_wall = 0;
            bool hit_wall = false;

            float eye_x = sinf(ray_angle);
            float eye_y = cosf(ray_angle);

            while(!hit_wall && distance_to_wall <= render_distance){
                distance_to_wall += 0.03f;
                int ray_x = (int)(player_x + eye_x * distance_to_wall);
                int ray_y = (int)(player_y + eye_y * distance_to_wall);

                if(ray_x < 0 || ray_x >= map_width || ray_y < 0 || ray_y >= map_height){
                    hit_wall = true;
                    distance_to_wall = render_distance;
                }
                else if (map[ray_y * map_width + ray_x] == '#'){
                    hit_wall = true;
                }
            
            }
            int ceiling = (float)(screen_height / 2.0) - screen_height / ((float)distance_to_wall);
            int floor = screen_height - ceiling;
            depth_buffer[x] = distance_to_wall;
            for(int y = 0; y < screen_height; y++){
                if(y < ceiling){
             
                    mvwaddch(win, y, x, ' ');
                }
                else if(y > floor){
                    
                    mvwaddch(win, y, x,  floor_char(distance_to_wall));
                }
                else{
                
                    mvwaddch(win, y, x, wall_char(distance_to_wall));
                }
            }
        }
        //enemy rendering?
        wattron(win, COLOR_PAIR(1));
        for(const Enemy& enemy : enemies){
            float offset_x = enemy.x - player_x;
            float offset_y = enemy.y - player_y;
            float distance_to_enemy = sqrtf(offset_x * offset_x + offset_y * offset_y);

            float enemy_angle = atan2f(offset_y, offset_x) - player_a;
            
            
            if (enemy_angle < -3.14159f) enemy_angle += 2.0f * 3.14159f;
            if (enemy_angle >  3.14159f) enemy_angle -= 2.0f * 3.14159f;
            
       
            bool is_in_fov = fabsf(enemy_angle) < (FOV / 2.0f);
            if (is_in_fov && distance_to_enemy > 0.5f && distance_to_enemy < render_distance) {
        
         
                int enemy_screen_x = (int)((screen_width / 2.0f) + (enemy_angle / FOV) * screen_width);
                
                
                int sprite_size = (int)(screen_height / distance_to_enemy);
                int ceiling = (screen_height / 2) - (sprite_size);
                int floor = (screen_height / 2) + (sprite_size);
                
                for (int sx = enemy_screen_x - (sprite_size / 2); sx < enemy_screen_x + (sprite_size / 2); ++sx) {
            
                    if (sx >= 0 && sx < screen_width && depth_buffer[sx] > distance_to_enemy) {
                        for (int sy = ceiling; sy < floor; ++sy) {
                            if (sy >= 0 && sy < screen_height) {
                                mvwaddch(win, sy, sx, enemy.symbol);
                            }
                        }
                    }
                }
            }
            // mvwprintw(win, screen_height - 1, screen_width - 10, to_string(distance_to_enemy).c_str());
        }


        // print map:
        for(int row = 0; row < map_height; ++row){
            for(int col = 0; col < map_width; ++col){
                mvwaddch(win, row, col, map[row * map_width + col]);
            }
            
        }
        for(Enemy& enemy : enemies){
            mvwaddch(win, enemy.y, enemy.x, enemy.symbol);
        }
        mvwaddch(win, player_y, player_x, '@');


       
        box(win, 0, 0);
        wrefresh(win);


    }

    endwin();
    return 0;

}

