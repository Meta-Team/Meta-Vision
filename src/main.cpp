#include "main.hpp"
#include "video_sources/camera.hpp"
#include "video_sources/file.hpp"
#include "video_targets/dummy.hpp"
#include "video_targets/file.hpp"
#include "video_targets/webserver.hpp"

#include <unistd.h>
#include <math.h>

/**
 * @brief Main logic of the program.
 *        Loads config, sets up video source, target, armor detection.
 * 
 * @param argc Number of arguments passed
 * @param argv Values of arguments passed
 * @return int Return value of the whole program
 */
int Main::main(int argc, char** argv){
    _loadConfig((argc > 1) ? argv[1] : DEFAULT_CONFIG_NAME);
    _prepareArmorDetect();

    if("camera" == config["system"]["video_source"].as<string>()) {
        _video_src = new VideoSourceCamera(
            config["system"]["video_sources"]["camera"]["id"].as<int>(),
            config["system"]["video_sources"]["camera"]["width"].as<int>(),
            config["system"]["video_sources"]["camera"]["height"].as<int>(),
            config["system"]["video_sources"]["camera"]["fps"].as<int>()
        );
    } else if("file" == config["system"]["video_source"].as<string>()) {
        _video_src = new VideoSourceFile(config["system"]["video_sources"]["file"]["filename"].as<string>());
    } else {
        cerror << "Invalid video source";
        return -1;
    }

    if("file" == config["system"]["video_target"].as<string>()) {
        _video_tgt = new VideoTargetFile(
            config["system"]["video_targets"]["file"]["folder"].as<string>(),
            _video_src->getWidth(),
            _video_src->getHeight(),
            _video_src->getFPS(),
            config["system"]["video_targets"]["file"]["segment_interval"].as<int>()
        );
    } else if("dummy" == config["system"]["video_target"].as<string>()) {
        _video_tgt = new VideoTargetDummy();
    } else if("webserver" == config["system"]["video_target"].as<string>()) {
        _video_tgt = new VideoTargetWebserver(
            config["system"]["video_targets"]["webserver"]["port"].as<int>()
        );
    } else {
        cerror << "Invalid video target";
        return -1;
    }

    _serial = new SerialStatus(
        config["system"]["serial"]["device"].as<string>()
    );

    Mat frame, resized;
    
    while(should_run) {
        if(!_video_src->thread_should_run) break;
        if(!_video_src->isAvailable()) continue;
        
        // Compare new frame ID vs previous frame ID
        // only proceed if they are different (new frame coming)
        static int prev_id = 0, next_id = 0;
        next_id = _video_src->getFrame(frame, prev_id);
        if(prev_id == next_id) continue;
        prev_id = next_id;

        // Get yaw/pitch before time-costly analyzation to ensure accuracy
        int yaw_current = _serial->rm_state.custom_gimbal_current.yaw;
        int pitch_current = _serial->rm_state.custom_gimbal_current.pitch;

        // Call Armor Detect routine, draw armor borderline
        RotatedRect rect = _armorDetect->analyze(frame);
        Point2f vertices[4];
        rect.points(vertices);

        if(vertices[0] != Point2f(0, 0)
            || vertices[1] != Point2f(0, 0)
            || vertices[2] != Point2f(0, 0)
            || vertices[3] != Point2f(0, 0)
        ) { // An armor has been detected
            for (int i = 0; i < 4; i++) {
                line(frame, vertices[i], vertices[(i+1)%4], Scalar(0,0,255), 2);
            }

            double centerX = (vertices[0].x + vertices[1].x + vertices[2].x + vertices[3].x) / 4;
            double centerY = (vertices[0].y + vertices[1].y + vertices[2].y + vertices[3].y) / 4;

            // cwarning << "Center: " << centerX << ", " << centerY;

            double yaw_target = yaw_current + _fraction2angle(2 * centerX / _video_src->getWidth() - 1, config["system"]["target_calibration"]["view_angle_x"].as<double>());
            double pitch_target = pitch_current + _fraction2angle(1 - 2 * centerY / _video_src->getHeight(), config["system"]["target_calibration"]["view_angle_y"].as<double>());
            
            cwarning << "Pitch " << pitch_target << ", Yaw " << yaw_target;

            _serial->send_gimbal(yaw_target, pitch_target);
        }

        // Write image
        _video_tgt->writeFrame(frame);
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param fraction Fraction of position on the direction, -1 to 1.
 * @param maxAngle Max angle the camera can capture.
 * @return int Difference in angle to point to the position
 */
double Main::_fraction2angle(double fraction, double maxAngle) {
    if(maxAngle > 90.0 || maxAngle < 0.0) {
        cerror << "Main::_fraction2angle: maxAngle out of range: " << maxAngle;
        return 0;
    }
    if(fraction > 1.0 || fraction < -1.0) {
        cerror << "Main::_fraction2angle: Fraction out of range: " << fraction;
        return 0;
    }

    double tan_maxAngle = tan(maxAngle / 180 * M_PI);
    double tan_angle = tan_maxAngle * fraction;
    return round(atan(tan_angle) / M_PI * 180);
}

/**
 * @brief Loads the config file, and changes working directory to the same folder as config file,
 *        to read resource files relative to the config file.
 * 
 * @param filename Filename to the config file
 */
void Main::_loadConfig(string filename) {
    // Load YAML file
    config = YAML::LoadFile(filename);
    csuccess << "Config file " << filename << " loaded";

    // Find base folder
    size_t last_slash = filename.find_last_of("/\\");
    string config_base_folder = (last_slash == string::npos ? "" : filename.substr(0, last_slash + 1));
    if(0 > chdir(config_base_folder.c_str())) {
        cerror << "Failed to change to directory: " << config_base_folder;
    } else {
        char buf[1024];
        if(getcwd(buf, 1024)) {
            csuccess << "Changed to directory: " << string(buf);
        } else {
            csuccess << "Changed to directory: " << config_base_folder;
        }
    }
}

/**
 * @brief Initializes the armor detection algorithm.
 *        Currently it only uses the ICRA2018_NJUST algorithm.
 */
void Main::_prepareArmorDetect() {
    _armorDetect = new ICRA2018_NJUST_Armor::Armor_Interface(config["algorithm"]["icra2018_njust_armor"]);

    // Read color of our team, configure armor detect algorithm to aim for enemy
    string ourTeam = config["game"]["our_team"].as<string>();
    std::transform(ourTeam.begin(), ourTeam.end(), ourTeam.begin(), ::tolower);
    if(ourTeam == "red") {
        cmessage << "We are Team " << color(FG_RED) << "RED" << color(FG_DEFAULT)
             << ", Enemy is Team " << color(FG_BLUE) << "BLUE" << color(FG_DEFAULT);
        _armorDetect->setEnemyColor(BLUE);
    } else if(ourTeam == "blue") {
        cmessage << "We are Team " << color(FG_BLUE) << "BLUE" << color(FG_DEFAULT)
             << ", Enemy is Team " << color(FG_RED) << "RED" << color(FG_DEFAULT);
        _armorDetect->setEnemyColor(RED);
    } else {
        throw std::invalid_argument("Invalid value of game/our_team");
    }
    csuccess << "Initialized Armor Detection";
}

/**
 * @brief Destroy the Main:: Main object
 */
Main::~Main() {
    if(_armorDetect != NULL) delete _armorDetect;
    if(_video_src != NULL) delete _video_src;
    if(_video_tgt != NULL) delete _video_tgt;
    if(_serial != NULL) delete _serial;
}
