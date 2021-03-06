#include "FrameworkPlugin/CPlugin.h"

using namespace FrameworkPlugin;
using namespace std;

/*
 ____  _             _           ___       _ _   _       _ _
|  _ \| |_   _  __ _(_)_ __     |_ _|_ __ (_) |_(_) __ _| (_)_______
| |_) | | | | |/ _` | | '_ \     | || '_ \| | __| |/ _` | | |_  / _ \
|  __/| | |_| | (_| | | | | |    | || | | | | |_| | (_| | | |/ /  __/
|_|   |_|\__,_|\__, |_|_| |_|___|___|_| |_|_|\__|_|\__,_|_|_/___\___|
               |___/       |_____|
*/
bool CPlugin::Plugin_Initialize(std::string config_file) {
    return true;
    }

/*
 ____  _             _           ____  _           _      _
|  _ \| |_   _  __ _(_)_ __     / ___|| |__  _   _| |_ __| | _____      ___ __
| |_) | | | | |/ _` | | '_ \    \___ \| '_ \| | | | __/ _` |/ _ \ \ /\ / / '_ \
|  __/| | |_| | (_| | | | | |    ___) | | | | |_| | || (_| | (_) \ V  V /| | | |
|_|   |_|\__,_|\__, |_|_| |_|___|____/|_| |_|\__,_|\__\__,_|\___/ \_/\_/ |_| |_|
               |___/       |_____|
*/
void CPlugin::Plugin_Shutdown() {
    }

/*
 ____  _             _           ____      _   _   _
|  _ \| |_   _  __ _(_)_ __     / ___| ___| |_| \ | | __ _ _ __ ___   ___
| |_) | | | | |/ _` | | '_ \   | |  _ / _ \ __|  \| |/ _` | '_ ` _ \ / _ \
|  __/| | |_| | (_| | | | | |  | |_| |  __/ |_| |\  | (_| | | | | | |  __/
|_|   |_|\__,_|\__, |_|_| |_|___\____|\___|\__|_| \_|\__,_|_| |_| |_|\___|
               |___/       |_____|
*/
std::string CPlugin::Plugin_GetName() {
    return string("CPlugin basic class plugin");
    }

/*
 ____  _             _           ____      _ __     __            _
|  _ \| |_   _  __ _(_)_ __     / ___| ___| |\ \   / /__ _ __ ___(_) ___  _ __
| |_) | | | | |/ _` | | '_ \   | |  _ / _ \ __\ \ / / _ \ '__/ __| |/ _ \| '_ \
|  __/| | |_| | (_| | | | | |  | |_| |  __/ |_ \ V /  __/ |  \__ \ | (_) | | | |
|_|   |_|\__,_|\__, |_|_| |_|___\____|\___|\__| \_/ \___|_|  |___/_|\___/|_| |_|
               |___/       |_____|
*/
std::string CPlugin::Plugin_GetVersion() {

    return string("1.0");

    }

/*
 ____  _             _           ____      _    ____ _
|  _ \| |_   _  __ _(_)_ __     / ___| ___| |_ / ___| | __ _ ___ ___
| |_) | | | | |/ _` | | '_ \   | |  _ / _ \ __| |   | |/ _` / __/ __|
|  __/| | |_| | (_| | | | | |  | |_| |  __/ |_| |___| | (_| \__ \__ \
|_|   |_|\__,_|\__, |_|_| |_|___\____|\___|\__|\____|_|\__,_|___/___/
               |___/       |_____|
*/
std::string CPlugin::Plugin_GetClass() {

    return string("");

    }

/*
 ____  _             _           ____      _   _                    _ _             ____       _   _
|  _ \| |_   _  __ _(_)_ __     / ___| ___| |_| |    ___   __ _  __| (_)_ __   __ _|  _ \ __ _| |_| |__
| |_) | | | | |/ _` | | '_ \   | |  _ / _ \ __| |   / _ \ / _` |/ _` | | '_ \ / _` | |_) / _` | __| '_ \
|  __/| | |_| | (_| | | | | |  | |_| |  __/ |_| |__| (_) | (_| | (_| | | | | | (_| |  __/ (_| | |_| | | |
|_|   |_|\__,_|\__, |_|_| |_|___\____|\___|\__|_____\___/ \__,_|\__,_|_|_| |_|\__, |_|   \__,_|\__|_| |_|
               |___/       |_____|                                            |___/
*/
std::string CPlugin::Plugin_GetLoadingPath() {
    return _loadingPath;
    }

/*
 ____  _             _           ____  _                 _                    _ _             ____       _   _
|  _ \| |_   _  __ _(_)_ __     / ___|| |_ ___  _ __ ___| |    ___   __ _  __| (_)_ __   __ _|  _ \ __ _| |_| |__
| |_) | | | | |/ _` | | '_ \    \___ \| __/ _ \| '__/ _ \ |   / _ \ / _` |/ _` | | '_ \ / _` | |_) / _` | __| '_ \
|  __/| | |_| | (_| | | | | |    ___) | || (_) | | |  __/ |__| (_) | (_| | (_| | | | | | (_| |  __/ (_| | |_| | | |
|_|   |_|\__,_|\__, |_|_| |_|___|____/ \__\___/|_|  \___|_____\___/ \__,_|\__,_|_|_| |_|\__, |_|   \__,_|\__|_| |_|
               |___/       |_____|                                                      |___/
*/
void CPlugin::Plugin_StoreLoadingPath(std::string path) {
    _loadingPath = path;
    }