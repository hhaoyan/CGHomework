//
//  main.cpp
//
//  Created by Haoyan Huo on 3/16/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "../demos/FresnelShading.h"


int main(int argc, char** argv){
    GLApplication * app = new FresnelShading();
    app->CreateWindow("Demo", 800, 600);
    app->RunMainLoop();
    delete app;
    
    return 0;
}
