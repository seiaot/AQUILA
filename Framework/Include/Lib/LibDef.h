#pragma once

#define SAFE_FREE(_p) {if((_p)!=nullptr){free((_p));_p=nullptr;}}
#define SAFE_DELETE(_p) {if((_p)!=nullptr){delete((_p));_p=nullptr;}}
#define SAFE_DELETE_ARRAY(_p) {if((_p)!=nullptr){delete[] ((_p));_p=nullptr;}}

#define INVALID_ASSET_HANDLE -1

