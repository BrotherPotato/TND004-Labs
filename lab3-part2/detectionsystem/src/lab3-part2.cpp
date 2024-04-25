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
};

struct CompleteLine {
    //Point start = Point{0, 0};
    //Point end = Point{0, 0};
    std::vector<Point> intermediaryPoints = {};
    double slope = 0.0;
};

//std::vector<std::vector<Point>>
// 
//  jag hatar denna klass s� mycket. Like varf�r �r det en class som bara hanterar slope och vector
//  av linesegment vi kommer bara beh�va 1 vector och ber�kna slope kan vara en funktion.
class Lines { 
public:
    Lines(); //constructor
    ~Lines() = default; //destructor

    void calcSlope(LineSegment&);

private:
    std::vector<LineSegment> lineVec;
};

Lines::Lines() {}

void Lines::calcSlope(LineSegment& L) {
    L.slope = (L.end.y - L.start.y) / (L.end.x - L.start.x);
}  // vill vi att den ber�knar f�r en eller f�r alla och ska den returnera n�got eller vill vi ta
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
}

void writeFile(std::vector<CompleteLine>& lines, std::string readFileName) {
    std::ofstream out_file("../data/segments-" + readFileName + ".txt");
    if (!out_file) {
        std::cout << "Error opening output file!!\n";
        return;
    }
    //out_file << lines.size() << '\n';
    for (const auto& line : lines) {
        if (line.intermediaryPoints.size() < 3) {
            continue;
        }
        //out_file << "(" << line.start.x << "," << line.start.y << ")";
        for (Point currentPoint : line.intermediaryPoints) {
            out_file << "(" << currentPoint.x << "," << currentPoint.y << ")->";
        }
        out_file << '\n';
    }
    out_file.close();
}

double calcSlope(Point start, Point end) { return (end.y - start.y) / (end.x - start.x); }

void cookLineSegments(std::vector<Point>& PV, std::vector<LineSegment>& LV) {
    for (int i = 0; i < std::ssize(PV); i++) {
        for (int j = 0; j < std::ssize(PV); j++) {
            LineSegment temp = LineSegment{PV[i], PV[j], calcSlope(PV[i], PV[j])};
            if (temp.slope != 0.0) {
                LV.push_back(temp);
            }
        }
    }
}

bool operator<(const LineSegment& LeftLineSeg, const LineSegment& RightLineSeg) {// used in stablesort
    return LeftLineSeg.slope < RightLineSeg.slope; 
}



int main() try {
    std::cout << "Enter the name of input points file: ";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt

    std::vector<Point> allPoints = fileReader(s);

    std::cout << "A";
	
    std::vector<LineSegment> allLines; 

	cookLineSegments(allPoints, allLines);

    std::stable_sort(allLines.begin(), allLines.end());

    std::vector<CompleteLine> allCompleteLines;

	//Lines lines; // like det h�r �r s� j�vla pantat :((((
    //lines.calcSlope(allLines[0]);

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