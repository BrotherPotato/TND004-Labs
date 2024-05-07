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

// std::vector<std::vector<Point>>
//
//   jag hatar denna klass s� mycket. Like varf�r �r det en class som bara hanterar slope och vector
//   av linesegment vi kommer bara beh�va 1 vector och ber�kna slope kan vara en funktion.

std::vector<Point> fileReader(const std::string& filename) {
    std::filesystem::path points_name =
        "C:/Programmering/TND004/TND004-Labs/lab3-part2/detectionsystem/data/" + filename;
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
        p.x /= 32767.0;
        p.y /= 32767.0;
    }
    return points;
}

void writeFile(std::vector<CompleteLine>& lines, std::string readFileName) {
    std::filesystem::path points_name =
        "C:/Programmering/TND004/TND004-Labs/lab3-part2/detectionsystem/data/output/segments-" +
        readFileName;
    std::cout << points_name << '\n';
    std::ofstream out_file(points_name);
    if (!out_file) {
        std::cout << "Error opening output file!!\n";
        return;
    }
    // out_file << lines.size() << '\n';
    for (const auto& line : lines) {
        if (line.intermediaryPoints.size() <= 3) {
            continue;
        }
        // out_file << "(" << line.start.x << "," << line.start.y << ")";

        out_file << line.intermediaryPoints[0].x * 32767.0 << " "
                 << line.intermediaryPoints[0].y * 32767.0 << " "
                 << line.intermediaryPoints[std::ssize(line.intermediaryPoints) - 1].x * 32767.0
                 << " "
                 << line.intermediaryPoints[std::ssize(line.intermediaryPoints) - 1].y * 32767.0
                 << std::endl;
    }
    out_file.close();
}

void writeConsole(std::vector<CompleteLine>& lines) {
    for (const auto& line : lines) {
        if (line.intermediaryPoints.size() < 3) {
            continue;
        }
        std::cout << line.slope << ": (" << line.intermediaryPoints[0].x * 32767.0 << ","
                  << line.intermediaryPoints[0].y * 32767.0 << ")";

        for (size_t i = 1; i < std::ssize(line.intermediaryPoints); i++) {
            std::cout << "->(" << line.intermediaryPoints[i].x * 32767.0 << ","
                      << line.intermediaryPoints[i].y * 32767.0 << ")";
        }

        std::cout << '\n';
    }
}

double calcSlope(Point& start, Point& end) {
    // double temp = (end.y - start.y) / (end.x - start.x);
    double num = end.y - start.y;
    double den = end.x - start.x;

    double delta = 1e-10;

    if (abs(num) < delta) {
        return 0.0;
    } else if (abs(den) < delta) {
        return std::numeric_limits<double>::infinity();
    }

    return num / den;
}

