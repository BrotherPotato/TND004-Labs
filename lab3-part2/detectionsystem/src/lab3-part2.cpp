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

// T(n)=O(n) S(n+1)=O(n)
std::vector<Point> fileReader(const std::string& filename) {
    std::filesystem::path points_name = data_dir / filename;

    std::cout << points_name << '\n';
    std::ifstream pointsFile(points_name);

    if (!pointsFile) {
        std::cout << "Points file error!!\n";
        assert(pointsFile);
        return {};
    }

    int n_points{0}; // S(1)
    pointsFile >> n_points;  // read number of particles

    std::vector<Point> points;
    points.reserve(n_points); // T(n)=O(n) S(n)=O(n)
    for (int i = 0; i < n_points; ++i) { // T(n)=O(n)
        Point temp = Point{};
        auto& p = points.emplace_back(temp); // T(1)=O(1)
        pointsFile >> p.x >> p.y;
    }
    return points;
}

// T(m)=O(m)
void writeFile(std::vector<CompleteLine>& lines, std::string readFileName) {
    std::filesystem::path points_name = data_dir / "output" / ("segments-" + readFileName);
    std::cout << points_name << '\n';
    std::ofstream out_file(points_name);
    if (!out_file) {
        std::cout << "Error opening output file!!\n";
        return;
    }
    for (const auto& line : lines) { // T(n)=O(m)
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
// T(m*n) = O(m*n) S(1) = O(1)
void writeConsole(std::vector<CompleteLine>& lines) {
    int index = 0; // S(1)
    for (const auto& line : lines) { // T(m)=O(m)
        if (line.intermediaryPoints.size() < 3) {
            continue;
        }
        std::cout << index << ": " << line.slope << ": (" << line.intermediaryPoints[0].x 
                  << ","
                  << line.intermediaryPoints[0].y << ")";

        for (size_t i = 1; i < std::ssize(line.intermediaryPoints); i++) { // T(n)=O(n)
            std::cout << "->(" << line.intermediaryPoints[i].x << ","
                      << line.intermediaryPoints[i].y << ")";
        }

        std::cout << '\n';
        index++;
    }
}

// T(1)=O(1) S(2)=O(1)
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

// T(2n^2) = O(n) S(n) = O(n)
void createLineSegments(std::vector<Point>& PV, std::vector<LinesFromPoint>& allLines) {
    for (int i = 0; i < std::ssize(PV); i++) {  // T(n)      S(n) from reserve
        LinesFromPoint linesFromP;
        linesFromP.lines.reserve(std::ssize(PV)); // T(n)
        linesFromP.start = PV[i];
        for (int j = 0; j < std::ssize(PV); j++) {  // T(n)
            if (i == j) continue;
            LineSegment tempo;
            tempo.VP.first = calcSlope(PV[i], PV[j]); // O(1)
            tempo.VP.second = PV[j];
            // linesFromP.slope = tempo.VP.first;
            linesFromP.lines.push_back(tempo); // T(1)
        }
        allLines.push_back(linesFromP);
    }
}
// T(1) = O(1) S(1) = O(1)
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

// T(2n*(n-1)) = O(n^2)  S(n) = O(n)
 void findCollinearPoints(std::vector<LinesFromPoint>& vecLinesFromP,
                          std::vector<CompleteLine>& CLV) {
     for (int i = 0; i < std::ssize(vecLinesFromP); i++) {  // step through all pointlines T(n) 
         CompleteLine tempLine;
         
         tempLine = CompleteLine{};
         tempLine.intermediaryPoints.reserve(std::ssize(vecLinesFromP)); // T(n) S(n)
         tempLine.intermediaryPoints.push_back(vecLinesFromP[i].start); 
         tempLine.slope = vecLinesFromP[i].lines[0].VP.first;
         tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[0].VP.second);
         //double currentSlope = tempLine.slope;

         for (int j = 0; j < std::ssize(vecLinesFromP[i].lines) - 1; j++) {  // T(n-1) n-1 lines to all other points
            auto it1 = vecLinesFromP[i].lines[j].VP;    // S(1)
            auto it2 = vecLinesFromP[i].lines[j + 1].VP; // S(1)
            // we either add the point from the line to the vector or we start a new line
            if ((it1.first == std::numeric_limits<double>::infinity() &&
                 it2.first == std::numeric_limits<double>::infinity()) ||
                (abs(it1.first - it2.first) < delta)) { 
                tempLine.intermediaryPoints.push_back(it2.second); // T(1)

            } else { 
                if (tempLine.intermediaryPoints.size() > 3) { // if we have more than 3 points in the line
                    CLV.push_back(tempLine); // T(1)
                }
                
                tempLine = CompleteLine{};
                tempLine.intermediaryPoints.push_back(vecLinesFromP[i].start); // T(1)
                tempLine.slope = vecLinesFromP[i].lines[j+1].VP.first;
                tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[j+1].VP.second); // T(1)
            }
         }
         // if completeLine is last of vecLinesFromP[i].lines
         if (tempLine.intermediaryPoints.size() > 3) {
            CLV.push_back(tempLine); // T(1)
         }
     }
 }
 // we are not really sure if we sould write max(m*n, m^2) or m*n or m^2 since m*n is large if m<n and m^2 is large if m>n
 // T(max(m*n, m^2)) = O(max(m*n, m^2)) S(m) = O(m)
