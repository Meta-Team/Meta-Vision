/**
 * Robomaster Vision program of Autocar
 * Copyright (c) 2018, Xidian University Robotics Vision group.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files(the "Software"), to deal in the Software without restriction.
 */


#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "armor_param.h"
#include "armor_sample.hpp"
#include "logging.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

int main(int argc, char **argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("enemy,E", po::value<string>()->default_value("red"), "enemy color [red/blue]")
            ("armor-param", po::value<string>()->default_value("../utilities/armor_sample/armor_params.xml"),
             "armor_param xml file")
            ("output-path,O", po::value<string>()->default_value("../training/output/"),
             "image output path")
            ("prefix,P", po::value<string>(), "image prefix")
            ("big", "detect big armor")
            ("input-file", po::value<string>(), "input video file");

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("enemy") && (vm["enemy"].as<string>() == "red" || vm["enemy"].as<string>() == "blue")) {
        cout << "Enemy color is set to " << vm["enemy"].as<string>() << ".\n";
    } else {
        cerror << "Enemy color error.";
        return 1;
    }

    if (vm.count("armor-param")) {
        cout << "armor_param xml file is set to " << vm["armor-param"].as<string>() << ".\n";
    } else {
        cerror << "armor_param xml file is not set.";
        return 1;
    }

    if (vm.count("output-path")) {
        cout << "Image output path is set to " << vm["output-path"].as<string>() << ".\n";
    } else {
        cerror << "Image output path is not set.";
        return 1;
    }

    if (vm.count("prefix")) {
        cout << "Image prefix is set to " << vm["prefix"].as<string>() << ".\n";
    } else {
        cerror << "Image prefix is not set.";
        return 1;
    }

    if (vm.count("input-file")) {
        cout << "Process video file " << vm["input-file"].as<string>() << ".\n";
    } else {
        cerror << "Input video file is not set.";
        return 1;
    }


    cv::VideoCapture capture_camera_forward(vm["input-file"].as<string>());
    if (!capture_camera_forward.isOpened()) {
        cerror << "Fail to open file " << vm["input-file"].as<string>();
        return 1;
    }

    string output_path = vm["output-path"].as<string>();
    if (output_path[output_path.length() - 1] != '/') output_path += "/";
    if (!fs::is_directory(output_path)) {
        cerror << "Output path " << output_path << " not exists or is not a directory.";
        return 1;
    }


    armor_param armorParam(vm["armor-param"].as<string>());
    armor_sample armorSample(armorParam, vm.count("big"), (vm["enemy"].as<string>() == "blue"),
                             output_path, vm["prefix"].as<string>());

    int index = 0;
    cv::Mat frame_forward;

    while (true) {
        capture_camera_forward >> frame_forward;
        if (frame_forward.empty()) break;
        armorSample.detect(frame_forward, index);
        try {
            imshow("result", frame_forward);
            cv::waitKey(1);
        }
        catch (cv::Exception e) {
            std::cout << "Show image error" << std::endl;
        }
//        cv::waitKey(0);
    }

    csuccess << "Complete.";

    return 0;
}