// naive implementation of linesegment
void cookLineSegments(std::vector<Point>& PV, std::vector<LinesFromPoint>& allLines
                      /*std::vector<std::pair<double, Point>>& VP*/) {
    for (int i = 0; i < std::ssize(PV); i++) {  // O(n)
        LinesFromPoint linesFromP;
        linesFromP.lines.reserve(std::ssize(PV));
        linesFromP.start = PV[i];
        for (int j = 0; j < std::ssize(PV); j++) {  // O(m)
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

bool operator<(const LineSegment& LeftLineSeg,
               const LineSegment& RightLineSeg) {  // used in stablesort
    if (LeftLineSeg.VP.first == std::numeric_limits<double>::infinity() &&
        RightLineSeg.VP.first != std::numeric_limits<double>::infinity()) {
        return true;
    } else if (RightLineSeg.VP.first == std::numeric_limits<double>::infinity()) {
        // om LLP == RLP så är false fortfarande korrekt :))
        return false;
    }

    return LeftLineSeg.VP.first < RightLineSeg.VP.first;
}

bool operator<(const Point& LeftPoint, const Point& RightPoint) {
    /*if (abs(LeftPoint.x - RightPoint.x) < 1e-10) {
        if (abs(LeftPoint.y - RightPoint.y) < 1e-10) {
            return true;
        } else {
            return LeftPoint.y < RightPoint.y;
        }
    }*/
    return LeftPoint.y < RightPoint.y;

    //        return LeftPoint.y > RightPoint.y;
    //    } else if (LeftPoint.x > RightPoint.x) {
    //        return true;
    //    } else if (LeftPoint.x <= RightPoint.x) {
    //        return false;
    //    }
    //
    //    return false;
}

bool operator<(const LinesFromPoint& LeftLinePoint, const LinesFromPoint& RightLinePoint) {
    if (LeftLinePoint.lines[0].VP.first == std::numeric_limits<double>::infinity() &&
        RightLinePoint.lines[0].VP.first != std::numeric_limits<double>::infinity()) {
        return true;
    } else if (RightLinePoint.lines[0].VP.first == std::numeric_limits<double>::infinity()) {
        // om LLP == RLP så är false fortfarande korrekt :))
        return false;
    }

    return LeftLinePoint.lines[0].VP.first < RightLinePoint.lines[0].VP.first;
}

bool operator<(CompleteLine& LeftLine, CompleteLine& RightLine) {
    // slope ljuger     kanske
    //    double LeftSlope = calcSlope(LeftLine.intermediaryPoints[0],
    //    LeftLine.intermediaryPoints[1]); double RightSlope =
    //    calcSlope(RightLine.intermediaryPoints[0], RightLine.intermediaryPoints[1]);
    //
    //
    //    if (LeftSlope == std::numeric_limits<double>::infinity() &&
    //        RightSlope != std::numeric_limits<double>::infinity()) {
    //        return true;
    //    }else if (RightSlope == std::numeric_limits<double>::infinity()) {
    //        return false;
    //    }
    //
    //    return LeftSlope < RightSlope;
    return LeftLine.slope < RightLine.slope;
    // return LeftLine.intermediaryPoints[0].y < RightLine.intermediaryPoints[0].y;
}

 void findCollinearPoints(std::vector<LinesFromPoint>& vecLinesFromP,
                          std::vector<CompleteLine>& CLV) {
     for (size_t i = 0; i < std::ssize(vecLinesFromP); i++) {  // step through all pointlines O(n)
         CompleteLine tempLine;
         tempLine.intermediaryPoints.reserve(
             std::ssize(vecLinesFromP[i].lines));  // reserve space for all points O(n)
         // tempLine.intermediaryPoints.push_back(vecLinesFromP[i][0].VP.second);

         size_t a = 0;

         tempLine.intermediaryPoints.push_back(vecLinesFromP[i].start);
         for (size_t j = 0; j < std::ssize(vecLinesFromP[i].lines);j++) {  // O(n) // step through
         //all lines with the same start point O(n)

             double first = vecLinesFromP[i].lines[a].VP.first;
             double second = vecLinesFromP[i].lines[j].VP.first;
             if (first == std::numeric_limits<double>::infinity() &&
                 second == std::numeric_limits<double>::infinity()) {

                 tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[j].VP.second);
                 std::cout << i << " " << vecLinesFromP[i].lines[j].VP.second.x << " "
                           << vecLinesFromP[i].lines[j].VP.second.y << '\n';

                 //continue;  // continue goes to the next iteration of the loop
             }

             else if (abs(first - second) < 1e-10) {

                 tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[j].VP.second);
                 std::cout << i << " " << vecLinesFromP[i].lines[j].VP.second.x << " "
                           << vecLinesFromP[i].lines[j].VP.second.y << '\n';
                 //continue;  // continue goes to the next iteration of the loop
             } else {
                 //a = j;
                 /*if (tempLine.intermediaryPoints.size() > 3) {
                     tempLine.slope =
                         calcSlope(tempLine.intermediaryPoints[0],
                         tempLine.intermediaryPoints[1]);
                     CLV.push_back(tempLine);*/
                 }
             }

             // a = j;

             /**/
         

         if (tempLine.intermediaryPoints.size() > 3) {
             tempLine.slope =
                 calcSlope(tempLine.intermediaryPoints[0], tempLine.intermediaryPoints[1]);
             CLV.push_back(tempLine);
             tempLine.intermediaryPoints.clear();
         }

         // remove duplicates

     } 
 }

/*
void findCollinearPoints(std::vector<LinesFromPoint>& vecLinesFromP,
                         std::vector<CompleteLine>& CLV) {
    int counter = 0;
    std::vector<CompleteLine> tempLines;
    tempLines.reserve(std::ssize(vecLinesFromP)/3);

    for (size_t i = 0; i < std::ssize(tempLines); i++) {
        tempLines[i].intermediaryPoints.reserve(std::ssize(vecLinesFromP));
        tempLines[i].intermediaryPoints.push_back(vecLinesFromP[i].start);
    }
    for (size_t i = 0; i < std::ssize(vecLinesFromP); i++) {

        for (size_t j = 0; j < std::ssize(vecLinesFromP[i].lines)-1; j++) {
            auto it1 = vecLinesFromP[i].lines[j].VP;
            auto it2 = vecLinesFromP[i].lines[j+1].VP;

                        if (abs(it1.first - it2.first) < 1e-10) {
                tempLines[counter].intermediaryPoints.push_back(it2.second);
            } else {
                counter++;
            }
        }
    }
    std::cout << "B;";
        for (size_t i = 0; i < counter; i++) {
        if (std::ssize(tempLines[i].intermediaryPoints) > 3) {
            tempLines[i].slope =
                calcSlope(tempLines[i].intermediaryPoints[0], tempLines[i].intermediaryPoints[1]);

            CLV.push_back(tempLines[i]);
        }
    }

}*/

void deleteDuplicates(std::vector<CompleteLine>& allCompleteLines) {
    std::vector<bool> duplicates(allCompleteLines.size(), false);
    for (size_t i = 0; i < std::ssize(allCompleteLines) - 1; i++) {
        // size_t a = i;
        if ((abs(allCompleteLines[i].slope - allCompleteLines[i + 1].slope) < 1e-10) ||
            (allCompleteLines[i].slope == std::numeric_limits<double>::infinity() &&
             allCompleteLines[i + 1].slope == std::numeric_limits<double>::infinity())) {
            for (size_t j = 0; j < std::ssize(allCompleteLines[i].intermediaryPoints);
                 j++) {  // step through the first line
                if (abs(allCompleteLines[i].intermediaryPoints[j].x -
                        allCompleteLines[i + 1].intermediaryPoints[0].x) < 1e-10 &&
                    abs(allCompleteLines[i].intermediaryPoints[j].y -
                        allCompleteLines[i + 1].intermediaryPoints[0].y) < 1e-10) {
                    duplicates[i + 1] = true;
                    // break;
                }
            }
        }
    }

    for (size_t i = std::ssize(allCompleteLines) - 1; i > 0; i--) {
        if (duplicates[i]) {
            allCompleteLines.erase(allCompleteLines.begin() + i);
        }
    }
}

int main() try {
    std::cout << "Enter the name of input points file: ";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt

    std::vector<Point> allPoints = fileReader(s);

    // std::cout << "A";

    // std::vector<LineSegment> allLines;
    // allLines.reserve(allPoints.size() * allPoints.size());  // lol

    // std::vector<std::vector<LineSegment>> setOfAllLines;
    // setOfAllLines.reserve(allPoints.size() * allPoints.size());  // lol

    // sortera också points?

    std::sort(allPoints.begin(), allPoints.end());  //, [](const Point& a, const Point& b) -> bool {
    //    /*if (abs(a.x - b.x) < 1e-16) {
    //        return a.y > b.y;
    //    }*/

    //    if (a.y > b.y || abs(a.y - b.y) < 1e-16) {
    //        return true;
    //    }
    //    /*if (a.x > b.x) {
    //        return true;
    //    }*/
    //    return false;
    //});

    std::vector<LinesFromPoint> allLines;

    cookLineSegments(allPoints, allLines);  // O(n+m)

    // sort lines by slope 2. in pdf
    // std::sort(allLines.begin(), allLines.end());  // nlogn
    std::cout << "A";

    for (size_t i = 0; i < std::ssize(allLines); i++) {
        std::stable_sort(allLines[i].lines.begin(), allLines[i].lines.end(),
                         [](const LineSegment& a, const LineSegment& b) -> bool {
                             /*if (a.VP.first == std::numeric_limits<double>::infinity() &&
                                 b.VP.first != std::numeric_limits<double>::infinity()) {
                                 return false;
                             } else if (b.VP.first == std::numeric_limits<double>::infinity()) {
                                 return true;
                             }*/
                             if (abs(a.VP.first - b.VP.first) < 1e-10) {
                                 return a.VP.second < b.VP.second;
                             }

                             return a.VP.first < b.VP.first;
                         });
    }

    /*
                  }*/
    // TODO: ta bort alla kopior
    // TODO: sl� ihop linjer

    // Vi måste ta bort kopior från allcompleteLines

    std::vector<CompleteLine> allCompleteLines;

    findCollinearPoints(allLines, allCompleteLines);  // n^2

    // std::cout << "SUMSUM: " << allCompleteLines.size() << "\n";

    for (size_t i = 0; i < std::ssize(allCompleteLines); i++) {  // n^2logn
        std::stable_sort(allCompleteLines[i].intermediaryPoints.begin(),
                         allCompleteLines[i].intermediaryPoints.end(),
                         [](const Point& a, const Point& b) -> bool {
                             if (abs(a.y - b.y) < 1e-10) {
                                 return a.x < b.x;
                             }

                             return a.y < b.y;
                         });
    }

    std::stable_sort(allCompleteLines.begin(), allCompleteLines.end(),
                     [](const CompleteLine& a, const CompleteLine& b) -> bool {
                         if (abs(a.intermediaryPoints[0].y - b.intermediaryPoints[0].y) < 1e-10) {
                             return a.intermediaryPoints[0].x < b.intermediaryPoints[0].x;
                         }

                         return a.intermediaryPoints[0].y < b.intermediaryPoints[0].y;
                     });  // nlogn

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