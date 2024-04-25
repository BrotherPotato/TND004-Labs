#include <linesdiscoverysystem/readfiles.h>

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

#include <rendering/window.h>
#include <fmt/format.h>

void plotData(const std::string& name);

/* ************************************* */

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct LineSegment {
    Point start = Point{0, 0};
    Point end = Point{0, 0};
    double slope = 0.0;
    double length = 0.0;
    double m = 0.0;
};

struct CompleteLine {
    // Point start = Point{0, 0};
    // Point end = Point{0, 0};
    std::vector<Point> intermediaryPoints = {};
    double slope = 0.0;
};

// std::vector<std::vector<Point>>
//
//   jag hatar denna klass så mycket. Like varför är det en class som bara hanterar slope och vector
//   av linesegment vi kommer bara behöva 1 vector och beräkna slope kan vara en funktion.
class Lines {
public:
    Lines();             // constructor
    ~Lines() = default;  // destructor

    void calcSlope(LineSegment&);

private:
    std::vector<LineSegment> lineVec;
};

Lines::Lines() {}

void Lines::calcSlope(LineSegment& L) {
    L.slope = (L.end.y - L.start.y) / (L.end.x - L.start.x);
}  // vill vi att den beräknar för en eller för alla och ska den returnera något eller vill vi ta
   // byrefrence

std::vector<Point> fileReader(const std::string& filename) {
    std::filesystem::path points_name = filename;
    std::cout << points_name << '\n';
    std::ifstream pointsFile(points_name);

    if (!pointsFile) {
        std::cout << "Points file error!!\n";
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

double calcSlope(Point& start, Point& end) {
    // double temp = (end.y - start.y) / (end.x - start.x);
    double num = end.y - start.y;
    double den = end.x - start.x;

    if (num == 0.0) {
        return 0.0;
    } else if (den == 0.0) {
        return -1.0;  // reading from left to right top to bottom
    }

    // double delta = 1e-10;

    return num / den;
}

double calcLength(LineSegment& L) {
    double x = L.end.x - L.start.x;
    double y = L.end.y - L.start.y;
    return sqrt(x * x + y * y);
}

// naive implementation of linesegment
void cookLineSegments(std::vector<Point>& PV, std::vector<LineSegment>& LV) {
    for (int i = 0; i < std::ssize(PV); i++) {
        for (int j = 0; j < std::ssize(PV); j++) {
            if (i == j) continue;
            LineSegment temp = LineSegment{PV[i], PV[j], calcSlope(PV[i], PV[j])};
            temp.m = temp.start.y - temp.slope * temp.start.x;  // m = y - kx
            // temp.length = calcLength(temp);
            if (temp.slope == -1) {
            LV.push_back(temp);  // n om vi inte reservar
            }
        }
    }
}

bool operator<(const LineSegment& LeftLineSeg,
               const LineSegment& RightLineSeg) {  // used in stablesort
    return LeftLineSeg.slope < RightLineSeg.slope;
}

void findCollinearPoints(std::vector<LineSegment>& LV, std::vector<CompleteLine>& CLV) {
    for (size_t i = 0; i < std::ssize(LV);) {
        CompleteLine tempLine;

        for (size_t j = i; j < std::ssize(LV); j++) {
            if (LV[i].slope != LV[j].slope) {
                break;
            }
            if (LV[i].m == LV[j].m || (LV[i].slope == -1 && LV[j].slope == -1)) {
                tempLine.intermediaryPoints.push_back(LV[j].start);
                tempLine.intermediaryPoints.push_back(LV[j].end);

            } else {
                // kys
            }
        }
        i += tempLine.intermediaryPoints.size() / 2;
        if (tempLine.intermediaryPoints.size() / 2 >= 3) {
            CLV.push_back(tempLine);
        }
    }
}

void removeDuplicates(std::vector<CompleteLine>& CLV) {
    
    for (size_t i = 0; i < std::ssize(CLV); i++) {
        //std::unique(CLV[i].intermediaryPoints.begin(), CLV[i].intermediaryPoints.end(),
        //            [&](Point& A, Point& B) { return A.x == B.y && A.y == B.y; });
        CLV[i].intermediaryPoints.erase(
            std::unique(CLV[i].intermediaryPoints.begin(), CLV[i].intermediaryPoints.end(),
                        [&](Point& A, Point& B) { return A.x == B.y && A.y == B.y; }));
        
        //for (size_t j = 0; j < CLV[i].intermediaryPoints.size(); j++) {

        //}

    std::cout << "this bitch empty";

    }
}

int main() try {
    std::cout << "Enter the name of input points file: ";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt

    std::vector<Point> allPoints = fileReader(s);

    std::cout << "A";

    std::vector<LineSegment> allLines;
    allLines.reserve(allPoints.size() * allPoints.size());  // lol

    cookLineSegments(allPoints, allLines);  // n^2, n^3 om vi inte använder reserve

    std::stable_sort(allLines.begin(), allLines.end());  // nlogn

    // TODO: ta bort alla kopior
    // TODO: slå ihop linjer

    std::vector<CompleteLine> allCompleteLines;

    findCollinearPoints(allLines, allCompleteLines);

    std::cout << "SUMSUM: " << allCompleteLines.size();


    removeDuplicates(allCompleteLines);

    for (size_t i = 0; i < allCompleteLines.size() - 1; i++) {
        std::cout << allCompleteLines[i].intermediaryPoints.size();
        for (size_t j = 0; j < allCompleteLines[i].intermediaryPoints.size() - 1; j++) {
            std::cout << "(" << allCompleteLines[i].intermediaryPoints[j].x << " , ";
            std::cout << allCompleteLines[i].intermediaryPoints[j].y << ") \n";
        }
    }


    writeFile(allCompleteLines, s);

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