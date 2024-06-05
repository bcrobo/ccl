#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <limits>

namespace fs = std::filesystem;

cv::Mat load_image_and_threshold(fs::path const &image, int threshold) {
  auto img = cv::imread(image.string(), cv::IMREAD_GRAYSCALE);
  auto thresholded = cv::Mat{};
  cv::threshold(img, thresholded, threshold, 255, cv::THRESH_BINARY);
  // Set borders to background
  thresholded.row(0).setTo(0);
  thresholded.row(thresholded.rows - 1).setTo(0);
  thresholded.col(0).setTo(0);
  thresholded.col(thresholded.cols - 1).setTo(0);
  return thresholded;
}

void first_scan(cv::Mat const &image, std::vector<int> &equivalences,
                cv::Mat &labels) {
  const auto cols = image.cols;
  const auto rows = image.rows;
  labels.setTo(0);
  auto l = 1;
  equivalences.clear();
  equivalences.emplace_back(std::numeric_limits<int>::max());
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < cols; ++c) {
      const auto idx = r * cols + c;
      // Skip non-object pixels
      if (image.data[idx] == 0)
        continue;

      const std::array neighbors = {
          labels.ptr<int>(r - 1, c - 1), labels.ptr<int>(r - 1, c),
          labels.ptr<int>(r - 1, c + 1), labels.ptr<int>(r, c - 1)};

      bool has_object_neighbors{false};
      auto min_label = equivalences[*neighbors[0]];
      // Get min_label provisionnal label among the neighbor object pixels
      for (auto i = 1; i < neighbors.size(); ++i) {
        if (*neighbors[i] != 0) {
          has_object_neighbors = true;
          if (equivalences[*neighbors[i]] < min_label) {
            min_label = equivalences[*neighbors[i]];
          }
        }
      }
      // All surrounding pixels are background
      if (not has_object_neighbors) {
        // Creates a new provisionnal label
        labels.at<int>(r, c) = l;
        equivalences.emplace_back(l);
        ++l;
      } else {
        for (auto const &n : neighbors) {
          if (*n != 0) {
            equivalences[*n] = min_label;
          }
        }
        labels.at<int>(r, c) = min_label;
      }
    }
  }
}

void subsequent_scan(cv::Mat const &image, cv::Mat &labels,
                     std::vector<int> &equivalences, bool &has_changed) {
  has_changed = false;
  for (auto r = 0; r < labels.rows; ++r) {
    for (auto c = 0; c < labels.cols; ++c) {
      const auto idx = r * labels.cols + c;
      // Skip background pixels
      if (image.data[idx] == 0)
        continue;

      const std::array neighbors = {
          labels.ptr<int>(r - 1, c - 1), labels.ptr<int>(r - 1, c),
          labels.ptr<int>(r - 1, c + 1), labels.ptr<int>(r, c - 1)};

      bool has_object_neighbors{false};
      auto min_label = equivalences[*neighbors[0]];
      for (auto i = 1; i < neighbors.size(); ++i) {
        if (*neighbors[i] != 0) {
          has_object_neighbors = true;
          if (equivalences[*neighbors[i]] < min_label) {
            min_label = equivalences[*neighbors[i]];
          }
        }
      }
      if (not has_object_neighbors)
        continue;

      if (min_label != labels.at<int>(r, c))
        has_changed = true;
      labels.at<int>(r, c) = min_label;

      for (auto const &n : neighbors) {
        if (*n != 0) {
          if (min_label != equivalences[*n]) {
            has_changed = true;
            equivalences[*n] = min_label;
          }
        }
      }
    }
  }
}

cv::Mat colorize_labels(const cv::Mat &labels,
                        const std::vector<int> &equivalences) {
  const auto num_colors = std::size(equivalences);

  cv::RNG rng(12345);
  std::vector<cv::Scalar> colors;
  for (auto i = 0; i < num_colors; ++i) {
    colors.emplace_back(rng.uniform(0, 255), rng.uniform(0, 255),
                        rng.uniform(0, 255));
  }

  cv::Mat cc(labels.size(), CV_8UC3);
  cc.setTo(0);
  for (auto r = 0; r < labels.rows; ++r) {
    for (auto c = 0; c < labels.cols; ++c) {
      const auto l = labels.at<int>(r, c);
      if (l == 0)
        continue;

      const auto &color = colors[equivalences[l]];
      cc.at<cv::Vec3b>(r, c) = cv::Vec3b(color[0], color[1], color[2]);
    }
  }
  return cc;
}
auto main(int argc, char **argv) -> int {
  if (argc != 2) {
    std::cerr << "- Provide an input image as argument.\n";
    return -1;
  }

  constexpr int threshold = 120;
  const auto img = load_image_and_threshold(argv[1], threshold);
  bool has_changed{true};
  bool first{true};
  cv::Mat labels(img.size(), CV_32S);
  std::vector<int> equivalences;
  while (has_changed) {
    if (first) {
      first_scan(img, equivalences, labels);
      first = false;
    }
    subsequent_scan(img, labels, equivalences, has_changed);
  }
  const auto label_image = colorize_labels(labels, equivalences);
  while (cv::waitKey(30) != 'q') {
    cv::imshow("image", img);
    cv::imshow("labels", label_image);
  }
}
