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
    std::filesystem::path points_name = filename;
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
    std::ofstream out_file("../data/segments-" + readFileName + ".txt");
    if (!out_file) {
        std::cout << "Error opening output file!!\n";
        return;
    }
    // out_file << lines.size() << '\n';
    for (const auto& line : lines) {
        if (line.intermediaryPoints.size() < 3) {
            continue;
        }
        // out_file << "(" << line.start.x << "," << line.start.y << ")";
        for (Point currentPoint : line.intermediaryPoints) {
            out_file << "(" << currentPoint.x << "," << currentPoint.y << ")->";
        }
        out_file << '\n';
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
        for (int j = i + 1; j < std::ssize(PV); j++) {  // O(m)

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
    if (abs(LeftPoint.x - RightPoint.x) < 1e-5) {
        return LeftPoint.y < RightPoint.y;
    }

    return LeftPoint.x < RightPoint.x;
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
        for (size_t j = 0; j < std::ssize(vecLinesFromP[i].lines);
             j++) {  // O(n) // step through all lines with the same start point O(n)

            double first = vecLinesFromP[i].lines[a].VP.first;
            double second = vecLinesFromP[i].lines[j].VP.first;
            if (first == std::numeric_limits<double>::infinity() &&
                second == std::numeric_limits<double>::infinity()) {

                tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[j].VP.second);
                std::cout << i << " " << vecLinesFromP[i].lines[j].VP.second.x << " "
                          << vecLinesFromP[i].lines[j].VP.second.y << '\n';

                continue;  // continue goes to the next iteration of the loop
            }

            if (abs(first - second) < 1e-10) {

                tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[j].VP.second);
                std::cout << i << " " << vecLinesFromP[i].lines[j].VP.second.x << " "
                          << vecLinesFromP[i].lines[j].VP.second.y << '\n';
                continue;  // continue goes to the next iteration of the loop
            }

            // a = j;

            /*if (tempLine.intermediaryPoints.size() > 3) {
                CLV.push_back(tempLine);
            }*/
        }

        if (tempLine.intermediaryPoints.size() > 3) {
            tempLine.slope =
                calcSlope(tempLine.intermediaryPoints[0], tempLine.intermediaryPoints[1]);
            CLV.push_back(tempLine);
        }

        // remove duplicates

    }  // for std::ssize(vecLinesFromP)

    // if (CLV.size() > 1) {
    //     for (size_t i = 0; i < CLV.size(); i++) {
    //         for (size_t j = 1; j < std::ssize(CLV[i].intermediaryPoints); j++) {
    //             if (CLV.size() > 1) {
    //                 if (CLV[i].slope == CLV[j].slope) {
    //                     if (CLV[i].intermediaryPoints[i].x == CLV[j].intermediaryPoints[j].x ||
    //                         CLV[i].intermediaryPoints[i].y == CLV[j].intermediaryPoints[j].y) {
    //                         CLV.erase(CLV.begin() + j);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
}

void deleteDuplicates(std::vector<CompleteLine>& allCompleteLines) {
    std::vector<bool> duplicates(allCompleteLines.size(), false);
    for (size_t i = 0; i < std::ssize(allCompleteLines) - 1; i++) {
        // size_t a = i;
        if ((abs(allCompleteLines[i].slope - allCompleteLines[i + 1].slope) < 1e-10) ||
            (allCompleteLines[i].slope == std::numeric_limits<double>::infinity() &&
             allCompleteLines[i+1].slope == std::numeric_limits<double>::infinity())) {
            for (size_t j = 0; j < std::ssize(allCompleteLines[i].intermediaryPoints); j++) {  // step through the first line
                if (abs(allCompleteLines[i].intermediaryPoints[j].x -
                        allCompleteLines[i + 1].intermediaryPoints[0].x) < 1e-10 &&
                    abs(allCompleteLines[i].intermediaryPoints[j].y -
                        allCompleteLines[i + 1].intermediaryPoints[0].y) < 1e-10) {
                    duplicates[i + 1] = true;
                    //break;
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

    // std::stable_sort(allPoints.begin(), allPoints.end());

    std::vector<LinesFromPoint> allLines;

    cookLineSegments(allPoints, allLines);  // O(n+m)

    // sort lines by slope 2. in pdf
    // std::sort(allLines.begin(), allLines.end());  // nlogn

    for (size_t i = 0; i < std::ssize(allLines); i++) {
        std::sort(allLines[i].lines.begin(), allLines[i].lines.begin(),
                  [](const LineSegment& a, const LineSegment& b) -> bool {
                      if (a.VP.first == std::numeric_limits<double>::infinity() &&
                          b.VP.first != std::numeric_limits<double>::infinity()) {
                          return false;
                      } else if (b.VP.first == std::numeric_limits<double>::infinity()) {
                          return true;
                      }

                      return a.VP.first < b.VP.first;
                  });
    }

    // TODO: ta bort alla kopior
    // TODO: sl� ihop linjer

    // Vi måste ta bort kopior från allcompleteLines

    std::vector<CompleteLine> allCompleteLines;

    findCollinearPoints(allLines, allCompleteLines);  // n^2

    // std::cout << "SUMSUM: " << allCompleteLines.size() << "\n";

    for (size_t i = 0; i < std::ssize(allCompleteLines); i++) {  // n^2logn
        std::sort(allCompleteLines[i].intermediaryPoints.begin(),
                  allCompleteLines[i].intermediaryPoints.end(),
                  [](const Point& a, const Point& b) -> bool {
                      if (abs(a.y - b.y) < 1e-10) {
                          return a.x < b.x;
                      }

                      return a.y < b.y;
                  });
    }

    std::sort(allCompleteLines.begin(), allCompleteLines.end(),
              [](const CompleteLine& a, const CompleteLine& b) -> bool {
                  if (abs(a.intermediaryPoints[0].y - b.intermediaryPoints[0].y) < 1e-10) {
                      return a.intermediaryPoints[0].x < b.intermediaryPoints[0].x;
                  }

                  return a.intermediaryPoints[0].y < b.intermediaryPoints[0].y;
              });  // nlogn

    deleteDuplicates(allCompleteLines);

    // writeFile(allCompleteLines, s);

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