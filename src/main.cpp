#include "main.hpp"
#include "interfaces/multithread.hpp"
#include "video_sources/file.hpp"
#include "video_targets/file.hpp"


int Main::main(int argc, char** argv){
    _loadConfig((argc >= 1) ? argv[1] : DEFAULT_CONFIG_NAME);
    _prepareArmorDetect();
    _video_src = new VideoSourceFile(_config["system"]["video_source"].as<string>());
    _video_tgt = new VideoTargetFile("test.mp4", VIDEO_WIDTH, VIDEO_HEIGHT);

    cmessage << flush;
    try {
        Mat frame, resized;
        int prev_id = 0, next_id = 0;
        while(_video_src->isAvailable()) {
            next_id = _video_src->getFrame(frame, prev_id);
            if(prev_id == next_id) continue;
            prev_id = next_id;

            resize(frame, resized, Size(VIDEO_WIDTH, VIDEO_HEIGHT));

            cout << "+" << flush;

            RotatedRect rect = _armorDetect->analyze(resized);
            Point2f vertices[4];
            rect.points(vertices);
            for (int i = 0; i < 4; i++) {
                line(resized, vertices[i], vertices[(i+1)%4], Scalar(0,0,255), 2);
            }

            _video_tgt->writeFrame(resized);
        }
    } catch(runtime_error re) {}
    cout << endl;
    
    return 0;
}

void Main::_loadConfig(string filename) {
    _config = YAML::LoadFile(filename);
    csuccess << "Config file " << filename << " loaded" << endlog;
}

void Main::_prepareArmorDetect() {
    _armorDetect = new ICRA2018_NJUST_Armor::Armor_Interface(_config["algorithm"]["icra2018_njust_armor"]);

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

// The real entrypoint
int main(int argc, char** argv) {
    Main mainFunction;
    try {
        return mainFunction.main(argc, argv);
    } catch(const std::invalid_argument& e) {
        cerror << e.what() << endlog;
        return -1;
    } catch(const std::runtime_error& e) {
        cerror << e.what() << endlog;
        return -1;
    }
}