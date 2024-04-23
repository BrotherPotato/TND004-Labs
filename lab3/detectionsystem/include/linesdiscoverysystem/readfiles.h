#pragma once

#include <iostream>
#include <vector>
#include <filesystem>

#include <rendering/window.h>

#include <fmt/format.h>

const std::filesystem::path data_dir{DATA_DIR};

/*
* Reads all line segments from file
* Line segments are represented by the start point (x_1, y_1) and end point (x_2, y_2)
* One line segment per line: x_1 y_1 x2 y_2
* Returns a vector of points that can be rendered
*/
std::vector<rendering::Point> readLineSegments(const std::filesystem::path& file);

/*
* Reads all points from a given input file -- see folder detectionsystem\data
* Returns a vector of points that can be rendered
*/
std::vector<rendering::Point> readPoints(const std::filesystem::path& file);
