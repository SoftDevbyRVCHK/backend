#ifndef ODYSSEY_FRAMEWORK_IDENTITY_HUMANOID_H
#define ODYSSEY_FRAMEWORK_IDENTITY_HUMANOID_H

#include "passport.h"
#include "characteristic.h"

namespace odyssey {

    namespace framework {
        
        namespace identity {
            
            struct humanoid {
                
                passport psprt;
                characteristic charcs;
                
                enum class profession_t {merchant, adventurer, government};
            };
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_IDENTITY_HUMANOID_H */