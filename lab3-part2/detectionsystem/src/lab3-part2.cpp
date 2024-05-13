#include <linesdiscoverysystem/readfiles.h>

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

#include <rendering/window.h>
#include <fmt/format.h>
#include <limits>

#include <map>

void plotData(const std::string& name);

/* ************************************* */

const double delta = 1e-10;

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct LineSegment {  // värdelös jälva struct
    // Point start = Point{0, 0};
    // Point end = Point{0, 0};
    // double slope = 0.0;
    // double length = 0.0;
    // double m = 0.0;
    std::pair<double, Point> VP = {};
};

struct LinesFromPoint {
    Point start = Point{0, 0};
    std::vector<LineSegment> lines = {};
    // double slope = 0.0;
};

struct CompleteLine {
    // Point start = Point{0, 0};
    // Point end = Point{0, 0};
    std::vector<Point> intermediaryPoints = {};
    double slope = 0.0;
    //  double m = 0.0;
};

// O(n)
std::vector<Point> fileReader(const std::string& filename) {
 /*   std::filesystem::path points_name =
        "C:/Github/TND004-Labs/lab3-part2/detectionsystem/data/" + filename;*/
    std::filesystem::path points_name = data_dir / filename;

    std::cout << points_name << '\n';
    std::ifstream pointsFile(points_name);

    if (!pointsFile) {
        std::cout << "Points file error!!\n";
        assert(pointsFile);
        return {};
    }

    int n_points{0};
    pointsFile >> n_points;  // read number of particles

    std::vector<Point> points;
    points.reserve(n_points);
    for (int i = 0; i < n_points; ++i) {
        Point temp = Point{};
        auto& p = points.emplace_back(temp);
        pointsFile >> p.x >> p.y;
        //p.x /= 32767.0;
        //p.y /= 32767.0;
    }
    return points;
}
// O(m)
void writeFile(std::vector<CompleteLine>& lines, std::string readFileName) {
    //std::filesystem::path points_name =
    //    "C:/Github/TND004-Labs/lab3-part2/detectionsystem/data/output/segments-" +
    //    readFileName;
    std::filesystem::path points_name = data_dir / "output" / ("segments-" + readFileName);
    std::cout << points_name << '\n';
    std::ofstream out_file(points_name);
    if (!out_file) {
        std::cout << "Error opening output file!!\n";
        return;
    }
    for (const auto& line : lines) {
        if (line.intermediaryPoints.size() <= 3) {
            continue;
        }

        out_file << line.intermediaryPoints[0].x << " "
                 << line.intermediaryPoints[0].y << " "
                 << line.intermediaryPoints[std::ssize(line.intermediaryPoints) - 1].x 
                 << " "
                 << line.intermediaryPoints[std::ssize(line.intermediaryPoints) - 1].y
                 << std::endl;
    }
    out_file.close();
}
// O(m^2)
void writeConsole(std::vector<CompleteLine>& lines) {
    int index = 0;
    for (const auto& line : lines) {
        if (line.intermediaryPoints.size() < 3) {
            continue;
        }
        std::cout << index << ": " << line.slope << ": (" << line.intermediaryPoints[0].x 
                  << ","
                  << line.intermediaryPoints[0].y << ")";

        for (size_t i = 1; i < std::ssize(line.intermediaryPoints); i++) {
            std::cout << "->(" << line.intermediaryPoints[i].x << ","
                      << line.intermediaryPoints[i].y << ")";
        }

        std::cout << '\n';
        index++;
    }
}

double calcSlope(Point& start, Point& end) {
    // double temp = (end.y - start.y) / (end.x - start.x);
    double num = end.y - start.y;
    double den = end.x - start.x;

    if (abs(num) < delta) {
        return 0.0;
    } else if (abs(den) < delta) {
        return std::numeric_limits<double>::infinity();
    }

    return num / den;
}

// naive implementation of linesegment
void cookLineSegments(std::vector<Point>& PV, std::vector<LinesFromPoint>& allLines) {
    for (int i = 0; i < std::ssize(PV); i++) {  // O(n)
        LinesFromPoint linesFromP;
        linesFromP.lines.reserve(std::ssize(PV));
        linesFromP.start = PV[i];
        for (int j = 0; j < std::ssize(PV); j++) {  // O(n)
            if (i == j) continue;
            LineSegment tempo;
            tempo.VP.first = calcSlope(PV[i], PV[j]);
            tempo.VP.second = PV[j];
            // linesFromP.slope = tempo.VP.first;
            linesFromP.lines.push_back(tempo);
        }
        allLines.push_back(linesFromP);
    }
}

bool operator<(const Point& LeftPoint, const Point& RightPoint) {
    if (abs(LeftPoint.y - RightPoint.y) < delta) {
		return LeftPoint.x < RightPoint.x;
	}
    return LeftPoint.y < RightPoint.y;
}

