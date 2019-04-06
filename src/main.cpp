#include "main.hpp"
#include "interfaces/multithread.hpp"
#include "video_sources/file.hpp"
#include "video_sources/camera.hpp"
#include "video_targets/file.hpp"

int Main::main(int argc, char** argv){
    _loadConfig((argc > 1) ? argv[1] : DEFAULT_CONFIG_NAME);
    _prepareArmorDetect();

    if("camera" == _config["system"]["video_source"].as<string>()) {
        _video_src = new VideoSourceCamera(
            _config["system"]["video_sources"]["camera"]["id"].as<int>(),
            _config["system"]["video_sources"]["camera"]["width"].as<int>(),
            _config["system"]["video_sources"]["camera"]["height"].as<int>(),
            _config["system"]["video_sources"]["camera"]["fps"].as<int>()
        );
    } else if("file" == _config["system"]["video_source"].as<string>()) {
        _video_src = new VideoSourceFile(_base_folder + _config["system"]["video_sources"]["file"]["filename"].as<string>());
    } else {
        cerror << "Invalid video source" << endlog;
        return -1;
    }

    if("file" == _config["system"]["video_target"].as<string>()) {
        _video_tgt = new VideoTargetFile(
            _base_folder + _config["system"]["video_targets"]["file"]["filename"].as<string>(),
            _config["system"]["video_sources"]["camera"]["width"].as<int>(),
            _config["system"]["video_sources"]["camera"]["height"].as<int>()
        );
    } else {
        cerror << "Invalid video target" << endlog;
        return -1;
    }

    Mat frame, resized;
    
    while(should_run) {
        if(!_video_src->isAvailable()) continue;
        
        // Compare new frame ID vs previous frame ID
        // only proceed if they are different (new frame coming)
        static int prev_id = 0, next_id = 0;
        next_id = _video_src->getFrame(frame, prev_id);
        if(prev_id == next_id) continue;
        prev_id = next_id;

        // Call Armor Detect routine, draw armor borderline
        RotatedRect rect = _armorDetect->analyze(frame);
        Point2f vertices[4];
        rect.points(vertices);
        for (int i = 0; i < 4; i++) {
            line(frame, vertices[i], vertices[(i+1)%4], Scalar(0,0,255), 2);
        }

        // Write image
        _video_tgt->writeFrame(frame);
    }

    return 0;
}

void Main::_loadConfig(string filename) {
    // Find base folder
    size_t last_slash = filename.find_last_of("/\\");
    _base_folder = (last_slash == string::npos ? "" : filename.substr(0, last_slash + 1));
    csuccess << "Resource base directory: " << _base_folder << endl;

    // Load YAML file
    _config = YAML::LoadFile(filename);
    csuccess << "Config file " << filename << " loaded" << endlog;
}

void Main::_prepareArmorDetect() {
    _armorDetect = new ICRA2018_NJUST_Armor::Armor_Interface(_base_folder, _config["algorithm"]["icra2018_njust_armor"]);

    // Read color of our team, configure armor detect algorithm to aim for enemy
    string ourTeam = _config["game"]["our_team"].as<string>();
    std::transform(ourTeam.begin(), ourTeam.end(), ourTeam.begin(), ::tolower);
    if(ourTeam == "red") {
        cmessage << "We are Team " << color(FG_RED) << "RED" << color(FG_DEFAULT)
             << ", Enemy is Team " << color(FG_BLUE) << "BLUE" << color(FG_DEFAULT) << endl;
        _armorDetect->setEnemyColor(BLUE);
    } else if(ourTeam == "blue") {
        cmessage << "We are Team " << color(FG_BLUE) << "BLUE" << color(FG_DEFAULT)
             << ", Enemy is Team " << color(FG_RED) << "RED" << color(FG_DEFAULT) << endl;
        _armorDetect->setEnemyColor(RED);
    } else {
        throw std::invalid_argument("Invalid value of game/our_team");
    }
    csuccess << "Initialized Armor Detection" << endlog;
}

Main::~Main() {
    if(_armorDetect != NULL) delete _armorDetect;
    if(_video_src != NULL) delete _video_src;
    if(_video_tgt != NULL) delete _video_tgt;
}
