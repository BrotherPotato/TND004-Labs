#include <linesdiscoverysystem/readfiles.h>

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

#include <rendering/window.h>
#include <fmt/format.h>
#include <limits>

void plotData(const std::string& name);

/* ************************************* */

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct LineSegment {
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
};

struct CompleteLine {
    // Point start = Point{0, 0};
    // Point end = Point{0, 0};
    std::vector<Point> intermediaryPoints = {};
    // double slope = 0.0;
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
        std::cout << "(" << line.intermediaryPoints[0].x << "," << line.intermediaryPoints[0].y << ")";

        for (size_t i = 1; i < std::ssize(line.intermediaryPoints); i++) {
            std::cout << "->(" << line.intermediaryPoints[i].x << "," << line.intermediaryPoints[i].y << ")";
        }

		std::cout << '\n';
	}
}

double calcSlope(Point& start, Point& end) {
    // double temp = (end.y - start.y) / (end.x - start.x);
    double num = end.y - start.y;
    double den = end.x - start.x;

    if (num == 0.0) {
        return 0.0;
    } else if (den == 0.0) {
        return std::numeric_limits<double>::infinity();
    }

    // double delta = 1e-10;

    return num / den;
}

// double calcLength(LineSegment& L) {
//     double x = L.end.x - L.start.x;
//     double y = L.end.y - L.start.y;
//     return sqrt(x * x + y * y);
// }

// naive implementation of linesegment
void cookLineSegments(std::vector<Point>& PV, std::vector<LinesFromPoint>& allLines
                      /*std::vector<std::pair<double, Point>>& VP*/) {
    for (int i = 0; i < std::ssize(PV); i++) { // O(n)
        LinesFromPoint linesFromP;
        linesFromP.lines.reserve(std::ssize(PV)); // - i - 1
        linesFromP.start = PV[i];
        for (int j = i + 1; j < std::ssize(PV); j++) {

            // LineSegment temp = LineSegment{PV[i], PV[j], calcSlope(PV[i], PV[j])};

            // std::pair<double, Point> tempp;
            // tempp.first = calcSlope(PV[i], PV[j]);
            // tempp.second = PV[j];
            LineSegment tempo;
            tempo.VP.first = calcSlope(PV[i], PV[j]);
            tempo.VP.second = PV[j];
            linesFromP.lines.push_back(tempo);

            // temp.m = temp.start.y - temp.slope * temp.start.x;  // m = y - kx
            //  temp.length = calcLength(temp);

            // VP.push_back(tempp);  // n om vi inte reservar
        }
        /*if (std::ssize(linesFromP.lines) >= 3) {
            allLines.push_back(linesFromP);
        }*/
        allLines.push_back(linesFromP);
    }
}

bool operator<(const LineSegment& LeftLineSeg,
               const LineSegment& RightLineSeg) {  // used in stablesort
    return LeftLineSeg.VP.first < RightLineSeg.VP.first;
}

bool operator<(const Point& LeftPoint, const Point& RightPoint) {
    if (abs(LeftPoint.y - RightPoint.y) > 0.0001) {
        return LeftPoint.x < RightPoint.x;
    }

	return LeftPoint.y < RightPoint.y;
}

void findCollinearPoints(std::vector<LinesFromPoint>& vecLinesFromP,
                         std::vector<CompleteLine>& CLV) {
    for (size_t i = 0; i < std::ssize(vecLinesFromP); i++) {  // step through all pointlines O(n)
        CompleteLine tempLine;
        tempLine.intermediaryPoints.reserve(std::ssize(vecLinesFromP[i].lines));  // reserve space for all points O(n)
        //tempLine.intermediaryPoints.push_back(vecLinesFromP[i][0].VP.second);

        size_t a = 0;
        tempLine.intermediaryPoints.push_back(vecLinesFromP[i].start);
        for (size_t j = 0; j < std::ssize(vecLinesFromP[i].lines); j++) {  // step through all lines with the same start point O(n)

            if (vecLinesFromP[i].lines[a].VP.first == std::numeric_limits<double>::infinity() &&
                vecLinesFromP[i].lines[j].VP.first == std::numeric_limits<double>::infinity()) {
                tempLine.intermediaryPoints.push_back(vecLinesFromP[i].lines[j].VP.second);
				continue; // continue goes to the next iteration of the loop
			}

            if (abs(vecLinesFromP[i].lines[a].VP.first -
                                vecLinesFromP[i].lines[j].VP.first) <
                            0.0001) {
                                tempLine.intermediaryPoints.push_back(
                                    vecLinesFromP[i].lines[j].VP.second);
                continue; // continue goes to the next iteration of the loop
            }
                
            a = j;
                
            if (tempLine.intermediaryPoints.size() > 3) {
                CLV.push_back(tempLine);
            }
            
        }

        if (tempLine.intermediaryPoints.size() > 3) {
            CLV.push_back(tempLine);
        }
    }
}

void removeDuplicates(std::vector<CompleteLine>& CLV) {

    for (size_t i = 0; i < std::ssize(CLV); i++) {
        // std::unique(CLV[i].intermediaryPoints.begin(), CLV[i].intermediaryPoints.end(),
        //             [&](Point& A, Point& B) { return A.x == B.y && A.y == B.y; });
        /*CLV[i].intermediaryPoints.erase(
            std::unique(CLV[i].intermediaryPoints.begin(), CLV[i].intermediaryPoints.end(),
                        [&](Point& A, Point& B) { return A.x == B.y && A.y == B.y; }));*/

        // for (size_t j = 0; j < CLV[i].intermediaryPoints.size(); j++) {

        //}

        // std::cout << "this bitch empty";
    }
}

int main() try {
    std::cout << "Enter the name of input points file: ";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt

    std::vector<Point> allPoints = fileReader(s);

    std::cout << "A";

    // std::vector<LineSegment> allLines;
    // allLines.reserve(allPoints.size() * allPoints.size());  // lol

    // std::vector<std::vector<LineSegment>> setOfAllLines;
    // setOfAllLines.reserve(allPoints.size() * allPoints.size());  // lol

    // sortera också points?

    std::vector<LinesFromPoint> allLines;

    cookLineSegments(allPoints, allLines);  // n^2, n^3 om vi inte anv�nder reserve

    //std::sort(allLines.begin(), allLines.end());  // nlogn

    // TODO: ta bort alla kopior
    // TODO: sl� ihop linjer

    std::vector<CompleteLine> allCompleteLines;

    findCollinearPoints(allLines, allCompleteLines);

    std::cout << "SUMSUM: " << allCompleteLines.size();

    

    for (size_t i = 0; i < std::ssize(allCompleteLines); i++) {
        std::stable_sort(allCompleteLines[i].intermediaryPoints.begin(),
                         allCompleteLines[i].intermediaryPoints.end());
    }

    //removeDuplicates(allCompleteLines);

    /*for (size_t i = 0; i < allCompleteLines.size() - 1; i++) {
        std::cout << allCompleteLines[i].intermediaryPoints.size();
        for (size_t j = 0; j < allCompleteLines[i].intermediaryPoints.size() - 1; j++) {
            std::cout << "(" << allCompleteLines[i].intermediaryPoints[j].x << " , ";
            std::cout << allCompleteLines[i].intermediaryPoints[j].y << ") \n";
        }
    }*/

    //writeFile(allCompleteLines, s);

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