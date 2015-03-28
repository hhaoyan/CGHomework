//
//  AssetManager.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/28/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_AssetManager_h
#define libOOGL_AssetManager_h

/**
 * @brief Asset manager.
 */
class AssetManager{
public:
    /**
     * @brief Get the filename of an asset.
     */
    static const char* GetAsset(const char* name){
        static std::string assetPath;
        
        if(!assetPath.size()){
            assetPath = __FILE__;
            
            for (long i = assetPath.size() - 1; i >= 0; --i) {
                if(assetPath[i] == '/' || assetPath[i] == '\\'){
                    assetPath.erase(assetPath.begin() + i + 1, assetPath.end());
                    break;
                }
            }
        }
        
        static std::string oldAsset;
        
        oldAsset = assetPath;
        oldAsset.append("../assets/");
        oldAsset.append(name);
        return oldAsset.c_str();
    }
};

#endif
