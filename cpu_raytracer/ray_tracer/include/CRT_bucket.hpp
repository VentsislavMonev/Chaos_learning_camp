#ifndef CRT_BUCKET_HPP
#define CRT_BUCKET_HPP

#include <algorithm>
#include <vector>

// a rectangular region of the image: [x0, x1) x [y0, y1)
struct CRT_bucket
{
    int x0, y0, x1, y1;

    int width()  const { return x1 - x0; }
    int height() const { return y1 - y0; }
};

inline constexpr int CRT_DEFAULT_BUCKET_SIZE = 16;

// splits a width x height image into buckets of at most bucket_size x bucket_size.
// buckets on the right and bottom edges are clipped to the image bounds.
inline std::vector<CRT_bucket> CRT_make_buckets(int image_width, int image_height, int bucket_size = CRT_DEFAULT_BUCKET_SIZE)
{
    bucket_size = std::max(1, bucket_size);

    std::vector<CRT_bucket> buckets;
    buckets.reserve(static_cast<size_t>((image_width  + bucket_size - 1) / bucket_size) *
                    static_cast<size_t>((image_height + bucket_size - 1) / bucket_size));

    for (int y = 0; y < image_height; y += bucket_size)
    {
        int y1 = std::min(y + bucket_size, image_height);
        for (int x = 0; x < image_width; x += bucket_size)
        {
            int x1 = std::min(x + bucket_size, image_width);
            buckets.push_back({ x, y, x1, y1 });
        }
    }
    return buckets;
}

#endif
