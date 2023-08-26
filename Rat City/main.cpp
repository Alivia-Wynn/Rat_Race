#include "raylib.h"
#include <cstdlib>
#include <iostream>


struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};
bool isOnGround(AnimData data, int windowHeight){
    return data.pos.y >= (windowHeight-data.rec.height);
}
AnimData updateAnimData(AnimData data, float deltaTime, int max_frame){
    //update running time
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime){
        data.runningTime = 0.0;
        data.rec.x = data.frame* data.rec.width;
            data.frame++;
            if (data.frame> max_frame){
                data.frame = 0;
            }
    }
    return data;
    
}
void infiniteScroll(float& x_coord,int rate,  float deltaTime, Texture2D image){
        x_coord -= rate * deltaTime;
        if (x_coord <= -image.width*2){
            x_coord = 0.0;
        }
        //draw backround
        Vector2 bg1Pos{x_coord, 0.0};
        Vector2 bg2Pos{x_coord + image.width*2, 0.0};
        DrawTextureEx(image, bg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(image, bg2Pos, 0.0, 2.0, WHITE); 
}
int main(){
    // dims
    const int windWidth = 512;
    const int windHeight = 380;

    

    int velocity = 0;

    //acceleration do to gravity
        //(pixels/second) / second
    const int gravity = 1'000;

    bool isInAir = false;

    //jump veloscity
    //pixels/second
    const int jumpVelocity{-600};

    InitWindow(windWidth, windHeight, "Rat Race");

    InitAudioDevice();

    

    Music bg_music = LoadMusicStream("music.mp3");  
    PlayMusicStream(bg_music);
    Sound jump = LoadSound("jump.mp3");

    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midgound = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    float bgX{};
    float mgX{};
    float fgX{};
    bool infinite{false};

    Texture2D nebula = LoadTexture("textures/Walk.png"); 
        //be sure to unload the texture at the end 
   
// createes an array of animData types
    const int sizeOfNebulae {10};
    AnimData nebulae[sizeOfNebulae]{}; 

    for (int i = 0; i < sizeOfNebulae; i++)
    {
       nebulae[i].rec.x = 0.0;
       nebulae[i].rec.y = 0.0;
       nebulae[i].rec.width = nebula.width/4;
       nebulae[i].rec.height = nebula.height;
       nebulae[i].pos.y = windHeight-nebula.height;
       nebulae[i].frame = 0;
       nebulae[i].runningTime = 0;
       nebulae[i].updateTime = 1.0/12.0;
       if (!infinite){
        nebulae[i].pos.x = windWidth + 300* (i + rand() % 2) ;
       }
       else{
        nebulae[i].pos.x = windWidth + 300* (i + 1);
       }

       

    }
    
    float finishLine{ nebulae[sizeOfNebulae-1].pos.x};
    int nebVelocity = -200;


    //Scary stuff
    //make sure to load sprites AFTER initializing the window
    Texture2D scarfy = LoadTexture("textures/scarfy.png"); 
        //be sure to unload the texture at the end 
    AnimData scarfyData;
        scarfyData.rec.width = scarfy.width/6;
        scarfyData.rec.height = scarfy.height;
        scarfyData.rec.x = 0;
        scarfyData.rec.y = 0;
        scarfyData.pos.x = (windWidth/2) - scarfyData.rec.width/2 ;
        scarfyData.pos.y = windHeight - scarfyData.rec.height;
        scarfyData.frame = 0;
        scarfyData.updateTime = 1.0/12.0;
        scarfyData.runningTime = 0.0;
    
    
    //initialize window
    
    SetTargetFPS(60);
    bool collision{false};
    bool music_on{true};
    bool open_text{true};
    bool start{false};
    int score{0};
    bool activate_finish{}; // this is used to start moving the finish line
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        UpdateMusicStream(bg_music);
        
        if(IsKeyPressed(KEY_M)){
            if (music_on){
                PauseMusicStream(bg_music);
                music_on = false;

            }
            else{
                ResumeMusicStream(bg_music);
                music_on = true;
            }
        }
         // delta time (time  since last frame)
        float dt = GetFrameTime();
        infiniteScroll(bgX, 20,  dt, background);
        infiniteScroll(mgX, 40,  dt, midgound);
        infiniteScroll(fgX, 80,  dt, foreground);
        DrawText("press 'M' to turn the music off", windWidth - 200, 20, 10, WHITE);
        DrawText(TextFormat("Score: %i", score),  20, 20, 10, WHITE);
        if (IsKeyPressed(KEY_SPACE)){
            open_text=false;
        }
        if(IsKeyPressed(KEY_I)){
            infinite = true;
            start = true;
        }
        else if (IsKeyPressed(KEY_F)){
            //start the game
            start = true;
            // activate the finish line
            activate_finish = true;
        }
        else if(!start && open_text){
        DrawText("Your city is overrun by rats!\nuse the 'W' key to jump\nuse the 'P' key to fall faster.\npress the space bar to continue", windWidth/4,150, 20, WHITE);
        }
        else if (!start && !open_text){
            DrawText("press 'I' to play infinitely,\npress 'F' to play a quick game", windWidth/4,windHeight/2, 20, WHITE);
        }
        
        if(activate_finish){
            finishLine +=(nebVelocity * dt);
        }
       // DrawText("you lose....", windWidth/4,  windHeight/2, 50, WHITE);        


       

        //ground check
        if (isOnGround(scarfyData, windHeight)){
            //on the ground
            velocity = 0;
            isInAir = false;
        }
        else{
            // in the air
            //apply gravity
            velocity += (gravity *dt);
            isInAir = true;
            
        }

        if (IsKeyPressed(KEY_W) && !isInAir && !collision &&start){
            velocity += jumpVelocity;
            PlaySound(jump);
            score += 10;
            
        }
        if(IsKeyPressed(KEY_P) && isInAir && !collision &&start){
            velocity -= jumpVelocity;
            PlaySound(jump);
        }
        
    

        //update scarfy position
        scarfyData.pos.y+= (velocity * dt);
        
    if (start){    
        for(int i = 0; i<sizeOfNebulae; i++){
            //update position of each nebulus
            nebulae[i].pos.x += (nebVelocity*dt);
            if (infinite){
                if (nebulae[i].pos.x <= -1000 ){
                    nebulae[i].pos.x = windWidth + 300* (i+1);
                    i = 0;
                }
            }

        }
    }    

    //update scarfy animation
    // update running time
    if (isOnGround(scarfyData, windHeight)){
        scarfyData = updateAnimData(scarfyData, dt, 5);
    }
    //update nebula animation
        for (int i=0; i<sizeOfNebulae; i++ ){
            nebulae[i] = updateAnimData(nebulae[i], dt, 3);
            
        }
           
       
        
        float pad{50};
       for (AnimData nebula : nebulae){
        Rectangle nebRec{nebula.pos.x + pad, nebula.pos.y + pad,nebula.rec.width- 2*pad, nebula.rec.height - 2*pad};
        Rectangle scarfyRec{scarfyData.pos.x, scarfyData.pos.y,scarfyData.rec.width, scarfyData.rec.height};
       
        if (CheckCollisionRecs(nebRec, scarfyRec)){
            collision = true;
        }
        
       }
        if (collision){
            DrawText("you lose....", windWidth/4,  windHeight/2, 50, WHITE);

        }
        else if (scarfyData.pos.x >= finishLine){
            DrawText("YOU WIN!", windWidth/4,  windHeight/2, 50, WHITE);

        }
     
        else if (start){

            //draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, {scarfyData.pos.x-20, scarfyData.pos.y}, WHITE);
            for(int i = 0; i<sizeOfNebulae; i++){
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
        }
    
        
       
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midgound);
    UnloadTexture(foreground);
    CloseAudioDevice();
    CloseWindow();


}