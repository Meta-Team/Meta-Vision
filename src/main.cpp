#include "main.hpp"

int Main::main(int argc, char** argv){
    _loadConfig((argc >= 1) ? argv[1] : DEFAULT_CONFIG_NAME);
    _prepareArmorDetect();
    _prepareVideoCapture();
    _prepareVideoWriter();

    Mat frame;
    // int i = 0;
    cmessage << flush;
    while(_vCapture->read(frame)) {
        Mat resized;
        resize(frame, resized, Size(640, 480));
        cout << "+" << flush;

        RotatedRect rect = _armorDetect->analyze(resized);
        Point2f vertices[4];
        rect.points(vertices);
        for (int i = 0; i < 4; i++) {
            line(resized, vertices[i], vertices[(i+1)%4], Scalar(0,0,255), 2);
        }
        _vWriter->write(resized);
        // cout << vertices[0] << vertices[1] << vertices[2] << vertices[3] << endl;
    }
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

void Main::_prepareVideoCapture() {
    _vCapture = new VideoCapture(_config["system"]["video_source"].as<string>());
    if(!_vCapture->isOpened()) {
        throw std::invalid_argument("Invalid input file");
    }
}

void Main::_prepareVideoWriter() {
    _vWriter = new VideoWriter("test.mp4", CV_FOURCC('m', 'p', '4', 'v'), 30, Size(640, 480));
    if(!_vWriter->isOpened()) {
        throw std::invalid_argument("Invalid output file");
    }
}

Main::~Main() {
    if(_armorDetect != NULL) delete _armorDetect;
    if(_vCapture != NULL) {
        _vCapture->release();
        delete _vCapture;
    }
    if(_vWriter != NULL) {
        _vWriter->release();
        delete _vWriter;
    }
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