// DENNA FUNGERAR
 //void findCollinearPoints(std::vector<LinesFromPoint>& vecLinesFromP,
 //                         std::vector<CompleteLine>& CLV) {
 //    for (int i = 0; i < std::ssize(vecLinesFromP); i++) {  // step through all pointlines O(n)
 //        std::vector<CompleteLine> tempLine;
 //        tempLine.reserve(std::ssize(vecLinesFromP)); // donno
 //        tempLine.push_back(CompleteLine{});
 //        int vecCounter = 0;
 //        for (int j = 0; j < std::ssize(vecLinesFromP[i].lines) - 1; j++) {  // O(n) // step through
 //           // all lines with the same start point O(n)
 //           
 //           auto it1 = vecLinesFromP[i].lines[j].VP;
 //           auto it2 = vecLinesFromP[i].lines[j + 1].VP;
 //           tempLine[vecCounter].intermediaryPoints.push_back(it1.second);
 //           if ((it1.first == std::numeric_limits<double>::infinity() &&
 //                it2.first == std::numeric_limits<double>::infinity()) ||
 //               (abs(it1.first - it2.first) < delta)) {
 //               // we want to add the next point to the line
 //               
 //           } else {
 //               if (tempLine[vecCounter].intermediaryPoints.size() >= 3) {
 //                   tempLine[vecCounter].slope =
 //                       calcSlope(tempLine[vecCounter].intermediaryPoints[0],
 //                                 tempLine[vecCounter].intermediaryPoints[1]);
 //                   tempLine[vecCounter].intermediaryPoints.push_back(vecLinesFromP[i].start);
 //                   CLV.push_back(tempLine[vecCounter]);
 //                   /*vecCounter++;
 //                   tempLine.push_back(CompleteLine{});*/
 //               }
 //               vecCounter++;
 //               tempLine.push_back(CompleteLine{});
 //           }
 //           // if completeLine is last of vecLinesFromP[i].lines
 //        }
 //        if ((vecLinesFromP[i].lines[std::ssize(vecLinesFromP[i].lines) - 2].VP.first ==
 //                 std::numeric_limits<double>::infinity() &&
 //             vecLinesFromP[i].lines[std::ssize(vecLinesFromP[i].lines) - 1].VP.first ==
 //                 std::numeric_limits<double>::infinity()) || 
 //            (abs(tempLine[vecCounter].slope - vecLinesFromP[i].lines[std::ssize(vecLinesFromP[i].lines) - 1].VP.first) <
 //             delta)) {
 //           // we want to add the next point to the line
 //           tempLine[vecCounter].intermediaryPoints.push_back(
 //               vecLinesFromP[i].lines[std::ssize(vecLinesFromP[i].lines) - 1].VP.second);
 //        }
 //        
 //        if (tempLine[vecCounter].intermediaryPoints.size() >= 3) {
 //           tempLine[vecCounter].slope = calcSlope(tempLine[vecCounter].intermediaryPoints[0],
 //                                                  tempLine[vecCounter].intermediaryPoints[1]);
 //           tempLine[vecCounter].intermediaryPoints.push_back(vecLinesFromP[i].start);
 //           CLV.push_back(tempLine[vecCounter]);
 //        }
 //    } 
 //}

 void findCollinearPoints(std::vector<LinesFromPoint>& vecLinesFromP,
                          std::vector<CompleteLine>& CLV) {
     for (int i = 0; i < std::ssize(vecLinesFromP); i++) {  // step through all pointlines O(n)
         CompleteLine tempLine;
         
         tempLine = CompleteLine{};
         tempLine.intermediaryPoints.push_back(vecLinesFromP[i].start);
         tempLine.slope = vecLinesFromP[i].lines[0].VP.first;
         tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[0].VP.second);
         //double currentSlope = tempLine.slope;

         for (int j = 0; j < std::ssize(vecLinesFromP[i].lines) - 1; j++) {  // O(n) // step through
            // all lines with the same start point O(n)

            auto it1 = vecLinesFromP[i].lines[j].VP;
            auto it2 = vecLinesFromP[i].lines[j + 1].VP;
            if ((it1.first == std::numeric_limits<double>::infinity() &&
                 it2.first == std::numeric_limits<double>::infinity()) ||
                (abs(it1.first - it2.first) < delta)) {
                tempLine.intermediaryPoints.push_back(it2.second);

            } else {
                if (tempLine.intermediaryPoints.size() > 3) {

                    CLV.push_back(tempLine);

                }
                
                tempLine = CompleteLine{};
                tempLine.intermediaryPoints.push_back(vecLinesFromP[i].start);
                tempLine.slope = vecLinesFromP[i].lines[j+1].VP.first;
                tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[j+1].VP.second);
            }
            // if completeLine is last of vecLinesFromP[i].lines
         }
         

         if (tempLine.intermediaryPoints.size() > 3) {

            CLV.push_back(tempLine);
         }
     }
 }

