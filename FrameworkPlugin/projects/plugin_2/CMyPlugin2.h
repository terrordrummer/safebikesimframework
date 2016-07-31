#include <string>
#include "FrameworkPlugin/CPlugin.h"

class CMyPlugin2 : public FrameworkPlugin::CPlugin {
    
    virtual bool Initialize(std::string config_file);
    virtual void Shutdown();
    virtual std::string GetName();
    virtual std::string GetVersion();

    };

FRAMEWORK_EXPORT_PLUGIN(CMyPlugin2);
