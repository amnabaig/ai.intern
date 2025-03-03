#include <iostream>
#include <opencv.hpp>
#include <opencv2>

using namespace cv;
using namespace dnn;

const std::string modelConfiguration = "yolov3.cfg";
const std::string modelWeights = "yolov3.weights";
const std::string classesFile = "coco.names";

std::vector<std::string> getOutputsNames(const Net& net) {
    static std::vector<std::string> names;
    if (names.empty()) {
        std::vector<int> outLayers = net.getUnconnectedOutLayers();
        std::vector<std::string> layersNames = net.getLayerNames();
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}

void detectObjects(Mat& frame, Net& net, const std::vector<std::string>& classes) {
    Mat blob;
    blobFromImage(frame, blob, 1 / 255.0, Size(416, 416), Scalar(), true, false);
    net.setInput(blob);
    std::vector<Mat> outs;
    net.forward(outs, getOutputsNames(net));

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<Rect> boxes;

    for (auto& output : outs) {
        for (int i = 0; i < output.rows; i++) {
            float confidence = output.at<float>(i, 4);
            if (confidence > 0.5) {
                int classId = std::max_element(output.ptr<float>(i) + 5, output.ptr<float>(i) + output.cols) - output.ptr<float>(i) - 5;
                float* data = output.ptr<float>(i);
                int centerX = static_cast<int>(data[0] * frame.cols);
                int centerY = static_cast<int>(data[1] * frame.rows);
                int width = static_cast<int>(data[2] * frame.cols);
                int height = static_cast<int>(data[3] * frame.rows);
                int x = centerX - width / 2;
                int y = centerY - height / 2;
                classIds.push_back(classId);
                confidences.push_back(confidence);
                boxes.push_back(Rect(x, y, width, height));
            }
        }
    }
    std::vector<int> indices;
    NMSBoxes(boxes, confidences, 0.5, 0.4, indices);

    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        Rect box = boxes[idx];
        int classId = classIds[idx];
        float confidence = confidences[idx];

        rectangle(frame, box, Scalar(0, 255, 0), 2);

        std::string label = classes[classId] + ": " + std::to_string(confidence);
        int baseline = 0;
        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        rectangle(frame, Point(box.x, box.y - labelSize.height),
                  Point(box.x + labelSize.width, box.y), Scalar(0, 255, 0), FILLED);
        putText(frame, label, Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
    }
}

int main() {
    Net net = readNetFromDarknet(modelConfiguration, modelWeights);
    if (net.empty()) {
        std::cerr << "Error: Could not load YOLO model" << std::endl;
        return -1;
    }
    std::ifstream ifs(classesFile.c_str());
    std::vector<std::string> classes;
    std::string line;
    while (std::getline(ifs, line)) {
        classes.push_back(line);
    }
    VideoCapture cap(0); 
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }
    Mat frame;
    while (cap.read(frame)) {
        detectObjects(frame, net, classes);
        imshow("Object Detection", frame);
        if (waitKey(1) == 27) break;
    }

    return 0;
}