void deleteDuplicates(std::vector<CompleteLine>& allCompleteLines) {
    std::vector<bool> duplicates(allCompleteLines.size(), false);
    for (int i = 0; i < std::ssize(allCompleteLines) - 1; i++) {
        if ((abs(allCompleteLines[i].slope - allCompleteLines[i + 1].slope) < delta) ||
            (allCompleteLines[i].slope == std::numeric_limits<double>::infinity() &&
             allCompleteLines[i + 1].slope == std::numeric_limits<double>::infinity())) {
            for (size_t j = 0; j < std::ssize(allCompleteLines[i].intermediaryPoints);
                 j++) {  // step through the first line
                if (abs(allCompleteLines[i].intermediaryPoints[j].x -
                        allCompleteLines[i + 1].intermediaryPoints[0].x) < delta &&
                    abs(allCompleteLines[i].intermediaryPoints[j].y -
                        allCompleteLines[i + 1].intermediaryPoints[0].y) < delta) {
                    duplicates[i + 1] = true;
                }
            }
        }
    }

    for (int i = std::ssize(allCompleteLines) - 1; i > 0; i--) {
        if (duplicates[i]) {
            allCompleteLines.erase(allCompleteLines.begin() + i);
        }
    }
}

int main() try {
    std::cout << "Enter the name of input points file: ";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt

    std::vector<Point> allPoints = fileReader(s);  // O(n)

    std::sort(allPoints.begin(), allPoints.end());  

    std::vector<LinesFromPoint> allLines;

    allLines.reserve(std::ssize(allPoints)); // reserve "one piece of space" for each point

    cookLineSegments(allPoints, allLines);  // O(n^2)

    allPoints.clear();  // clear allPoints O(n)

    // sort lines by slope 2. in pdf

    for (size_t i = 0; i < std::ssize(allLines); i++) {
        std::stable_sort(allLines[i].lines.begin(), allLines[i].lines.end(),
                         [](const LineSegment& a, const LineSegment& b) -> bool {
                             if (abs(a.VP.first - b.VP.first) < delta) {
                                 return a.VP.second < b.VP.second;
                             }

                             return a.VP.first < b.VP.first;
                         });
    }


    std::vector<CompleteLine> allCompleteLines;
    // we reserve for the duplicate completed lines
    allCompleteLines.reserve(std::ssize(allLines) * (std::ssize(allLines)-1)/4);  // total number of line segments is m(m-1), 4 points per complete line

    findCollinearPoints(allLines, allCompleteLines);  // n^2

    allLines.clear();  // clear allLines O(m)

    for (size_t i = 0; i < std::ssize(allCompleteLines); i++) {  // n^2logn
        std::stable_sort(allCompleteLines[i].intermediaryPoints.begin(),
                         allCompleteLines[i].intermediaryPoints.end(),
                         [](const Point& a, const Point& b) -> bool {
                             if (abs(a.y - b.y) < delta) {
                                 return a.x < b.x;
                             }

                             return a.y < b.y;
                         });
    }

    std::stable_sort(allCompleteLines.begin(), allCompleteLines.end(), // nlogn
                     [](const CompleteLine& a, const CompleteLine& b) -> bool {
                         if (abs(a.intermediaryPoints[0].y - b.intermediaryPoints[0].y) < delta) {
                             if (abs(a.intermediaryPoints[0].x - b.intermediaryPoints[0].x) < delta ) {
                                 return a.slope < b.slope;
                             }
                             return a.intermediaryPoints[0].x < b.intermediaryPoints[0].x;

                         }

                         return a.intermediaryPoints[0].y < b.intermediaryPoints[0].y; 

                     });  

    // Vi måste ta bort kopior från allcompleteLines
    deleteDuplicates(allCompleteLines);

    writeFile(allCompleteLines, s);

    writeConsole(allCompleteLines);

    plotData(s);
} catch (const std::exception& e) {
    fmt::print("Error: {}", e.what());
    return 1;
}

/* ************************************* */

void plotData(const std::string& name) {
    std::filesystem::path points_name = name;
    const auto points = readPoints(data_dir / points_name);

    std::filesystem::path segments_name = "segments-" + name;
    const auto lines = readLineSegments(data_dir / "output" / segments_name);

    rendering::Window window(850, 850, rendering::Window::UseVSync::Yes);
    while (!window.shouldClose()) {
        window.beginFrame();
        window.clear({0, 0, 0, 1});
        window.drawLines(lines);    // to plot the line segments discovered
        window.drawPoints(points);  // to plot input points
        window.endFrame();
    }
}