void deleteDuplicates(std::vector<CompleteLine>& allCompleteLines) {
    std::vector<bool> duplicates(allCompleteLines.size(), false); // S(m)
    // This block of code is T(m*n)
    for (int i = 0; i < std::ssize(allCompleteLines) - 1; i++) {  // T(m)
        if ((abs(allCompleteLines[i].slope - allCompleteLines[i + 1].slope) < delta) ||
            (allCompleteLines[i].slope == std::numeric_limits<double>::infinity() &&
             allCompleteLines[i + 1].slope == std::numeric_limits<double>::infinity())) {
            for (size_t j = 0; j < std::ssize(allCompleteLines[i].intermediaryPoints); // T(n)
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
    // This block of code is T(m^2)
    for (int i = std::ssize(allCompleteLines) - 1; i > 0; i--) {  // T(m)
        if (duplicates[i]) {
            allCompleteLines.erase(allCompleteLines.begin() + i); // T(m)
        }
    }
}
// n is the number of points, m is the number of lines
int main() try {
    std::cout << "Enter the name of input points file: ";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt

    std::vector<Point> allPoints = fileReader(s);  // O(n)

    std::sort(allPoints.begin(), allPoints.end());  // O(n*log(n))

    std::vector<LinesFromPoint> allLines;

    allLines.reserve(std::ssize(allPoints)); // reserve "one piece of space" for each point S(n)=O(n)

    createLineSegments(allPoints, allLines);  // T(2n^2) = O(n) S(n) = O(n)

    allPoints.clear();  // clear allPoints O(n)

    // sort lines by slope T(n^2*log(n)) = O(n^2*log(n)) S(1) = O(1)
    for (size_t i = 0; i < std::ssize(allLines); i++) { // n
        std::stable_sort(allLines[i].lines.begin(), allLines[i].lines.end(), // n*log(n)
                         [](const LineSegment& a, const LineSegment& b) -> bool {
                             if (abs(a.VP.first - b.VP.first) < delta) { // S(1)
                                 return a.VP.second < b.VP.second;
                             }

                             return a.VP.first < b.VP.first;
                         });
    }


    std::vector<CompleteLine> allCompleteLines;
    // we reserve for the duplicate completed lines
    // T(n) = O(n) S(n^2) = O(n^2) // we use n since the size of allLines is the same as the number of points
    allCompleteLines.reserve(std::ssize(allLines) * (std::ssize(allLines)-1)/4);  // total number of line segments is n(n-1), 4 points per complete line

    findCollinearPoints(allLines, allCompleteLines);  // T(2n*(n-1)) = O(n^2)  S(n) = O(n)

    allLines.clear();  // clear allLines O(m)

    // use n since we sort by points
    // T(n^2*log(n)) = O(n^2*log(n)) S(1) = O(1)
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
    // use m sice we sort by lines
    // T(m*log(m)) = O(m*log(m)) S(1) = O(1)
    std::stable_sort(allCompleteLines.begin(), allCompleteLines.end(), // mlogm
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
    deleteDuplicates(allCompleteLines); // T(max(m*n, m^2)) = O(max(m*n, m^2)) S(m) = O(m)

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