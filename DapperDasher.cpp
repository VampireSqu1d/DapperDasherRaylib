#include "raylib.h"

struct AnimData {
    Rectangle rec;
    Vector2 position;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeght) {
    return data.position.y >= windowHeght - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame) {
    //update nebula's frame
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime) {
        data.runningTime = 0.0;
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame) {
           data.frame = 0;
        }  
    }
    return data;
}

int main() {
    //window dimensions
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;
    // initialize the window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");

    // acceleration due to gravity (pixels/frame)/frame
    const int gravity{1000};

    //nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int nebulaArrayZise{3};
    AnimData nebulas[nebulaArrayZise];

    for (int i = 0; i < nebulaArrayZise; i++) {
        nebulas[i].rec.width = nebula.width/8;
        nebulas[i].rec.height = nebula.height/8;
        nebulas[i].rec.x = 0.0;
        nebulas[i].rec.y = 0.0; 
        nebulas[i].position.x = windowDimensions[0] + (600 * i);
        nebulas[i].position.y = windowDimensions[1] - nebula.height/8;
        nebulas[i].frame = 0;
        nebulas[i].updateTime = 1.0/16.0;
        nebulas[i].runningTime = 0.0;
    }
    // nebula x velocity
    int nebulaVel{-300};

    float finishLine{nebulas[nebulaArrayZise - 1].position.x};
    
    //scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0.0;
    scarfyData.rec.y = 0.0;
    scarfyData.position.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
    scarfyData.position.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0;
    // is the rectanlge in the air?
    bool isInAir{};
    // jump velocity for scarfy pixels/sec
    const int jumpVel{-600};
    int velocity{0};// vertical velocity for scarfy


    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};

    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float midX{};

    Texture2D foreground = LoadTexture("textures/foreground.png");
    float frX{};

    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        // start drawing
        const float dt{GetFrameTime()};
        BeginDrawing();
        ClearBackground(WHITE);
        

        bgX -= 20 * dt;
        if (bgX <= -background.width*2) {
            bgX = 0.0;
        }
        //draw the BG
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        
        Vector2 bg2Pos{bgX + background.width*2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        midX -= 40 * dt;
        if (midX <= -midground.width*2) {
            midX = 0.0;
        }
        // draw the midground
        Vector2 mg1Pos{midX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        
        Vector2 mg2Pos{midX + midground.width*2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        frX -= 80 * dt;
        if (frX <= -foreground.width*2) {
            frX = 0.0;
        }
        // draw the foregroud
        Vector2 fr1Pos{frX, 0.0};
        DrawTextureEx(foreground, fr1Pos, 0.0, 2.0, WHITE);

        Vector2 fr2Pos{frX + foreground.width*2, 0.0};
        DrawTextureEx(foreground, fr2Pos, 0.0, 2.0, WHITE);

        

        // perform ground check
        if (isOnGround(scarfyData, windowDimensions[1])) {
            // rectangle is on the ground
            velocity = 0;
            isInAir = false;
        }
        else {
            // rectangle is in the air
            velocity += gravity * dt;
            isInAir = true;
        }
        // jump check
        if (IsKeyPressed(KEY_SPACE) && !isInAir) {
            velocity += jumpVel;
        }
        
        for (int i = 0; i < nebulaArrayZise; i++) {
            // update nebula position
            nebulas[i].position.x += nebulaVel * dt;
            if (nebulas[i].position.x <= 0 - nebulas[i].rec.width) {
                nebulas[i].position.x = windowDimensions[0] + (i * 300);
            }
            //update nebula's frame
            nebulas[i] = updateAnimData(nebulas[i], dt, 7);
        }

        //update finsh line position
        finishLine += nebulaVel * dt;
        
        // update scarfy's position
        scarfyData.position.y += velocity * dt;
        //update scarfy's frame
        if (!isInAir) {
            scarfyData = updateAnimData(scarfyData, dt, 5); 
        }
        
        // check for collisions
        float pad{50};
        for(AnimData nebula: nebulas) {

            Rectangle nebRec {
                nebula.position.x + pad,
                nebula.position.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };

            Rectangle scarfyRec{
                scarfyData.position.x,
                scarfyData.position.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };

            //collision = CheckCollisionRecs(nebRec, scarfyRec);
            if (CheckCollisionRecs(nebRec, scarfyRec)) {
                collision = true;
            }
            
        }
        
        if(collision) {
            DrawText("Game Over!", windowDimensions[0]*0.3, windowDimensions[1]*0.5, 40, RED);
        } else if (scarfyData.position.x >= finishLine) {
            DrawText("You Win!", windowDimensions[0]*0.4, windowDimensions[1]*0.5, 40, GREEN);
        } else {
            // draw nebulas
            for (int i = 0; i < nebulaArrayZise; i++) {
                DrawTextureRec(nebula, nebulas[i].rec, nebulas[i].position, WHITE);    
            }

            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.position, RED);
        }
        // stop drawing
        EndDrawing();
    }
    // unload all textures in memory
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(foreground);
    UnloadTexture(midground);
    CloseWindow();
}


